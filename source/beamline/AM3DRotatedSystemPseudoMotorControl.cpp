#include "AM3DRotatedSystemPseudoMotorControl.h"

AM3DRotatedSystemPseudoMotorControl::AM3DRotatedSystemPseudoMotorControl(AxisDesignation axis,
																		 AMControl* globalXAxis,
																		 AMControl* globalYAxis,
																		 AMControl* globalZAxis,
																		 const QString &name,
																		 const QString &units,
																		 QObject *parent,
																		 const QString &description) :
	AM3DMotionPseudoMotorControl(axis, globalXAxis, globalYAxis, globalZAxis, name, units, parent, description)
{

}

QVector3D AM3DRotatedSystemPseudoMotorControl::globalAxisToPrime(const QVector3D &globalVector) const
{
	// Test values for now, obv.
	return QVector3D(globalVector.x() + 10, globalVector.y() + 10, globalVector.z() + 10);
}

QVector3D AM3DRotatedSystemPseudoMotorControl::primeAxisToGlobal(const QVector3D &primeVector) const
{
	// Test values for now, obv.
	return QVector3D(primeVector.x() - 10, primeVector.y() - 10, primeVector.z() - 10);
}
