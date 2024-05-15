//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <sstream>
#include <complex>
#include <vector>
#include <cmath>
#include <algorithm>
#include <thread>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define NUM_READ 512
static const int n_read = NUM_READ; /*!< Sample count & data points & FFT size */
static fftw_complex *in, *out; /*!< Input and output arrays of the transform */
static fftw_plan fftwp; /**!
			 * FFT plan that will contain
			 * all the data that FFTW needs
			 * to compute the FFT
			 */
static int n; /*!< Used at raw I/Q data to complex conversion */

double out_r, out_i; /*!< Real and imaginary parts of FFT *out values */
static double harmonic_power, dBFS; /*!< Amplitude & dB */
std::thread samples_thread;

std::vector<double> lut_signed_iq;

TForm1 *Form1;
rtlsdr_dev_t *dev=NULL; //rtl-sdr device
DeviceConfig config;
ChartConfig chartConf;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{


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
  Chart1->BottomAxis->SetMinMax(0,n_read);

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

}

/*!
 * Create FFT graph from raw I/Q samples read from RTL-SDR.
 *
 * \param sample_c sample count, also used for FFT size
 * \param buf array that contains I/Q samples
 */
static void create_fft(int sample_c, uint8_t *buf){
	/**!
	 * Configure FFTW to convert the samples in time domain to frequency domain.
	 * Allocate memory for 'in' and 'out' arrays.
	 * fftw_complex type is a basically double[2] where
	 * the [0] element holds the real part and the [1] element holds the imaginary part.
	 * in -> Complex numbers processed from 8-bit I/Q values.
	 * out -> Output of FFT (computed from complex input).
	 */
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*sample_c);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*sample_c);
	/**!
	 * Declare FFTW plan which is responsible for having in and out data.
	 * First parameter (sample_c) -> FFT size
	 * FFTW_FORWARD/FFTW_BACKWARD -> Indicates the direction of the transform.
	 * Technically, sign of the exponent in the transform.
	 * FFTW_MEASURE/FFTW_ESTIMATE
	 * Use FFTW_MEASURE if you want to execute several FFTs and find the
	 * best computation in certain amount of time. (Usually a few seconds)
	 * FFTW_ESTIMATE is the contrary. Does not run any computation, just
	 * builds a reasonable plan.
	 * If you are dealing with many transforms of the same FFT size and
	 * initialization time is not important, use FFT_MEASURE.
	 */
	fftwp = fftw_plan_dft_1d(sample_c, in, out, FFTW_FORWARD, FFTW_MEASURE);
	/**!
	 * Convert buffer from IQ to complex ready for FFTW.
	 * RTL-SDR outputs 'IQIQIQ...' so we have to read two samples
	 * at the same time. 'n' is declared for this approach.
	 * Sample is 127 for zero signal, so substract ~127.5 for exact value.
	 * Loop through samples and fill 'in' array with complex samples.
	 *
	 * NOTE: There is a common issue with cheap RTL-SDR receivers which
	 * is 'center frequency spike' / 'central peak' problem related to
	 * I/Q imbalance. This problem can be solved with a implementation of
	 * some algorithms.
	 * More detail:
	 * https://github.com/roger-/pyrtlsdr/issues/94
	 * https://wiki.analog.com/resources/eval/user-guides/ad-fmcomms1-ebz/iq_correction
	 *
	 * TODO #1: Implement I/Q correction
	 */
	for (int i=0; i<sample_c; i++){
		in[i][0] = lut_signed_iq[buf[i*2]];
		in[i][1] = lut_signed_iq[buf[i*2+1]];
	}
	/**!
	 * Convert the complex samples to complex frequency domain.
	 * Compute FFT.
	 */
	fftw_execute(fftwp);
	Form1->Series1->Delete(0,sample_c);
	Application->ProcessMessages();
	std::rotate(&out[0], &out[(sample_c>>1)],&out[sample_c]);
	for (int i=0; i < sample_c; i+=2){
	   out_r = out[i][0] * out[i][0];
	   out_i = out[i][1] * out[i][1];
	   harmonic_power = out_r + out_i;
	   dBFS = 10 * log10(harmonic_power/(sample_c*sample_c));
	   Form1->Series1->AddXY(i,dBFS);
	}
	/**!
	 * Deallocate FFT plan.
	 * Free 'in' and 'out' memory regions.
	 */
	fftw_destroy_plan(fftwp);
	fftw_free(in);
	fftw_free(out);
   //	read_count++;
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

	create_fft(n_read, n_buf);
	if(!config.read_samples){
		rtlsdr_cancel_async(dev);
	}else {
		rtlsdr_read_async(dev, async_read_callback, NULL, 0, n_read * n_read);
	}
	//_cont_read - 0 for only one read and 1 for continues read
	//_num_read - number of read to do
	//read_count - current read count; is increased after the each fft output
	//_refresh_rate - how long to wait beetwen the read operations(default 500ms)
