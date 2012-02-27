#ifndef STORAGE_H
#define STORAGE_H

#include <QObject>
#include <qdebug.h>
#include "uEye.h"

class Storage : public QObject {
	Q_OBJECT

public:
	Storage(QObject *parent, HIDS*);
	~Storage();

private:
	HIDS *cam;
	int index;
public slots:
	void saveLastImage();
};

#endif // STORAGE_H
