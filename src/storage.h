#ifndef STORAGE_H
#define STORAGE_H

#include <QObject>
#include <qdebug.h>
#include "uEye.h"
#include <QTime>
#include <qfile.h>

typedef QVector<unsigned long long> ULongLongVector;

class Storage : public QObject {
	Q_OBJECT

public:
	Storage(QObject *parent = 0);
	~Storage();

private:
	QFile file;
	bool first;
public slots:
	void saveBuffer(char *, int, int, ULongLongVector);
	void writeHeader(int, int, int, int);
	void finalize();
};

#endif // STORAGE_H
