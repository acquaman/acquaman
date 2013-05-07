#include "AMShapeOverlayVideoWidgetModel2.h"

#include <QGraphicsRectItem>
#include <QResizeEvent>
#include "ui/AMCrosshairOverlayVideoWidget2.h"
#include "ui/AMShapeData2.h"

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
    AMShapeData2 newRectData(rectangle_);
   // AMShapeData2* newRectData = new AMShapeData2(rectangle_,"Rectangle" + index_);
    rectangleList_.insert(index_,newRectData);
    rectangleList_[index_].setName("Rectangle" + index_);
    rectangleList_[index_].setOtherData("Coordinate:");
    rectangleList_[index_].setIdNumber(index_ * 13);
}

void AMShapeOverlayVideoWidgetModel2::finishRectangle(QPointF position)
{
    rectangleList_[index_].rectangle()->setBottomRight(position);
}

void AMShapeOverlayVideoWidgetModel2::deleteRectangle(QPointF position)
{
    for(int i = 0; i < index_ + 1; i++)
    {
        if(rectangleList_[i].rectangle()->contains(position))
        {
           rectangleList_.remove(i);
           rectangleList_.insert(i,rectangleList_[index_]);
           rectangleList_.remove(index_);
           index_--;
        }
    }
}

void AMShapeOverlayVideoWidgetModel2::selectCurrentRectangle(QPointF position)
{
    int i = 0;
    while(!rectangleList_[i].rectangle()->contains(position) && i <= index_)
    {
       i++;
    }
    if(i <= index_)
    {
        current_ = i;
        currentVector_ =rectangleList_[current_].rectangle()->topLeft() - position;
    }
    else current_ = index_ + 1;
}

void AMShapeOverlayVideoWidgetModel2::moveCurrentRectangle(QPointF position)
{
    if(current_ <= index_)
    {
        rectangleList_[current_].rectangle()->moveTopLeft(position + currentVector_);
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
    return coordinateTransform(rectangleList_[index].rectangle()->topLeft());
}

QPointF AMShapeOverlayVideoWidgetModel2::rectangleBottomRight(int index)
{
    return coordinateTransform(rectangleList_[index].rectangle()->bottomRight());
}


QRectF AMShapeOverlayVideoWidgetModel2::rectangle(int index)
{
    //QRectF* rectangle = new QRectF();
    QRectF rectangle;
    rectangle.setTopLeft(rectangleTopLeft(index));
    rectangle.setBottomRight(rectangleBottomRight(index));
    return rectangle;
}


