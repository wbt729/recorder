#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QObject>
#include <QDebug>
#include <QLabel>
#include <QWheelEvent>
#include <QRubberBand>

class ImageLabel : public QLabel
{
	Q_OBJECT

public:
	ImageLabel(QObject *parent);
	~ImageLabel();
private:
	double scalingFactor;
	QPixmap pixmap;
	QPoint origin;
	QRubberBand *rubberBand;

public slots:
	void setImage(QImage *);

protected:
	virtual void wheelEvent(QWheelEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);

signals:
	void mouseWheelSteps(int);
	void newRoi(QRect);
};

#endif // IMAGELABEL_H
