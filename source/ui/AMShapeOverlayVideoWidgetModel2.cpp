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

double const AMShapeOverlayVideoWidgetModel2::FOCALLENGTH =5.0;


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
    shapeList_.insert(index_,polygon);
    shapeList_[index_].setName("Shape " + QString::number(index_));
    shapeList_[index_].setOtherData("Coordinate:");
    shapeList_[index_].setIdNumber(index_ * 13);
    shapeList_[index_].setRotation(0);
    shapeList_[index_].setCoordinate(QVector3D(0,0,FOCALLENGTH));
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
    for(int i = 0; i < index_ + 1; i++)
    {
        if(contains(position,i))
        {
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
    qDebug()<<"Attempting to selectCurrentShape"<<QString::number(i);
    while(isValid(i) && !contains(position,i))
    {
       i++;
    }
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
    double bz = FOCALLENGTH;
    QPointF position;
    position.setX(((coordinate.x()-0.5)*bz/coordinate.z())+0.5);
    position.setY(((coordinate.y()-0.5)*bz/coordinate.z())+0.5);
    return position;
}

QVector3D AMShapeOverlayVideoWidgetModel2::transform2Dto3D(QPointF point, double z)
{
    QVector3D coordinate;
    double bz = FOCALLENGTH;
    coordinate.setX(((point.x()-0.5)*z/bz)+0.5);
    coordinate.setY(((point.y()-0.5)*z/bz)+0.5);
    coordinate.setZ(z);
    return coordinate;
}

double AMShapeOverlayVideoWidgetModel2::transformDimension(double dimension, QVector3D coordinate)
{
    double bz = FOCALLENGTH;
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

QPolygonF AMShapeOverlayVideoWidgetModel2::applyRotation(int index)
{
    QPolygonF shape(*shapeList_[index].shape());
    double z = shapeList_[index].coordinate().z();
    double rotation = shapeList_[index].rotation();
    double length = (shapeList_[index].shape()->data()[TOPRIGHT].x() - shapeList_[index].shape()->data()[TOPLEFT].x())/2;
    QPolygonF polygon;
    polygon<<getRotatedPoint(shape.first(),z,rotation, QPointF(shape.first().x()+length,shape.first().y()));
    shape.remove(0);
    polygon<<getRotatedPoint(shape.first(),z,rotation, QPointF(shape.first().x()-length,shape.first().y()));
    shape.remove(0);
    polygon<<getRotatedPoint(shape.first(),z,rotation, QPointF(shape.first().x()-length,shape.first().y()));
    shape.remove(0);
    polygon<<getRotatedPoint(shape.first(),z,rotation, QPointF(shape.first().x()+length,shape.first().y()));
    shape.remove(0);
    polygon<<polygon.first();
    return polygon;

}

QPointF AMShapeOverlayVideoWidgetModel2::getRotatedPoint(QPointF point, double z, double rotation, QPointF center)
{

    QVector3D coordinate = transform2Dto3D(point,z);
    QVector3D newCenter = transform2Dto3D(center,z);
    QVector3D direction = newCenter - coordinate;
    ///should be parallel to the x axis
    QVector3D newX = ((1 - cos(rotation))*direction);
    QVector3D zDirection(0,0,-1*direction.x());
    QVector3D newZ = sin(rotation)*zDirection;
    coordinate = coordinate + newX + newZ;
    return transform3Dto2D(coordinate);




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

bool AMShapeOverlayVideoWidgetModel2::contains(QPointF position, int index)
{
    qDebug()<<"Looking for current Shape";
    qDebug()<<QString::number(position.x())<<QString::number(position.y());
    return subShape(index).containsPoint(position,Qt::OddEvenFill);
    //return shapeList_[index].shape()->containsPoint(position, Qt::OddEvenFill);
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

    QPolygonF shape = subShape(index);
    QPointF topLeft = shape.first();
    shape.remove(0);
    QPointF topRight = shape.first();
    shape.remove(0);
    QPointF bottomRight = shape.first();
    shape.remove(0);
    QPointF bottomLeft = shape.first();
    shape.remove(0);
    shape.clear();
    QPointF newTopLeft = coordinateTransform(topLeft);
    QPointF newTopRight = coordinateTransform(topRight);
    QPointF newBottomRight = coordinateTransform(bottomRight);
    QPointF newBottomLeft = coordinateTransform(bottomLeft);
    QPolygonF newShape;
    qDebug()<<QString::number(newTopLeft.x())<<QString::number(newTopLeft.y());
    qDebug()<<QString::number(newTopRight.x())<<QString::number(newTopRight.y());
    qDebug()<<QString::number(newBottomRight.x())<<QString::number(newBottomRight.y());
    qDebug()<<QString::number(newBottomLeft.x())<<QString::number(newBottomLeft.y());
    newShape<<newTopLeft<<newTopRight<<newBottomRight<<newBottomLeft<<newTopLeft;
    return newShape;

}

QPolygonF AMShapeOverlayVideoWidgetModel2::subShape(int index)
{
    QPolygonF shape(*shapeList_[index].shape());
    if(shapeList_[index].rotation() != 0) shape = applyRotation(index);
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



