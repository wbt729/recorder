#include "sampler.h"

Sampler::Sampler() {
	int bufferSize = 100;
	roi = QRect();
	roiArea = 1000000;
	data = QVector<double>(bufferSize, 0);
	lengthHighPass = 0;
	lengthLowPass = 0;
}

Sampler::~Sampler() {
}

void Sampler::setMat(cv::Mat *mat) {
	cv::Mat buf = mat->clone();
	double meanBlue = 0;
	double meanGreen = 0;
	double meanRed = 0;
	int steps = 0;

	//set correct border of roi
	if(roi.right() > buf.cols) roi.setRight(buf.cols);
	if(roi.bottom() > buf.rows) roi.setBottom(buf.rows);
	if(roi.left() < 0) roi.setLeft(0);
	if(roi.top() < 0) roi.setTop(0);

	roiArea = (roi.width()-1)*(roi.height()-1);


	for(int y = roi.y(); (y < roi.bottom()); y++) {
		for(int x = roi.x(); (x < roi.right()); x++) {
			meanBlue += buf.at<unsigned short>(y, x*3);
			meanGreen += buf.at<unsigned short>(y, x*3+1);
			meanRed += buf.at<unsigned short>(y, x*3+2);
			steps++;
		}
	}

	meanGreen = meanGreen/roiArea;
	meanRed = meanRed/roiArea;
	meanBlue = meanBlue/roiArea;

	data.erase(data.begin());
	data << meanGreen;

	double tmp = 0;

	if(lengthLowPass != 0) {
		for(int i=0; i<lengthLowPass; i++) {
			tmp += data.at(data.size()-1-i);
		}
		meanGreen = tmp/lengthLowPass;
	}

	if(lengthHighPass != 0) {
		tmp = 0;
		for(int i=0; i<lengthHighPass; i++) {
			tmp += data.at(data.size()-1-i);
		}
		meanGreen -= tmp/lengthHighPass;
	}

	emit newSamples(meanRed, meanGreen, meanBlue);
}

void Sampler::setRoi(QRect r) {
	roi = r;
	qDebug() << roi.topLeft().x() << roi.topLeft().y();
	qDebug() << roi.bottomRight().x() << roi.bottomRight().y();
	//qDebug() << roiArea;
	qDebug() << "new ROI";
}

void Sampler::setLP(int l) {
	lengthLowPass = l;
}

void Sampler::setHP(int l) {
	lengthHighPass = l;
}