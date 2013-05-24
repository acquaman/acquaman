#include "AMShapeData2.h"



#include <QString>
#include <QVector3D>
#include <QPolygonF>
#include <QDebug>

AMShapeData2::AMShapeData2()
{
    shape_ = new QPolygonF();
    coordinateIndex_ = -1;

}

AMShapeData2::AMShapeData2(QPolygonF shape, QString name, QString otherData,  double idNumber)
{
    shape_ = new QPolygonF();
    *shape_ = shape;
    setName(name);
    setOtherData(otherData);
    setIdNumber(idNumber);
    coordinateIndex_ = -1;

}

QPolygonF* AMShapeData2::shape()
{
    return shape_;
}

QString AMShapeData2::name()
{
    return name_;
}

QString AMShapeData2::otherData()
{
    return otherData_;
}

double AMShapeData2::idNumber()
{
    return idNumber_;
}

QVector3D AMShapeData2::coordinate(int index)
{
    if(validIndex(index))
        return coordinate_[index];
    else
        return QVector3D(0,0,0);
}

double AMShapeData2::height()
{
    return height_;
}

double AMShapeData2::width()
{
    return width_;
}

double AMShapeData2::rotation()
{
    return rotation_;
}

double AMShapeData2::tilt()
{
    return tilt_;
}

void AMShapeData2::setShape(QPolygonF shape)
{
    *shape_ = shape;
}

void AMShapeData2::setName(QString name)
{
    name_ = name;
}

void AMShapeData2::setOtherData(QString otherData)
{
    otherData_ = otherData;
}

void AMShapeData2::setIdNumber(double idNumber)
{
    idNumber_ = idNumber;
}

void AMShapeData2::setCoordinate(QVector3D coordinate, int index)
{
    if(validIndex(index))
        coordinate_[index] = coordinate;
    else qDebug()<<"Failed to set coordinate; invalid index";
}

void AMShapeData2::setCoordinateShape(QVector<QVector3D> coordinates, int count)
{
    if(coordinates.isEmpty()) return;
    coordinate_.clear();
    for(int i = 0; i < count; i++)
    {
        qDebug()<<"Assigning coordinate"<<i<<"Which is"<<coordinates[i];
        if(coordinateIndex_ < i) coordinateIndex_ = i;
        coordinate_<<coordinates[i];
    }
}

void AMShapeData2::setHeight(double height)
{
    height_ = height;
}

void AMShapeData2::setWidth(double width)
{
    width_ = width;
}

void AMShapeData2::setRotation(double rotation)
{
    rotation_ = rotation;
}

void AMShapeData2::setTilt(double tilt)
{
    tilt_ = tilt;
}

bool AMShapeData2::validIndex(int index)
{
    return (index >= 0 && index <= coordinateIndex_);
}
