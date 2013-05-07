#include "AMShapeOverlayVideoWidgetView2.h"
#include <QGraphicsRectItem>
#include <QResizeEvent>

#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>

AMShapeOverlayVideoWidgetView2::AMShapeOverlayVideoWidgetView2(QWidget *parent, bool useOpenGlViewport):
    AMOverlayVideoWidget2(parent, useOpenGlViewport)
{
    cursorX_ = 0.5;
    cursorY_ = 0.5;

    rectangleTopX_ = 0.5;
    rectangleTopY_ = 0.5;

    QPen pen(QColor(Qt::red));
    QBrush brush(QColor(Qt::blue));

     rectangle_ = scene()->addRect(0.5,0.5,20,20,pen,brush);

     redrawScene();

     connect(videoItem_, SIGNAL(nativeSizeChanged(QSizeF)), this, SLOT(reviewCrosshairLinePositions()));

     //mouse is not held down before signal connected
     drawing_ = false;
     //mouse press -
     connect(this, SIGNAL(mousePressed(QPointF)), this, SLOT(startRectangle(QPointF)));
     connect(this, SIGNAL(mouseReleased(QPointF)),this, SLOT(finishRectangle(QPointF)));

}

void AMShapeOverlayVideoWidgetView2::redrawScene()
{
   double width;
   double height;
   double xRectangleCoordinate = coordinateTransform(rectangleTopX_,XCOORDINATE);
   double yRectangleCoordinate = coordinateTransform(rectangleTopY_,YCOORDINATE);

   if(drawing_)
   {
       width =  coordinateTransform(cursorX(),XCOORDINATE) - xRectangleCoordinate;
       height = coordinateTransform(cursorY(),YCOORDINATE) - yRectangleCoordinate;

   }
   else
   {
       width =  coordinateTransform(rectangleBottomX_,XCOORDINATE) - xRectangleCoordinate;
       height = coordinateTransform(rectangleBottomY_,YCOORDINATE) - yRectangleCoordinate;
   }
    rectangle_->setRect(xRectangleCoordinate,yRectangleCoordinate,width,height);
}

void AMShapeOverlayVideoWidgetView2::resizeEvent(QResizeEvent *event)
{
    AMOverlayVideoWidget2::resizeEvent(event);

    redrawScene();
}

void AMShapeOverlayVideoWidgetView2::mousePressEvent(QMouseEvent *e)
{
    AMOverlayVideoWidget2::mousePressEvent(e);

    if(e->button() == Qt::LeftButton)
        emit mousePressed(mapSceneToVideo(mapToScene(e->pos())));
}

void AMShapeOverlayVideoWidgetView2::mouseReleaseEvent(QMouseEvent *e)
{
    AMOverlayVideoWidget2::mouseReleaseEvent(e);

    if(e->button() == Qt::LeftButton)
    {
        emit mouseReleased(mapSceneToVideo(mapToScene(e->pos())));
    }
}

QPointF AMShapeOverlayVideoWidgetView2::mapSceneToVideo(const QPointF &sceneCoordinate) const
{
    // for more comments, see the more verbose implementation in reviewCrosshairLinePostions()
    QSizeF viewSize = videoItem_->size();
    QSizeF scaledSize = videoItem_->nativeSize();
    scaledSize.scale(viewSize, videoItem_->aspectRatioMode());

    QRectF activeRect = QRectF(QPointF((viewSize.width()-scaledSize.width())/2,
                                       (viewSize.height()-scaledSize.height())/2),
                               scaledSize);

    // activeRect is now a rectangle in scene coordinates that covers the actual area of the video [not the area of the videoWidget, which may be smaller or larger depending on the aspect ratio mode and aspect ratio of the actual video feed]

    qreal yScene = (sceneCoordinate.y() - activeRect.top())/activeRect.height();
    qreal xScene = (sceneCoordinate.x() - activeRect.left())/activeRect.width();

    return QPointF(xScene, yScene);
}

void AMShapeOverlayVideoWidgetView2::setCursorX(double cursorX)
{
    cursorX_ = cursorX;
}

void AMShapeOverlayVideoWidgetView2::setCursorY(double cursorY)
{
    cursorY_ = cursorY;
}

void AMShapeOverlayVideoWidgetView2::startRectangle(QPointF position)
{
    rectangleTopX_ =position.x();
    rectangleTopY_ =position.y();
    setCursorX(position.x());
    setCursorY(position.y());

    //drawing the shape
    drawing_ = true;
    connect(this,SIGNAL(mouseMoved(QPointF)), this, SLOT(drawRectangle(QPointF)));

    //scene()->addLine(cursorX(),cursorY(),QCursor::pos().x(),QCursor::pos().y(),pen);
   // rectangle_ = scene()->addRect(cursorX(),cursorY(), 10, 10, pen, brush);
   redrawScene();

}

void AMShapeOverlayVideoWidgetView2::finishRectangle(QPointF position)
{

    rectangleBottomX_ = position.x();
    rectangleBottomY_ = position.y();
    //stop drawing
    drawing_ = false;
    disconnect(this,SLOT(drawRectangle(QPointF)));

    redrawScene();
}

double AMShapeOverlayVideoWidgetView2::coordinateTransform(double coordinate, Coordinate typeCoordinate)
{

    QSizeF viewSize = videoItem_->size();
    // first we need to find out the native size of the video. (Well, actually just the aspect ratio, but...)
    QSizeF videoSize = videoItem_->nativeSize();

    // scale the video size to the view size, obeying the transformation mode
    QSizeF scaledSize = videoSize;
    scaledSize.scale(viewSize, videoItem_->aspectRatioMode());

    // now, scaledSize will either be:
        // same as viewSize, if view and video have same aspect ratio, or the video is being stretched with Qt::IgnoreAspectRatio
        // One dimension the same, other dimension smaller than viewSize, if Qt::KeepAspectRatio
        // or One dimension the same, other dimension LARGER than viewSize, if Qt::KeepAspectRatioByExpanding

    QRectF activeRect = QRectF(QPointF((viewSize.width()-scaledSize.width())/2,
                                       (viewSize.height()-scaledSize.height())/2),
                               scaledSize);

    if(typeCoordinate == XCOORDINATE)
    {
        return activeRect.left() + coordinate*activeRect.width();

    }
    else if(typeCoordinate == YCOORDINATE)
    {
        return activeRect.top() + coordinate*activeRect.height();
    }
    return 0;
}


void AMShapeOverlayVideoWidgetView2::mouseMoveEvent(QMouseEvent *e)
{
    AMOverlayVideoWidget2::mouseMoveEvent(e);

    if(e->MouseMove)
        emit mouseMoved(mapSceneToVideo(mapToScene(e->pos())));
}

void AMShapeOverlayVideoWidgetView2::drawRectangle(QPointF position)
{
    setCursorX(position.x());
    setCursorY(position.y());
    redrawScene();
}

