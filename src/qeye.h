//Qt C++ uEye capture and recording class

//the actual state is confusing crap
//a rewrite is very neccessary

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


class QEye : public QObject {
	Q_OBJECT

public:
	QEye();
	~QEye();
	int init(QString fileName = "d:\\work\\ueye.ini", int ringBufferSize = 200, int id = 0);
	int exit();
	int setExposure(double);
	double getExposure();
	void enableQImageConversion(bool enable = true);
	void enableCVMatConversion(bool enable = true);
	int setColorMode(INT mode);
	static int countFreeCams();

public slots:
	int startCapturing();
	int stopCapturing();
	int startRecording();
	int stopRecording();

private:
	void makeConnections();
	int createRingBuffer(int);
	int getWidth();
	int getHeight();

	HIDS cam;
	Converter *converter;
	Grabber *grabber;
	int bytesPerPixel, bitsPerPixel, bitsPerChannel, colorChannels, width, height;
	int sizeRingBuffer;	//in elements
	int sizeImage;	//in bytes
	int imagesReceived;
	int maxConversionRate;
	bool recording, running;
	QThread *grabberThread;
	QThread *converterThread;

private slots:
	void onNewFrame();

signals:
	void ready();
	void frameToConvert(char *);
	void newImage(QImage *);
	void newMat(cv::Mat *);
};

#endif // QEYE_H


////Qt C++ uEye capture and recording class
//
////the actual state is confusing crap
////a rewrite is very neccessary
//
//#ifndef QEYE_H
//#define QEYE_H
//
//#include "opencv2/opencv.hpp"
//#include <QObject>
//#include <QDebug>
//#include <QImage>
//#include <QThread>
//#include <qfile.h>
//#include "uEye.h"
//#include "grabber.h"
//#include "storage.h"
//#include <qtimer.h>
//#include <converter.h>
//
//
//class QEye : public QObject {
//	Q_OBJECT
//
//public:
//	QEye(QObject *parent);
//	~QEye();
//	int init(int id = 0);
//	int loadParameters(QString);
//	int setColorMode(int);
//	int getWidth();
//	int getHeight();
//	int createBuffers(int);
//	bool isRunning();
//	bool isRecording();
//	int exit();
//	double getExposure();
//	void setExposure(double);
//	int setTrigger(bool);
//	int imagesReceived();
//	int errorsReceived();
//	int imagesRecorded();
//	static int countFreeCams();
//	void makeCvMat(bool);
//
//private:
//	//QFile logFile;
//	HIDS cam;
//	INT bitsPerSample;
//	INT channels;
//	INT bitsPerPixel;
//	INT bytesPerPixel;
//	IS_RECT AOIRect;
//	INT colorMode;
//	char** imageMemory;
//	INT* memoryID;
//	int bufferSize;
//	Grabber *grabber;
//	Converter *converter;
//	Storage *storage;
//	QThread *grabberThread;
//	QThread *storageThread;
//	QThread *converterThread;
//	bool isConverting;
//	bool makePreview;
//	bool running, recording;
//	bool processingImage;
//	QImage *image;
//	int maxPreviewFreq;
//	
//	QString filename;
//
//	int numImagesReceived;
//	int numImagesRecorded;
//	int numErrors;
//	int width;
//	int height;
//private slots:
//	void onConversionDone(QImage *);
//	void onPreviewTimer();
//public slots:
//	void startCapture();
//	void stopCapture();
//	void onNewFrame(char *);
//	void onErrors(int);
//	void convertBlock();
//	void startRecording();
//	void stopRecording();
//signals:
//	void converting(int, int);
//	void newImage(QImage *);
//	void newMat(cv::Mat *);
//	void starting();
//	void stopping();
//	void errors(int);
//	void newFrame(char *);
//	void linBufFull(char *, int);
//	void countersChanged(int, int, int); //images received, images recorded, errors
//};
//
//#endif // QEYE_H
