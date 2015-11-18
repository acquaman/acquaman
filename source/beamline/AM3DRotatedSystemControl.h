#ifndef AM3DROTATEDSYSTEMCONTROL_H
#define AM3DROTATEDSYSTEMCONTROL_H

#include "beamline/AM3DCoordinatedSystemControl.h"

#include <QQuaternion>
/*!
  * A class representing an interface which coordinates movements in some
  * arbitrarily defined rotated coordinate system across a global coordinate
  * system, given by the provided globalAxis controls.
  */
class AM3DRotatedSystemControl : public AM3DCoordinatedSystemControl
{
    Q_OBJECT
public:
	/*!
	  * Creates a motor for performing movements within the provided axis, in a
	  * system which is not yet rotated from the global system.
	  * Rotations can be performed on the system using the rotate function
	  * \param axis ~ The axis is the rotated system that the motor will represent.
	  * \param globalXAxis ~ The motor control for the global X Axis.
	  * \param globalYAxis ~ The motor control for the global Y Axis.
	  * \param globalZAxis ~ The motor control for the global Z Axis.
	  * \param name ~ A name to give this motor control.
	  * \param units ~ The units of measurement for the axis.
	  * \param parent ~ The QObject parent.
	  * \param description ~ A human readable description of the motor.
	  */
	explicit AM3DRotatedSystemControl(AxisDesignation axis,
												 AMControl* globalXAxis,
												 AMControl* globalYAxis,
												 AMControl* globalZAxis,
												 const QString &name,
												 const QString &units,
												 QObject *parent = 0,
												 const QString &description = "");

	/*!
	  * Rotates the coordinate system using the provided Euler angles.
	  * NOTE: Rotations are provided in sequence, X, Y, Z. and are applied
	  * cumulatively.
	  * \param rX ~ The value to rotate the system about the X Axis (1st)
	  * \param rY ~ The value to rotate the system about the Y Axis (2nd)
	  * \param rZ ~ The value to rotate the system about the Z Axis (3rd)
	  */
	void rotate(double rX, double rY, double rZ);

	/*!
	  * Resets the rotation of the coordinate system back to that of the global
	  * system used natively by the motors.
	  */
	void resetRotationsToGlobal();

	/*!
	  * Helper method which converts the provided vector from the global system
	  * to the rotated system in which this motor operates.
	  * \param globalVector ~ The vector of movement in the global system which
	  * is to be converted.
	  */
	QVector3D globalAxisToPrime(const QVector3D &globalVector) const;

	/*!
	  * Helper method which converts the provided vector from the rotated system
	  * in which this motor operates back to the global system.
	  * \param primeVector ~ The vector of movement in the rotated prime axis which
	  * is to be converted.
	  */
	QVector3D primeAxisToGlobal(const QVector3D &primeVector) const;
signals:

public slots:
protected:


	QQuaternion globalToPrimeTransform_;
	QQuaternion primeToGlobalTransform_;
};

#endif // AM3DROTATEDSYSTEMCONTROL_H
