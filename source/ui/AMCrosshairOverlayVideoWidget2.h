#ifndef AMCROSSHAIROVERLAYVIDEOWIDGET2_H
#define AMCROSSHAIROVERLAYVIDEOWIDGET2_H

#include "ui/AMOverlayVideoWidget2.h"

class QGraphicsLineItem;
class AMShapeOverlayVideoWidgetModel2;

/// This class adds the capability of drawing a crosshair on top of an AMOverlayVideoWidget.
/*! The crosshair position is configurable using setCrosshairPosition() as a fraction of the video size, and referenced over top of the video, taking into account the proper aspect ratio and scaling/letterboxing.  Not only that, but you can observe the user's mouse interaction with the video display, via signals for mousePressed(), mouseReleased(), etc., which provide click positions in the same coordinate system.

For fun, you can connect the mouseDoubleClicked() signal to the setCrosshairPosition() slot to allow the user to re-position the crosshair by double-clicking. */

class AMCrosshairOverlayVideoWidget2 : public AMOverlayVideoWidget2
{
	Q_OBJECT
public:
	/// Constructor.
    explicit AMCrosshairOverlayVideoWidget2(QWidget *parent = 0, bool usOpenGlViewport = true);

	/// Returns the current pen used to draw the crosshair lines
	QPen crosshairPen() const;
	/// Returns the current position of the crosshair, in normalized coordinates. (ex: 0,0 = top left; 1,1 = bottom right)
	QPointF crosshairPosition() const { return QPointF(crosshairX_, crosshairY_); }
	/// Returns whether the crosshair is currently visible
	bool crosshairVisible() const;

    enum selectMode{DRAW, MOVE, EDIT};
    void setDrawMode();
    void setMoveMode();
    void setEditMode();


signals:
	/// Emitted when the left mouse button is pressed down. (The position is reported as percentage of the video screen width and height; ie: from 0 to 1)
	void mousePressed(const QPointF& position);
	/// Emitted when the left mouse is released. (The position is reported as percentage of the video screen width and height; ie: from 0 to 1).  Not emitted for the second click of a double-click; see mouseDoubleClicked() instead.
	void mouseReleased(const QPointF& position);
	/// Emitted when the left mouse button is released from a double-click. (The position is reported as percentage of the video screen width and height; ie: from 0 to 1).
	void mouseDoubleClicked(const QPointF& position);

    /// Emitted when the mouse moves - in shapeoverlay
    void mouseMoved(const QPointF& position);
    /// Emitted when the right mouse button is pressed down
    void mouseRightClicked(const QPointF& position);
    /// Emitted when the left mouse is pressed down, in move mode
    void mouseMovePressed(const QPointF& position);
    void mouseMovedMoveMode(const QPointF& position);


public slots:

	/// Set the crosshair position on the video display, using normalized coordinates. (ex: 0,0 = top left; 1,1 = bottom right)
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

    QMap<int,QGraphicsRectItem*> rectangle_;
    AMShapeOverlayVideoWidgetModel2* shapeModel_;
    int index_;
    selectMode mode_;

    void addNewRectangle();
    void deleteRectangle();




	/// Re-implemented to emit signals for mouse events
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);


    /// added mouse move event
    virtual void mouseMoveEvent(QMouseEvent *e);

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

#endif // AMCROSSHAIROVERLAYVIDEOWIDGET2_H
