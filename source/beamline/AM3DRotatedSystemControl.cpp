#include "AM3DRotatedSystemControl.h"

AM3DRotatedSystemControl::AM3DRotatedSystemControl(AxisDesignation axis,
																		 AMControl* globalXAxis,
																		 AMControl* globalYAxis,
																		 AMControl* globalZAxis,
																		 const QString &name,
																		 const QString &units,
																		 QObject *parent,
																		 const QString &description) :
	AM3DCoordinatedSystemControl(axis, globalXAxis, globalYAxis, globalZAxis, name, units, parent, description)
{

}

void AM3DRotatedSystemControl::rotate(double rX, double rY, double rZ)
{
	QVector3D vX(1, 0, 0);
	QVector3D vY(0, 1, 0);
	QVector3D vZ(0, 0, 1);

	QQuaternion qX = QQuaternion::fromAxisAndAngle(vX, rX);
	QQuaternion qY = QQuaternion::fromAxisAndAngle(vY, rY);
	QQuaternion qZ = QQuaternion::fromAxisAndAngle(vZ, rZ);

	QQuaternion qTemp = qX * qY;

	globalToPrimeTransform_ = qTemp * qZ;
	globalToPrimeTransform_.normalize();
	primeToGlobalTransform_ = globalToPrimeTransform_.conjugate();

	updateStates();
}

QVector3D AM3DRotatedSystemControl::globalAxisToPrime(const QVector3D &globalVector) const
{
	return globalToPrimeTransform_.rotatedVector(globalVector);
}

QVector3D AM3DRotatedSystemControl::primeAxisToGlobal(const QVector3D &primeVector) const
{
	return primeToGlobalTransform_.rotatedVector(primeVector);
}


