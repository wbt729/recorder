//cv::Mat goes in, samples come out

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
	void setLP(int);
	void setHP(int);

signals:
	void newSamples(double, double, double);

private:
	int lengthHighPass, lengthLowPass;
	QRect roi;
	int roiArea;
	QVector<double> data;	
	QVector<double> a; //filter coefficients
};

#endif // SAMPLER_H
