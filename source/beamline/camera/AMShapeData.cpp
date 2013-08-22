#include "AMShapeData.h"




#include <QString>
#include <QVector3D>
#include <QPolygonF>
#include <QDebug>

/// constructor
AMShapeData::AMShapeData(QObject* parent)
    :QObject(parent)
{
    shape_ = new QPolygonF();
    setOtherDataFieldOne("TestString");
    setOtherDataFieldTwo("");
    coordinateCount_ = -1;
    visible_ = true;
}

/// constructor
AMShapeData::AMShapeData(QPolygonF shape, QString name, QString otherDataFieldOne, QString otherDataFieldTwo, double idNumber, QObject *parent)
    :QObject(parent)
{
    shape_ = new QPolygonF();
    *shape_ = shape;
    setName(name);
    setOtherDataFieldOne(otherDataFieldOne);
    setOtherDataFieldTwo(otherDataFieldTwo);
    setIdNumber(idNumber);
    coordinateCount_ = -1;
    visible_ = true;
}


AMShapeData::~AMShapeData()
{
    coordinate_.clear();
}

QPolygonF* AMShapeData::shape() const
{
    return shape_;
}

QString AMShapeData::name() const
{
    return name_;
}

QString AMShapeData::otherDataFieldOne() const
{
    return otherDataFieldOne_;
}

QString AMShapeData::otherDataFieldTwo() const
{
    return otherDataFieldTwo_;
}

double AMShapeData::idNumber() const
{
    return idNumber_;
}

QVector3D AMShapeData::coordinate(int index) const
{
    if(validIndex(index))
        return coordinate_[index];
    else
        return QVector3D(0,0,0);
}


double AMShapeData::rotation() const
{
    return zAxisRotation_;
}

double AMShapeData::tilt() const
{
    return xAxisRotation_;
}

double AMShapeData::yAxisRotation() const
{
    return yAxisRotation_;
}

bool AMShapeData::visible() const
{
    return visible_;
}


void AMShapeData::setShape(const QPolygonF shape)
{
    *shape_ = shape;
}

void AMShapeData::setName(QString name)
{
    if(name_ != name)
    {
        name_ = name;
        emit nameChanged(name_);
    }
}

void AMShapeData::setOtherDataFieldOne(QString otherData)
{
    if(otherDataFieldOne_ != otherData)
    {
        qDebug()<<"AMShapeData::setOtherDataFieldOne - setting otherDataOne";
        otherDataFieldOne_ = otherData;
        emit otherDataFieldOneChanged(otherData);
    }
}

void AMShapeData::setOtherDataFieldTwo(QString otherDataFieldTwo)
{
    if(otherDataFieldTwo != otherDataFieldTwo)
    {
        otherDataFieldTwo_ = otherDataFieldTwo;
        emit otherDataFieldTwoChanged(otherDataFieldTwo);
    }
}

void AMShapeData::setIdNumber(double idNumber)
{
    idNumber_ = idNumber;
}

void AMShapeData::setCoordinate(QVector3D coordinate, int index)
{
    if(validIndex(index))
    {
        coordinate_[index] = coordinate;
    }
}

void AMShapeData::setCoordinateShape(QVector<QVector3D> coordinates, int count)
{
    if(coordinates.isEmpty()) return;
    coordinate_.clear();
    for(int i = 0; i < count; i++)
    {
        if(coordinateCount_ < i)
        {
            coordinateCount_ = i;
        }
        coordinate_<<coordinates[i];
        if(coordinateCount_ < coordinate_.count())
            coordinateCount_ = coordinate_.count();
    }
}


void AMShapeData::setRotation(double zAxisRotation)
{
    zAxisRotation_ = zAxisRotation;
}

void AMShapeData::setTilt(double xAxisRotation)
{
    xAxisRotation_ = xAxisRotation;
}

void AMShapeData::setYAxisRotation(double yAxisRotation)
{
    yAxisRotation_ = yAxisRotation;
}

void AMShapeData::setVisible(bool visible)
{
    visible_ = visible;
}

void AMShapeData::copy(const AMShapeData *other)
{
    setName(other->name());
    setOtherDataFieldOne(other->otherDataFieldOne());
    setIdNumber(other->idNumber());
    setRotation(other->rotation());
    setTilt(other->tilt());
    setYAxisRotation(other->yAxisRotation());
    setVisible(other->visible());
    setShape(*other->shape());
    QVector<QVector3D> nullShape;
    for(int i = 0; i < other->count(); i++)
    {
        nullShape.append(QVector3D(0,0,0));
    }
    setCoordinateShape(nullShape,other->count());
    for(int i = 0; i < other->count(); i++)
    {
        setCoordinate(other->coordinate(i),i);

    }
    if(count() != other->count())
    {
        qDebug()<<"AMShapeData::copy - counts not equal";
    }
}

/// finds the center of the shape - must be rectangular
QVector3D AMShapeData::centerCoordinate() const
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
int AMShapeData::count() const
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

bool AMShapeData::isEqual(const AMShapeData &other) const
{
    if(other.name_ != name_)
        return false;
    if(other.idNumber_ != idNumber_)
        return false;
    if(other.otherDataFieldOne_ != otherDataFieldOne_)
        return false;
    if(other.zAxisRotation_ != zAxisRotation_)
        return false;
    if(other.xAxisRotation_ != xAxisRotation_)
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
bool AMShapeData::validIndex(int index) const
{
    return (index >= 0 && index <= coordinateCount_);
}
