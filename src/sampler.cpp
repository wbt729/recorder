#include "sampler.h"

Sampler::Sampler() {

}

Sampler::~Sampler() {

}

void Sampler::setMat(cv::Mat *mat) {
	//qDebug() << "new mat";
	cv::Mat buf = mat->clone();
	double meanBlue = 0;
	double meanGreen = 0;
	double meanRed = 0;
	for(int y = 0; y < 100; y++) {
		for(int x = 0; x < 100; x++) {
			meanBlue += (double) buf.at<unsigned short>(y,x);
			meanGreen += (double) buf.at<unsigned short>(y,x);
			meanRed += (double) buf.at<unsigned short>(y,x);
		}
	}

	meanGreen = meanGreen/(10000);
	meanRed = meanRed/(10000);
	meanBlue = meanBlue/(10000);

	emit newSamples(meanRed, meanGreen, meanBlue);
}

void Sampler::setRoi(QRect roi) {
	;
}