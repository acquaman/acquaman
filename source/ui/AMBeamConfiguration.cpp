#include "AMBeamConfiguration.h"



int const AMBeamConfiguration::ONE = 0;
int const AMBeamConfiguration::TWO = 1;
int const AMBeamConfiguration::THREE = 2;
int const AMBeamConfiguration::FOUR = 3;
int const AMBeamConfiguration::CLOSE = 4;


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


void AMBeamConfiguration::setPositionOne(QVector<QVector3D> positionOne)
{
    positionOne_ = positionOne;
}

void AMBeamConfiguration::setPositionTwo(QVector<QVector3D> positionTwo)
{
    positionTwo_ = positionTwo;
}

void AMBeamConfiguration::setRay(QVector<QVector3D> rayOne, int index)
{
    positionOne_[index]=rayOne[ONE];
    positionTwo_[index]=rayOne[TWO];

    if(index == ONE)
    {
        positionOne_[CLOSE]=rayOne[ONE];
        positionTwo_[CLOSE]=rayOne[TWO];
    }
}

