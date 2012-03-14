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
#include "meanplot.h"
#include "sampler.h"

class Recorder : public QMainWindow
{
	Q_OBJECT

public:
	Recorder(bool t = false, bool r = false, bool n = false, QWidget *parent = 0, Qt::WFlags flags = 0);	//trigger, record, noPlot
	~Recorder();
private:
	QThread *samplerThread;
	QProgressDialog *progressDialog;
	QEye *cam;
	MeanPlot *plot;
	Sampler *sampler;
	ImageLabel *imageLabel;
	QPushButton *recordButton;
	QPushButton *stopButton;
	QPushButton *convertButton;
	int errors;
	TiffConverter *conv;
	bool record; //start triggered recording at startup
	bool trigger; //use external trigger
	bool noPlot;
	virtual QSize sizeHint();
private slots:
	void doThings();
	void onLabelMouseWheel(int);
	void onCountersChanged(int, int, int);
	void onConvertButtonClicked();
	void onConverting(int, int);
	void onRecordButton(bool);
protected:
	virtual void closeEvent(QCloseEvent *);
};

#endif // RECORDER_H
