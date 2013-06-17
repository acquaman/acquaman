#ifndef AMBEAMMODEL_H
#define AMBEAMMODEL_H

#include <QVector3D>
#include <QVector>
#include "dataman/database/AMDbObject.h"


typedef QVector<QVector3D> AMQVector3DVector;
Q_DECLARE_METATYPE(AMQVector3DVector);



class AMBeamConfiguration : public AMDbObject
{
    Q_OBJECT
    //Q_PROPERTY(type name READ access WRITE mutate)
    Q_PROPERTY(AMQVector3DVector positionOne READ positionOne WRITE setPositionOne)
    Q_PROPERTY(AMQVector3DVector positionTwo READ positionTwo WRITE setPositionTwo)
public:
    explicit AMBeamConfiguration(QObject *parent = 0);

    AMQVector3DVector positionOne();
    AMQVector3DVector positionTwo();
    AMQVector3DVector ray(int index);
    QList<QVector3D> beamOne();
    QList<QVector3D> beamTwo();

    void setPositionOne(AMQVector3DVector positionOne);
    void setPositionTwo(AMQVector3DVector positionTwo);
    void setRay(QVector<QVector3D> ray, int index);
    void setBeamOne(QList<QVector3D> beamOne);
    void setBeamTwo(QList<QVector3D> beamTwo);

    void alignPositionTwo();

    int count();

protected:
    QVector3D findCenter(QVector<QVector3D>);


protected:
    QVector<QVector3D> positionOne_;
    QVector<QVector3D> positionTwo_;

};

#endif // AMBEAMMODEL_H
