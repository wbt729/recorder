#include "qeye.h"

QEye::QEye(QObject *parent) {
	cam = NULL;
	bitsPerPixel = 0;
	colorMode = 0;
	imageMemory = NULL;
	memoryID = NULL;
	bufferSize = 0;
	running = false;
	grabber = new Grabber(this, &cam, &running);
	storage = new Storage(this);
	converter = new Converter(this);
	grabberThread = new QThread;
	storageThread = new QThread;
	converterThread = new QThread;
	grabber->moveToThread(grabberThread);
	storage->moveToThread(storageThread);
	converter->moveToThread(converterThread);
	grabberThread->start();
	storageThread->start();
	converterThread->start();
	connect(grabber, SIGNAL(newFrame()), this, SLOT(onNewFrame()));
	//connect(grabber, SIGNAL(newFrame()), this, SLOT(frameDelay()));
	connect(grabber, SIGNAL(errors(int)), this, SLOT(onError(int)));
	//connect(grabber, SIGNAL(newFrame()), storage, SLOT(saveLastImage()));
	connect(this, SIGNAL(starting()), grabber, SLOT(start()));
	connect(this, SIGNAL(newFrame(char *)), converter, SLOT(charToQImage(char *)));
	connect(converter, SIGNAL(newImage(QImage *)), this, SLOT(onConversionDone(QImage *)));
	connect(this, SIGNAL(linBufFull(char *, int)), storage, SLOT(saveLinBuf(char *, int)));
	filename = QString("d:\\work\\dump");
	QFile::remove(filename);
	sizeLinBuf = 100;
	linBufIndex = 0;
	isConverting = false;
	imagesReceived = 0;
	useFirstLinBuf = true;
	bitsPerSample = 0;
	channels = 0;
	bytesPerPixel = 0;

}

QEye::~QEye() {
}

int QEye::init(int id) {
	if(id != 0) cam |= id;
	return is_InitCamera(&cam, NULL);
}

int	QEye::loadParameters(QString str) {
	return is_LoadParameters(cam, str.toLatin1());
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
	if(is_AOI(cam, IS_AOI_IMAGE_GET_AOI, &AOIRect, sizeof(AOIRect)) == 0) return AOIRect.s32Height;
	else return -1;
}

int QEye::getWidth() {
	if(is_AOI(cam, IS_AOI_IMAGE_GET_AOI, &AOIRect, sizeof(AOIRect)) == 0) return AOIRect.s32Width;
	else return -1;
}

int QEye::createRingBuffer(int size) {
	bufferSize = size;
	imageMemory = new char*[bufferSize];
	memoryID = new INT[bufferSize];
	for(int i=0; i < bufferSize; i++) {
		if(is_AllocImageMem(cam, getWidth(), getHeight(), bitsPerPixel, &imageMemory[i], &memoryID[i])) return -1;
		if(is_AddToSequence(cam, imageMemory[i], memoryID[i])) return -1;
	}
	is_InitImageQueue(cam, 0);

	linBuf = new char[2*sizeLinBuf*getWidth()*getHeight()*bytesPerPixel];

	return 0;
}

void QEye::startCapture() {
	converter->setResolution(getWidth(), getHeight(), channels, bitsPerSample, bytesPerPixel);
	grabber->blockSignals(false);
	running = true;
	emit starting();
	qDebug() << "start";
}

void QEye::onNewFrame() {
	imagesReceived++;
	char *memAct, *memLast;
	INT id = 0;
	is_GetActSeqBuf(cam, &id, &memAct, &memLast);
	qDebug() << "buffer id" << id;
	//emit newImage(image);

	offset = (useFirstLinBuf ? 0 : sizeLinBuf*getWidth()*getHeight()*bytesPerPixel);
	//offset = 0;

	qDebug() << "copy mem" << is_CopyImageMem(cam, memLast, NULL, &linBuf[offset+linBufIndex*getWidth()*getHeight()*bytesPerPixel]);
	//qDebug() << "copy mem" << is_CopyImageMem(cam, memLast, memoryID[0], &linBuf[offset+linBufIndex*getWidth()*getHeight()*bytesPerPixel]);
	qDebug() << "unlock mem" << is_UnlockSeqBuf(cam, NULL, memLast);

	if(!isConverting) {
		isConverting = true;
		//emit(newFrame(memLast));
		emit(newFrame(&linBuf[offset+linBufIndex*getWidth()*getHeight()*bytesPerPixel]));
	}
	else
		qDebug() << "still converting";

	linBufIndex++;
	if(linBufIndex >= sizeLinBuf) {
		emit linBufFull(&linBuf[offset], getWidth()*getHeight()*bytesPerPixel*sizeLinBuf);
		linBufIndex = 0;
		useFirstLinBuf = !useFirstLinBuf;
	}
	return;
}

void QEye::stopCapture() {
	qDebug() << "QEye stop capture";
	running = false;

}

bool QEye::isRunning() {
	return running;
}

int QEye::exit() {
	qDebug() << "QEye: exit";
	grabber->blockSignals(true);
	converter->blockSignals(true);
	storage->blockSignals(true);
	disconnect(this, SIGNAL(onNewFrame()));
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
}

int QEye::setTrigger(bool external) {
	if(external) return is_SetExternalTrigger(cam, IS_SET_TRIGGER_LO_HI);
	else return is_SetExternalTrigger(cam, IS_SET_TRIGGER_SOFTWARE);
}

void QEye::onConversionDone(QImage *image) {
	emit newImage(image);
	isConverting = false;
}

void QEye::convertBlock() {
	converter->blockToTIFFs();
}