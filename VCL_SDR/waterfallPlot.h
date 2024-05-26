//---------------------------------------------------------------------------

#ifndef waterfallPlotH
#define waterfallPlotH
//---------------------------------------------------------------------------
class WaterfallPlot{
	private:
		Graphics::TBitmap*  bitMap1;
		Graphics::TBitmap*  bitMap2;
		TImage *image;
		int linePointer=0;
		void fillBitmap(double values[], int size);


	public:
		WaterfallPlot(TImage *image);
		~WaterfallPlot();
		void setDimensions(int width, int height);
		void addLine(double values[], int size);

};
#endif
