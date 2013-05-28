#include "AMBeamModel.h"



int const AMBeamModel::ONE = 0;
int const AMBeamModel::TWO = 1;
int const AMBeamModel::THREE = 2;
int const AMBeamModel::FOUR = 3;
int const AMBeamModel::CLOSE = 4;


AMBeamModel::AMBeamModel(QObject *parent) :
    AMDbObject(parent)
{
}

QVector<QVector3D> AMBeamModel::positionOne()
{
    return positionOne_;
}

QVector<QVector3D> AMBeamModel::positionTwo()
{
    return positionTwo_;
}

QVector<QVector3D> AMBeamModel::ray(int index)
{
    QVector<QVector3D> ray;
    ray<<(positionOne_.at(index))<<(positionTwo_.at(index));
    return ray;
}


void AMBeamModel::setPositionOne(QVector<QVector3D> positionOne)
{
    positionOne_ = positionOne;
}

void AMBeamModel::setPositionTwo(QVector<QVector3D> positionTwo)
{
    positionTwo_ = positionTwo;
}

void AMBeamModel::setRay(QVector<QVector3D> rayOne, int index)
{
    positionOne_[index]=rayOne[ONE];
    positionTwo_[index]=rayOne[TWO];

    if(index == ONE)
    {
        positionOne_[CLOSE]=rayOne[ONE];
        positionTwo_[CLOSE]=rayOne[TWO];
    }
}

