//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <VCLTee.Chart.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.Series.hpp>
#include "./include/rtl-sdr.h"
#include "./include/fftw3.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.NumberBox.hpp>
#include "waterfallPlot.h"
#include <Vcl.MPlayer.hpp>
//---------------------------------------------------------------------------
extern rtlsdr_dev_t *dev;

class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TButton *StartRTLSDR;
	TButton *Button2;
	TMemo *Memo1;
	TChart *Chart1;
	TFastLineSeries *Series1;
	TLabel *Label1;
	TButton *Button1;
	TButton *Button3;
	TTrackBar *TrackBar1;
	TLabel *TrackValue;
	TFastLineSeries *Series2;
	TButton *Button4;
	TNumberBox *NumberBox1;
	TLabel *avg_power;
	TButton *Button5;
	TImage *Image1;
	TLabel *Label2;
	TTrackBar *TrackBar2;
	TLabel *Label3;
	TPanel *infoPanel;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TPanel *Panel2;
	TLabel *infoDevName;
	TLabel *infoSampleRate;
	TLabel *infoFFTSize;
	TLabel *infoGain;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall StartRTLSDRClick(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall TrackBar1Change(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall NumberBox1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);


private:	// User declarations

public:		// User declarations
					WaterfallPlot *plot;
					TMediaPlayer* Mp;
	__fastcall TForm1(TComponent* Owner);
};

class DeviceConfig{
  public:
   int gains[100]; //avaliable tuner gains in tenths of a dB, 115 means 11.5 dB
   int gain_n;
   uint8_t *buffer;
   uint32_t out_block_size = 16 * 16384;
			int bytes_in_response;

   const int n_read = 512; /*!< Sample count & data points & FFT size */

   DeviceConfig();
   ~DeviceConfig();


   int center_frequency = 100'000'000;
   int leftBound = (center_frequency-1'024'000)/1'000;
   int rightBound = (center_frequency+1'024'000)/1'000;
   int freq_step = 2'048'000/n_read/1'000;

   bool avg_center_power=0;

   uint32_t sample_rate = 512*4000;//2 048 000
   bool freq_update = false;
   bool shutDown= false;
   bool read_samples=true;
			bool trashold_mode=false;
   bool isSimulation=false;

   void setCenterFrequency(rtlsdr_dev_t *dev){
	   rtlsdr_set_center_freq(dev, this->center_frequency);
	   leftBound=(center_frequency-1'024'000)/1'000;
	   rightBound=(center_frequency+1'024'000)/1'000;
   }
};

DeviceConfig::DeviceConfig(){
	buffer = static_cast<uint8_t *>(malloc(out_block_size * sizeof(uint8_t)));
}
DeviceConfig::~DeviceConfig(){
	free (buffer);
}

//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
