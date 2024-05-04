//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
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
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall StartRTLSDRClick(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall TrackBar1Change(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};

class DeviceConfig{
  public:
   int gains[100]; //avaliable tuner gains in tenths of a dB, 115 means 11.5 dB
   uint8_t *buffer;
   uint32_t out_block_size = 16 * 16384;
   int bytes_in_response;

   DeviceConfig();
   ~DeviceConfig();

   //uint32_t center_frequency = 433920000;
   uint32_t center_frequency = 101'700'000;
   uint32_t sample_rate = 512*4000;//2 048 000
};

DeviceConfig::DeviceConfig(){
	buffer = static_cast<uint8_t *>(malloc(out_block_size * sizeof(uint8_t)));
}
DeviceConfig::~DeviceConfig(){
	free (buffer);
}

class ChartConfig{
 public:
   // Number of points we'll be displaying
  int MaxPoints=512;
  // Number of points deleted when scrolling chart
  int ScrollPoints = 5000;
  bool Stopped = true;
};




//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
