#ifndef STORAGE_H
#define STORAGE_H

#include <QObject>
#include <qdebug.h>
#include "uEye.h"
#include <QTime>
#include <qfile.h>

class Storage : public QObject {
	Q_OBJECT

public:
	Storage(QObject *parent = 0);
	~Storage();

private:
	QFile file;
public slots:
	void saveLinBuf(char *, int);
};

#endif // STORAGE_H
