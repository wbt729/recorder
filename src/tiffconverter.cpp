#include "tiffconverter.h"

TiffConverter::TiffConverter(QObject *parent)
	: QObject(parent)
{
	width = 640;
	height = 512;
	//width = 320;
	//height = 240;
	samplesPerPixel = 3;
	bitsPerSample = 10;
}

TiffConverter::~TiffConverter() {

}

//void TiffConverter::convert() {
//	qDebug() << "convert";
//
//	QString filename("d:\\work\\dump");
//	QFile file(filename);
//	file.open(QIODevice::ReadOnly);
//	QByteArray dumpData;
//	dumpData = file.readAll();
//	file.close();
//	char *image=dumpData.data();
//
//	for(int i=0; i<5; i++) {
//		QString outFilename(tr("d:\\work\\img\\%1.tif").arg(i));
//
//		//TIFF *out=TIFFOpen("d:\\work\\img\\new.tif","w");
//		TIFF *out=TIFFOpen(outFilename.toLatin1(),"w");
//
//		TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
//		TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
//		TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
//		TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bitsPerSample);
//		TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
//		TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
//
//		tsize_t linebytes = samplesPerPixel*width;
//		unsigned char *buf=NULL;
//
//		if (TIFFScanlineSize(out) == linebytes)
//			buf=(unsigned char*)_TIFFmalloc(linebytes);
//		else
//			buf=(unsigned char*)_TIFFmalloc(TIFFScanlineSize(out));
//		TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, width*samplesPerPixel));
//
//		for(int row=0; row<height; row++) {
//			memcpy(buf, &image[row*linebytes+i*width*height*samplesPerPixel*bitsPerSample/8], linebytes);
//			qDebug() << "writing row" << row;
//			if(TIFFWriteScanline(out, buf, row, 0)<0) {
//				qDebug() << "something went wrong during writing the tiff";
//			}
//		}
//		TIFFClose(out);
//	}
//}

void TiffConverter::convert() {
	qDebug() << "convert";

	QString filename("d:\\work\\dump");
	QFile file(filename);
	file.open(QIODevice::ReadOnly);
	QByteArray dumpData;
	dumpData = file.readAll();
	file.close();
	char *image=dumpData.data();

	////reverse bits
	//for(int i=0; i<dumpData.size(); i++) {
	//	unsigned char b = (unsigned char) image[i];
	//	qDebug() << b;
	//	b = ((b * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32;
	//	//qDebug() << b;
	//	image[i] = (char) b;
	//	qDebug() << (unsigned char) image[i];
	//}

	for(int i=0; i<1; i++) {
		QString outFilename(tr("d:\\work\\img\\%1.tif").arg(i));

		//TIFF *out=TIFFOpen("d:\\work\\img\\new.tif","w");
		TIFF *out=TIFFOpen(outFilename.toLatin1(),"w");

		TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
		TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
		TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
		TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bitsPerSample);
		TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

		tsize_t linebytes = TIFFScanlineSize(out);
		unsigned char *buf=NULL;
		buf=(unsigned char*)_TIFFmalloc(linebytes);

		//int a = (int) TIFFScanlineSize(out);

		//if (TIFFScanlineSize(out) == linebytes)
		//	buf=(unsigned char*)_TIFFmalloc(linebytes);
		//else
		//	buf=(unsigned char*)_TIFFmalloc(TIFFScanlineSize(out));
		TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, width*samplesPerPixel));
		
		QFile file("d:\\work\\textdump.txt");
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream *str = new QTextStream(&file);

		for(int row=0; row<height; row++) {
			char *newLine = convertLine(image, width, row, linebytes, str);
			memcpy(buf, newLine, linebytes);
			qDebug() << "writing row" << row;
			if(TIFFWriteScanline(out, buf, row, 0)<0) {
				qDebug() << "something went wrong during writing the tiff";
			}
		}
		TIFFClose(out);
		file.close();
	}
}

char* TiffConverter::convertLine(char* image, int width, int rowNum, tsize_t linebytes, QTextStream *str) {
	int lbytes = (int) (int32) linebytes;
	QVector<uint16> samples;
	char* line = new char[linebytes+5];
	memset(line, 0, linebytes);
	int offset = rowNum*width;



	////read one line
	//for(int i=0; i<width; i++) {
	//	uint32 block = 0;
	//	block += (uint8) image[offset+i*4] << 24;
	//	block += (uint8) image[offset+i*4+1] << 16;
	//	block += (uint8) image[offset+i*4+2] << 8;
	//	block += (uint8) image[offset+i*4+3];
	//	////qDebug() << block;
	//	uint16 r = (uint16) ((block & 0xffc00000) >> 22);
	//	//uint16 r = (uint16) ((block & 0xffc00000));
	//	uint16 g = (uint16) ((block & 0x003ff000) >> 12);
	//	uint16 b = (uint16) ((block & 0x00000ffc) >> 2);
	//	//uint16 b = 0x00000011;
	//	//qDebug() << "reading samples" << "r" << r << "g" << g << "b" << b;

	//	samples << r << g << b;
	//	//if(i<width/2)
	//	//	samples << 0 << 0 << 1023;
	//	//else
	//	//	samples << 1023 << 0 << 0;
	//}
	int j=0;
	for(int i=0; i<width; i++) {
		uint32 block = 0;
		//block += (uint8) image[offset+4*i+3] << 24;
		block += (uint8) image[offset+4*i+3] << 24;
		block += (uint8) image[offset+4*i+2] << 16;
		block += (uint8) image[offset+4*i+1] << 8;
		block += (uint8) image[offset+4*i];
		//qDebug() << block;

		uint16 r = (uint16) ((block & 0x3ff00000) >> 20);
		uint16 g = (uint16) ((block & 0x000ffc00) >> 10);
		uint16 b = (uint16) ((block & 0x000003ff) >> 0);

		samples << r << g << b;

		str->operator<<(tr("%1 ").arg(r));

		//if(i<10 && rowNum == 10) samples << 500 << 0 << 0;
		//else samples << 0 << 0 << 0;

		j++;
	}
	str->operator<<("\n");

	//qDebug() << samples.size();

	//write one line
	uchar buf[5];
	uchar sBuf[2];
	uint16 s;
	int linePos = 0;
	for(int i=0; i<samples.size(); i+=4) {
		memset(&buf, 0, 5);
		for(int j=0; j<4; j++) {
			s = samples[i+j];
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
			line[linePos+k] = buf[k];
		}
		linePos += 5;
	}

	return line;
}