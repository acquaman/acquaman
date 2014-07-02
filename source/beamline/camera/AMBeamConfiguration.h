/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMBEAMMODEL_H
#define AMBEAMMODEL_H

#include <QVector3D>
#include <QVector>
#include "dataman/database/AMDbObject.h"


//typedef QVector<QVector3D> AMQVector3DVector;
//Q_DECLARE_METATYPE(AMQVector3DVector)



/// Holds the information for where the beam is positioned.
/// contains a 3D representation of the beam using two sets of points
/// to define two shapes.  The beam is interpreted as the rays connecting the two shapes together.

class AMBeamConfiguration : public AMDbObject
{
    Q_OBJECT
    //Q_PROPERTY(type name READ access WRITE mutate)
    Q_PROPERTY(AMQVector3DVector positionOne READ positionOne WRITE setPositionOne)
    Q_PROPERTY(AMQVector3DVector positionTwo READ positionTwo WRITE setPositionTwo)
public:
    explicit AMBeamConfiguration(QObject *parent = 0);

    AMQVector3DVector positionOne() const;
    AMQVector3DVector positionTwo() const;
    AMQVector3DVector ray(int index) const;
    QList<QVector3D> beamOne() const;
    QList<QVector3D> beamTwo() const;

    void setPositionOne(AMQVector3DVector positionOne);
    void setPositionTwo(AMQVector3DVector positionTwo);
    void setRay(QVector<QVector3D> ray, int index);
    void setBeamOne(QList<QVector3D> beamOne);
    void setBeamTwo(QList<QVector3D> beamTwo);

    void alignPositionTwo();

    int count() const;

protected:
    QVector3D findCenter(QVector<QVector3D>) const;


protected:
    QVector<QVector3D> positionOne_;
    QVector<QVector3D> positionTwo_;


};

#endif // AMBEAMMODEL_H
