//this version of the QEye Qt-wrapper is meant to be used with static camera parameters only
//meaning the resolution and color mode cannot be changed at runtime, the only parameter that can be
//modified for now is the exposure time

#include "qeye.h"

QEye::QEye() {
	cam = NULL;
	linBuffer = NULL;
	bytesPerPixel = 0;
	bitsPerPixel = 0;
	bitsPerChannel = 0;
	colorChannels = 0;
	width = 0;
	height = 0;
	sizeRingBuffer = 0;
	sizeLinBuffer = 0;
	sizeFrame = 0;
	recording = false;
	running = false;
	useFirstLinBuffer = true;
	linBufferIndex = 0;
	imagesReceived = 0;
}

QEye::~QEye() {
}

//prepare the ring buffer from the uEye API, break if anything goes wrong
//since a half working image queue won't do any good
int QEye::createRingBuffer(int size) {
	sizeRingBuffer = size;
	char *ringBufferCell;
	INT cellId;
	qDebug() << width << height << bitsPerPixel << size;
	for(int i=0; i < sizeRingBuffer; i++) {
		//allocate memory for one single cell of the ring buffer
		if(is_AllocImageMem(cam, width, height, bitsPerPixel, &ringBufferCell, &cellId) != IS_SUCCESS) {
			qDebug("QEye: cannot allocate ring buffer memory, no image queue created");
			return -1;
		}
		//add the newly allocated memory to the ring buffer sequence
		if(is_AddToSequence(cam, ringBufferCell, cellId)) {
			qDebug("QEye: cannot add memory to ring buffer, no image queue created");
			return -1;
		}
	}
	if(is_InitImageQueue(cam, 0)) {
		qDebug("QEye: cannot initialize image queue");
		return -1;
	}
	return 0;
}

//deinitializes the camera
//the grabber thread has to be slaughtered cruelly since
//in the externally triggered mode it gets stuck waiting for the next image
//this is probably not a good way to do this
//a better way might be to use a single software trigger here and then end
//the grabber thread gracefully
int QEye::exit() {
	blockSignals(true);
	is_StopLiveVideo(cam, IS_FORCE_VIDEO_STOP);
	grabberThread->terminate();
	converterThread->quit();
	storageThread->quit();
	is_ExitCamera(cam);
	return 0;
}

//get height from camera settings and update member variable
int QEye::getHeight() {
	IS_RECT AOIRect;
	if(is_AOI(cam, IS_AOI_IMAGE_GET_AOI, &AOIRect, sizeof(AOIRect)) == 0) {
		height = AOIRect.s32Height;
		return height;
	}
	else return -1;
}

//get width from camera settings and update member variable
int QEye::getWidth() {
	IS_RECT AOIRect;
	if(is_AOI(cam, IS_AOI_IMAGE_GET_AOI, &AOIRect, sizeof(AOIRect)) == 0) {
		width = AOIRect.s32Width;
		return width;
	}
	else return -1;
}

double QEye::getExposure() {
	double exposure;
	if (is_Exposure(cam, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&exposure, sizeof(exposure)) == IS_SUCCESS)
		return exposure;
	else
		return -1;
}

//prepares the camera for capturing, break if any of the neccessary steps fail
//the order of the following steps can not be changed, since they depend on one another
int QEye::init(QString fileName, int ringBufferSize, int linBufferSize, int id) {
	if(id != 0) cam |= id;
	if(is_InitCamera(&cam, NULL) != IS_SUCCESS) {	//initialisation
		qDebug("QEye: cannot init camera");
		return -1;
	}
	if(is_LoadParameters(cam, fileName.toLatin1()) != IS_SUCCESS) {	//load parameters from settings file
		qDebug("QEye: cannot load parameters");
		return -1;
	}
	if(getWidth() == -1 || getHeight() == -1) {		//get geometrical resolution
		qDebug("QEye: cannot get image dimensions");
		return -1;
	}
	if(setColorMode(IS_CM_BGR10V2_PACKED) != IS_SUCCESS) {	//set 10 bit color mode
		qDebug() << "QEye: cannot set color mode";
		return -1;
	}

	if(createRingBuffer(ringBufferSize) != IS_SUCCESS) {		//initialize ring buffer
		qDebug("QEye: cannot create ring buffer");
		return -1;
	}

	sizeFrame = width*height*bytesPerPixel;

	//setup helper objects and threads
	grabber = new Grabber(&cam);
	grabberThread = new QThread();
	grabber->moveToThread(grabberThread);
	grabberThread->start();

	converter = new Converter();
	converter->setResolution(width, height, colorChannels, bitsPerChannel, bytesPerPixel);
	converterThread = new QThread();
	converter->moveToThread(converterThread);
	converterThread->start();

	storage = new Storage();
	storageThread = new QThread();
	storage->moveToThread(storageThread);
	storageThread->start();

	//reserve memory and setup linear buffer
	//there are two linear buffers of the same size
	//that get filled and written to the disk alternately
	sizeLinBuffer = linBufferSize;
	linBuffer = new char[2*sizeFrame*sizeLinBuffer];
	useFirstLinBuffer = true;

	//setup communication through qt signal and slots
	makeConnections();

	return 0;
}

