#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QObject>
#include <QLabel>
#include <QWheelEvent>

class ImageLabel : public QLabel
{
	Q_OBJECT

public:
	ImageLabel(QObject *parent);
	~ImageLabel();
private:
	QPixmap pixmap;

public slots:
	void setImage(QImage *);

protected:
	virtual void wheelEvent(QWheelEvent *);
signals:
	void mouseWheelSteps(int);
};

#endif // IMAGELABEL_H
