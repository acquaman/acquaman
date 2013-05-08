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
    // make connections
}



void AMShapeOverlayVideoWidgetModel2::startRectangle(QPointF position)
{
    index_++;
    rectangle_.setTopLeft(position);
    rectangle_.setBottomRight(position);
    AMShapeData2 newRectData(rectangle_);
   // AMShapeData2* newRectData = new AMShapeData2(rectangle_,"Rectangle" + index_);
    rectangleList_.insert(index_,newRectData);
    rectangleList_[index_].setName("Rectangle" + QString::number(index_));
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
        qDebug()<<rectangleList_[current_].name();
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

void AMShapeOverlayVideoWidgetModel2::moveAllRectangles(QPointF position)
{
    qDebug()<<"Moving Rectangles";
    for(int i = 0; i <= index_; i++)
    {
        rectangleList_[i].rectangle()->moveTopLeft(position + rectangleVector_[i]);
    }
}

void AMShapeOverlayVideoWidgetModel2::setRectangleVectors(QPointF position)
{
    qDebug()<<"Setting Rectangle Vectors";
    rectangleVector_  = new QPointF[index_ + 1];
    for(int i = 0; i <= index_; i++)
    {
        rectangleVector_[i] = rectangleList_[i].rectangle()->topLeft() - position;
    }
}

void AMShapeOverlayVideoWidgetModel2::finishCurrentRectangle(QPointF position)
{
    rectangleList_[current_].rectangle()->setBottomRight(position);
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

bool AMShapeOverlayVideoWidgetModel2::isValid(int index)
{
    if(index <= index_ && index >= 0) return true;
    else return false;
}

void AMShapeOverlayVideoWidgetModel2::deleteRectangleVector()
{
    delete[]rectangleVector_;
}


QRectF AMShapeOverlayVideoWidgetModel2::rectangle(int index)
{
    //QRectF* rectangle = new QRectF();
    QRectF rectangle;
    rectangle.setTopLeft(rectangleTopLeft(index));
    rectangle.setBottomRight(rectangleBottomRight(index));
    return rectangle;
}

QString AMShapeOverlayVideoWidgetModel2::currentName()
{
    if(isValid(current_)) return rectangleList_[current_].name();
    else return "";
}

void AMShapeOverlayVideoWidgetModel2::setCurrentName(QString name)
{
    rectangleList_[current_].setName(name);
}

QString AMShapeOverlayVideoWidgetModel2::currentInfo()
{
    if(isValid(current_)) return rectangleList_[current_].otherData();
    else return "";
}

void AMShapeOverlayVideoWidgetModel2::setCurrentInfo(QString info)
{
    rectangleList_[current_].setOtherData(info);
}

int AMShapeOverlayVideoWidgetModel2::currentIndex()
{
    return current_;
}

void AMShapeOverlayVideoWidgetModel2::setCurrentIndex(int current)
{
    current_ = current;
}




