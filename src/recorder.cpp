#include "recorder.h"

Recorder::Recorder(bool t, bool r, bool n, QWidget *parent, Qt::WFlags flags) {
	noPlot = n;
	record = r;
	trigger = t;
	QWidget *centralWidget = new QWidget();
	QGridLayout *layout = new QGridLayout();
	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);
	plotWidget = new MeanPlotWidget(this);
		
	cam = new QEye();
	camThread = new QThread();
	cam->moveToThread(camThread);
	camThread->start();
	conv = new TiffConverter(this);
	imageLabel = new ImageLabel(this);
	imageLabel->setText(tr("bla"));
	layout->addWidget(imageLabel,0,0);

	recordButton = new QPushButton("Record");
	recordButton->setCheckable(true);
	layout->addWidget(recordButton);

	triggerButton = new QPushButton("External Trigger");
	triggerButton->setCheckable(true);
	layout->addWidget(triggerButton);
	
	createMenus();
	makeConnections();
	errors = 0;

	QTimer::singleShot(.1, this, SLOT(doThings()));
}

void Recorder::makeConnections() {
	connect(cam, SIGNAL(newImage(QImage *)), imageLabel, SLOT(setImage(QImage *)));
	//connect(cam, SIGNAL(errors(int)), this, SLOT(onError(int)));
	connect(cam, SIGNAL(transferCountersChanged(int, int, int)), this, SLOT(onCountersChanged(int, int, int)));
	connect(recordButton, SIGNAL(toggled(bool)), this, SLOT(onRecordButton(bool)));
	connect(triggerButton, SIGNAL(toggled(bool)), cam, SLOT(setExternalTrigger(bool)));
	connect(imageLabel, SIGNAL(mouseWheelSteps(int)), this, SLOT(onLabelMouseWheel(int)));
	connect(plotWidget, SIGNAL(closing()), this, SLOT(onPlotWidgetClosing()));
	connect(showPlot, SIGNAL(triggered()), this, SLOT(onShowPlot()));
}

void Recorder::createMenus() {
	showPlot = new QAction(tr("Show Plot"), this);
	showPlot->setCheckable(true);
	showPlot->setChecked(false);
	setMenuBar(new QMenuBar());
	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(showPlot);

}

void Recorder::onShowPlot() {
	if(showPlot->isChecked()) {
		plotWidget->show();
		connect(cam, SIGNAL(newMat(cv::Mat *)), plotWidget, SLOT(newMat(cv::Mat *)));
		connect(imageLabel, SIGNAL(newRoi(QRect)), plotWidget, SLOT(newRoi(QRect)));
	}
	else {
		plotWidget->hide();
		disconnect(cam, SIGNAL(newMat(cv::Mat *)), plotWidget, SLOT(newMat(cv::Mat *)));
		
	}
}

Recorder::~Recorder() {
}

QSize Recorder::sizeHint() {
	return QSize(800, 600);
}

void Recorder::onRecordButton(bool checked) {
	if(checked)
		cam->startRecording();
	else
		cam->stopRecording();
}

void Recorder::doThings() {
	if(cam->init() != 0) {
		int camsFound = QEye::countFreeCams();
		statusBar()->showMessage("Init failed");
		QMessageBox noCamMsgBox(this);
		noCamMsgBox.setText(tr("There are %1 uEye cameras on this network. No free camera could be found. Check if the camera is already opened somewhere else and check the network connections. Then restart this program.").arg(camsFound));
		noCamMsgBox.exec();
		recordButton->setDisabled(true);
		return;
	}
	else {
		cam->startCapturing();
		statusBar()->showMessage("Ready");

		if(trigger) {
			cam->setExternalTrigger(true);
		}

		if(record) {
			recordButton->setChecked(true);
			cam->startRecording();
		}
	}
}

void Recorder::closeEvent(QCloseEvent *event) {
	blockSignals(true);
	statusBar()->showMessage("Shutting down");
	cam->exit();
	plotWidget->close();
	qDebug("recorder: this is the end");
}

void Recorder::onLabelMouseWheel(int steps) {
	double exp = 0;
	exp = cam->getExposure();
	cam->setExposure(exp + (double) steps/10);
}

void Recorder::onCountersChanged(int received, int recorded, int errors) {
	statusBar()->showMessage(tr("Images Received: %1, Errors: %2, Images Recorded: %3").arg(received).arg(recorded).arg(errors));
}

void Recorder::onPlotWidgetClosing() {
	showPlot->setChecked(false);
	disconnect(cam, SIGNAL(newMat(cv::Mat *)), plotWidget, SLOT(newMat(cv::Mat *)));
}