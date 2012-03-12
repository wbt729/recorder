#ifndef SAMPLER_H
#define SAMPLER_H

#include <QObject>
#include "opencv2/opencv.hpp"
#include <qrect.h>
#include <qdebug.h>
#include <QTime>

class Sampler : public QObject
{
	Q_OBJECT

public:
	Sampler();
	~Sampler();

public slots:
	void setMat(cv::Mat *);
	void setRoi(QRect);

signals:
	void newSamples(double, double, double);

private:
	QTime *t;
	QRect roi;
	int roiArea;
	QVector<QVector<double> > data;	
	QVector<double> a; //filter coefficients
	void filter();
};

#endif // SAMPLER_H
