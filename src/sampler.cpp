#include "sampler.h"

Sampler::Sampler() {
	int bufferSize = 100;
	roi = QRect();
	roiArea = 1000000;
	data = QVector<double>(bufferSize, 0);

	a << -0.0039 <<  -0.0104 <<  -0.0099  <<  0.0075  <<  0.0308  <<  0.0273 <<  -0.0187 <<  -0.0649 <<  -0.0318  <<  0.1126  <<  0.2925  <<  0.3746  <<  0.2925
    << 0.1126 <<  -0.0318 <<  -0.0649 <<  -0.0187  <<  0.0273  <<  0.0308  <<  0.0075 <<  -0.0099 <<  -0.0104 <<  -0.0039;
}

Sampler::~Sampler() {

}

void Sampler::setMat(cv::Mat *mat) {
	cv::Mat buf = mat->clone();
	double meanBlue = 0;
	double meanGreen = 0;
	double meanRed = 0;
	int steps = 0;
	for(int y = roi.y(); y < roi.bottom()+1; y++) {
		for(int x = roi.x(); x < roi.right()+1; x++) {
			meanBlue += buf.at<unsigned short>(y, x*3);
			meanGreen += buf.at<unsigned short>(y, x*3+1);
			meanRed += buf.at<unsigned short>(y, x*3+2);
			steps++;
		}
	}

	//qDebug() << steps << roiArea;
	meanGreen = meanGreen/roiArea;
	meanRed = meanRed/roiArea;
	meanBlue = meanBlue/roiArea;

	data.erase(data.begin());
	data << meanGreen;
	//qDebug() << meanGreen;

	int length = 8;
	double tmp = 0;
	for(int i=0; i<length; i++) {
		tmp += data.at(data.size()-1-i);
	}
	meanGreen = tmp/length;
	//meanGreen = meanGreen - data.at
	//filter();

	emit newSamples(meanRed, meanGreen, meanBlue);
}

void Sampler::setRoi(QRect r) {
	roi = r;
	roiArea = roi.width()*roi.height();
	qDebug() << roi.topLeft().x() << roi.topLeft().y();
	qDebug() << roi.bottomRight().x() << roi.bottomRight().y();
	qDebug() << roiArea;
	qDebug() << "new ROI";
}

void Sampler::filter() {
	double tmp = 0;
	for(int i=0; i<a.size(); i++) {
		tmp += a.at(i)*data.at(data.size()-i-1);
	}
	data.replace(data.size()-1,tmp);
}