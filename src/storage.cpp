#include "storage.h"

Storage::Storage(QObject *parent) {
	QString path = "d:\\work\\";
	QString filename = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
	QString extension = ".msr";
	file.setFileName(path + filename + extension);
	first = true;
}

Storage::~Storage() {
}

//appends a buffer to a dump file, this needs error checking since
//we won't know, when we are losing frames
void Storage::saveBuffer(char *buf, int bytesPerFrame, int numberOfFrames, ULongLongVector timestamps) {
	QTime timer;
	timer.start();
	qDebug() << "open file" << file.open(QIODevice::WriteOnly | QIODevice::Append);
	if(first) {
		file.write((const char*) &bytesPerFrame, sizeof(int));
		first = false;
	}
	for(int i=0; i<numberOfFrames; i++) {
		unsigned long long timestamp = timestamps.at(i);
		qDebug() << "write timestamp" << file.write((const char*) &timestamp, sizeof(unsigned long long));
		qDebug() << "write data" << file.write(buf+i*bytesPerFrame+(i+1)*sizeof(unsigned long long), bytesPerFrame);
	}
	file.close();
	qDebug() << "done writing, elapsed time:" << timer.elapsed() << "ms";
}