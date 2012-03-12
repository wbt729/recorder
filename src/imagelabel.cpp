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
	//qDebug() << width() << img->size().width() << scalingFactor;
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
	//qDebug() << (int) event->pos().x();
    origin = event->pos();
    //if (!rubberBand)
    //    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
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
	//qDebug() << scaledRoi.topLeft().x() << scaledRoi.topLeft().y();
	//qDebug() << scaledRoi.bottomRight().x() << scaledRoi.bottomRight().y();

    //rubberBand->hide();
    // determine selection, for example using QRect::intersects()
    // and QRect::contains().
}