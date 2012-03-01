#include "grabber.h"

Grabber::Grabber(HIDS *camera, QObject *parent) {
	cam = camera;
	running = false;
	sizeLinBuf = 0;
	linBufIndex = 0;
	useFirstLinBuf = true;
	absFrameIndex = 0;
}

Grabber::~Grabber() {
}

void Grabber::init(int w, int h, int bpp) {
	sizeLinBuf = 100;
	bytesPerPixel = bpp;
	width = w;
	height = h;
	frameEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	linBuf = new char[2*sizeLinBuf*width*height*bytesPerPixel];
	qDebug() << "init frame event" << is_InitEvent(*cam, frameEvent, IS_SET_EVENT_FRAME);
	qDebug() << "enable frame event" << is_EnableEvent(*cam, IS_SET_EVENT_FRAME);
}

void Grabber::start() {

	running = true;
	qDebug() << "start capturing" << is_CaptureVideo(*cam, IS_DONT_WAIT);
	QTimer::singleShot(0, this, SLOT(grab()));
}

void Grabber::grab() {
	QTime timer;
	timer.start();
	//qDebug() << "grab";
	DWORD dRet = WaitForSingleObject(frameEvent, 2000);


	if(dRet == WAIT_OBJECT_0) {
		//new image received
		//qDebug() << "new Frame";
		INT id = 0;
		offset = (useFirstLinBuf ? 0 : sizeLinBuf*width*height*bytesPerPixel);
		is_GetActSeqBuf(*cam, &id, &memAct, &memLast);

		is_CopyImageMem(*cam, memLast, NULL, &linBuf[offset+linBufIndex*width*height*bytesPerPixel]);
		is_UnlockSeqBuf(*cam, NULL, memLast);
		emit newFrame(&linBuf[offset+linBufIndex*width*height*bytesPerPixel]);
		linBufIndex++;
		absFrameIndex++;
		qDebug() << "absFrameIndex" << absFrameIndex;
		if(linBufIndex >= sizeLinBuf) {
			emit linBufFull(&linBuf[offset], width*height*bytesPerPixel*sizeLinBuf);
			linBufIndex = 0;
			useFirstLinBuf = !useFirstLinBuf;
		}
	}
	else
		emit errors(1);

	if(running) {
		QTimer::singleShot(0, this, SLOT(grab()));
	}
	else {
		qDebug() << "stopping video" << is_StopLiveVideo(*cam, IS_WAIT);
	}
	qDebug() << "processing elapsed time:" << timer.elapsed() << "ms";
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
	return;
}