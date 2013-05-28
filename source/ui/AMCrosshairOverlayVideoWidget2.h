#ifndef AMCROSSHAIROVERLAYVIDEOWIDGET2_H
#define AMCROSSHAIROVERLAYVIDEOWIDGET2_H

#include "ui/AMOverlayVideoWidget2.h"

class QGraphicsLineItem;
class AMShapeOverlayVideoWidgetModel2;
class AMCameraConfigurationModel;

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

    /// Used to set the current mode selected on the toolbar
    enum selectMode{DRAW, MOVE, EDIT, SHIFT, OPERATION, GROUP};
    void setDrawMode();
    void setMoveMode();
    void setEditMode();
    void setShiftMode();
    void setOperationMode();
    void setGroupMode();

    /// Set the current motor coordinates
    void setMotorCoordinate(double x, double y, double z, double r);
    double motorRotation();
    double motorX();
    double motorY();
    double motorZ();

    /// Functions for modifying object information
    QString currentName();
    void setCurrentName(QString);
    QString currentInfo();
    void setCurrentInfo(QString);

    /// Get the index of the currently selected object
    int currentIndex();

    double xCoordinate();
    double yCoordinate();
    double zCoordinate();
    double rotation();
    double tilt();

    void setX(double x);
    void setY(double y);
    void setZ(double z);
    void setRotation(double rotation);
    void setTilt(double tilt);

    void moveCurrentToCoordinate();

    void toggleDistortion();


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
    /// Emitted when the right mouse is pressed down, in move mode
    void mouseMoveRightPressed(const QPointF& position);
    /// Emitted when the mouse is moved, while in move mode
    void mouseMovedMoveMode(const QPointF& position);
    /// Emitted when the currently selected item has changed
    void currentChanged();
    /// Emitted when the left mouse button is pressed, in edit mode
    void mouseEditPressed(const QPointF& position);
    /// Emitted when the right mouse button is pressed, in edit mode
    void mouseEditRightPressed(const QPointF& position);
    /// Emitted when the left mouse buTOPLEFTtton is pressed, in shift mode
    void mouseShiftPressed(const QPointF& position);
    /// Emitted when the right mouse button is pressed, in shift mode
    void mouseShiftRightPressed(const QPointF& position);
    /// Emitted when the left mouse button is pressed, in operation mode
    void mouseOperationPressed(const QPointF& position, const QPointF& crossHairPosition);
    void mouseOperationSelect(const QPointF& position);
    /// Emitted when the left mouse button is pressed, in group mode
    void mouseGroupPressed(const QPointF& position);


    void setCoordinate(double x, double y, double z);

    // for beam configuring
    void oneSelect();
    void twoSelect();
    void beamChanged(QObject*);


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

    void setCameraModel(AMCameraConfigurationModel*);

    void intersection();




protected:
	double crosshairX_, crosshairY_;
	QGraphicsLineItem* crosshairXLine_, *crosshairYLine_;

    static const QColor BORDERCOLOUR;
    static const QColor ACTIVEBORDERCOLOUR;



    /// Map of QGraphicsPolygonItem, corresponds to a map of AMShapeData2
    QMap<int,QGraphicsPolygonItem*> shapes_;

    /// The model for all the shapes displayed (except the crosshair)
    AMShapeOverlayVideoWidgetModel2* shapeModel_;

    QGraphicsPolygonItem* groupRectangle_;

    bool groupRectangleActive_;

    /// the max index in use (number of rectangles - 1)
    int index_;
    /// current mode selected by toolbar
    selectMode mode_;
    /// the index of the currently selected item
    int current_;

    QVector<QGraphicsPolygonItem*> intersections_;

    /// Add and remove shapes from the scene
    void addNewShape();
    void deleteShape();

    /// manages changing the selection
    void currentSelectionChanged();

    void createGroupRectangle();

    void destroyGroupRectangle();



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

    void createIntersectionShapes(QVector<QPolygonF>);

    void clearIntersections();



protected slots:
	/// Helper function to (re-)position the crosshair lines when the view is resized or the lines are moved
	void reviewCrosshairLinePositions();



};

#endif // AMCROSSHAIROVERLAYVIDEOWIDGET2_H
