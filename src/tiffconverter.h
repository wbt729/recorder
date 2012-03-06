#ifndef TIFFCONVERTER_H
#define TIFFCONVERTER_H

#include <QObject>
#include <qfile.h>
#include <QDebug>
#include <qtextstream.h>
//extern "C" {
#include "tiff.h"
#include "tiffio.h"
//}

class TiffConverter : public QObject
{
	Q_OBJECT

public:
	TiffConverter(QObject *parent);
	~TiffConverter();

public slots:
	char* convertLine(char*, int, int, tsize_t, QTextStream*);
	void convert();

private:
	int width;
	int height;
	int samplesPerPixel;
	int bitsPerSample;
};

#endif // TIFFCONVERTER_H
