#include "imagelabel.h"

ImageLabel::ImageLabel(QObject *parent) {
	scalingFactor = 0;
	setFixedSize(QSize(400,300));
	origin = QPoint();
	rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
	setAlignment(Qt::AlignTop);
}

ImageLabel::~ImageLabel() {
}

void ImageLabel::setImage(QImage *img) {
	scalingFactor = (double) width()/img->size().width();
	pixmap.convertFromImage(*img);
	pixmap = pixmap.scaledToWidth(width());
	setPixmap(pixmap);
}

void ImageLabel::wheelEvent(QWheelEvent *event) {
	int numDegrees = event->delta()/8;
	int numSteps = numDegrees/15;
	emit mouseWheelSteps(numSteps);
}

void ImageLabel::mousePressEvent(QMouseEvent *event) {
    origin = event->pos();
    rubberBand->setGeometry(QRect(origin, QSize()));
    rubberBand->show();
}

void ImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *event)
{
	QRect roi = rubberBand->frameGeometry();
	QRect scaledRoi; //
	scaledRoi.setX(roi.x()/scalingFactor);
	scaledRoi.setY(roi.y()/scalingFactor);
	scaledRoi.setWidth(roi.width()/scalingFactor);
	scaledRoi.setHeight(roi.height()/scalingFactor);

	emit newRoi(scaledRoi);
}