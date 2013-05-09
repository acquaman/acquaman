#include "AMShapeOverlayVideoWidgetModel2.h"

#include <QGraphicsRectItem>
#include <QResizeEvent>
#include <QVector3D>

#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>
#include <math.h>

/// Constructor
AMShapeOverlayVideoWidgetModel2::AMShapeOverlayVideoWidgetModel2(QObject *parent) :
    QObject(parent)
{
    index_ = -1;
}


/// creates a new rectangle, and initializes its data
void AMShapeOverlayVideoWidgetModel2::startRectangle(QPointF position)
{
    index_++;
    rectangle_.setTopLeft(position);
    rectangle_.setBottomRight(position);
    AMShapeData2 newRectData(rectangle_);
    rectangleList_.insert(index_,newRectData);
    rectangleList_[index_].setName("Rectangle " + QString::number(index_));
    rectangleList_[index_].setOtherData("Coordinate:");
    rectangleList_[index_].setIdNumber(index_ * 13);
    current_ = index_;
}

/// Changes the bottom right corner of the current rectangle
void AMShapeOverlayVideoWidgetModel2::finishRectangle(QPointF position)
{
    rectangleList_[index_].rectangle()->setBottomRight(position);
    rectangleList_[index_].setHeight(rectangleList_[index_].rectangle()->height());
    rectangleList_[index_].setWidth(rectangleList_[index_].rectangle()->width());
}

/// deletes a rectangle from the list
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

/// selects a rectangle which is under the cursor
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

/// moves the currently selected rectangle by position + currentVector_
void AMShapeOverlayVideoWidgetModel2::moveCurrentRectangle(QPointF position)
{
    if(current_ <= index_)
    {
        rectangleList_[current_].rectangle()->moveTopLeft(position + currentVector_);
    }
}

/// moves all rectangles by position + rectangleVector_[index]
void AMShapeOverlayVideoWidgetModel2::moveAllRectangles(QPointF position)
{

    for(int i = 0; i <= index_; i++)
    {
        rectangleList_[i].rectangle()->moveTopLeft(position + rectangleVector_[i]);
    }
}

/// assigns all rectangle vectors, relative to position
void AMShapeOverlayVideoWidgetModel2::setRectangleVectors(QPointF position)
{

    rectangleVector_  = new QPointF[index_ + 1];
    for(int i = 0; i <= index_; i++)
    {
        rectangleVector_[i] = rectangleList_[i].rectangle()->topLeft() - position;
    }
}

/// makes all rectangles bigger or smaller, keeps the centre constant
void AMShapeOverlayVideoWidgetModel2::zoomAllRectangles(QPointF position)
{
    for(int i = 0; i <= index_; i++)
    {
        qDebug()<<QString::number(rectangleList_[i].rectangle()->x()) +" "+ QString::number(rectangleList_[i].rectangle()->y());
        QSizeF oldSize = rectangleList_[i].rectangle()->size();
        QSizeF newSize = (zoomPoint_.y()/position.y())*oldSize;
        QPointF center = rectangleList_[i].rectangle()->center() + (-1*(newSize.height()/oldSize.height() -1))*(zoomCenter_ - rectangleList_[i].rectangle()->center());
        rectangleList_[i].rectangle()->setSize(newSize);
        rectangleList_[i].rectangle()->moveCenter(center);
        rectangleList_[i].setOtherData(QString::number(rectangleList_[i].rectangle()->x()) +" "+ QString::number(rectangleList_[i].rectangle()->y()));
    }
    zoomPoint_ = position;
}


/// Changes the size of the current rectangle, keeps its top left constant
void AMShapeOverlayVideoWidgetModel2::finishCurrentRectangle(QPointF position)
{
    rectangleList_[current_].rectangle()->setBottomRight(position);
    rectangleList_[current_].setHeight(rectangleList_[current_].rectangle()->height());
    rectangleList_[current_].setWidth(rectangleList_[current_].rectangle()->width());
}

void AMShapeOverlayVideoWidgetModel2::setCoordinates(QVector3D coordinate, int index)
{
    rectangleList_[index].setCoordinate(coordinate);
}

void AMShapeOverlayVideoWidgetModel2::setCoordinates(double x, double y, double z, int index)
{
    qDebug()<<"changing coordinates";
    if(-1 == index) index = current_;
    QVector3D coordinate;
    coordinate.setX(x);
    coordinate.setY(y);
    coordinate.setZ(z);
    setCoordinates(coordinate, index);
}

QVector3D AMShapeOverlayVideoWidgetModel2::coordinate(int index)
{
    return rectangleList_[index].coordinate();
}

QVector3D AMShapeOverlayVideoWidgetModel2::currentCoordinate()
{
    return coordinate(current_);
}

QPointF AMShapeOverlayVideoWidgetModel2::transform3Dto2D(QVector3D coordinate)
{
    // bx = ax*bz/az
    // by = ay*bz/az
    // define a bz value
    double bz = 1.0;
    QPointF position;
    position.setX(((coordinate.x()-0.5)*bz/coordinate.z())+0.5);
    position.setY(((coordinate.y()-0.5)*bz/coordinate.z())+0.5);
    return position;
}

double AMShapeOverlayVideoWidgetModel2::transformDimension(double dimension, QVector3D coordinate)
{
    //redefine bz...
    double bz = 1.0;
    double newDimension = dimension*bz/coordinate.z();
    return newDimension;
}

void AMShapeOverlayVideoWidgetModel2::changeCoordinate()
{
    qDebug()<<"before:"<<QString::number(rectangleList_[current_].rectangle()->x())<<QString::number(rectangleList_[current_].rectangle()->y());
    rectangleList_[current_].rectangle()->setHeight(transformDimension(rectangleList_[current_].height(),rectangleList_[current_].coordinate()));
    rectangleList_[current_].rectangle()->setWidth(transformDimension(rectangleList_[current_].width(),rectangleList_[current_].coordinate()));
    rectangleList_[current_].rectangle()->setWidth(applyRotation(rectangleList_[current_].rectangle()->width()));
    rectangleList_[current_].rectangle()->moveCenter(transform3Dto2D(rectangleList_[current_].coordinate()));
    qDebug()<<"after:"<<QString::number(rectangleList_[current_].rectangle()->x())<<QString::number(rectangleList_[current_].rectangle()->y());
}

/// transforms a coordinate point for display
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

double AMShapeOverlayVideoWidgetModel2::applyRotation(double width)
{
    // apply the rotation
    // however, for proper rotation, we need a non-rectangle
    // will implement very basic rotation (just modify the width)
    double newWidth = width*cos(rectangleList_[current_].rotation());
    return newWidth;

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

/// checks if an index is valid
bool AMShapeOverlayVideoWidgetModel2::isValid(int index)
{
    if(index <= index_ && index >= 0) return true;
    else return false;
}


void AMShapeOverlayVideoWidgetModel2::deleteRectangleVector()
{
    delete[]rectangleVector_;
}

double AMShapeOverlayVideoWidgetModel2::rotation(int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    return rectangleList_[index].rotation();
}

void AMShapeOverlayVideoWidgetModel2::setRotation(double rotation, int index)
{
    if(-1 == index)
    {
        index = current_;
    }
   rectangleList_[index].setRotation(rotation);
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


void AMShapeOverlayVideoWidgetModel2::setZoomPoint(QPointF position)
{
    zoomPoint_ = position;
    zoomCenter_ = position;
}



