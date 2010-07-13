#ifndef ACQMAN_CAMWIDGET_H_
#define ACQMAN_CAMWIDGET_H_

#include <QWidget>
#include <QComboBox>

#include <mediaobject.h>
#include <videowidget.h>
#include <QPaintEvent>
#include <QPainter>

#include <QDebug>

/// A replacement for Phonon::VideoWidget that can display a cross-hair and emit signals when clicked in a specific position.

class AMCrosshairVideoWidget : public Phonon::VideoWidget {

	Q_OBJECT

public:
	AMCrosshairVideoWidget(QWidget* parent = 0)
		: Phonon::VideoWidget(parent) {

		crosshairX_ = 0.5;
		crosshairY_ = 0.5;
		crosshairPen_ = QPen(QColor(Qt::red));
		crosshairVisible_ = true;
	}

public:
	QPointF crosshairPos() const { return QPointF(crosshairX_, crosshairY_); }
	QPen crosshairPen() const { return crosshairPen_; }
	bool crosshairVisible() const { return crosshairVisible_; }

public slots:
	void setCrosshairPos(const QPointF& pos) {
		crosshairX_ = pos.x();
		crosshairY_ = pos.y();
	}

	void setCrosshairPen(const QPen& pen) {
		crosshairPen_ = pen;
	}

	void showCrosshair(bool crosshairVisible = true) {
		crosshairVisible_ = crosshairVisible;
	}

signals:
	void videoClicked(const QPointF& position);

protected:
	double crosshairX_, crosshairY_;
	QPen crosshairPen_;
	bool crosshairVisible_;

	virtual void paintEvent(QPaintEvent *event) {
		Phonon::VideoWidget::paintEvent(event);

		if(crosshairVisible_) {
			qDebug() << "drawing line...";
			QPainter painter(this);
			painter.setPen(crosshairPen_);
			painter.drawLine(QPointF(width()*crosshairX_, 0), QPointF(width()*crosshairX_, height()));
			painter.drawLine(QPointF(0, height()*crosshairY_), QPointF(width(), height()*crosshairY_));
			painter.end();
		}
	}

	//virtual void

};

// A widget that provides a view onto multiple Axis-server MJPEG streams,
// using the Phonon multimedia library

class CamWidget : public QWidget {
	
	Q_OBJECT
	
public:
	CamWidget(const QString& cameraName, const QUrl& cameraAddress, QWidget* parent = 0);
	virtual ~CamWidget();
	
	void setupVideo();
	
	void addSource(const QString& cameraName, const QUrl& cameraAddress);
	
public slots:

	void onSourceChanged(int index);
	
	
protected:
	AMCrosshairVideoWidget* videoWidget_;
	Phonon::MediaObject* mediaObject_;
	
	QComboBox* cameraList_;
	
	
};

#endif /*CAMWIDGET_H_*/
