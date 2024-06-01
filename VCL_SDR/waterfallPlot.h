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
		void fillBitmap(double values[], size_t size);


	public:
		WaterfallPlot(TImage *image);
		~WaterfallPlot();
		void setDimensions(size_t width, size_t height);
		void addLine(double values[], size_t size);

};
#endif
