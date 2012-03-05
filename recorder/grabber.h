#ifndef GRABBER_H
#define GRABBER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include "uEye.h"
//#include "process.h"

class Grabber : public QObject
{
	Q_OBJECT


public:
	Grabber(HIDS*, QObject *parent = 0);
	~Grabber();
	void init(int, int, int);
	//int imagesReceived();

public slots:
	void start();
	void stop();
	void startRecording();
	void stopRecording();

private:
	char *memAct, *memLast, *linBuf;
	INT id;
	HIDS *cam;
	
	INT sizeLinBuf;
	bool running, recording, useFirstLinBuf;
	int linBufIndex, offset, frameSize;
	
	int width, height, bytesPerPixel;
	//int absFrameIndex;	//number of images received
	//int absRecIndex;	//number of images recorded

#if defined _WIN64 || defined _WIN32
	HANDLE frameEvent;
#endif

	void onNewFrame();
	DWORD waitForFrame();


private slots:
	void grab();
	void checkForErrors();

signals:
	//void newFrame(int, int, char *);
	void newFrame(char *);
	void errors(int);
	void linBufFull(char*, int);
};

#endif // GRABBER_H
