#include "storage.h"

Storage::Storage(QObject *parent) {
	file.setFileName("d:\\work\\dump");
}

Storage::~Storage() {
}

void Storage::saveLinBuf(char *buf, int length) {
	QTime timer;
	timer.start();
	qDebug() << "open file" << file.open(QIODevice::WriteOnly | QIODevice::Append);
	qDebug() << "write data" << file.write(buf, length);
	file.close();
	qDebug() << "done writing, elapsed time:" << timer.elapsed() << "ms";
}