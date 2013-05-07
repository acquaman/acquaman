#ifndef AMSHAPEOVERLAYVIDEOWIDGETVIEW2_H
#define AMSHAPEOVERLAYVIDEOWIDGETVIEW2_H

#include "ui/AMOverlayVideoWidget2.h"

class QGraphicsRectItem;

/// This class adds the capability of drawing shapes on top of the Video overlay.
/// Hopefully it doesn't kill the crosshair.

class AMShapeOverlayVideoWidgetView2 : public AMOverlayVideoWidget2
{

    Q_OBJECT

public:
    /// Constructor
    explicit AMShapeOverlayVideoWidgetView2(QWidget *parent = 0, bool usOpenGlViewport = true);

signals:
    /// Emitted when the left mouse button is pressed down. (The position is reported as percentage of the video screen width and height; ie: from 0 to 1)
    void mousePressed(const QPointF& position);
    /// Emitted when the left mouse is released. (The position is reported as percentage of the video screen width and height; ie: from 0 to 1).  Not emitted for the second click of a double-click; see mouseDoubleClicked() instead.
    void mouseReleased(const QPointF& position);

    /// Emitted when the mouse moves
    void mouseMoved(const QPointF& position);

protected:
    /// Cursor position for drawing a line
    double cursorX_, cursorY_;
    double rectangleTopX_, rectangleTopY_;
    double rectangleBottomX_,rectangleBottomY_;
    bool drawing_;
    QGraphicsRectItem* rectangle_;

    /// Line drawing cursor positions
    double cursorX(){return cursorX_;}
    double cursorY(){return cursorY_;}
    void setCursorX(double cursorX);
    void setCursorY(double cursorY);

    ///
    enum Coordinate { XCOORDINATE,YCOORDINATE};
    /// for ease, create a coordinate transform function
    /// takes a double transforms to where you are actually clicking
    /// as a doubles, must also pass wether it's x or y
    double coordinateTransform(double, Coordinate);


    /// Re-implemented to emit signals for mouse events
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    /// added mouse move event
    virtual void mouseMoveEvent(QMouseEvent *e);

    /// We catch resize events and re-position the crosshair lines as required
    void resizeEvent(QResizeEvent *event);

    /// Helper function to convert scene coordinates to video-relative coordinates. (This is only tricky because depending on the videoItem()'s aspectRatioMode(), there might be letterbox bars at the top or sides of the display.
    QPointF mapSceneToVideo(const QPointF& sceneCoordinate) const;


protected slots:

    /// Function to redraw the shapes after a change
    void redrawScene();




    /// start position of rectangle
    void startRectangle(QPointF);

    /// end position of rectangle
    void finishRectangle(QPointF);

    /// drawRectangle
    void drawRectangle(QPointF);


};

#endif // AMSHAPEOVERLAYVIDEOWIDGETVIEW2_H
