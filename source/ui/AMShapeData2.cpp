#include "AMShapeData2.h"



#include <QString>

AMShapeData2::AMShapeData2()
{
    rectangle_ = new QRectF();

}

AMShapeData2::AMShapeData2(QRectF rectangle, QString name, QString otherData,  double idNumber)
{
    rectangle_ = new QRectF();
    *rectangle_ = rectangle;
    setName(name);
    setOtherData(otherData);
    setIdNumber(idNumber);

}

QRectF* AMShapeData2::rectangle()
{
    return rectangle_;
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

void AMShapeData2::setRectangle(QRectF rectangle)
{
    *rectangle_ = rectangle;
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
