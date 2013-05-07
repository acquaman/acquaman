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
    index_ = -1;
}



void AMShapeOverlayVideoWidgetModel2::startRectangle(QPointF position)
{
    index_++;
    rectangle_.setTopLeft(position);
    rectangle_.setBottomRight(position);
    rectangleList_.insert(index_,rectangle_);
}

void AMShapeOverlayVideoWidgetModel2::finishRectangle(QPointF position)
{
    rectangleList_[index_].setBottomRight(position);
}

void AMShapeOverlayVideoWidgetModel2::deleteRectangle(QPointF position)
{
    for(int i = 0; i < index_ + 1; i++)
    {
        if(rectangleList_[i].contains(position))
        {
           rectangleList_.remove(i);
           rectangleList_.insert(i,rectangleList_[index_]);
           rectangleList_.remove(index_);
           index_--;
        }
    }
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

QPointF AMShapeOverlayVideoWidgetModel2::rectangleTopLeft(int index)
{
    return coordinateTransform(rectangleList_[index].topLeft());
}

QPointF AMShapeOverlayVideoWidgetModel2::rectangleBottomRight(int index)
{
    return coordinateTransform(rectangleList_[index].bottomRight());
}


QRectF AMShapeOverlayVideoWidgetModel2::rectangle(int index)
{
    //QRectF* rectangle = new QRectF();
    QRectF rectangle;
    rectangle.setTopLeft(rectangleTopLeft(index));
    rectangle.setBottomRight(rectangleBottomRight(index));
    return rectangle;
}


