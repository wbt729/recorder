#include "recorder.h"

Recorder::Recorder(bool t, bool r, bool n, QWidget *parent, Qt::WFlags flags) {
	noPlot = n;
	record = r;
	trigger = t;
	QWidget *centralWidget = new QWidget();
	QGridLayout *layout = new QGridLayout();
	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);
	//setMinimumSize(1000, 600);

	//if(!noPlot) {
	//	hpSpinBox = new QSpinBox();
	//	lpSpinBox = new QSpinBox();
	//	hpSpinBox->setRange(0, 100);
	//	lpSpinBox->setRange(0, 100);
	//	plot = new MeanPlot();
	//	sampler = new Sampler();
	//	samplerThread = new QThread();
	//	sampler->moveToThread(samplerThread);
	//	samplerThread->start();
	//}
	plotWidget = new MeanPlotWidget(this);
	//plotWidget->show();
		
	cam = new QEye();
	camThread = new QThread();
	cam->moveToThread(camThread);
	camThread->start();
	//progressDialog = new QProgressDialog(this);
	conv = new TiffConverter(this);
	imageLabel = new ImageLabel(this);
	imageLabel->setText(tr("bla"));
	layout->addWidget(imageLabel,0,0);

	//if(!noPlot) {
	//	layout->addWidget(plot,0,1,1,2);
	//	layout->addWidget(new QLabel("LP"),1,1);
	//	layout->addWidget(new QLabel("HP"),1,2);
	//	layout->addWidget(lpSpinBox, 2,1);
	//	layout->addWidget(hpSpinBox, 2,2);
	//}

	recordButton = new QPushButton("record");
	recordButton->setCheckable(true);
	//convertButton = new QPushButton("convert");
	layout->addWidget(recordButton,1,0);
	//layout->addWidget(convertButton,2,0);	
	
	connect(recordButton, SIGNAL(toggled(bool)), this, SLOT(onRecordButton(bool)));
	//connect(convertButton, SIGNAL(clicked()), this, SLOT(onConvertButtonClicked()));
	connect(cam, SIGNAL(newImage(QImage *)), imageLabel, SLOT(setImage(QImage *)));
	connect(imageLabel, SIGNAL(mouseWheelSteps(int)), this, SLOT(onLabelMouseWheel(int)));
	//connect(cam, SIGNAL(newQImage(QImage *)), this, SLOT(onNewImage()));
	connect(cam, SIGNAL(errors(int)), this, SLOT(onError(int)));
	connect(cam, SIGNAL(transferCountersChanged(int, int, int)), this, SLOT(onCountersChanged(int, int, int)));

	//if(!noPlot) {
	//	connect(lpSpinBox, SIGNAL(valueChanged(int)), sampler, SLOT(setLP(int)));
	//	connect(hpSpinBox, SIGNAL(valueChanged(int)), sampler, SLOT(setHP(int)));
	//	connect(sampler, SIGNAL(newSamples(double, double, double)), plot, SLOT(updateData(double, double, double)));
	//	connect(cam, SIGNAL(newMat(cv::Mat *)), sampler, SLOT(setMat(cv::Mat *)));
	//	connect(imageLabel, SIGNAL(newRoi(QRect)), sampler, SLOT(setRoi(QRect)));
	//	//cam->makeCvMat(true);
	//}


	createMenus();
	errors = 0;

	QTimer::singleShot(.1, this, SLOT(doThings()));
}

void Recorder::createMenus() {
	showPlot = new QAction(tr("Show Plot"), this);
	showPlot->setCheckable(true);
	showPlot->setChecked(false);
	setMenuBar(new QMenuBar());
	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(showPlot);
	connect(showPlot, SIGNAL(triggered()), this, SLOT(onShowPlot()));
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
		//close();
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
		//else {
		//	cam->startCapturing();
		//}
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

//void Recorder::onConvertButtonClicked() {
//	connect(cam, SIGNAL(convertingBlock(int, int)), this, SLOT(onConvertingBlock(int, int)));
//	progressDialog->setCancelButton(0);
//	progressDialog->setModal(true);
//	progressDialog->show();
//	cam->convertBlock();
//}

//void Recorder::onConvertingBlock(int recent, int total) {
//	progressDialog->setMaximum(total);
//	progressDialog->setValue(recent);
//	if(recent == total)
//		progressDialog->close();
//}