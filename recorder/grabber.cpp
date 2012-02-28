#include "grabber.h"

Grabber::Grabber(QObject *parent, HIDS *camera, bool *state) {
	cam = camera;
	capturing = state;
}

Grabber::~Grabber() {

}

void Grabber::start() {
	//is_SetSensorTestImage(*cam, IS_TEST_IMAGE_BLACK, NULL;)
	//is_SetSensorTestImage(*cam, IS_TEST_IMAGE_WHITE, NULL);
	//is_SetSensorTestImage(*cam, IS_TEST_IMAGE_WEDGE_GRAY_SENSOR, NULL);
	//
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	qDebug() << "initEvent" << is_InitEvent(*cam, hEvent, IS_SET_EVENT_FRAME);
	qDebug() << "enable Event" << is_EnableEvent(*cam, IS_SET_EVENT_FRAME);
	//
	int index = 0;
	int ret = 0;
	DWORD dRet = 0;
	UEYE_CAPTURE_STATUS_INFO captureStatusInfo;
	is_InitEvent(*cam, hEvent, IS_SET_EVENT_FRAME);
	qDebug() << "start capturing" << is_CaptureVideo(*cam, IS_DONT_WAIT);
	do {
		dRet = WaitForSingleObject(hEvent, 2000);
		if(dRet == WAIT_OBJECT_0) {
			qDebug() << "wait object ret" << dRet;
			emit newFrame();
			index++;
		}
		else 
			emit errors(1);
	}
	while(*capturing);
	qDebug() << "stopping video" << is_StopLiveVideo(*cam, IS_WAIT);
	is_DisableEvent(*cam, IS_SET_EVENT_FRAME);
	qDebug("done");
	return;
}

void Grabber::stop() {
	return;
}