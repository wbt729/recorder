#ifndef GRABBER_H
#define GRABBER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QFile>
#include "uEye.h"
//#include "process.h"

class Grabber : public QObject
{
	Q_OBJECT


public:
	Grabber(HIDS*);
	~Grabber();
	void init(int, int, int);

private:
	HIDS *cam;
#if defined _WIN64 || defined _WIN32
	HANDLE frameEvent;
#endif
	DWORD waitForFrame();
	int timeout;	//in ms

private slots:
	void grab();
	void checkForErrors();
public slots:
	int start();
signals:
	void newFrame();
	void errors(int);
};

#endif // GRABBER_H

//#ifndef GRABBER_H
//#define GRABBER_H
//
//#include <QObject>
//#include <QDebug>
//#include <QTimer>
//#include <QTime>
//#include <QFile>
//#include "uEye.h"
////#include "process.h"
//
//class Grabber : public QObject
//{
//	Q_OBJECT
//
//
//public:
//	Grabber(HIDS*, QObject *parent = 0);
//	~Grabber();
//	void init(int, int, int);
//
//public slots:
//	void start();
//	void stop();
//	void startRecording();
//	void stopRecording();
//	void saveErrorIndices();
//
//private:
//	QVector<int> errorIndices;
//	char *memAct, *memLast, *linBuf;
//	INT id;
//	HIDS *cam;
//	
//	INT sizeLinBuf;
//	bool running, recording, useFirstLinBuf;
//	int linBufIndex, offset, frameSize, imagesReceived, imagesRecorded;
//	
//	int width, height, bytesPerPixel;
//
//#if defined _WIN64 || defined _WIN32
//	HANDLE frameEvent;
//#endif
//
//	void onNewFrame();
//	DWORD waitForFrame();
//
//
//private slots:
//	void grab();
//	void checkForErrors();
//
//signals:
//	void newFrame(char *);
//	void errors(int);
//	void linBufFull(char*, int);
//};
//
//#endif // GRABBER_H
