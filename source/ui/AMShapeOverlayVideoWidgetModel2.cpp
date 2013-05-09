#include "AMShapeOverlayVideoWidgetModel2.h"

#include <QGraphicsRectItem>
#include <QResizeEvent>
#include <QVector3D>

#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>
#include <math.h>

int const AMShapeOverlayVideoWidgetModel2::TOPLEFT = 0;
int const AMShapeOverlayVideoWidgetModel2::TOPRIGHT = 1;
int const AMShapeOverlayVideoWidgetModel2::BOTTOMRIGHT = 2;
int const AMShapeOverlayVideoWidgetModel2::BOTTOMLEFT = 3;
int const AMShapeOverlayVideoWidgetModel2::TOPCLOSE = 4;


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
    QPolygonF polygon = constructRectangle(position,position);
//    rectangle_.setTopLeft(QPointF(position));
//    qDebug()<<"mouse position"<<QString::number(position.x())<<QString::number(position.y());
//    rectangle_.setBottomRight(position);
//    QPolygonF polygon(rectangle_);
//    AMShapeData2 newShapeData(polygon);
//    shapeList_.insert(index_,newShapeData);
    if(!polygon.isClosed())qDebug()<<"Polygon is not closed...";
    shapeList_.insert(index_,polygon);
    shapeList_[index_].setName("Shape " + QString::number(index_));
    shapeList_[index_].setOtherData("Coordinate:");
    shapeList_[index_].setIdNumber(index_ * 13);
    current_ = index_;
}

/// Changes the bottom right corner of the current rectangle
void AMShapeOverlayVideoWidgetModel2::finishRectangle(QPointF position)
{

     if(!isValid(current_))return;
    //QPointF* data = shapeList_[index_].shape()->data();
    QPointF topLeft = shapeList_[current_].shape()->first();
    QPolygonF newPolygon = constructRectangle(topLeft,position);
    QPolygonF* polygon = shapeList_[current_].shape();
    shapeList_[current_].setShape(newPolygon);
    QSizeF newSize = size(topLeft,position);
    //delete polygon;
  // double height = rectangle.height();
//    double width = rectangle.width();
    shapeList_[index_].setHeight(newSize.height());
    shapeList_[index_].setWidth(newSize.width());
}

/// deletes a rectangle from the list
void AMShapeOverlayVideoWidgetModel2::deleteRectangle(QPointF position)
{
    qDebug()<<"Attempting to delete rectangles";
    for(int i = 0; i < index_ + 1; i++)
    {
        if(shapeList_[i].shape()->containsPoint(position,Qt::OddEvenFill))
        {
            qDebug()<<"Deleting rectangle"<<QString::number(i);
           shapeList_.remove(i);
           shapeList_.insert(i,shapeList_[index_]);
           shapeList_.remove(index_);
           index_--;
        }
    }
}

/// selects a rectangle which is under the cursor
void AMShapeOverlayVideoWidgetModel2::selectCurrentShape(QPointF position)
{
    int i = 0;
    while(!shapeList_[i].shape()->containsPoint(position,Qt::OddEvenFill) && i <= index_)
    {
       i++;
    }
    qDebug()<<QString::number(i)<<QString::number(index_);
    if(i <= index_)
    {//    shapeVector_  = new QPointF[index_ + 1];
        //    for(int i = 0; i <= index_; i++)
        //    {
        //        shapeVector_[i] = shapeList_[current_].shape()->data()[TOPLEFT] - position;
        //    }
        current_ = i;
        currentVector_ =position;//shapeList_[current_].shape()->data()[TOPLEFT] - position;
        qDebug()<<shapeList_[current_].name();
    }
    else current_ = index_ + 1;
}

/// moves the currently selected rectangle by position + currentVector_
void AMShapeOverlayVideoWidgetModel2::moveCurrentShape(QPointF position)
{
    if(current_ <= index_)
    {
        shapeList_[current_].shape()->translate(position - currentVector_);
    }
    currentVector_ = position;
}

/// moves all rectangles by position + rectangleVector_[index]
void AMShapeOverlayVideoWidgetModel2::moveAllShapes(QPointF position)
{

    for(int i = 0; i <= index_; i++)
    {
        shapeList_[i].shape()->translate(position - currentVector_);
    }
    currentVector_ = position;
}

/// assigns all rectangle vectors, relative to position
void AMShapeOverlayVideoWidgetModel2::setShapeVectors(QPointF position)
{
      currentVector_ = position;

}

