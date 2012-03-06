#ifndef QEYE_H
#define QEYE_H

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
	QEye(QObject *parent);
	~QEye();
	int init(int id = 0);
	int loadParameters(QString);
	int setColorMode(int);
	int getWidth();
	int getHeight();
	int createBuffers(int);
	bool isRunning();
	bool isRecording();
	int exit();
	double getExposure();
	void setExposure(double);
	int setTrigger(bool);
	int imagesReceived();
	int errorsReceived();
	int imagesRecorded();
	static int countFreeCams();

private:
	//QFile logFile;
	HIDS cam;
	INT bitsPerSample;
	INT channels;
	INT bitsPerPixel;
	INT bytesPerPixel;
	IS_RECT AOIRect;
	INT colorMode;
	char** imageMemory;
	INT* memoryID;
	int bufferSize;
	Grabber *grabber;
	Converter *converter;
	Storage *storage;
	QThread *grabberThread;
	QThread *storageThread;
	QThread *converterThread;
	bool isConverting;
	bool makePreview;
	bool running, recording;
	bool processingImage;
	QImage *image;
	int maxPreviewFreq;
	
	QString filename;

	int numImagesReceived;
	int numImagesRecorded;
	int numErrors;
	int width;
	int height;
private slots:
	void onConversionDone(QImage *);
	void onPreviewTimer();

public slots:
	void startCapture();
	void stopCapture();
	void onNewFrame(char *);
	void onErrors(int);
	void convertBlock();
	void startRecording();
	void stopRecording();
signals:
	void converting(int, int);
	void newImage(QImage *);
	void starting();
	void stopping();
	void errors(int);
	//void newFrame(int, int, char *);
	void newFrame(char *);
	void linBufFull(char *, int);
	void countersChanged(int, int, int); //images received, images recorded, errors
};

#endif // QEYE_H