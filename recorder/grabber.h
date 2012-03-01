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

public slots:
	void start();
	void stop();

private:
	char *memAct, *memLast;
	INT id;
	HIDS *cam;
//	HANDLE frameEvent;
	bool running;
	INT sizeLinBuf;
	bool useFirstLinBuf;
	int offset;
	int linBufIndex;
	int width;
	int height;
	char *linBuf;
	int bytesPerPixel;
	int absFrameIndex;

private slots:
	void grab();

signals:
	void newFrame(int, char *);
	void errors(int);
	void linBufFull(char*, int);
};

#endif // GRABBER_H
