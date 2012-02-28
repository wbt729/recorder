#include "imagelabel.h"

ImageLabel::ImageLabel(QObject *parent) {
	setFixedSize(QSize(800,600));
}

ImageLabel::~ImageLabel() {
}

void ImageLabel::setImage(QImage *img) {
	pixmap.convertFromImage(*img);
	//pixmap = pixmap.scaledToHeight(800);
	setPixmap(pixmap);
}

void ImageLabel::wheelEvent(QWheelEvent *event) {
	int numDegrees = event->delta()/8;
	int numSteps = numDegrees/15;
	emit mouseWheelSteps(numSteps);
}