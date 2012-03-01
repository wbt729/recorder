#include "recorder.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Recorder w;
    w.show();
    return a.exec();
}
