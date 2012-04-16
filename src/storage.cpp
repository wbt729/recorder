#include "storage.h"

Storage::Storage(QObject *parent) {
	file.setFileName("d:\\work\\dump");
}

Storage::~Storage() {
}

//appends a buffer to a dump file, this needs error checking since
//we won't know, when we are lose frames
void Storage::saveBuffer(char *buf, int length) {
	QTime timer;
	timer.start();
	qDebug() << "open file" << file.open(QIODevice::WriteOnly | QIODevice::Append);
	qDebug() << "write data" << file.write(buf, length);
	file.close();
	qDebug() << "done writing, elapsed time:" << timer.elapsed() << "ms";
}