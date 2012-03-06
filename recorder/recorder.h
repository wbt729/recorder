#ifndef RECORDER_H
#define RECORDER_H

#include <QtGui/QMainWindow>
#include <QtGui/QStatusBar>
#include <qgridlayout.h>
#include <QTimer>
#include <QLayout>
#include <qpushbutton.h>
#include "qeye.h"
#include "imagelabel.h"
#include "tiffconverter.h"
#include <QErrorMessage>
#include <QMessageBox>

class Recorder : public QMainWindow
{
	Q_OBJECT

public:
	Recorder(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Recorder();
private:
	QEye *cam;
	ImageLabel *imageLabel;
	QPushButton *recordButton;
	QPushButton *stopButton;
	QPushButton *convertButton;
	int errors;
	TiffConverter *conv;
private slots:
	void doThings();
	void onLabelMouseWheel(int);
	void onCountersChanged(int, int, int);
protected:
	virtual void closeEvent(QCloseEvent *);
};

#endif // RECORDER_H
