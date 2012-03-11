#ifndef PLOT_H_
#define PLOT_H_

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_scale_engine.h>
#include <QTimer>
#include <iostream>

class MeanPlot : public QwtPlot {
	Q_OBJECT
public:
	MeanPlot();
	virtual ~MeanPlot();
private:
    QwtPlot *plot;
    QwtPlotDirectPainter *painter;
    QwtPlotCurve *curve;
    QVector<double> d_x;
    QVector<double> d_y;
    QwtLinearScaleEngine *scaleEngine;
	virtual QSize sizeHint();
	virtual QSize minimumSizeHint();

public slots:
	void updateData(double, double, double);
};

#endif /* PLOT_H_ */
