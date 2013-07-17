#include "AMShapeData.h"




#include <QString>
#include <QVector3D>
#include <QPolygonF>
#include <QDebug>

/// constructor
AMShapeData::AMShapeData()
{
    shape_ = new QPolygonF();
    coordinateCount_ = -1;

}

/// constructor
AMShapeData::AMShapeData(QPolygonF shape, QString name, QString otherData,  double idNumber)
{
    shape_ = new QPolygonF();
    *shape_ = shape;
    setName(name);
    setOtherData(otherData);
    setIdNumber(idNumber);
    coordinateCount_ = -1;

}

AMShapeData::~AMShapeData()
{
    coordinate_.clear();
}

QPolygonF* AMShapeData::shape()
{
    return shape_;
}

QString AMShapeData::name()
{
    return name_;
}

QString AMShapeData::otherData()
{
    return otherData_;
}

double AMShapeData::idNumber()
{
    return idNumber_;
}

QVector3D AMShapeData::coordinate(int index)
{
    if(validIndex(index))
        return coordinate_[index];
    else
        return QVector3D(0,0,0);
}

double AMShapeData::height()
{
    return height_;
}

double AMShapeData::width()
{
    return width_;
}

double AMShapeData::rotation()
{
    return rotation_;
}

double AMShapeData::tilt()
{
    return tilt_;
}

double AMShapeData::yAxisRotation()
{
    return yAxisRotation_;
}

void AMShapeData::setShape(QPolygonF shape)
{
    *shape_ = shape;
}

void AMShapeData::setName(QString name)
{
    name_ = name;
}

void AMShapeData::setOtherData(QString otherData)
{
    otherData_ = otherData;
}

void AMShapeData::setIdNumber(double idNumber)
{
    idNumber_ = idNumber;
}

void AMShapeData::setCoordinate(QVector3D coordinate, int index)
{
    if(validIndex(index))
        coordinate_[index] = coordinate;
}

void AMShapeData::setCoordinateShape(QVector<QVector3D> coordinates, int count)
{
    if(coordinates.isEmpty()) return;
    coordinate_.clear();
    for(int i = 0; i < count; i++)
    {
        if(coordinateCount_ < i) coordinateCount_ = i;
        coordinate_<<coordinates[i];
    }
}

void AMShapeData::setHeight(double height)
{
    height_ = height;
}

void AMShapeData::setWidth(double width)
{
    width_ = width;
}

void AMShapeData::setRotation(double rotation)
{
    rotation_ = rotation;
}

void AMShapeData::setTilt(double tilt)
{
    tilt_ = tilt;
}

void AMShapeData::setYAxisRotation(double yAxisRotation)
{
    yAxisRotation_ = yAxisRotation;
}

/// finds the center of the shape - must be rectangular
QVector3D AMShapeData::centerCoordinate()
{
    QVector3D center = QVector3D(0,0,0);
    for(int i = 0; i < (coordinateCount_); i++)// dont want the last point
    {
        center += coordinate(i);
    }
    return center/(double)(coordinateCount_ );
}

/// shifts the shape by the given amount
void AMShapeData::shift(QVector3D shift)
{
    for(int i = 0; i < count(); i++)
    {
        setCoordinate(coordinate_[i] +  shift,i);
    }
}

/// shifts the shape to the given location
void AMShapeData::shiftTo(QVector3D shift)
{
    shift -= centerCoordinate();
    for(int i = 0; i < count(); i++)
    {
        setCoordinate(coordinate_[i] +  shift,i);
    }
}

/// returns a count of the number of coordinates
int AMShapeData::count()
{
    return coordinateCount_;
}

/// checks to see if the shape is backwards
bool AMShapeData::backwards()
{
    if(count() < 3)return false;
    QVector3D points [3];
    for(int i = 0; i < 3 ; i++)
    {
        points[i] = QVector3D(shape_->at(i));
    }
    QVector3D rayOne = points[1] - points[0];
    QVector3D rayTwo = points[2] - points[1];
    QVector3D normal = QVector3D::normal(rayOne,rayTwo);
    // normal should be either positive or negative 1;
    return(normal.z() < 0);
}

bool AMShapeData::operator ==(const AMShapeData &other) const
{
    if(other.name_ != name_)
        return false;
    if(other.idNumber_ != idNumber_)
        return false;
    if(other.otherData_ != otherData_)
        return false;
    if(other.rotation_ != rotation_)
        return false;
    if(other.height_ != height_)
        return false;
    if(other.width_ != width_)
        return false;
    if(other.tilt_ != tilt_)
        return false;
    if(other.yAxisRotation_ != yAxisRotation_)
        return false;
    if(other.coordinateCount_ != coordinateCount_)
        return false;
    if(other.shape_ != shape_)
        return false;
    for(int i = 0; i < coordinateCount_; i++)
    {
        if(other.coordinate_.at(i) != coordinate_.at(i))
            return false;
    }
    return true;
}

/// checks for a valid coordinate index
bool AMShapeData::validIndex(int index)
{
    return (index >= 0 && index <= coordinateCount_);
}