/// makes all rectangles bigger or smaller, keeps the centre constant
void AMShapeOverlayVideoWidgetModel2::zoomAllShapes(QPointF position)
{
    for(int i = 0; i <= index_; i++)
    {
        qDebug()<<QString::number(shapeList_[i].shape()->data()[0].x()) +" "+ QString::number(shapeList_[i].shape()->data()[0].y());
        QPointF* data = shapeList_[i].shape()->data();
        QPointF topLeft = data[TOPLEFT];
        QSizeF oldSize;
        oldSize.setHeight(data[BOTTOMLEFT].y()-data[TOPLEFT].y());
        oldSize.setWidth(data[TOPRIGHT].x() - data[TOPLEFT].x());
        QRectF newRectangle(topLeft,oldSize);
        QSizeF newSize = (zoomPoint_.y()/position.y())*oldSize;
        QPointF center = newRectangle.center() + (-1*(newSize.height()/oldSize.height() -1))*(zoomCenter_ - newRectangle.center());
        newRectangle.setSize(newSize);
        newRectangle.moveCenter(center);

        shapeList_[i].setShape(QPolygonF(newRectangle));
    }
    zoomPoint_ = position;
}




void AMShapeOverlayVideoWidgetModel2::setCoordinates(QVector3D coordinate, int index)
{
    shapeList_[index].setCoordinate(coordinate);
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
    return shapeList_[index].coordinate();
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

    double height = transformDimension(shapeList_[current_].height(),shapeList_[current_].coordinate());
    double width = transformDimension(shapeList_[current_].width(),shapeList_[current_].coordinate());
    QSizeF size = QSizeF(width,height);
    QPointF* data = shapeList_[current_].shape()->data();
    QPointF topLeft = data[TOPLEFT];
    QRectF rectangle = QRectF(topLeft, size);
    rectangle.moveCenter(transform3Dto2D(shapeList_[current_].coordinate()));
    shapeList_[current_].setShape(QPolygonF(rectangle));

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
    double newWidth = width*cos(shapeList_[current_].rotation());
    return newWidth;

}

QPolygonF AMShapeOverlayVideoWidgetModel2::constructRectangle(QPointF topLeft, QPointF bottomRight)
{
    QPolygonF polygon;
    QPointF topRight(bottomRight.x(),topLeft.y());
    QPointF bottomLeft(topLeft.x(),bottomRight.y());
    polygon<<topLeft<<topRight<<bottomRight<<bottomLeft<<topLeft;
    return polygon;

}

QSizeF AMShapeOverlayVideoWidgetModel2::size(QPointF topLeft, QPointF bottomRight)
{
    QSizeF size;
    QPointF difference = bottomRight - topLeft;
    size.setHeight(difference.y());
    size.setWidth(difference.x());
    return size;
}



void AMShapeOverlayVideoWidgetModel2::setViewSize(QSizeF viewSize)
{
    viewSize_ = viewSize;
}

void AMShapeOverlayVideoWidgetModel2::setScaledSize(QSizeF scaledSize)
{
    scaledSize_ = scaledSize;
}

QPointF AMShapeOverlayVideoWidgetModel2::shapeTopLeft(int index)
{
    return coordinateTransform(shapeList_[index].shape()->data()[TOPLEFT]);
}

QPointF AMShapeOverlayVideoWidgetModel2::shapeBottomRight(int index)
{
    return coordinateTransform(shapeList_[index].shape()->data()[BOTTOMRIGHT]);
}

/// checks if an index is valid
bool AMShapeOverlayVideoWidgetModel2::isValid(int index)
{
    if(index <= index_ && index >= 0) return true;
    else return false;
}

double AMShapeOverlayVideoWidgetModel2::rotation(int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    return shapeList_[index].rotation();
}

void AMShapeOverlayVideoWidgetModel2::setRotation(double rotation, int index)
{
    if(-1 == index)
    {
        index = current_;
    }
   shapeList_[index].setRotation(rotation);
}


QPolygonF AMShapeOverlayVideoWidgetModel2::shape(int index)
{
    QPolygonF shape(*shapeList_[index].shape());
    QPointF topLeft = shape.first();
    shape.remove(0);
    shape.remove(0);
    QPointF bottomRight = shape.first();
    QPointF topCorner = coordinateTransform(topLeft);
    QPointF bottomCorner = coordinateTransform(bottomRight);
    shape = constructRectangle(topCorner,bottomCorner);

    return shape;
}

QString AMShapeOverlayVideoWidgetModel2::currentName()
{
    if(isValid(current_)) return shapeList_[current_].name();
    else return "";
}

void AMShapeOverlayVideoWidgetModel2::setCurrentName(QString name)
{
    shapeList_[current_].setName(name);
}

QString AMShapeOverlayVideoWidgetModel2::currentInfo()
{
    if(isValid(current_)) return shapeList_[current_].otherData();
    else return "";
}

void AMShapeOverlayVideoWidgetModel2::setCurrentInfo(QString info)
{
    shapeList_[current_].setOtherData(info);
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



