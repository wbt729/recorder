#include "recorder.h"

#include <QtGui>
#include <QApplication>
#include <qstring.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	bool record = false;
	bool trigger = false;

	//parse cli arguments
	for(int i=0; i<argc; i++) {
		qDebug() << QString(QObject::tr("argument %1: %2").arg(i).arg(argv[i]));
		QString arg = QObject::tr("%1").arg(argv[i]);
		if(arg == QString("record")) {
			record = true;
			qDebug() << "start recording immediately";
		}

		else if (arg == QString("trigger")) {
			trigger = true;
		}

		else
			qDebug() << QObject::tr("argument %1 ignored").arg(i);
	}
	
    Recorder w(trigger, record);
    w.show();
    return a.exec();
}