//	if (_cont_read && read_count < _num_read){
//		usleep(1000*_refresh_rate);
//		rtlsdr_read_async(dev, async_read_callback, NULL, 0, n_read * n_read);
//	}else{
//		log_info("Done, exiting...\n");
//		do_exit();
//	}
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
	Memo1->Lines->Add("No devices found.");
	return;
} else{
	Memo1->Lines->Add(message.sprintf(L"Device found: %hs ", rtlsdr_get_device_name(0)));
}

//open the first default(0) rtl-sdr device
if (0 != rtlsdr_open(&dev, 0)) {
	Memo1->Lines->Add("Failed to open the rtl-sdr device");
	return;
} else{
	Memo1->Lines->Add("rtl-sdr device is opened");
	count = rtlsdr_get_tuner_gains(dev, NULL);
	Memo1->Lines->Add(message.sprintf(L"Supported gain values (%d): ", count));

	count = rtlsdr_get_tuner_gains(dev, config.gains);
	message.SetLength(0);
	int manual_gain_val;
	for (int i = 0; i < count; i++){
		 message+=FloatToStr(config.gains[i]/10.0)+", ";
		 if (config.gains[i] > 250 && config.gains[i] < 300)
			manual_gain_val = config.gains[i];
	}
	message.SetLength(message.Length() -2);
	Memo1->Lines->Add(message);

	//manually set gain mode (0 for automatic)
	rtlsdr_set_tuner_gain_mode(dev, 1);
	if(!rtlsdr_set_tuner_gain(dev, manual_gain_val)){
		Memo1->Lines->Add(message.sprintf(L"Gain set to %.1f", manual_gain_val/10.0));
	} else {
		Memo1->Lines->Add("Failed to set tuner gain");
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
			Memo1->Lines->Add("Failed to reset buffers.");
			return;
		}
	}
	config.read_samples=true;
	config.shutDown=false;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
config.read_samples=false;
config.shutDown = true;
}
//---------------------------------------------------------------------------

void readSamples(){
	rtlsdr_read_async(dev, async_read_callback, NULL, 0, n_read * n_read);
	if (config.shutDown) {
		rtlsdr_close(dev);
	}
	if(config.freq_update){
	   rtlsdr_set_center_freq(dev, config.center_frequency);
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
void signal_simulation(int frequency){
 uint8_t signal_buf[n_read*2];
 for (int i=0;i<n_read; i++) {
	signal_buf[i*2]=(uint8_t)(127.5*cos(2*M_PI*frequency*(1.0/config.sample_rate)*i)+128);
    signal_buf[i*2+1]=(uint8_t)(127.5*sin(2*M_PI*frequency*(1.0/config.sample_rate)*i)+128);
 }

// for (int i = 0; i < 512; i++) {
//   RealTimeAdd(Form1->Series1,signal_buf[i*2], i);
//  // RealTimeAdd(Form1->Series2,signal_buf[i*2+1], i);
// }
 create_fft(n_read, signal_buf);
}

void __fastcall TForm1::Button3Click(TObject *Sender)
{
	signal_simulation(100'000);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TrackBar1Change(TObject *Sender)
{
   Series1->Delete(0,n_read);
   Series2->Delete(0,n_read);
   TrackValue->Caption=TrackBar1->Position;
   signal_simulation(TrackBar1->Position);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
rtlsdr_read_sync(dev, config.buffer, config.out_block_size, &config.bytes_in_response);
Form1->Series1->Delete(0,n_read);
Form1->Series2->Delete(0,n_read);
   for (int i = 0; i < n_read; i++) {
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



