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
	int createRingBuffer(int);
	bool isRunning();
	int exit();
	double getExposure();
	void setExposure(double);
	int setTrigger(bool);

private:
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
	bool running;
	bool processingImage;
	QImage *image;
	char *linBuf;
	QString filename;
	INT sizeLinBuf;
	bool useFirstLinBuf;
	int offset;
	int linBufIndex;
	int imagesReceived;
private slots:
	//void frameDelay();
	void onConversionDone(QImage *);

public slots:
	void startCapture();
	void stopCapture();
	void onNewFrame();
	void onError(int);
	void convertBlock();
signals:
	void newImage(QImage *);
	void starting();
	void stopping();
	void errors(int);
	void newFrame(char *);
	void linBufFull(char *, int);
};

#endif // QEYE_H
