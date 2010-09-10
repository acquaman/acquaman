#ifndef AMBEAMLINECAMERAWIDGET_H
#define AMBEAMLINECAMERAWIDGET_H

#include <QWidget>
#include <QComboBox>
#include "ui/AMVideoPlayerWidget.h"

#include <QPaintEvent>
#include <QPainter>

#include <QDebug>

/// This class is an upgrade for AMVideoPlayerWidget, that can display a cross-hair and emit signals when clicked in a specific position.  The cross hair position and click locations are reported as a percentage of the video height and width, from 0 to 1.
class AMCrosshairVideoWidget : public AMVideoPlayerWidget {

	Q_OBJECT

public:
	AMCrosshairVideoWidget(QWidget* parent = 0);

public:
	QPointF crosshairPosition() const { return QPointF(crosshairX_, crosshairY_); }
	QPen crosshairPen() const { return crosshairPen_; }
	bool crosshairVisible() const { return crosshairVisible_; }

public slots:
	void setCrosshairPosition(const QPointF& pos) {
		crosshairX_ = pos.x();
		crosshairY_ = pos.y();
		update();
	}

	void setCrosshairPen(const QPen& pen) {
		crosshairPen_ = pen;
		update();
	}

	void setCrossHairVisible(bool crosshairVisible = true) {
		crosshairVisible_ = crosshairVisible;
		update();
	}

signals:
	/// Emitted when the left mouse button is pressed down. (The position is reported as percentage of the video screen width and height; ie: from 0 to 1)
	void mousePressed(const QPointF& position);
	/// Emitted when the left mouse is released. (The position is reported as percentage of the video screen width and height; ie: from 0 to 1).  Not emitted for the second click of a double-click; see mouseDoubleClicked() instead.
	void mouseReleased(const QPointF& position);
	/// Emitted when the left mouse button is released from a double-click. (The position is reported as percentage of the video screen width and height; ie: from 0 to 1).
	void mouseDoubleClicked(const QPointF& position);

protected:
	double crosshairX_, crosshairY_;
	QPen crosshairPen_;
	bool crosshairVisible_;

	/// Re-implemented to draw the cross-hair
	virtual void paintEvent(QPaintEvent *event);
	/// Re-implemented to emit signals for mouse events
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);

	/// Indicates that the second press of a double-click has happened
	bool doubleClickInProgress_;
};






/// A widget that provides a view onto multiple Axis-server MJPEG streams (uses AMVideoPlayerWidget, which uses the VLC video library)
class AMBeamlineCameraWidget : public QWidget {

	Q_OBJECT

public:
	AMBeamlineCameraWidget(const QString& cameraName, const QUrl& cameraAddress, QWidget* parent = 0);
	virtual ~AMBeamlineCameraWidget();

	void setupVideo();

	void addSource(const QString& cameraName, const QUrl& cameraAddress);

public slots:

	void onSourceChanged(int index);

protected slots:
	void debugMousePress(const QPointF& pos) {
		qDebug() << "MOUSE PRESSED at " << pos;
	}
	void debugMouseRelease(const QPointF& pos) {
		qDebug() << "MOUSE RELEASED at " << pos;
	}
	void debugMouseDoubleClicked(const QPointF& pos) {
		qDebug() << "MOUSE DOUBLE CLICKED at " << pos;
		videoWidget_->setCrosshairPosition(pos);
	}


protected:

	AMCrosshairVideoWidget* videoWidget_;
	QComboBox* cameraList_;


};

#endif /*AMBEAMLINECAMERAWIDGET_H*/
