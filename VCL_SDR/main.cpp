//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include <sstream>
#include <complex>
#include <vector>
#include <cmath>
#include <algorithm>
#include <thread>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

static fftw_complex *in, *out; /*!< Input and output arrays of the transform */
static fftw_plan fftwp; /**!
			 * FFT plan that will contain
			 * all the data that FFTW needs
			 * to compute the FFT
			 */
static int n; /*!< Used at raw I/Q data to complex conversion */
DeviceConfig config;
double out_r, out_i; /*!< Real and imaginary parts of FFT *out values */
double harmonic_power; /*!< Amplitude & dB */
double dBFS[512];
std::thread samples_thread;

std::vector<double> lut_signed_iq;
std::vector<double> lut_hann_window;




TForm1 *Form1;
rtlsdr_dev_t *dev=NULL; //rtl-sdr device

ChartConfig chartConf;
bool isSimulation = false;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
   TrackBar1->Enabled = isSimulation;

  Chart1->ClipPoints = false;
  Chart1->Title->Visible = false;
  Chart1->Legend->Visible = false;
  Chart1->LeftAxis->Axis->Width = 1;
  Chart1->BottomAxis->Axis->Width = 1;
  Chart1->BottomAxis->RoundFirstLabel = false;
  Chart1->View3D = false;
  // Prepare series.
  // Disable AutoRepaint and X Order

  // AutoRepaint=False means "real-time" drawing mode.
  // Points are displayed just after adding them,
  // without redrawing the whole chart.
  Series1->AutoRepaint = False;
  // Set Ordering to none, to increment speed when adding points
  Series1->XValues->Order = loNone;

  // Initialize axis scales
  // we're assuming left axis values are within [0,250]
  //Chart1->LeftAxis->SetMinMax(-20,45);
  Chart1->LeftAxis->SetMinMax(-90,-30);
  Chart1->MarginLeft=0;
  Chart1->MarginRight=1;
  Chart1->BottomAxis->SetMinMax(config.leftBound, config.rightBound);

  // Speed tips:
  // When using only a single thread, disable locking:
  Chart1->Canvas->ReferenceCanvas->Pen->OwnerCriticalSection = 0;
  Series1->LinePen->OwnerCriticalSection = 0;


  // For Windows NT, 2000 and XP only:
  // Speed realtime painting with solid pens of width 1.
  Series1->FastPen = true;

  // Set axis calculations in "fast mode".
  // Note: For Windows Me and 98 might produce bad drawings when
  //       chart zoom is very big.
  Chart1->Axes->FastCalc = True;
  Series1->DrawAllPoints=false;

  for (unsigned int i = 0; i < 0x100; i++){
	 lut_signed_iq.push_back((i - 127.4f) / 128.0f);
  }
   for (unsigned int i = 0; i < config.n_read; i++){
	 lut_hann_window.push_back(0.5 * (1 - cos(2*M_PI*i/config.n_read)));
  }





}

/*!
 * Create FFT graph from raw I/Q samples read from RTL-SDR.
 *
 * \param sample_c sample count, also used for FFT size
 * \param buf array that contains I/Q samples
 */
static void create_fft(int sample_c, uint8_t *buf){

	config.avg_center_power = 0;
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*sample_c);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*sample_c);
	fftwp = fftw_plan_dft_1d(sample_c, in, out, FFTW_FORWARD, FFTW_MEASURE);
	for (int i=0; i<sample_c; i++){
		in[i][0] = lut_signed_iq[buf[i*2]]*lut_hann_window[i];
		in[i][1] = lut_signed_iq[buf[i*2+1]]*lut_hann_window[i];
	}
	fftw_execute(fftwp);
	Form1->Series1->Delete(0,sample_c);
	if(!isSimulation){
		Application->ProcessMessages();
	}
	std::rotate(out[0], out[(sample_c>>1)],out[sample_c]);
	for (int i=0; i < sample_c; i++){
	   out_r = out[i][0] * out[i][0];
	   out_i = out[i][1] * out[i][1];
	   harmonic_power = out_r + out_i;
	   dBFS[i] = 10 * log10(harmonic_power/(sample_c*sample_c));
	   if(!config.trashold_mode){
		  Form1->Series1->AddXY(config.leftBound+i*config.freq_step,dBFS[i]);
	   }
	}
	 Form1->plot->addLine(dBFS,sample_c);
	for (int i=0; i < 40; i++){
		config.avg_center_power+=dBFS[(sample_c>>1)-20+i];
	}
	config.avg_center_power/=40.0;
	Form1->avg_power->Caption=config.avg_center_power;
	if(config.trashold_mode && config.avg_center_power >(-20.0)){
		Form1->Memo1->Lines->Add("Signal detected");
		config.trashold_mode=!config.trashold_mode;
	}
	fftw_destroy_plan(fftwp);
	fftw_free(in);
	fftw_free(out);
}

/*!
 * Asynchronous read callback.
 * Program jump to this function after read operation finished.
 * Runs create_fft() function and provides continuous read
 * depending on the -C argument with refresh rate.
 * Exits if -C argument is not given.
 *
 * \param n_buf raw I/Q samples
 * \param len length of buffer
 * \param ctx context which is given at rtlsdr_read_async(...)
 */
static void async_read_callback(uint8_t *n_buf, uint32_t len, void *ctx){

	create_fft(config.n_read, n_buf);
	if(!config.read_samples){
		rtlsdr_cancel_async(dev);
	}else {
		rtlsdr_read_async(dev, async_read_callback, NULL, 0, config.n_read * config.n_read);
	}
}


