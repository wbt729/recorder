#ifndef CONVERTER_H
#define CONVERTER_H

#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QVector>
#include <qimage.h>
#include <QtGui/QApplication>
#include <qdir.h>
#include "tiff.h"
#include "tiffio.h"

class Converter : public QObject
{
    Q_OBJECT

private:
	QImage image;
	int width;
	int height;
	int channels;
	int bitsPerSample;
	int bytesPerPixel;
	QVector<unsigned short> readSamples(unsigned char *);

public:
    Converter(int, int, int, QObject *parent = 0);
    ~Converter();
	void setResolution(int, int, int);

public slots:
	void charToQImage(char *);
	void blockToTIFFs();

signals:
	void newImage(QImage*);
};

#endif // CONVERTER_H
