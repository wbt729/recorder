#include "sampler.h"

Sampler::Sampler() {
	int bufferSize = 100;
	roi = QRect();
	roiArea = 1000000;
	data = QVector<double>(bufferSize, 0);
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

	//qDebug() << steps << roiArea;
	meanGreen = meanGreen/roiArea;
	meanRed = meanRed/roiArea;
	meanBlue = meanBlue/roiArea;

	data.erase(data.begin());
	data << meanGreen;

	int length = 15;
	double tmp = 0;
	for(int i=0; i<length; i++) {
		tmp += data.at(data.size()-1-i);
	}
	
	meanGreen = tmp/length;
	//data.replace(data.size()-1, meanGreen);

	//length = 10;
	//tmp = 0;
	//for(int i=0; i<length; i++) {
	//	tmp += data.at(data.size()-1-i);
	//}

	//meanGreen -= tmp/length;

	//tmp = tmp - data.at(data.size()-2);

	
	//meanGreen -= tmp/length;

	//data.erase(data.begin());
	//data << meanGreen;

	emit newSamples(meanRed, meanGreen, meanBlue);
}

void Sampler::setRoi(QRect r) {
	roi = r;
	qDebug() << roi.topLeft().x() << roi.topLeft().y();
	qDebug() << roi.bottomRight().x() << roi.bottomRight().y();
	//qDebug() << roiArea;
	qDebug() << "new ROI";
}