#include "grabber.h"

Grabber::Grabber(HIDS *camera, QObject *parent) {
	cam = camera;
	running = false;
	recording = false;
	sizeLinBuf = 0;
	linBufIndex = 0;
	useFirstLinBuf = true;
	recording = false;
	imagesRecorded = 0;
	imagesReceived = 0;
	errorIndices.clear();
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
		checkForErrors();
		if(waitForFrame() == 0)
			onNewFrame();
		else {
			emit errors(1);
			errorIndices << imagesReceived;
		}

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
		return WaitForSingleObject(frameEvent, 100000);
	#else if defined LINUX
		return (DWORD) is_WaitEvent(*cam, IS_SET_EVENT_FRAME, 1000);
	#endif
}

void Grabber::onNewFrame() {
	imagesReceived += 1;
	INT id = 0;
	offset = (useFirstLinBuf ? 0 : sizeLinBuf*frameSize);
	is_GetActSeqBuf(*cam, &id, &memAct, &memLast);
	if(recording) {
		imagesRecorded += 1;
		is_CopyImageMem(*cam, memLast, NULL, &linBuf[offset+linBufIndex*frameSize]);
		linBufIndex++;
		if(linBufIndex >= sizeLinBuf) {
			emit linBufFull(&linBuf[offset], frameSize*sizeLinBuf);
			linBufIndex = 0;
			useFirstLinBuf = !useFirstLinBuf;
		}
	}
	is_UnlockSeqBuf(*cam, NULL, memLast);
	emit newFrame(memLast);
}

void Grabber::checkForErrors() {
	UEYE_CAPTURE_STATUS_INFO captureStatusInfo;
	is_CaptureStatus(*cam, IS_CAPTURE_STATUS_INFO_CMD_GET, (void*) &captureStatusInfo, sizeof(captureStatusInfo));		//get ueye api error counter
	if(captureStatusInfo.dwCapStatusCnt_Total) {
		emit errors(captureStatusInfo.dwCapStatusCnt_Total);
		is_CaptureStatus(*cam, IS_CAPTURE_STATUS_INFO_CMD_RESET, NULL, 0);		//reset ueye api error counter
		errorIndices << imagesReceived;
	}
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

void Grabber::stop() {
	running = false;
	is_DisableEvent(*cam, IS_SET_EVENT_FRAME);
	return;
}

void Grabber::saveErrorIndices() {
	QFile file("d:\\work\\errors.txt");
	if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);
		for(int i=0; i<errorIndices.size(); i++) {
			out << errorIndices.at(i) << "\n";
		}
		file.close();
	}
}