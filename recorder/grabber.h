#ifndef GRABBER_H
#define GRABBER_H

#include <QObject>
#include <QDebug>
#include "uEye.h"
#include "process.h"

class Grabber : public QObject
{
	Q_OBJECT

public:
	Grabber(QObject *parent, HIDS*, bool*);
	~Grabber();
public slots:
	void start();
	void stop();

private:
	bool *capturing;
	HIDS *cam;

signals:
	void newFrame();
	void errors(int);
};

#endif // GRABBER_H
