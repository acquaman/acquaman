/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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






/// A widget that provides a view onto multiple Axis-server MJPEG streams (uses AMVideoPlayerWidget, which uses the VLC video library).  Pauses the video stream when hidden to save CPU consumption.
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

	/// (Stop) the video stream when hidden (to save CPU usage)
	virtual void hideEvent(QHideEvent *e) {
		QWidget::hideEvent(e);
		videoWidget_->stop();
	}

	/// (Play) the video stream when un-hidden
	virtual void showEvent(QShowEvent *e) {
		QWidget::showEvent(e);
		videoWidget_->play();
	}


};


#endif /*AMBEAMLINECAMERAWIDGET_H*/
