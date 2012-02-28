#include "converter.h"

Converter::Converter(int w, int h, int c, QObject *parent)
    : QObject(parent) {
	width = w;
	height = h;
	channels = c;
	bitsPerSample = 10;
	bytesPerPixel = 4;
}


Converter::~Converter() {
}

//convert 10bit data to 8bit image for displaying
void Converter::charToQImage(char *rawDat) {
	//convert to unsigned char since only bitshift ops on unsigned values fill with zeros
	unsigned char *rawData = (unsigned char *) rawDat;

	image = QImage(width, height, QImage::Format_RGB888);
	image.fill(0);
	
	QVector<unsigned short> samples = readSamples(rawData);
	//sift right by two to get 8 bit values
	for(int i=0; i<height; i++) {
		for(int j=0; j<width; j++) {
			image.setPixel(j,i,qRgb(samples[channels*(i*width+j)] >> 2,
									samples[channels*(i*width+j)+1] >> 2,
									samples[channels*(i*width+j)+2] >> 2));
		}
	}
	emit newImage(&image);
}

void Converter::setResolution(int w, int h, int c) {
	width = w;
	height = h;
	channels = c;
}



//returns a vector of unsigned shorts that contains the samples with 10 bit precision
QVector<unsigned short> Converter::readSamples(unsigned char *rawData) {
	QVector<unsigned short> samples = QVector<unsigned short>(width*height*channels, 0); //preallocate memory and fill with zeros
	unsigned short r, g, b;
	r = 0; g = 0; b = 0;

	int offset = 0;

	for(int i=0; i<height; i++) {
		for(int j=0; j<width; j++) {
			r = 0; g = 0; b = 0;
			//bitwise magic
			//copies two bytes to short value in reverse order
			//10 bit value by masking the bits that are not
			//part of the color channel and shifting the
			//remaining bits, that 10 significant bits remain
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
	return samples;
}

void Converter::blockToTIFFs() {
	qDebug() << "converting binary dump to TIFFs";
	QFile file("dump");
	if(!file.open(QIODevice::ReadOnly)) {
		qDebug() << "cannot open file";
		return;
	}

	unsigned char *rawData = new unsigned char[width*height*bytesPerPixel];
	QDataStream in(&file);

	//iterate through data dump
	for(int l = 0; l < file.size()/(width*height*bytesPerPixel); l++) {
		qDebug() << "converting image" << l;
		/*qDebug() << "reading raw data from block" << */
		in.readRawData((char*) rawData, width*height*bytesPerPixel);
		QVector<unsigned short> samples = readSamples(rawData);

		QString filenameTIFF = QString(tr("d:\\work\\img\\%1.tiff").arg(l));
		TIFF *out=TIFFOpen(filenameTIFF.toLatin1(),"w");

		TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
		TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
		TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, channels);
		TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bitsPerSample);
		TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

		tsize_t linebytes = TIFFScanlineSize(out);
		unsigned char *buf=NULL;
		buf=(unsigned char*)_TIFFmalloc(linebytes);

		TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, width*channels));
		
		char *newLine = new char[linebytes+10];
		for(int row=0; row<height; row++) {

			uchar buf[5];
			uchar sBuf[2];
			unsigned short s;
			int linePos = 0;
			for(int i=0; i<channels*width; i+=4) {
				//qDebug() << i;
				memset(&buf, 0, 5);
				for(int j=0; j<4; j++) {
					s = samples[row*width*channels+i+j];
					switch(j) {
						case 0:
							s = s << 6;
							sBuf[0] = (char) (s >> 8);
							sBuf[1] = (char) s;
							buf[0] = buf[0] | sBuf[0];
							buf[1] = buf[1] | sBuf[1];
							break;
						case 1:
							s = s << 4;
							sBuf[0] = (char) (s >> 8);
							sBuf[1] = (char) s;
							buf[1] = buf[1] | sBuf[0];
							buf[2] = buf[2] | sBuf[1];
							break;
						case 2:
							s = s << 2;
							sBuf[0] = (char) (s >> 8);
							sBuf[1] = (char) s;
							buf[2] = buf[2] | sBuf[0];
							buf[3] = buf[3] | sBuf[1];
							break;
						case 3:
							sBuf[0] = (char) (s >> 8);
							sBuf[1] = (char) s;
							buf[3] = buf[3] | sBuf[0];
							buf[4] = buf[4] | sBuf[1];
							break;
					}
				}
				for(int k=0; k<5; k++) {
					newLine[linePos+k] = buf[k];
				}
				linePos += 5;
			}
			//memcpy(buf, newLine, linebytes);
			//qDebug() << "writing row" << row;
			//if(TIFFWriteScanline(out, buf, row, 0)<0) {
			if(TIFFWriteScanline(out, newLine, row, 0)<0) {
				qDebug() << "something went wrong during writing the tiff";
			return;
			}

		}
		TIFFClose(out);
	}
	file.close();
}