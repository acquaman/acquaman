#include "AMShapeData2.h"



#include <QString>
#include <QVector3D>
#include <QPolygonF>

AMShapeData2::AMShapeData2()
{
    shape_ = new QPolygonF();

}

AMShapeData2::AMShapeData2(QPolygonF shape, QString name, QString otherData,  double idNumber)
{
    shape_ = new QPolygonF();
    *shape_ = shape;
    setName(name);
    setOtherData(otherData);
    setIdNumber(idNumber);

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

QVector3D AMShapeData2::coordinate()
{
    return coordinate_;
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

void AMShapeData2::setCoordinate(QVector3D coordinate)
{
    coordinate_ = coordinate;
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
