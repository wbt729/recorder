//this version of the QEye Qt-wrapper is meant to be used with static camera parameters only
//meaning the resolution and color mode cannot be changed at runtime, the only parameter that can be
//modified for now is the exposure time

//TODO
//get and save time stamps
//log errors
//add block conversion for 8bit grayvalue images
//delete dump file at start of recording or even better create new dump file in designated dump file place

#include "qeye.h"

QEye::QEye() {
	qRegisterMetaType<ULongLongVector>("ULongLongVector");
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
	bytesPerFrame = 0;
	recording = false;
	running = false;
	useFirstLinBuffer = true;
	linBufferIndex = 0;
	numImagesReceived = 0;
	numImagesRecorded = 0;
	numErrors = 0;

	storageThread = new QThread();
	grabberThread = new QThread();
	converterThread = new QThread();
}

QEye::~QEye() {
}

int QEye::convertBlock() {
	converter->blockToTIFFs();
	return 0;
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
	if(grabberThread->isRunning()) {
		qDebug() << "QEye: terminate grabber thread";
		grabberThread->terminate();
	}
	if(converterThread->isRunning())
		converterThread->quit();
	if(storageThread->isRunning())
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
	if (is_Exposure(cam, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&exposure, sizeof(exposure)) == IS_SUCCESS) {
		qDebug() << "QEye: exposure" << exposure;
		return exposure;
	}
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
		qDebug("QEye: cannot load parameters");						//function depreciated by IDS, should be replaced
		qDebug() << fileName.toLatin1();
		return -1;
	}
	if(getWidth() == -1 || getHeight() == -1) {		//get geometrical resolution
		qDebug("QEye: cannot get image dimensions");
		return -1;
	}
	if(getColorMode() != IS_SUCCESS) {
		qDebug() << "QEye: cannot get color mode";
		return -1;
	}

	if(createRingBuffer(ringBufferSize) != IS_SUCCESS) {		//initialize ring buffer
		qDebug("QEye: cannot create ring buffer");
		return -1;
	}

	bytesPerFrame = width*height*bytesPerPixel;

	//setup helper objects and threads
	grabber = new Grabber(&cam);
	grabber->moveToThread(grabberThread);
	grabberThread->start();

	converter = new Converter();
	converter->setResolution(width, height, colorChannels, bitsPerChannel, bytesPerPixel);
	converter->moveToThread(converterThread);
	converterThread->start();

	storage = new Storage();
	storage->moveToThread(storageThread);
	storageThread->start();

	//reserve memory and setup linear buffer
	//there are two linear buffers of the same size
	//that get filled and written to the disk alternately
	sizeLinBuffer = linBufferSize;
	linBuffer = new char[2*bytesPerFrame*sizeLinBuffer];
	useFirstLinBuffer = true;
	//timestamps = QVector<int>(sizeLinBuffer, 0);
	timestamps = ULongLongVector(sizeLinBuffer, 0);

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
	connect(this, SIGNAL(linBufferFull(char *, int, int, ULongLongVector)), storage, SLOT(saveBuffer(char *, int, int, ULongLongVector)));
}

int QEye::getColorMode() {
	int ret = is_SetColorMode(cam, IS_GET_COLOR_MODE);
	switch(ret) {
		case IS_SET_CM_RGB30:
			bitsPerChannel = 10;
			bitsPerPixel = 32;		//3*10bits + 2 padding bits
			bytesPerPixel = 4;
			colorChannels = 3;
			return 0;
		case IS_CM_MONO8:
			bitsPerChannel = 8;
			bitsPerPixel = 8;
			bytesPerPixel = 1;
			colorChannels = 1;
			return 0;
		default:
			return -1;
	}
	return 0;
}

//react to a new incoming frame
//copy to linear buffer, flush linear buffer is full
//and convert image for preview
void QEye::onNewFrame() {
	INT id;
	char *thisFrame, *lastFrame;
	if(is_GetActSeqBuf(cam, &id, &thisFrame, &lastFrame) == IS_SUCCESS) {
		if(recording) {
			//get image info of the previous image (therefore id-1) to get the timestamp
			UEYEIMAGEINFO imageInfo;
			INT ret = is_GetImageInfo(cam, id-1, &imageInfo, sizeof(imageInfo)); 
			if(ret == IS_SUCCESS) {
				unsigned long long u64Timestamp= imageInfo.u64TimestampDevice;
				qDebug() << u64Timestamp;
				timestamps.replace(linBufferIndex, u64Timestamp);
			}
			else
				timestamps.replace(linBufferIndex, -1);
			int offset = (useFirstLinBuffer ? 0 : sizeLinBuffer*bytesPerFrame);		//offset to first or second linear buffer
			is_CopyImageMem(cam, lastFrame, NULL, &linBuffer[offset+linBufferIndex*bytesPerFrame]);
			linBufferIndex++;
			if(linBufferIndex >= sizeLinBuffer) {
				emit linBufferFull(&linBuffer[offset], bytesPerFrame, sizeLinBuffer, timestamps);
				linBufferIndex = 0;
				useFirstLinBuffer = !useFirstLinBuffer;
				timestamps.fill(0);
			}
		}
		is_UnlockSeqBuf(cam, NULL, lastFrame);
		emit frameToConvert(lastFrame);
		numImagesReceived += 1;
		if(recording)
			numImagesRecorded += 1;
		emit transferCountersChanged(numImagesReceived, numErrors, numImagesRecorded);
	}
	else {
		qDebug() << "QEye: panic, could not get ring buffer id";
		throw "QEye: panic, could not get ring buffer id";
	}
}


//set color mode
//automatically select best mode for this camera
int QEye::setColorMode() {
	//int ret;
	//if(mode == is_cm_bgr10v2_packed) {
	//	ret = is_setcolormode(cam, is_cm_bgr10v2_packed);
	//	qdebug() << ret;
	//	if(ret == is_success) {
	//			bitsperchannel = 10;
	//			bitsperpixel = 32;		//3*10bits + 2 padding bits
	//			bytesperpixel = 4;
	//			colorchannels = 3;
	//			return 0;
	//	}
	//	else
	//		return -1;
	//}
	//else {
	//	qdebug() << "qeye:: color mode not supported";
	//	return -1;
	//}
	return 0;
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
	getExposure();
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

void QEye::onErrors(int e) {
	numErrors += e;
}