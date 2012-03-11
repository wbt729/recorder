#ifndef CONVERTER_H
#define CONVERTER_H

#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QVector>
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
	QThread *qimageThread;
	QThread *cvMatThread;
	QImage image;
	int width;
	int height;
	int channels;
	int bitsPerSample;
	int bytesPerPixel;
	QVector<unsigned short> readSamples(unsigned char *);

public:
    Converter(QObject *parent = 0);
    ~Converter();
	void setResolution(int, int, int, int, int);

public slots:
	void charToQImage(char *);
	void blockToTIFFs();

signals:
	void newImage(QImage*);
	void converting(int, int);	//recent image, total number of images
};

#endif // CONVERTER_H
