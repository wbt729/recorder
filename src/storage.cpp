#include "storage.h"

Storage::Storage(QObject *parent) {
	QString path = "d:\\work\\";
	QString filename = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
	QString extension = ".msr";
	file.setFileName(path + filename + extension);
	
}

Storage::~Storage() {
	file.close();
}

void Storage::writeHeader(int height, int width, int colorMode, int bytesPerFrame) {
	qDebug() << "open file" << file.open(QIODevice::WriteOnly | QIODevice::Append);
	if(file.isOpen()) {
		file.write((const char*) &colorMode, sizeof(int));
		file.write((const char*) &bytesPerFrame, sizeof(int));
		file.write((const char*) &height, sizeof(int));
		file.write((const char*) &width, sizeof(int));
	}
	else
		qDebug() << "storage: cant write, file not open";
}

//appends a buffer to a dump file, this needs error checking since
//we won't know, when we are losing frames
void Storage::saveBuffer(char *buf, int bytesPerFrame, int numberOfFrames, ULongLongVector timestamps) {
	if(file.isOpen()) {
		QTime timer;
		timer.start();

		for(int i=0; i<numberOfFrames; i++) {
			unsigned long long timestamp = timestamps.at(i);
			//qDebug() << "write timestamp";
			file.write((const char*) &timestamp, sizeof(unsigned long long));
			//qDebug() << "write data" << file.write(buf+i*bytesPerFrame+(i+1)*sizeof(unsigned long long), bytesPerFrame);	//this can't be right
			//qDebug() << "write data";
			file.write(buf+i*bytesPerFrame, bytesPerFrame);	//this can't be right
		}
		
		qDebug() << "done writing, elapsed time:" << timer.elapsed() << "ms";
	}
	else
		qDebug() << "storage: cant write, file not open";
}