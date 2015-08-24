#ifndef AM3DROTATEDSYSTEMPSEUDOMOTORCONTROL_H
#define AM3DROTATEDSYSTEMPSEUDOMOTORCONTROL_H

#include "beamline/AM3DMotionPseudoMotorControl.h"

class AM3DRotatedSystemPseudoMotorControl : public AM3DMotionPseudoMotorControl
{
    Q_OBJECT
public:
	explicit AM3DRotatedSystemPseudoMotorControl(AxisDesignation axis,
												 AMControl* globalXAxis,
												 AMControl* globalYAxis,
												 AMControl* globalZAxis,
												 const QString &name,
												 const QString &units,
												 QObject *parent = 0,
												 const QString &description = "");

signals:

public slots:
protected:
	QVector3D globalAxisToPrime(const QVector3D &globalVector) const;
	QVector3D primeAxisToGlobal(const QVector3D &primeVector) const;

};

#endif // AM3DROTATEDSYSTEMPSEUDOMOTORCONTROL_H
