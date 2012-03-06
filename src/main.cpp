#include "recorder.h"

#include <QtGui>
#include <QApplication>
#include <qstring.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	bool headless = false;

	for(int i=0; i<argc; i++) {
		qDebug() << QString(QObject::tr("argument %1: %2").arg(i).arg(argv[i]));
		QString arg = QObject::tr("%1").arg(argv[i]);
		if(arg == QString("headless")) {
			qDebug() << "headless activated";
		}
	}
	
    Recorder w;
    w.show();
    return a.exec();
}
