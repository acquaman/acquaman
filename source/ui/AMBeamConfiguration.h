#ifndef AMBEAMMODEL_H
#define AMBEAMMODEL_H

#include <QVector3D>
#include <QVector>
#include "dataman/database/AMDbObject.h"




class AMBeamConfiguration : public AMDbObject
{
    Q_OBJECT
    //Q_PROPERTY(type name READ access WRITE mutate)
    Q_PROPERTY(QList<QVector3D> positionOne READ beamOne WRITE setBeamOne)
    Q_PROPERTY(QList<QVector3D> positionTwo READ beamTwo WRITE setBeamTwo)
public:
    explicit AMBeamConfiguration(QObject *parent = 0);

    QVector<QVector3D> positionOne();
    QVector<QVector3D> positionTwo();
    QVector<QVector3D> ray(int index);
    QList<QVector3D> beamOne();
    QList<QVector3D> beamTwo();

    void setPositionOne(QVector<QVector3D> positionOne);
    void setPositionTwo(QVector<QVector3D> positionTwo);
    void setRay(QVector<QVector3D> ray, int index);
    void setBeamOne(QList<QVector3D> beamOne);
    void setBeamTwo(QList<QVector3D> beamTwo);

    void alignPositionTwo();

protected:
    QVector3D findCenter(QVector<QVector3D>);


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