//---------------------------------------------------------------------------
void __fastcall TForm1::StartRTLSDRClick(TObject *Sender)
{
//RTLSDR_API int rtlsdr_open(rtlsdr_dev_t **dev, uint32_t index)
int do_exit = 0;
UnicodeString message;
int count, r;


int device_count = rtlsdr_get_device_count();
if (!device_count) {
	Memo1->Lines->Add(L"Пристрою не знайдено");
	return;
} else{
	Memo1->Lines->Add(message.sprintf(L"Новий пристрій знайдено: %hs ", rtlsdr_get_device_name(0)));
	infoDevName->Caption=message.sprintf(L"%hs",rtlsdr_get_device_name(0));
}

//open the first default(0) rtl-sdr device
if (0 != rtlsdr_open(&dev, 0)) {
	Memo1->Lines->Add(L"Помилка відкриття rtl-sdr пристрою");
	return;
} else{
	Memo1->Lines->Add(L"rtl-sdr пристрій ініціалізовано");
	config.gain_n = rtlsdr_get_tuner_gains(dev, NULL);
	Memo1->Lines->Add(message.sprintf(L"Можливі значення підсилення (%d): ", config.gain_n));
	rtlsdr_get_tuner_gains(dev, config.gains);
	message.SetLength(0);
	int manual_gain_val;
	for (int i = 0; i < config.gain_n; i++){
		 message+=FloatToStr(config.gains[i]/10.0)+", ";
		 if (config.gains[i] > 250 && config.gains[i] < 300)
			manual_gain_val = config.gains[i];
	}
	message.SetLength(message.Length() -2);
	Memo1->Lines->Add(message);

	//manually set gain mode (0 for automatic)
	rtlsdr_set_tuner_gain_mode(dev, 1);
	if(!rtlsdr_set_tuner_gain(dev, manual_gain_val)){
		Memo1->Lines->Add(message.sprintf(L"Підсилення встановлено %.1f", manual_gain_val/10.0));
		infoGain->Caption=message.sprintf(L"%.1f dB", manual_gain_val/10.0);
	} else {
		Memo1->Lines->Add(L"Неможливо встановити підсилення");
	}

    /**!
 * Enable or disable offset tuning for zero-IF tuners, which allows to avoid
	 * problems caused by the DC offset of the ADCs and 1/f noise.
	 */
	rtlsdr_set_offset_tuning(dev, 1);
	//rtlsdr_set_testmode(dev, 1);
	rtlsdr_set_center_freq(dev, config.center_frequency);
	rtlsdr_set_sample_rate(dev, config.sample_rate);

	/* Reset endpoint before we start reading from it (mandatory) */
	int r = rtlsdr_reset_buffer(dev);
		if (r < 0){
			Memo1->Lines->Add("Помилка очистки буфера");
			return;
		}
	}
	config.read_samples=true;
	config.shutDown=false;
	infoFFTSize->Caption=config.n_read;
	infoSampleRate->Caption=config.sample_rate;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
config.read_samples=false;
config.shutDown = true;
}
//---------------------------------------------------------------------------

void readSamples(){
	//blocks till config.read_samples is true
	rtlsdr_read_async(dev, async_read_callback, NULL, 0, config.n_read * config.n_read);
	if (config.shutDown) {
		rtlsdr_close(dev);
	}
	if(config.freq_update){
	   config.setCenterFrequency(dev);
	   Form1->Chart1->BottomAxis->SetMinMax(config.leftBound, config.rightBound);
	   config.read_samples=true;
	   config.freq_update=false;
	   readSamples();
	}
}

void __fastcall TForm1::Button1Click(TObject *Sender)
{
	readSamples();
}
//---------------------------------------------------------------------------
void signal_simulation(){
 uint8_t signal_buf[config.n_read*2];
 for (int i=0;i<config.n_read; i++) {
	double multiplier = 0.5 * (1 - cos(2*M_PI*i/config.n_read));
	//double multiplier = 1;
	signal_buf[i*2]=(uint8_t)(multiplier*(127.5*cos(2*M_PI*config.center_frequency*(1.0/config.sample_rate)*i))+128);
	signal_buf[i*2+1]=(uint8_t)(multiplier*(127.5*sin(2*M_PI*config.center_frequency*(1.0/config.sample_rate)*i))+128);
 }
 create_fft(config.n_read, signal_buf);
}

void __fastcall TForm1::Button3Click(TObject *Sender)
{
	isSimulation=!isSimulation;
	TrackBar1->Enabled = isSimulation;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TrackBar1Change(TObject *Sender)
{

   Series1->Delete(0,config.n_read);
   Series2->Delete(0,config.n_read);
   TrackValue->Caption=TrackBar1->Position;
   config.center_frequency=TrackBar1->Position;
   signal_simulation();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
rtlsdr_read_sync(dev, config.buffer, config.out_block_size, &config.bytes_in_response);
Form1->Series1->Delete(0,config.n_read);
Form1->Series2->Delete(0,config.n_read);
   for (int i = 0; i < config.n_read; i++) {
   Form1->Series1->AddXY(i, config.buffer[i*2]);
   Form1->Series2->AddXY(i, config.buffer[i*2+1]);
}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::NumberBox1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
if (Key == VK_RETURN)
	{
		config.freq_update=true;
		config.center_frequency =NumberBox1->Value*1'000'000;
		config.read_samples=false;
	}
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button5Click(TObject *Sender)
{
	config.trashold_mode=!config.trashold_mode;
    config.freq_update=true;
	config.center_frequency =NumberBox1->Value*1'000'000;
	config.read_samples=false;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormShow(TObject *Sender)
{
plot = new WaterfallPlot(this->Image1);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
 delete(plot);
}
//---------------------------------------------------------------------------



