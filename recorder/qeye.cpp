#include "qeye.h"

QEye::QEye(QObject *parent) {
	logFile.setFileName("log.txt");
	logFile.open(QIODevice::Text | QIODevice::WriteOnly);
	cam = NULL;
	bitsPerPixel = 0;
	colorMode = 0;
	imageMemory = NULL;
	memoryID = NULL;
	bufferSize = 0;
	running = false;
	maxPreviewFreq = 15; //Hz;
	makePreview = true;

	grabber = new Grabber(&cam);
	storage = new Storage();
	converter = new Converter();

	//setup threads
	//grabber and storage objects and threads really only should be created, once init was successfull
	//same goes for the ringbuffer
	grabberThread = new QThread();
	storageThread = new QThread;
	converterThread = new QThread;

	grabber->moveToThread(grabberThread);
	storage->moveToThread(storageThread);
	converter->moveToThread(converterThread);

	grabberThread->start();
	storageThread->start();
	converterThread->start();
	grabberThread->setPriority(QThread::TimeCriticalPriority);

	//signals and slots should be thought through again
	connect(converter, SIGNAL(newImage(QImage *)), this, SLOT(onConversionDone(QImage *)));
	connect(grabber, SIGNAL(newFrame(int, char *)), this, SLOT(onNewFrame(int, char *)));
	connect(grabber, SIGNAL(linBufFull(char *, int)), storage, SLOT(saveLinBuf(char *, int)));
	connect(grabber, SIGNAL(errors(int)), this, SLOT(onError(int)));
	connect(this, SIGNAL(starting()), grabber, SLOT(start()));
	connect(this, SIGNAL(stopping()), grabber, SLOT(stop()));
	connect(this, SIGNAL(newFrame(char *)), converter, SLOT(charToQImage(char *)));

	//dont the dump file here, better in the constructor of the storage object
	filename = QString("d:\\work\\dump");
	QFile::remove(filename);
	

	numImagesReceived = 0;
	bitsPerSample = 0;
	channels = 0;
	isConverting = false;
	bytesPerPixel = 0;
	width = 0;
	height = 0;
}

void QEye::startRecording() {
	grabber->startRecording();
}

void QEye::stopRecording() {
	grabber->stopRecording();
}

QEye::~QEye() {
}

int QEye::init(int id) {
	if(id != 0) cam |= id;
	int ret = is_InitCamera(&cam, NULL);
	if(ret) {
		getWidth();
		getHeight();
	}
	return ret;
}

int	QEye::loadParameters(QString str) {
	int ret = is_LoadParameters(cam, str.toLatin1());
	if(ret) {
		getWidth();
		getHeight();
	}
	return ret;
}

int QEye::imagesReceived() {
	return numImagesReceived;
}

int QEye::setColorMode(INT mode) {
	switch(mode) {
		case IS_CM_RGB10V2_PACKED:
			bitsPerPixel = 32;
			bytesPerPixel = 4;
			channels = 3;
			bitsPerSample = 10;
			break;
		case IS_CM_BGRA8_PACKED:	//TODO set other parameters
			bitsPerPixel = 32;
			break;
		case IS_CM_RGB8_PACKED:
			bitsPerPixel = 24;
			break;
		case IS_CM_MONO8:
			bitsPerPixel = 8;
			break;
		case IS_SET_CM_RGB24:
			bitsPerPixel = 24;
			break;
		default:
			return -1;
	}
	bytesPerPixel = bitsPerPixel/8;
	qDebug() << "color mode" << mode << "bits per pixel" << bitsPerPixel << "bytes per pixel" << bytesPerPixel;
	return is_SetColorMode(cam, mode);
}

int QEye::getHeight() {
	if(is_AOI(cam, IS_AOI_IMAGE_GET_AOI, &AOIRect, sizeof(AOIRect)) == 0) {
		height = AOIRect.s32Height;
		return height;
	}
	else return -1;
}

int QEye::getWidth() {
	if(is_AOI(cam, IS_AOI_IMAGE_GET_AOI, &AOIRect, sizeof(AOIRect)) == 0) {
		width = AOIRect.s32Width;
		return width;
	}
	else return -1;
}


int QEye::createBuffers(int sizeRing) {
	bufferSize = sizeRing;
	imageMemory = new char*[bufferSize];
	memoryID = new INT[bufferSize];
	for(int i=0; i < bufferSize; i++) {
		if(is_AllocImageMem(cam, getWidth(), getHeight(), bitsPerPixel, &imageMemory[i], &memoryID[i])) return -1;	//TODO replace with width height etc from local variable to save bandwith
		if(is_AddToSequence(cam, imageMemory[i], memoryID[i])) return -1;
	}
	is_InitImageQueue(cam, 0);
	return 0;
}

void QEye::startCapture() {
	grabber->init(getWidth(), getHeight(), bytesPerPixel);
	converter->setResolution(getWidth(), getHeight(), channels, bitsPerSample, bytesPerPixel);
	grabber->blockSignals(false);
	running = true;
	emit starting();
	qDebug() << "start";
}

void QEye::onNewFrame(int numImg, char *buf) {
	numImagesReceived = numImg;

	if(!isConverting && makePreview) {
		makePreview = false;
		isConverting = true;
		emit(newFrame(buf));
		QTimer::singleShot(1000/maxPreviewFreq, this, SLOT(onPreviewTimer()));
	}
	return;
}

void QEye::onPreviewTimer() {
	makePreview = true;
	return;
}

void QEye::stopCapture() {
	qDebug() << "QEye stop capture";
	running = false;
	emit stopping();
	return;
}

bool QEye::isRunning() {
	return running;
}

int QEye::exit() {
	qDebug() << "QEye: exit";
	grabber->blockSignals(true);
	converter->blockSignals(true);
	storage->blockSignals(true);
	disconnect(this, SIGNAL(onNewFrame(int, char*)));
	stopCapture();
	grabberThread->quit();
	storageThread->quit();
	converterThread->quit();
	qDebug() << "QEye: wait for Threads to finish";
	qDebug() << "QEye: grabber thread" << grabberThread->wait();
	qDebug() << "QEye: storage thread" << storageThread->wait();
	qDebug() << "QEye: converter thread" << converterThread->wait();
	return is_ExitCamera(cam);
}

double QEye::getExposure() {
	double exposure;
	is_Exposure(cam, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&exposure, sizeof(exposure));
	return exposure;
}

void QEye::setExposure(double exp) {
	INT retVal = is_Exposure(cam, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&exp, sizeof(exp));
	return;
}

void QEye::onError(int num) {
	emit errors(num);
	return;
}

int QEye::setTrigger(bool external) {
	if(external) return is_SetExternalTrigger(cam, IS_SET_TRIGGER_LO_HI);
	else return is_SetExternalTrigger(cam, IS_SET_TRIGGER_SOFTWARE);
}

void QEye::onConversionDone(QImage *image) {
	emit newImage(image);
	isConverting = false;
	return;
}

void QEye::convertBlock() {
	converter->blockToTIFFs();
	return;
}
