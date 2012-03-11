#ifndef SAMPLER_H
#define SAMPLER_H

#include <QObject>
#include "opencv2/opencv.hpp"
#include <qrect.h>
#include <qdebug.h>

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
	
};

#endif // SAMPLER_H
