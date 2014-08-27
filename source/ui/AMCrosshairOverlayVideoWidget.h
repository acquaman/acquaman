/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMCROSSHAIROVERLAYVIDEOWIDGET_H
#define AMCROSSHAIROVERLAYVIDEOWIDGET_H

#include "ui/AMOverlayVideoWidget.h"

class QGraphicsLineItem;

/// This class adds the capability of drawing a crosshair on top of an AMOverlayVideoWidget.
/*! The crosshair position is configurable using setCrosshairPosition() as a fraction of the video size, and referenced over top of the video, taking into account the proper aspect ratio and scaling/letterboxing.  Not only that, but you can observe the user's mouse interaction with the video display, via signals for mousePressed(), mouseReleased(), etc., which provide click positions in the same coordinate system.

For fun, you can connect the mouseDoubleClicked() signal to the setCrosshairPosition() slot to allow the user to re-position the crosshair by double-clicking. */

class AMCrosshairOverlayVideoWidget : public AMOverlayVideoWidget
{
	Q_OBJECT
public:
	/// Constructor.
 	virtual ~AMCrosshairOverlayVideoWidget();
	explicit AMCrosshairOverlayVideoWidget(QWidget *parent = 0, bool usOpenGlViewport = true);

	/// Returns the current pen used to draw the crosshair lines
	QPen crosshairPen() const;
	/// Returns the current position of the crosshair, in normalized coordinates. (ex: 0,0 = top left 1,1 = bottom right)
	QPointF crosshairPosition() const { return QPointF(crosshairX_, crosshairY_); }
	/// Returns whether the crosshair is currently visible
	bool crosshairVisible() const;

signals:
	/// Emitted when the left mouse button is pressed down. (The position is reported as percentage of the video screen width and height ie: from 0 to 1)
	void mousePressed(const QPointF& position);
	/// Emitted when the left mouse is released. (The position is reported as percentage of the video screen width and height ie: from 0 to 1).  Not emitted for the second click of a double-click. See mouseDoubleClicked() instead.
	void mouseReleased(const QPointF& position);
	/// Emitted when the left mouse button is released from a double-click. (The position is reported as percentage of the video screen width and height ie: from 0 to 1).
	void mouseDoubleClicked(const QPointF& position);

public slots:

	/// Set the crosshair position on the video display, using normalized coordinates. (ex: 0,0 = top left 1,1 = bottom right)
	void setCrosshairPosition(const QPointF& pos) {
		crosshairX_ = pos.x();
		crosshairY_ = pos.y();
		reviewCrosshairLinePositions();
	}

	/// Set the pen used to draw the crosshair lines
	void setCrosshairPen(const QPen& pen);

	/// Enable or disable the crosshair
	void setCrosshairVisible(bool crosshairVisible = true);


protected:
	double crosshairX_, crosshairY_;
	QGraphicsLineItem* crosshairXLine_, *crosshairYLine_;

	/// Re-implemented to emit signals for mouse events
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);

	/// Indicates that the second press of a double-click has happened
	bool doubleClickInProgress_;


	/// We catch resize events and re-position the crosshair lines as required
	void resizeEvent(QResizeEvent *event);

	/// Helper function to convert scene coordinates to video-relative coordinates. (This is only tricky because depending on the videoItem()'s aspectRatioMode(), there might be letterbox bars at the top or sides of the display.
	QPointF mapSceneToVideo(const QPointF& sceneCoordinate) const;

protected slots:
	/// Helper function to (re-)position the crosshair lines when the view is resized or the lines are moved
	void reviewCrosshairLinePositions();
};

#endif // AMCROSSHAIROVERLAYVIDEOWIDGET_H
