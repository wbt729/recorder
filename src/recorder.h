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
#include <qprogressdialog.h>
#include <QMessageBox>

class Recorder : public QMainWindow
{
	Q_OBJECT

public:
	Recorder(bool r = false, QWidget *parent = 0, Qt::WFlags flags = 0);
	~Recorder();
private:
	QProgressDialog *progressDialog;
	QEye *cam;
	ImageLabel *imageLabel;
	QPushButton *recordButton;
	QPushButton *stopButton;
	QPushButton *convertButton;
	int errors;
	TiffConverter *conv;
	bool record; //start triggered recording at startup
private slots:
	void doThings();
	void onLabelMouseWheel(int);
	void onCountersChanged(int, int, int);
	void onConvertButtonClicked();
	void onConverting(int, int);
protected:
	virtual void closeEvent(QCloseEvent *);
};

#endif // RECORDER_H
