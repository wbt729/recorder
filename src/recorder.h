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
#include "meanplotwidget.h"
//#include "sampler.h"
#include <qspinbox.h>
#include <qmenubar.h>

class Recorder : public QMainWindow
{
	Q_OBJECT

public:
	Recorder(bool t = false, bool r = false, bool n = false, QWidget *parent = 0, Qt::WFlags flags = 0);	//trigger, record, noPlot
	~Recorder();
private:
	QThread *camThread;

	QThread *samplerThread;
	//QProgressDialog *progressDialog;
	QEye *cam;
	MeanPlotWidget *plotWidget;
	Sampler *sampler;
	ImageLabel *imageLabel;
	QPushButton *recordButton;
	QPushButton *triggerButton;
	int errors;
	TiffConverter *conv;
	bool record; //start triggered recording at startup
	bool trigger; //use external trigger
	bool noPlot;
	virtual QSize sizeHint();
	QMenu *viewMenu;
	QAction *showPlot;
	void createMenus();
	void makeConnections();
private slots:
	void doThings();
	void onLabelMouseWheel(int);
	void onCountersChanged(int, int, int);
	void onRecordButton(bool);
	void onShowPlot();
	void onPlotWidgetClosing();
protected:
	virtual void closeEvent(QCloseEvent *);
};

#endif // RECORDER_H