//all qt signal and slot connections go here
void QEye::makeConnections() {
	connect(grabber, SIGNAL(newFrame()), this, SLOT(onNewFrame()));
	connect(this, SIGNAL(frameToConvert(char *)), converter, SLOT(makePreview(char *)));
	connect(converter, SIGNAL(newImage(QImage *)), this, SIGNAL(newImage(QImage *)));
	connect(converter, SIGNAL(newMat(cv::Mat *)), this, SIGNAL(newMat(cv::Mat *)));
	connect(this, SIGNAL(linBufferFull(char *, int)), storage, SLOT(saveBuffer(char *, int)));
}

//react to a new incoming frame
//copy to linear buffer, flush linear buffer is full
//and convert image for preview
void QEye::onNewFrame() {
	INT id;
	char *thisFrame, *lastFrame;
	if(is_GetActSeqBuf(cam, &id, &thisFrame, &lastFrame) == IS_SUCCESS) {
		if(recording) {
			int offset = (useFirstLinBuffer ? 0 : sizeLinBuffer*sizeFrame);		//offset to first or second linear buffer
			is_CopyImageMem(cam, lastFrame, NULL, &linBuffer[offset+linBufferIndex*sizeFrame]);
			linBufferIndex++;
			if(linBufferIndex >= sizeLinBuffer) {
				emit linBufferFull(&linBuffer[offset], sizeFrame*sizeLinBuffer);
				linBufferIndex = 0;
				useFirstLinBuffer = !useFirstLinBuffer;
			}
		}

		is_UnlockSeqBuf(cam, NULL, lastFrame);
		emit frameToConvert(lastFrame);
	}
	else {
		qDebug() << "QEye: panic, could not get ring buffer id";
		throw "QEye: panic, could not get ring buffer id";
	}
}


//set color mode, at the moment only 10 bit rgb is supported
int QEye::setColorMode(INT mode) {
	INT ret;
	if(mode == IS_CM_BGR10V2_PACKED) {
		ret = is_SetColorMode(cam, IS_CM_BGR10V2_PACKED);
		qDebug() << ret;
		if(ret == IS_SUCCESS) {
				bitsPerChannel = 10;
				bitsPerPixel = 32;		//3*10bits + 2 padding bits
				bytesPerPixel = 4;
				colorChannels = 3;
				return 0;
		}
		else
			return -1;
	}
	else {
		qDebug() << "QEye:: color mode not supported";
		return -1;
	}
}

//sets exposure, doesn't check yet, if the new value exeeds the 
//maximum exposure for the selected frame rate
//note that the max exposure in external trigger mode is about
// 2/3 of the max frame rate in freerun (software trigger) mode
int QEye::setExposure(double exp) {
	if(exp < 0) {
		qDebug("QEye: cannot set negative exposure");
		return -1;
	}
	INT retVal = is_Exposure(cam, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&exp, sizeof(exp));
	return (int) retVal;
}

//choose trigger mode between hard- and software trigger
int QEye::setExternalTrigger(bool external) {
	if(external) return (int) is_SetExternalTrigger(cam, IS_SET_TRIGGER_LO_HI);
	else return (int) is_SetExternalTrigger(cam, IS_SET_TRIGGER_SOFTWARE);
}


//get number of cameras in network without having to connect
int QEye::countFreeCams() {
	INT ret;
	INT num;
	ret = is_GetNumberOfCameras(&num);
	return (int) num;
}

int QEye::startCapturing() {
	if(is_CaptureVideo(cam, IS_DONT_WAIT) != IS_SUCCESS)
		return -1;
	else
		grabber->start();
	return 0;
}

int QEye::stopCapturing() {
	return 0;
}

int QEye::startRecording() {
	recording = 1;
	return 0;
}

int QEye::stopRecording() {
	recording = 0;
	return 0;
}

