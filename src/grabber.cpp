#include "grabber.h"

Grabber::Grabber(HIDS *camera) {
	cam = camera;
	timeout = 4* 3600 * 1000;	//four hours
	#if defined _WIN64 || defined _WIN32
	frameEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	qDebug() << "init frame event" << is_InitEvent(*cam, frameEvent, IS_SET_EVENT_FRAME);
	#endif
	qDebug() << "enable frame event" << is_EnableEvent(*cam, IS_SET_EVENT_FRAME);
}

Grabber::~Grabber() {
	is_ExitEvent(*cam, IS_SET_EVENT_FRAME);
}

int Grabber::start() {
	qDebug("Grabber: start capturing");
	QTimer::singleShot(0, this, SLOT(grab()));
	return 0;
}

void Grabber::grab() {
		QTime timer;
		timer.start();
		checkForErrors();

		if(waitForFrame() == 0) {
			//qDebug() << "new Frame";
			emit newFrame();
		}
		else
			emit errors(1);

		QTimer::singleShot(0, this, SLOT(grab())); //set timer to call next grab
		return;
}


DWORD Grabber::waitForFrame() {
	#if defined _WIN64 || defined _WIN32
		return WaitForSingleObject(frameEvent, timeout);
	#else if defined LINUX
		return (DWORD) is_WaitEvent(*cam, IS_SET_EVENT_FRAME, timeout);
	#endif
}

void Grabber::checkForErrors() {
	UEYE_CAPTURE_STATUS_INFO captureStatusInfo;
	is_CaptureStatus(*cam, IS_CAPTURE_STATUS_INFO_CMD_GET, (void*) &captureStatusInfo, sizeof(captureStatusInfo));		//get ueye api error counter
	if(captureStatusInfo.dwCapStatusCnt_Total) {
		emit errors(captureStatusInfo.dwCapStatusCnt_Total);
		is_CaptureStatus(*cam, IS_CAPTURE_STATUS_INFO_CMD_RESET, NULL, 0);		//reset ueye api error counter
	}
}