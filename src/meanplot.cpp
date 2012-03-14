#include "meanplot.h"

MeanPlot::MeanPlot() {
	setTitle("rPPG");
	//int size=600;
	int size=350;
	for(int i=0;i<size;i++) {
	    	d_x.append((double) i);
	    	d_y.append(0);
	    }
	//setAutoReplot(true);
	setAxisAutoScale(QwtPlot::yLeft);
	QwtLinearScaleEngine *scaleEngine = new QwtLinearScaleEngine();
	scaleEngine->setReference(5);
	scaleEngine->setAttribute(QwtScaleEngine::Symmetric);
//	scaleEngine->autoScale(200, d_y[0], d_y[99], d_x[0]);
	setAxisScaleEngine(QwtPlot::yLeft, scaleEngine);
	setAxisTitle(QwtPlot::xBottom,"Zeit");
	setAxisTitle(QwtPlot::yLeft,"Amplitude");

	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::red);

	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setPen(pen);
	curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->setRawSamples(d_x.data(), d_y.data(), size);
    curve->attach(this);
}

MeanPlot::~MeanPlot() {
}

QSize MeanPlot::sizeHint() {
	return QSize(400,300);
}

QSize MeanPlot::minimumSizeHint() {
	return QSize(400,300);
}

void MeanPlot::updateData(double meanRed, double meanGreen, double meanBlue) {
	d_y.erase(d_y.begin());
	//meanGreen = (meanGreen + 4*d_y.last())/5;
	//meanGreen = meanGreen;
//	setAxisScale(QwtPlot::yLeft, (d_y.last()+meanGreen)/2-4, (d_y.last()+meanGreen)/2+4);
//	setAxisScale(QwtPlot::yLeft, -2, +2);
	d_y.append(meanGreen);

//	std::cout << "mean rec: " << meanGreen << std::endl;
	//setAxisScale(QwtPlot::yLeft, meanGreen-20, meanGreen+20);
	setAxisScale(QwtPlot::yLeft, -4, +4);
	//setAxisScale(QwtPlot::yLeft, 0, 1023);

	replot();
}
