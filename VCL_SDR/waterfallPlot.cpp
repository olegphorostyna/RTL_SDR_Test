//---------------------------------------------------------------------------

#pragma hdrstop

#include "waterfallPlot.h"
//---------------------------------------------------------------------------
WaterfallPlot::WaterfallPlot(TImage *image){
	 this->image =image;
	 bitMap1=new Graphics::TBitmap;
	 bitMap2=new Graphics::TBitmap;
	 bitMap1->PixelFormat=TPixelFormat::pf24bit;
	 this->setDimensions(image->Width, image->Height);
	  //test
	 //bitMap1->Canvas->Ellipse(0,0,150,150);
	 //image->Canvas->Draw(-20,-20,bitMap1);
	 randomize();
	//BYTE *ptr = (BYTE*)bitMap1->ScanLine[bitMap1->Height-1];
//	BYTE *ptr = (BYTE*)bitMap1->ScanLine[bitMap1->Height-100];
	//for (int x = 0; x < bitMap1->Width; x++){
	  //	ptr[x*3]=random(0xff);
	  //	ptr[x*3+1]=random(0xff);
	  //	ptr[x*3+2]=random(0xff);
   //	}
	bitMap2->Canvas->Brush->Color=clGrayText;
	bitMap2->Canvas->Brush->Style=bsSolid;
	bitMap2->Canvas->FillRect(TRect(0,0,bitMap2->Width,bitMap2->Height));
	image->Canvas->Draw(0,0,bitMap2);

   //	image->Canvas->Draw(0,0,bitMap1);


}
WaterfallPlot::~WaterfallPlot(){
	 delete bitMap1;
	 delete bitMap2;

}
void WaterfallPlot::setDimensions(size_t width, size_t height){
	 bitMap1->Width=width;
	 bitMap2->Width=width;
	 bitMap1->Height=height;
	 bitMap2->Height=height;
}

void WaterfallPlot::addLine(double values[], size_t size){
	fillBitmap(values, size);
   //	HRGN MyRgn;
	//MyRgn = ::CreateRectRgn(0,0,image->Width,linePointer);
	//::SelectClipRgn(image->Canvas->Handle,MyRgn);
	//image->Canvas->Draw(0,linePointer-image->Height,bitMap1);
	//image->Canvas->Draw(0,-250,bitMap1);
	linePointer=(++linePointer)%(image->Height);
	//::DeleteObject(MyRgn);
	//image->Invalidate();
	image->Canvas->Draw(0,0,bitMap1);
}

void WaterfallPlot::fillBitmap(double values[], size_t size){
	//BYTE *ptr = (BYTE*)bitMap1->ScanLine[bitMap1->Height-1];
	int step =bitMap1->Width/size;
	BYTE *ptr = (BYTE*)bitMap1->ScanLine[linePointer];
	for (int x = 0; x < size; x++){
			BYTE rand = (values[x])/128*255;
//            BYTE rand = random(256);
		for(int j=0;j<step;j++){
			ptr[x*step*3+j*3]=rand;
			ptr[x*step*3+j*3+1]=rand;
			ptr[x*step*3+j*3+2]=rand;
		}
	}
}
#pragma package(smart_init)
