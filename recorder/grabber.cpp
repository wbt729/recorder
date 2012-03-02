#include "grabber.h"

Grabber::Grabber(HIDS *camera, QObject *parent) {
	cam = camera;
	running = false;
	recording = false;
	sizeLinBuf = 0;
	linBufIndex = 0;
	useFirstLinBuf = true;
	absFrameIndex = 0;
	recording = false;
}

Grabber::~Grabber() {
	delete linBuf;
	is_ExitEvent(*cam, IS_SET_EVENT_FRAME);
}

void Grabber::init(int w, int h, int bpp) {
	sizeLinBuf = 100;
	bytesPerPixel = bpp;
	width = w;
	height = h;
	frameSize = width*height*bytesPerPixel;
	//linBuf = new char[2*sizeLinBuf*width*height*bytesPerPixel];
	linBuf = new char[2*sizeLinBuf*frameSize];
#if defined _WIN64 || defined _WIN32
	frameEvent = CreateEvent(NULL, FALSE, FALSE, NULL); //Linux
	qDebug() << "init frame event" << is_InitEvent(*cam, frameEvent, IS_SET_EVENT_FRAME);
#endif
	qDebug() << "enable frame event" << is_EnableEvent(*cam, IS_SET_EVENT_FRAME);
}

void Grabber::start() {
	running = true;
	qDebug() << "start capturing" << is_CaptureVideo(*cam, IS_DONT_WAIT);
	QTimer::singleShot(0, this, SLOT(grab()));
}

void Grabber::grab() {
	if(running) {
		QTime timer;
		timer.start();

		if(waitForFrame() == 0)
			onNewFrame();
		else
			onError();

		QTimer::singleShot(0, this, SLOT(grab())); //set timer to call next grab
		//qDebug() << "processing elapsed time:" << timer.elapsed() << "ms";
		return;
	}
	else {
		qDebug() << "stopping video" << is_StopLiveVideo(*cam, IS_WAIT);
		return;
	}
}

DWORD Grabber::waitForFrame() {
	#if defined _WIN64 || defined _WIN32
		return WaitForSingleObject(frameEvent, 1000);
	#else if defined LINUX
		return (DWORD) is_WaitEvent(*cam, IS_SET_EVENT_FRAME, 1000);
	#endif
}

void Grabber::onError() {
	emit errors(1);
}

void Grabber::onNewFrame() {
	INT id = 0;
	offset = (useFirstLinBuf ? 0 : sizeLinBuf*frameSize);
	is_GetActSeqBuf(*cam, &id, &memAct, &memLast);
	if(recording) {
		is_CopyImageMem(*cam, memLast, NULL, &linBuf[offset+linBufIndex*frameSize]);
		linBufIndex++;
		if(linBufIndex >= sizeLinBuf) {
			emit linBufFull(&linBuf[offset], frameSize*sizeLinBuf);
			linBufIndex = 0;
			useFirstLinBuf = !useFirstLinBuf;
		}
	}
	is_UnlockSeqBuf(*cam, NULL, memLast);
	absFrameIndex++;
	emit newFrame(absFrameIndex, memLast);

}

void Grabber::startRecording() {
	qDebug() << "start Recording";
	linBufIndex = 0;
	useFirstLinBuf = true;
	recording = true;
	return;
}

void Grabber::stopRecording() {
	qDebug() << "stop Recording";
	recording = false;
	return;
}

//void Grabber::start() {
//	//is_SetSensorTestImage(*cam, IS_TEST_IMAGE_BLACK, NULL;)
//	//is_SetSensorTestImage(*cam, IS_TEST_IMAGE_WHITE, NULL);
//	//is_SetSensorTestImage(*cam, IS_TEST_IMAGE_WEDGE_GRAY_SENSOR, NULL;)
//	//is_SetErrorReport(*cam, IS_ENABLE_ERR_REP);
//	//
//	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
//	qDebug() << "initEvent" << is_InitEvent(*cam, hEvent, IS_SET_EVENT_FRAME);
//	qDebug() << "enable Event" << is_EnableEvent(*cam, IS_SET_EVENT_FRAME);
//	//
//	int index = 0;
//	int ret = 0;
//	DWORD dRet = 0;
//	UEYE_CAPTURE_STATUS_INFO captureStatusInfo;
//	is_InitEvent(*cam, hEvent, IS_SET_EVENT_FRAME);
//	qDebug() << "start capturing" << is_CaptureVideo(*cam, IS_DONT_WAIT);
//	do {
//		is_CaptureStatus(*cam, IS_CAPTURE_STATUS_INFO_CMD_GET, (void*) &captureStatusInfo, sizeof(captureStatusInfo));
//		if(captureStatusInfo.dwCapStatusCnt_Total) emit errors(captureStatusInfo.dwCapStatusCnt_Total);
//		if(captureStatusInfo.dwCapStatusCnt_Total > 2) {
//			qDebug() << "panic";
//		}
//		is_CaptureStatus(*cam, IS_CAPTURE_STATUS_INFO_CMD_RESET, (void*) &captureStatusInfo, sizeof(captureStatusInfo));
//		dRet = WaitForSingleObject(hEvent, 2000);
//		if(dRet == WAIT_OBJECT_0) {
//			qDebug() << "wait object ret" << dRet;
//			emit newFrame();
//			index++;
//		}
//		else 
//			emit errors(1);
//	}
//	while(*capturing);
//	qDebug() << "stopping video" << is_StopLiveVideo(*cam, IS_WAIT);
//	is_DisableEvent(*cam, IS_SET_EVENT_FRAME);
//	qDebug("done");
//	return;
//}

void Grabber::stop() {
	running = false;
	is_DisableEvent(*cam, IS_SET_EVENT_FRAME);
	return;
}
