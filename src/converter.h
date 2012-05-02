#ifndef CONVERTER_H
#define CONVERTER_H

#include "opencv2/opencv.hpp"
#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QVector>
#include <QTimer>
#include <QThread>
#include <qimage.h>
#include <QtGui/QApplication>
#include <qdir.h>
#include "tiff.h"
#include "tiffio.h"


class Converter : public QObject
{
    Q_OBJECT

private:
	int maxConversionRate;
	bool ready; //this limits the converstion rate
	bool convert;
	QImage image;
	cv::Mat mat;
	int width;
	int height;
	int channels;
	int bitsPerChannel;
	int bytesPerPixel;
	QVector<unsigned short> readSamples(unsigned char *);

private slots:
	void onTimer();

public:
    Converter();
    ~Converter();
	void setResolution(int width, int height, int channels, int bitsPerChannel, int bytesPerPixel);

public slots:
	void makePreview(char *);
	//void charToCvMat(char *);
	//void blockToTIFFs();

signals:
	void newImage(QImage*);
	void newMat(cv::Mat*);
	void convertingBlock(int, int);	//recent image, total number of images
};

#endif // CONVERTER_H