//#include "qeye.h"
//
//QEye::QEye(QObject *parent) {
//	cam = NULL;
//	bitsPerPixel = 0;
//	colorMode = 0;
//	imageMemory = NULL;
//	memoryID = NULL;
//	bufferSize = 0;
//	running = false;
//	recording = false;
//	maxPreviewFreq = 20; //Hz;
//	makePreview = true;
//	numImagesReceived = 0;
//	numImagesRecorded = 0;
//	numErrors = 0;
//	bitsPerSample = 0;
//	channels = 0;
//	isConverting = false;
//	bytesPerPixel = 0;
//	width = 0;
//	height = 0;
//
//	grabber = new Grabber(&cam);
//	storage = new Storage();
//	converter = new Converter();
//
//	//setup threads
//	//grabber and storage objects and threads really only should be created, once init was successful
//	//same goes for the ringbuffer
//	grabberThread = new QThread;
//	storageThread = new QThread;
//	converterThread = new QThread;
//
//	grabber->moveToThread(grabberThread);
//	storage->moveToThread(storageThread);
//	converter->moveToThread(converterThread);
//
//	grabberThread->start();
//	storageThread->start();
//	converterThread->start();
//	grabberThread->setPriority(QThread::TimeCriticalPriority);
//
//	//signals and slots should be thought through again
//	connect(converter, SIGNAL(newImage(QImage *)), this, SLOT(onConversionDone(QImage *)));
//	connect(converter, SIGNAL(newImage(QImage *)), this, SIGNAL(newImage(QImage *)));
//	connect(converter, SIGNAL(newMat(cv::Mat *)), this, SIGNAL(newMat(cv::Mat *)));
//	connect(converter, SIGNAL(converting(int, int)), this, SIGNAL(converting(int, int)));
//	connect(grabber, SIGNAL(linBufFull(char *, int)), storage, SLOT(saveLinBuf(char *, int)));
//	connect(this, SIGNAL(starting()), grabber, SLOT(start()));
//	connect(this, SIGNAL(stopping()), grabber, SLOT(stop()));
//	connect(this, SIGNAL(newFrame(char *)), converter, SLOT(charToQImage(char *)));
//	//connect(this, SIGNAL(newFrame(char *)), converter, SLOT(charToCvMat(char *)));
//	connect(grabber, SIGNAL(errors(int)), this, SLOT(onErrors(int)));	//forward error signal
//	connect(grabber, SIGNAL(newFrame(char*)), this, SLOT(onNewFrame(char*)));
//}
//
//void QEye::makeCvMat(bool m) {
//	if(m)
//		connect(this, SIGNAL(newFrame(char *)), converter, SLOT(charToCvMat(char *)));
//	else
//		disconnect(this, SIGNAL(newFrame(char *)), converter, SLOT(charToCvMat(char *)));
//}
//
//
//void QEye::onErrors(int e) {
//	numErrors += e;
//	emit countersChanged(numImagesReceived, numImagesRecorded, numErrors);
//}
//
//void QEye::startRecording() {
//	//dont remove the dump file here, better in the constructor of the storage object, also put it into a local file with a relative path
//	filename = QString("d:\\work\\dump");
//	QFile::remove(filename);
//
//	if(!running)
//		startCapture();
//	recording = true;
//	grabber->startRecording();
//}
//
//void QEye::stopRecording() {
//	recording = false;
//	grabber->stopRecording();
//}
//
//QEye::~QEye() {
//}
//
//int QEye::init(int id) {
//	if(id != 0) cam |= id;
//	int ret = is_InitCamera(&cam, NULL);
//	if(ret) {
//		getWidth();
//		getHeight();
//	}
//	return ret;
//}
//
//int QEye::imagesReceived() {
//	return numImagesReceived;
//}
//
//int QEye::imagesRecorded() {
//	return numImagesRecorded;
//}
//
//int QEye::getHeight() {
//	//qDebug() << "get height";
//	INT retVal = -1;
//	retVal = is_AOI(cam, IS_AOI_IMAGE_GET_AOI, &AOIRect, sizeof(AOIRect));
//	if(retVal == 0) {
//		height = AOIRect.s32Height;
//		return height;
//	}
//	else return -1;
//}
//
//int QEye::getWidth() {
//	//qDebug() << "get width";
//	if(is_AOI(cam, IS_AOI_IMAGE_GET_AOI, &AOIRect, sizeof(AOIRect)) == 0) {
//		width = AOIRect.s32Width;
//		return width;
//	}
//	else return -1;
//}
//
//int QEye::createBuffers(int sizeRing) {
//	bufferSize = sizeRing;
//	imageMemory = new char*[bufferSize];
//	memoryID = new INT[bufferSize];
//	for(int i=0; i < bufferSize; i++) {
//		if(is_AllocImageMem(cam, getWidth(), getHeight(), bitsPerPixel, &imageMemory[i], &memoryID[i])) return -1;	//TODO replace with width height etc from local variable to save bandwith
//		if(is_AddToSequence(cam, imageMemory[i], memoryID[i])) return -1;
//	}
//	is_InitImageQueue(cam, 0);
//	return 0;
//}
//
//void QEye::onNewFrame(char *buf) {
//	numImagesReceived++;
//	if(recording)
//		numImagesRecorded++;
//
//	if(!isConverting && makePreview) {
//		makePreview = false;
//		isConverting = true;
//		emit(newFrame(buf));
//		QTimer::singleShot(1000/maxPreviewFreq, this, SLOT(onPreviewTimer()));
//	}
//	emit countersChanged(numImagesReceived, numImagesRecorded, numErrors);
//	return;
//}
//
//void QEye::onPreviewTimer() {
//	makePreview = true;
//	return;
//}
//
//void QEye::stopCapture() {
//	qDebug() << "QEye stop capture";
//	running = false;
//	emit stopping();
//	return;
//}
//
//void QEye::onConversionDone(QImage *image) {
//	emit newImage(image);
//	isConverting = false;
//	return;
//}
//
//void QEye::convertBlock() {
//	converter->blockToTIFFs();
//	return;
//}
//
//int QEye::setColorMode(INT mode) {
//	switch(mode) {
//		case IS_CM_RGB10V2_PACKED:
//			bitsPerPixel = 32;
//			bytesPerPixel = 4;
//			channels = 3;
//			bitsPerSample = 10;
//			break;
//		case IS_CM_BGRA8_PACKED:	//TODO set other parameters
//			bitsPerPixel = 32;
//			break;
//		case IS_CM_RGB8_PACKED:
//			bitsPerPixel = 24;
//			break;
//		case IS_CM_MONO8:
//			bitsPerPixel = 8;
//			break;
//		case IS_SET_CM_RGB24:
//			bitsPerPixel = 24;
//			break;
//		default:
//			return -1;
//	}
//	bytesPerPixel = bitsPerPixel/8;
//	qDebug() << "color mode" << mode << "bits per pixel" << bitsPerPixel << "bytes per pixel" << bytesPerPixel;
//	return is_SetColorMode(cam, mode);
//}
//
//int	QEye::loadParameters(QString str) {
//	int ret = is_LoadParameters(cam, str.toLatin1());
//	if(ret) {
//		getWidth();
//		getHeight();
//	}
//	return ret;
//}
//
//double QEye::getExposure() {
//	double exposure;
//	is_Exposure(cam, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&exposure, sizeof(exposure));
//	return exposure;
//}
//
//void QEye::setExposure(double exp) {
//	INT retVal = is_Exposure(cam, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&exp, sizeof(exp));
//	return;
//}
//
//int QEye::setTrigger(bool external) {
//	if(external) return is_SetExternalTrigger(cam, IS_SET_TRIGGER_LO_HI);
//	else return is_SetExternalTrigger(cam, IS_SET_TRIGGER_SOFTWARE);
//}
//
//bool QEye::isRunning() {
//	return running;
//}
//
//int QEye::exit() {
//	qDebug() << "QEye: exit";
//	grabber->blockSignals(true);
//	converter->blockSignals(true);
//	storage->blockSignals(true);
//	//disconnect(this, SIGNAL(onNewFrame(int, char*)));
//	grabber->saveErrorIndices();
//	stopCapture();
//	grabberThread->quit();
//	storageThread->quit();
//	converterThread->quit();
//	qDebug() << "QEye: wait for Threads to finish";
//	grabberThread->terminate();	//this is not good, grabber thread is just killed without cleaning up
//	qDebug() << "QEye: grabber thread" << grabberThread->wait();
//	qDebug() << "QEye: storage thread" << storageThread->wait();
//	qDebug() << "QEye: converter thread" << converterThread->wait();
//	return is_ExitCamera(cam);
//}
//
//void QEye::startCapture() {
//	grabber->init(getWidth(), getHeight(), bytesPerPixel);
//	converter->setResolution(getWidth(), getHeight(), channels, bitsPerSample, bytesPerPixel);
//	grabber->blockSignals(false);
//	running = true;
//	emit starting();
//	qDebug() << "start";
//}
//
//int QEye::countFreeCams() {
//	int ret;
//	INT num;
//	ret = is_GetNumberOfCameras(&num);
//	return (int) num;
//}