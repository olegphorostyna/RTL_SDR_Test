//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <sstream>
#include <complex>
#include <vector>
#include <cmath>


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
static float amp, db; /*!< Amplitude & dB */
int _num_read = 50;

std::vector<std::complex<double>> out_val;

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
  Chart1->LeftAxis->SetMinMax(0,260);
  Chart1->BottomAxis->SetMinMax(1,chartConf.MaxPoints);

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


}

//void RealTimeAdd(TChartSeries *Series, float YValue ){
// float XValue;
//	if(Series->Count() == 0){
//	  XValue=1;}
//	  else{
//	   XValue=Series->XValues->Last()+1;
//	  }
//
//	Series->AddXY(XValue,YValue);
//}

void RealTimeAdd(TChartSeries *Series, float YValue, int xValue){
	Series->AddXY(xValue,YValue);
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
	 * Sample is 127 for zero signal, so substract ~127.34 for exact value.
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
//	using namespace std::complex_literals;
//		out_val.push_back((buf[n]-127.34) + (buf[n+1]-127.34) * 1i);
		in[i][0] = ((buf[i*2]-127.34));
		in[i][1] = ((buf[i*2+1]-127.34));
		//in[i][0] = ((buf[i*2]-127.4)/128);
		//in[i][1] = ((buf[i*2+1]-127.4)/128);
	}
	/**!
	 * Convert the complex samples to complex frequency domain.
	 * Compute FFT.
	 */
	fftw_execute(fftwp);
	Form1->Series1->Delete(0,sample_c);
	Application->ProcessMessages();
	for (int i=0; i < sample_c; i+=2){
	   out_r = out[i][0] * out[i][0];
	   //Memo1->Lines->Add(out_r);
	   out_i = out[i][1] * out[i][1];
	   //Memo1->Lines->Add("Failed to set tuner gain");
	   amp = sqrt(out_r + out_i);
	   db = 10 * log10(amp);
	   RealTimeAdd(Form1->Series1, db, i);
	   //RealTimeAdd(Form1->Series1, out_r, i);
	}

//	if(!_cont_read && _use_gnuplot)
//		log_info("Creating FFT graph from samples using gnuplot...\n");
//	else if (!_cont_read && !_use_gnuplot)
//		log_info("Reading samples...\n");
//	if(_use_gnuplot)
//		gnuplot_exec("plot '-' smooth frequency with linespoints lt -1 notitle\n");
//	for (int i=0; i < sample_c; i++){
//		/**!
//		 * Compute magnitude from complex values. [Sqr(Re^2 + Im^2)]
//		 * Compute amplitude (dB) from magnitude. [10 * Log(magnitude)]
//		 *
//		 * TODO #5: Check correctness of this calculation.
//		 */
//		out_r = creal(out[i]) * creal(out[i]);
//		out_i = cimag(out[i]) * cimag(out[i]);
//		amp = sqrt(out_r + out_i);
//		if (!_mag_graph)
//			db = 10 * log10(amp);
//		else
//			db = amp;
//		if(_write_file)
//			fprintf(file, "%d	%f\n", i+1, db);
//		if(_use_gnuplot)
//			gnuplot_exec("%d	%f\n", db, i+1);
//		/**!
//		 * Fill sample_bin with ID and values.
//		 *
//		 * NOTE: sample_bin is not used anywhere.
//		 *
//		 * TODO #2: Find the maximum value of samples, show it on graph
//		 * with a different color.  Might be useful for frequency scanner.
//		 * If you want to sort values see qsort function.
//		 * Example code: qsort(sample_bin, n_read, sizeof(Bin), cmp_sample);
//		 */
//		sample_bin[i].id = i;
//		sample_bin[i].val = db;
//	}
//	if(_use_gnuplot){
//		/**!
//		 * Stop giving points to gnuplot with 'e' command.
//		 * Have to flush the output buffer for [read -> graph] persistence.
//		 */
//		gnuplot_exec("e\n");
//		fflush(gnuplotPipe);
//	}
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
	if(!_num_read){
	rtlsdr_cancel_async(dev);
   }else {
  _num_read--;
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
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
rtlsdr_cancel_async(dev);
rtlsdr_close(dev);
dev=NULL;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
	/*!
 * Read samples from the device asynchronously. This function will block until
 * it is being canceled using rtlsdr_cancel_async()
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param cb callback function to return received samples
 * \param ctx user specific context to pass via the callback function
 * \param buf_num optional buffer count, buf_num * buf_len = overall buffer size
 *		  set to 0 for default buffer count (15)
 * \param buf_len optional buffer length, must be multiple of 512,
 *		  should be a multiple of 16384 (URB size), set to 0
 *		  for default buffer length (16 * 32 * 512)
 * \return 0 on success
 */
rtlsdr_read_async(dev, async_read_callback, NULL, 0, n_read * n_read);
}
//---------------------------------------------------------------------------
void signal_simulation(int frequency){
 uint8_t signal_buf[1024];
 for (int i=0;i<n_read; i++) {
   signal_buf[i*2]=(uint8_t)(127*sin(2*M_PI*frequency*(1.0/config.sample_rate)*i)+128);
   signal_buf[i*2+1]=(uint8_t)(127*cos(2*M_PI*frequency*(1.0/config.sample_rate)*i)+128);
 }

// for (int i = 0; i < 512; i++) {
//   RealTimeAdd(Form1->Series1,signal_buf[i*2], i);
// }
 create_fft(n_read, signal_buf);
}

void __fastcall TForm1::Button3Click(TObject *Sender)
{
	signal_simulation(1'000'00);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TrackBar1Change(TObject *Sender)
{
   Series1->Delete(0,512);
   TrackValue->Caption=TrackBar1->Position;
   signal_simulation(TrackBar1->Position);
}
//---------------------------------------------------------------------------

