#ifndef PLOT_H_
#define PLOT_H_

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_scale_engine.h>
#include <QTimer>
#include <iostream>
#include <QWheelEvent>

class MeanPlot : public QwtPlot {
	Q_OBJECT
public:
	MeanPlot();
	virtual ~MeanPlot();
private:
	bool floating;
    QwtPlot *plot;
    QwtPlotDirectPainter *painter;
    QwtPlotCurve *curve;
    QVector<double> d_x;
    QVector<double> d_y;
    QwtLinearScaleEngine *scaleEngine;
	virtual QSize sizeHint();
	virtual QSize minimumSizeHint();
	int scale;

public slots:
	void updateData(double, double, double);
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void wheelEvent(QWheelEvent *);
};

#endif /* PLOT_H_ */
