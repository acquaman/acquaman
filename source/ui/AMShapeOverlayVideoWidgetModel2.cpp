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
}



void AMShapeOverlayVideoWidgetModel2::startRectangle(QPointF position)
{
    rectangle_.setTopLeft(position);
    rectangle_.setBottomRight(position);
}

void AMShapeOverlayVideoWidgetModel2::finishRectangle(QPointF position)
{
    rectangle_.setBottomRight(position);
}

QPointF AMShapeOverlayVideoWidgetModel2::coordinateTransform(QPointF coordinate)
{


    QRectF activeRect = QRectF(QPointF((viewSize().width()-scaledSize().width())/2,
                                       (viewSize().height()-scaledSize().height())/2),
                               scaledSize());

    QPointF fixedCoordinate;
    qreal xCoord = activeRect.left() + coordinate.x()*activeRect.width();
    qreal yCoord = activeRect.top() + coordinate.y()*activeRect.height();
    fixedCoordinate.setX(xCoord);
    fixedCoordinate.setY(yCoord);
    return fixedCoordinate;


}



void AMShapeOverlayVideoWidgetModel2::setViewSize(QSizeF viewSize)
{
    viewSize_ = viewSize;
}

void AMShapeOverlayVideoWidgetModel2::setScaledSize(QSizeF scaledSize)
{
    scaledSize_ = scaledSize;
}

QPointF AMShapeOverlayVideoWidgetModel2::rectangleTopLeft()
{
    return coordinateTransform(rectangle_.topLeft());
}

QPointF AMShapeOverlayVideoWidgetModel2::rectangleBottomRight()
{
    return coordinateTransform(rectangle_.bottomRight());
}


QRectF AMShapeOverlayVideoWidgetModel2::rectangle()
{
    //QRectF* rectangle = new QRectF();
    QRectF rectangle;
    rectangle.setTopLeft(rectangleTopLeft());
    rectangle.setBottomRight(rectangleBottomRight());
    return rectangle;
}
