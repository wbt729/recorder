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
	storage = new Storage(this, &cam);
	grabberThread = new QThread;
	storageThread = new QThread;
	grabber->moveToThread(grabberThread);
	storage->moveToThread(storageThread);
	grabberThread->start();
	storageThread->start();
	//connect(grabber, SIGNAL(newFrame()), this, SLOT(onNewFrame()));
	connect(grabber, SIGNAL(newFrame()), this, SLOT(frameDelay()));
	connect(grabber, SIGNAL(errors(int)), this, SLOT(onError(int)));
	//connect(grabber, SIGNAL(newFrame()), storage, SLOT(saveLastImage()));
	connect(this, SIGNAL(starting()), grabber, SLOT(start()));
	filename = QString("d:\\work\\dump");
	QFile::remove(filename);
	sizeLinBuf = 1;
	linBufIndex = 0;
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
			break;
		case IS_CM_BGRA8_PACKED:
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

//int QEye::createBuffer(int size) {
//	bufferSize = size;
//	//imageMemory = new char*[bufferSize];
//	memoryID = new INT[bufferSize];
//
//	for(int i=0; i < bufferSize; i++) {
//		HANDLE hMem = GlobalAlloc(0, getWidth()*getHeight()*bitsPerPixel);
//		//char* pcMem = (char*)GlobalLock(hMem);
//		mymem = (char*) hMem;
//		qDebug() << "set allocated mem" << is_SetAllocatedImageMem(cam, getWidth(), getHeight(), bitsPerPixel, mymem, &memoryID[i]);
//		qDebug() << "add to sequence" << is_AddToSequence(cam, mymem, memoryID[i]);
//	}
//	is_InitImageQueue(cam, 0);
//	return 0;
//}

void QEye::startCapture() {
	running = true;
	emit starting();
	qDebug() << "start";
}

void QEye::frameDelay() {
	QTimer::singleShot(.5, this, SLOT(onNewFrame()));
}

void QEye::onNewFrame() {

	
	//char *sto = new char[getWidth()*getHeight()*bitsPerPixel/8];

	char *memAct, *memLast;
	is_GetActSeqBuf(cam, NULL, &memAct, &memLast);
	image = new QImage((uchar*) memLast, getWidth(), getHeight(), QImage::Format_RGB888);
	//image = new QImage((uchar*) memLast, getWidth(), getHeight(), QImage::Format_Indexed8);
	emit newImage(image);

	//offset = (useFirstLinBuf ? 0 : sizeLinBuf*getWidth()*getHeight()*bytesPerPixel);
	offset = 0;

	qDebug() << "copy mem" << is_CopyImageMem(cam, memLast, NULL, &linBuf[offset+linBufIndex*getWidth()*getHeight()*bytesPerPixel]);
	//qDebug() << "copy mem" << is_CopyImageMem(cam, memLast, memoryID[0], &linBuf[offset+linBufIndex*getWidth()*getHeight()*bytesPerPixel]);
	qDebug() << "unlock mem" << is_UnlockSeqBuf(cam, NULL, memLast);

	linBufIndex++;
	if(linBufIndex >= sizeLinBuf) {
		QFile file(filename);
		qDebug() << "open file" << file.open(QIODevice::WriteOnly | QIODevice::Append);
		//qDebug() << "write data" << file.write(&linBuf[offset], getWidth()*getHeight()*bytesPerPixel*sizeLinBuf);
		qDebug() << "write data" << file.write(memLast, getWidth()*getHeight()*bytesPerPixel);
		file.close();
		linBufIndex = 0;
		stopCapture();
	}

	return;
}

void QEye::stopCapture() {
	running = false;
}

bool QEye::isRunning() {
	return running;
}

int QEye::exit() {
	disconnect(this, SIGNAL(onNewFrame()));
	stopCapture();
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