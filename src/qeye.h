//Qt C++ uEye capture and recording class


#ifndef QEYE_H
#define QEYE_H

//#include "opencv2/opencv.hpp"
#include <QObject>
#include <QDebug>
#include <QImage>
#include <QThread>
#include <qfile.h>
#include "uEye.h"
#include "grabber.h"
#include "storage.h"
#include <qtimer.h>
#include <converter.h>

typedef QVector<unsigned long long> ULongLongVector;



class QEye : public QObject {
	Q_OBJECT

public:
	QEye();
	~QEye();
	int init(QString fileName = "/cam/set1", int ringBufferSize = 400, int linBufferSize = 100, int id = 0);
	int exit();
	int setExposure(double);
	double getExposure();
	void enableQImageConversion(bool enable = true);
	void enableCVMatConversion(bool enable = true);
	int setColorMode();
	int getColorMode();
	static int countFreeCams();

public slots:
	int startCapturing();
	int stopCapturing();
	int startRecording();
	int stopRecording();
	int setExternalTrigger(bool);
	int convertBlock();

private:
	void makeConnections();
	int createRingBuffer(int);
	int getWidth();
	int getHeight();

	HIDS cam;
	Converter *converter;
	Grabber *grabber;
	Storage *storage;
	char *linBuffer;
	bool useFirstLinBuffer;
	int linBufferIndex;
	int bytesPerFrame, bytesPerPixel, bitsPerPixel, bitsPerChannel, colorChannels, width, height;
	int sizeRingBuffer, sizeLinBuffer;	//image size in bytes, ring and lin buffer size in elements (n*sizeImage)
	int numImagesReceived;
	int numImagesRecorded;
	int numErrors;
	//QVector<int> timestamps;
	ULongLongVector timestamps;

	int maxConversionRate;
	bool recording, running;
	QThread *grabberThread, *storageThread, *converterThread;

private slots:
	void onNewFrame();
	void onErrors(int);

signals:
	void ready();
	void frameToConvert(char *);
	void newImage(QImage *);
	void newMat(cv::Mat *);
	void linBufferFull(char *, int, int, ULongLongVector);
	void convertingBlock(int, int);
	void transferCountersChanged(int, int, int);	//received, errors, recorded
};

#endif // QEYE_H