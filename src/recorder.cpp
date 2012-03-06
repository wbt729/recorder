#include "recorder.h"

Recorder::Recorder(bool r, QWidget *parent, Qt::WFlags flags) {
	record = r;
	QWidget *centralWidget = new QWidget();
	QGridLayout *layout = new QGridLayout();
	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);

	cam = new QEye(this);
	progressDialog = new QProgressDialog(this);
	conv = new TiffConverter(this);
	imageLabel = new ImageLabel(this);
	imageLabel->setText(tr("bla"));
	layout->addWidget(imageLabel);
	recordButton = new QPushButton("start");
	stopButton = new QPushButton("stop");
	convertButton = new QPushButton("convert");
	layout->addWidget(recordButton);
	layout->addWidget(stopButton);
	layout->addWidget(convertButton);
	connect(recordButton, SIGNAL(clicked()), cam, SLOT(startRecording()));
	connect(stopButton, SIGNAL(clicked()), cam, SLOT(stopRecording()));
	//connect(convertButton, SIGNAL(clicked()), cam, SLOT(convertBlock()));
	connect(convertButton, SIGNAL(clicked()), this, SLOT(onConvertButtonClicked()));

	connect(cam, SIGNAL(newImage(QImage *)), imageLabel, SLOT(setImage(QImage *)));
	connect(imageLabel, SIGNAL(mouseWheelSteps(int)), this, SLOT(onLabelMouseWheel(int)));
	connect(cam, SIGNAL(newImage(QImage *)), this, SLOT(onNewImage()));
	connect(cam, SIGNAL(errors(int)), this, SLOT(onError(int)));
	connect(cam, SIGNAL(countersChanged(int, int, int)), this, SLOT(onCountersChanged(int, int, int)));
	errors = 0;

	QTimer::singleShot(.1, this, SLOT(doThings()));
}

Recorder::~Recorder() {
}

void Recorder::doThings() {
	if(cam->init() != 0) {
		int camsFound = QEye::countFreeCams();
		statusBar()->showMessage("Init failed");
		QMessageBox noCamMsgBox(this);
		noCamMsgBox.setText(tr("There are %1 uEye cameras on this network. No free camera could be found. Check if camera is already open or check network connections. Then try again.").arg(camsFound));
		noCamMsgBox.exec();
		//close();
		return;
	}
	else {
		cam->loadParameters("d:\\work\\ueye.ini");
		//cam->setColorMode(IS_CM_RGB8_PACKED);
		cam->setColorMode(IS_CM_RGB10V2_PACKED);
		cam->createBuffers(400);
		statusBar()->showMessage("Ready");

		if(record) {
			cam->startRecording();
			cam->setTrigger(true);
		}
		else {
			cam->setTrigger(true);
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
	progressDialog->show();
	cam->convertBlock();
}

void Recorder::onConverting(int recent, int total) {
	progressDialog->setMaximum(total);
	progressDialog->setValue(recent);
	if(recent == total)
		progressDialog->close();
}