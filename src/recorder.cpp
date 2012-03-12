#include "recorder.h"

Recorder::Recorder(bool t, bool r, QWidget *parent, Qt::WFlags flags) {
	record = r;
	trigger = t;
	QWidget *centralWidget = new QWidget();
	QGridLayout *layout = new QGridLayout();
	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);
	setMinimumSize(1000, 600);

	sampler = new Sampler();
	samplerThread = new QThread();
	sampler->moveToThread(samplerThread);
	samplerThread->start();

	plot = new MeanPlot();
	cam = new QEye(this);
	progressDialog = new QProgressDialog(this);
	conv = new TiffConverter(this);
	imageLabel = new ImageLabel(this);
	imageLabel->setText(tr("bla"));
	layout->addWidget(imageLabel,0,0);
	layout->addWidget(plot,0,1);
	recordButton = new QPushButton("record");
	recordButton->setCheckable(true);
	//stopButton = new QPushButton("stop");
	convertButton = new QPushButton("convert");
	layout->addWidget(recordButton,1,0);
	//layout->addWidget(stopButton);
	layout->addWidget(convertButton,2,0);
	connect(recordButton, SIGNAL(toggled(bool)), this, SLOT(onRecordButton(bool)));
	//connect(recordButton, SIGNAL(clicked()), cam, SLOT(startRecording()));
	//connect(stopButton, SIGNAL(clicked()), cam, SLOT(stopRecording()));
	//connect(convertButton, SIGNAL(clicked()), cam, SLOT(convertBlock()));
	connect(convertButton, SIGNAL(clicked()), this, SLOT(onConvertButtonClicked()));
	connect(sampler, SIGNAL(newSamples(double, double, double)), plot, SLOT(updateData(double, double, double)));
	connect(imageLabel, SIGNAL(newRoi(QRect)), sampler, SLOT(setRoi(QRect)));

	connect(cam, SIGNAL(newImage(QImage *)), imageLabel, SLOT(setImage(QImage *)));
	connect(cam, SIGNAL(newMat(cv::Mat *)), sampler, SLOT(setMat(cv::Mat *)));
	connect(imageLabel, SIGNAL(mouseWheelSteps(int)), this, SLOT(onLabelMouseWheel(int)));
	connect(cam, SIGNAL(newImage(QImage *)), this, SLOT(onNewImage()));
	connect(cam, SIGNAL(errors(int)), this, SLOT(onError(int)));
	connect(cam, SIGNAL(countersChanged(int, int, int)), this, SLOT(onCountersChanged(int, int, int)));
	errors = 0;

	QTimer::singleShot(.1, this, SLOT(doThings()));
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
		noCamMsgBox.setText(tr("There are %1 uEye cameras on this network. No free camera could be found. Check if the camera is already opened somewhere else or check network connections. Then restart this program.").arg(camsFound));
		noCamMsgBox.exec();
		recordButton->setDisabled(true);
		//close();
		return;
	}
	else {
		cam->loadParameters("d:\\work\\ueye.ini");
		//cam->setColorMode(IS_CM_RGB8_PACKED);
		cam->setColorMode(IS_CM_RGB10V2_PACKED);
		cam->createBuffers(400);
		statusBar()->showMessage("Ready");

		if(trigger) {
			cam->setTrigger(true);
		}

		if(record) {
			recordButton->setChecked(true);
			cam->startRecording();
		}
		else {
			cam->startCapture();
		}
	}
}

void Recorder::closeEvent(QCloseEvent *event) {
	statusBar()->showMessage("Shutting down");
	cam->exit();
}

void Recorder::onLabelMouseWheel(int steps) {
	double exp = 0;
	exp = cam->getExposure();
	cam->setExposure(exp + (double) steps/5);
}

void Recorder::onCountersChanged(int received, int recorded, int errors) {
	statusBar()->showMessage(tr("Images Received: %1, Images Recorded: %2, Errors: %3").arg(received).arg(recorded).arg(errors));
}

void Recorder::onConvertButtonClicked() {
	connect(cam, SIGNAL(converting(int, int)), this, SLOT(onConverting(int, int)));
	progressDialog->setCancelButton(0);
	progressDialog->setModal(true);
	progressDialog->show();
	cam->convertBlock();
}

void Recorder::onConverting(int recent, int total) {
	progressDialog->setMaximum(total);
	progressDialog->setValue(recent);
	if(recent == total)
		progressDialog->close();
}