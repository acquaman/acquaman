#include "AMBeamConfiguration.h"



#define TOPLEFT 0
#define CLOSE 4



AMBeamConfiguration::AMBeamConfiguration(QObject *parent) :
    AMDbObject(parent)
{
}

QVector<QVector3D> AMBeamConfiguration::positionOne()
{
    return positionOne_;
}

QVector<QVector3D> AMBeamConfiguration::positionTwo()
{
    return positionTwo_;
}

QVector<QVector3D> AMBeamConfiguration::ray(int index)
{
    QVector<QVector3D> ray;
    ray<<(positionOne_.at(index))<<(positionTwo_.at(index));
    return ray;
}

QList<QVector3D> AMBeamConfiguration::beamOne()
{
    QList<QVector3D> beam;
    for(int i = 0; i < positionOne_.count(); i++)
    {
        beam<<positionOne_.at(i);
    }
    return beam;
}

QList<QVector3D> AMBeamConfiguration::beamTwo()
{
    QList<QVector3D> beam;
    for(int i = 0; i < positionTwo_.count(); i++)
    {
        beam<<positionTwo_.at(i);
    }
    return beam;
}

#include <QDebug>
void AMBeamConfiguration::setPositionOne(QVector<QVector3D> positionOne)
{
    positionOne_ = positionOne;
//    qDebug()<<(findCenter(positionOne)-QVector3D(0,0,40));
}

void AMBeamConfiguration::setPositionTwo(QVector<QVector3D> positionTwo)
{
    positionTwo_ = positionTwo;
}

void AMBeamConfiguration::setRay(QVector<QVector3D> rayOne, int index)
{
    positionOne_[index]=rayOne[0];
    positionTwo_[index]=rayOne[1];

    if(index == TOPLEFT)
    {
        positionOne_[CLOSE]=rayOne[0];
        positionTwo_[CLOSE]=rayOne[1];
    }
}

void AMBeamConfiguration::setBeamOne(QList<QVector3D> beamOne)
{
    positionOne_.clear();
    for(int i = 0; i < beamOne.count(); i++)
    {
        positionOne_<<beamOne.at(i);
    }
}

void AMBeamConfiguration::setBeamTwo(QList<QVector3D> beamTwo)
{
    positionTwo_.clear();
    for(int i = 0; i < beamTwo.count(); i++)
    {
        positionTwo_<<beamTwo.at(i);
    }
}

void AMBeamConfiguration::alignPositionTwo()
{
    QVector3D centerOne = findCenter(positionOne_);
    QVector3D centerTwo = findCenter(positionTwo_);
    QVector3D ray = centerTwo - centerOne;
    QVector<QVector3D> newPosition;
    for(int i = 0; i < positionOne_.count(); i++)
    {
        newPosition<<(positionOne_.at(i)+ray);
    }
    setPositionTwo(newPosition);
}

int AMBeamConfiguration::count()
{
    return std::min(positionOne_.count(), positionTwo_.count());
}

QVector3D AMBeamConfiguration::findCenter(QVector<QVector3D> shape)
{
    QVector3D sum(0,0,0);
    int count = 0;
    while(!shape.isEmpty())
    {
        sum += shape.first();
        shape.remove(0);
        count++;
    }
    return sum/count;

}

