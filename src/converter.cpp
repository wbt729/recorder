#include "converter.h"

Converter::Converter() {
	maxConversionRate = 15;
	width = 0;
	height = 0;
	channels = 0;
	bitsPerChannel = 0;
	bytesPerPixel = 0;
	ready = true;
}


Converter::~Converter() {
}

//convert 10bit data to 8bit image for displaying
void Converter::makePreview(char *rawDat) {
	if(!ready) 
		return;
	else {
		convert = true;
		ready = false;		//start conversion, block new conversion requests for 1/maxConversionRate seconds
		QTimer::singleShot(1000/maxConversionRate, this, SLOT(onTimer()));
	}

	mat.create(height, width, CV_16UC3);

	//convert to unsigned char since only bitshift ops on _unsigned_ 
	//values fill with zeros
	unsigned char *rawData = (unsigned char *) rawDat;

	image = QImage(width, height, QImage::Format_RGB888);
	image.fill(0);
	
	QVector<unsigned short> samples = readSamples(rawData);
	//shift right by two to get 8 bit values
	if(bitsPerChannel == 10 && channels == 3) {
		for(int i=0; i<height; i++) {
			for(int j=0; j<width; j++) {
				image.setPixel(j,i,qRgb(samples[channels*(i*width+j)] >> 2,
										samples[channels*(i*width+j)+1] >> 2,
										samples[channels*(i*width+j)+2] >> 2));
			
				mat.at<unsigned short>(i, j*3) = samples[channels*(i*width+j)+2];
				mat.at<unsigned short>(i, j*3+1) = samples[channels*(i*width+j)+1];
				mat.at<unsigned short>(i, j*3+2) = samples[channels*(i*width+j)];
			}
		}
	}
	else if(bitsPerChannel == 8 && channels == 1) {
		for(int i=0; i<height; i++) {
			for(int j=0; j<width; j++) {
				image.setPixel(j,i,qRgb(samples[channels*(i*width+j)],
										samples[channels*(i*width+j)],
										samples[channels*(i*width+j)]));
			
				mat.at<unsigned short>(i, j*3) = samples[channels*(i*width+j)];
				mat.at<unsigned short>(i, j*3+1) = samples[channels*(i*width+j)];
				mat.at<unsigned short>(i, j*3+2) = samples[channels*(i*width+j)];
			}
		}
	}
	emit newImage(&image);
	emit newMat(&mat);
}

//returns a vector of unsigned shorts that contains the samples with 10 bit precision
QVector<unsigned short> Converter::readSamples(unsigned char *rawData) {
	QVector<unsigned short> samples = QVector<unsigned short>(width*height*channels, 0); //preallocate memory and fill with zeros
	unsigned short r, g, b, gy;
	r = 0; g = 0; b = 0; gy = 0;

	int offset = 0;

	if(bitsPerChannel == 10 && channels == 3) {

		for(int i=0; i<height; i++) {
			for(int j=0; j<width; j++) {
				r = 0; g = 0; b = 0;
				//bitwise magic
				//copies two bytes to short value in reverse order
				//due to endianess
				//10 bit value is determined by masking the bits that are not
				//part of the color channel and shifting the
				//remaining bits, so that 10 significant bits remain
				r = r | (rawData[offset+3] << 8);
				r = r | rawData[offset+2];
				r = (r & 0x3ff0) >> 4; //10 bit

				g = g | (rawData[offset+2] << 8);
				g = g | rawData[offset+1];
				g = (g & 0x0ffc) >> 2; //10 bit

				b = b | (rawData[offset+1] << 8);
				b = b | rawData[offset];
				b = (b & 0x03ff); //10 bit

	//			qDebug() << i << j << "offset" << offset;
	//			qDebug() << r << g << b;
				//samples << r << g << b;
				samples.replace(channels*(i*width+j), r);
				samples.replace(channels*(i*width+j)+1, g);
				samples.replace(channels*(i*width+j)+2, b);
				offset += 4;
			}
		}
	}
	else if(bitsPerChannel == 8 && channels == 1) {
		for(int i=0; i<height; i++) {
			for(int j=0; j<width; j++) {
				gy = 0;
				gy = rawData[i*width+j];
				samples.replace(channels*(i*width+j), gy);
			}
		}
	}
	return samples;
}

void Converter::setResolution(int w, int h, int c, int bps, int bpp) {
	width = w;
	height = h;
	channels = c;
	bitsPerChannel = bps;
	bytesPerPixel = bpp;
}

void Converter::onTimer() {
	ready = true;
}