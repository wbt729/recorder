#ifndef MEANPLOTWIDGET_H
#define MEANPLOTWIDGET_H

#include <QWidget>
#include "meanplot.h"
#include <qspinbox.h>
#include <qgridlayout.h>
#include <sampler.h>
#include <qlabel.h>

class MeanPlotWidget : public QWidget
{
	Q_OBJECT

public:
	MeanPlotWidget(QWidget *parent = 0);
	~MeanPlotWidget();

private:
	MeanPlot *plot;
	QSpinBox *hpSpinBox;
	QSpinBox *lpSpinBox;
	Sampler *sampler;

public slots:
	void newMat(cv::Mat *);
	void newRoi(QRect);
protected:
	virtual void closeEvent(QCloseEvent *);
signals:
	void closing();
};

#endif // MEANPLOTWIDGET_H
