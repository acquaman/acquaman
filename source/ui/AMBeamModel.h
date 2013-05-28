#ifndef AMBEAMMODEL_H
#define AMBEAMMODEL_H

#include <QVector3D>
#include <QVector>
#include "dataman/database/AMDbObject.h"




class AMBeamModel : public AMDbObject
{
    Q_OBJECT
    //Q_PROPERTY(type name READ access WRITE mutate)
public:
    explicit AMBeamModel(QObject *parent = 0);

    QVector<QVector3D> positionOne();
    QVector<QVector3D> positionTwo();
    QVector<QVector3D> ray(int index);

    void setPositionOne(QVector<QVector3D> positionOne);
    void setPositionTwo(QVector<QVector3D> positionTwo);
    void setRay(QVector<QVector3D> ray, int index);


protected:
    static const int ONE;
    static const int TWO;
    static const int THREE;
    static const int FOUR;
    static const int CLOSE;

    QVector<QVector3D> positionOne_;
    QVector<QVector3D> positionTwo_;

};

#endif // AMBEAMMODEL_H
