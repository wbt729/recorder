#include "storage.h"

Storage::Storage(QObject *parent, HIDS* camera) {
	cam = camera;
	index = 0;
}

Storage::~Storage() {
}

void Storage::saveLastImage() {
	QString str = QString(tr("d:\\work\\img\\%1.bmp").arg(index));
	qDebug() << "saving" << is_SaveImage(*cam, str.toLatin1());
	index++;
}