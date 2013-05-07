#include "AMShapeOverlayVideoWidgetModel2.h"

#include <QGraphicsRectItem>
#include <QResizeEvent>
#include "ui/AMCrosshairOverlayVideoWidget2.h"

#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>

AMShapeOverlayVideoWidgetModel2::AMShapeOverlayVideoWidgetModel2(QObject *parent) :
    QObject(parent)
{


    cursorX_ = 0.5;
    cursorY_ = 0.5;

    rectangleTopX_ = 0.5;
    rectangleTopY_ = 0.5;

    drawing_ = false;



}



void AMShapeOverlayVideoWidgetModel2::startRectangle(QPointF position)
{
    rectangleTopX_ =position.x();
    rectangleTopY_ =position.y();
    rectangleBottomX_ = position.x();
    rectangleBottomY_ = position.y();


    //scene()->addLine(cursorX(),cursorY(),QCursor::pos().x(),QCursor::pos().y(),pen);
   // rectangle_ = scene()->addRect(cursorX(),cursorY(), 10, 10, pen, brush);


}

void AMShapeOverlayVideoWidgetModel2::finishRectangle(QPointF position)
{

    rectangleBottomX_ = position.x();
    rectangleBottomY_ = position.y();




}

double AMShapeOverlayVideoWidgetModel2::coordinateTransform(double coordinate, Coordinate typeCoordinate)
{


    QRectF activeRect = QRectF(QPointF((viewSize().width()-scaledSize().width())/2,
                                       (viewSize().height()-scaledSize().height())/2),
                               scaledSize());

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



void AMShapeOverlayVideoWidgetModel2::setViewSize(QSizeF viewSize)
{
    viewSize_ = viewSize;
}

void AMShapeOverlayVideoWidgetModel2::setScaledSize(QSizeF scaledSize)
{
    scaledSize_ = scaledSize;
}

double AMShapeOverlayVideoWidgetModel2::rectangleX()
{
    return coordinateTransform(rectangleTopX_,XCOORDINATE);
}

double AMShapeOverlayVideoWidgetModel2::rectangleY()
{
    return coordinateTransform(rectangleTopY_,YCOORDINATE);
}

double AMShapeOverlayVideoWidgetModel2::rectangleHeight()
{
    return coordinateTransform(rectangleBottomY_,YCOORDINATE) - coordinateTransform(rectangleTopY_,YCOORDINATE);
}

double AMShapeOverlayVideoWidgetModel2::rectangleWidth()
{
    return coordinateTransform(rectangleBottomX_,XCOORDINATE) - coordinateTransform(rectangleTopX_,XCOORDINATE);
}
