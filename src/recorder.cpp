#include "recorder.h"

Recorder::Recorder(bool t, bool r, bool n, QWidget *parent, Qt::WFlags flags) {
	noPlot = n;
	record = r;
	trigger = t;
	QWidget *centralWidget = new QWidget();
	QGridLayout *layout = new QGridLayout();
	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);
	setMinimumSize(1000, 600);

	if(!noPlot) {
		hpSpinBox = new QSpinBox();
		lpSpinBox = new QSpinBox();
		hpSpinBox->setRange(0, 100);
		lpSpinBox->setRange(0, 100);
		plot = new MeanPlot();
		sampler = new Sampler();
		samplerThread = new QThread();
		sampler->moveToThread(samplerThread);
		samplerThread->start();
	}
		
	cam = new QEye(this);
	progressDialog = new QProgressDialog(this);
	conv = new TiffConverter(this);
	imageLabel = new ImageLabel(this);
	imageLabel->setText(tr("bla"));
	layout->addWidget(imageLabel,0,0);

	if(!noPlot)
		layout->addWidget(plot,0,1,1,2);
		layout->addWidget(new QLabel("LP"),1,1);
		layout->addWidget(new QLabel("HP"),1,2);
		layout->addWidget(lpSpinBox, 2,1);
		layout->addWidget(hpSpinBox, 2,2);

	recordButton = new QPushButton("record");
	recordButton->setCheckable(true);
	convertButton = new QPushButton("convert");
	layout->addWidget(recordButton,1,0);
	layout->addWidget(convertButton,2,0);	
	
	connect(recordButton, SIGNAL(toggled(bool)), this, SLOT(onRecordButton(bool)));
	connect(convertButton, SIGNAL(clicked()), this, SLOT(onConvertButtonClicked()));
	connect(cam, SIGNAL(newImage(QImage *)), imageLabel, SLOT(setImage(QImage *)));
	connect(imageLabel, SIGNAL(mouseWheelSteps(int)), this, SLOT(onLabelMouseWheel(int)));
	connect(cam, SIGNAL(newImage(QImage *)), this, SLOT(onNewImage()));
	connect(cam, SIGNAL(errors(int)), this, SLOT(onError(int)));
	connect(cam, SIGNAL(countersChanged(int, int, int)), this, SLOT(onCountersChanged(int, int, int)));

	if(!noPlot) {
		connect(lpSpinBox, SIGNAL(valueChanged(int)), sampler, SLOT(setLP(int)));
		connect(hpSpinBox, SIGNAL(valueChanged(int)), sampler, SLOT(setHP(int)));
		connect(sampler, SIGNAL(newSamples(double, double, double)), plot, SLOT(updateData(double, double, double)));
		connect(cam, SIGNAL(newMat(cv::Mat *)), sampler, SLOT(setMat(cv::Mat *)));
		connect(imageLabel, SIGNAL(newRoi(QRect)), sampler, SLOT(setRoi(QRect)));
		cam->makeCvMat(true);
	}

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
		noCamMsgBox.setText(tr("There are %1 uEye cameras on this network. No free camera could be found. Check if the camera is already opened somewhere else and check the network connections. Then restart this program.").arg(camsFound));
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
	cam->setExposure(exp + (double) steps/75);
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