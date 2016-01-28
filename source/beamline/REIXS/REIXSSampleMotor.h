#ifndef REIXSSAMPLEMOTOR_H
#define REIXSSAMPLEMOTOR_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/AMMotorGroup.h"

#include <QVector3D>
#include <QQuaternion>

#define REIXS_GLOBAL_MAX_X 30.0
#define REIXS_GLOBAL_MIN_X 0.0
#define REIXS_GLOBAL_MAX_Y 30.0
#define REIXS_GLOBAL_MIN_Y 0.0

// Error Codes
#define REIXS_SAMPLE_MOTOR_INVALID_DIRECTION 199840



/*!
  * A class which represents a single motor for performing horizontal moves within
  * the plane of the sample plate.
  *
  * As the sample plate rotates about the vertical axis, the plane of the horizontal
  * motions is also rotated to produce a new 'prime' coordinate system. Moves within
  * this new system will then be translated to parallel moves of the normal and
  * horizontal controls of the global system producing in-plane moves of the sample
  * plate.
  */
class REIXSSampleMotor : public AMPseudoMotorControl
{
    Q_OBJECT
public:
	/*! Create an instance of a REIXS sample motor with the provided parameters:
	 *
	 * \param direction ~ The direction of motion (only Horizontal and Normal are
	 * supported).
	 * \param name ~ The name of the sample motor
	 * \param units ~ The units of the sample motor
	 * \param horizontalTranslationControl ~ The control used to perform horizontal
	 * motions in the global coordinate system.
	 * \param normalTranslationControl ~ The control used to perform normal motions
	 * in the global coordinate system.
	 * \param verticalRotationControl ~ The vertical rotation control which is monitored
	 * in order to rotate the coordinate system of this control
	 * \param parent ~ The QObject parent of the sample motor
	 * \param description ~ A description of the sample motor
	 */
    explicit REIXSSampleMotor(AMMotorGroupObject::MotionDirection direction,
	                          const QString& name,
	                          const QString& units,
	                          AMControl* horizontalTranslationControl,
	                          AMControl* normalTranslationControl,
	                          AMControl* verticalRotationControl,
	                          QObject *parent = 0,
	                          const QString& description = QString());

	/// The offset to the angle used for rotating the coordinate system
	double angleOffset() const;

	/// Sets the angle offset used for rotating the coordinate system
	void setAngleOffset(double offset);

	/// The current total rotation of the system
	double totalRotation() const;

	/// The current direction of motion.
	AMMotorGroupObject::MotionDirection direction() const;
signals:

public slots:

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the minimum value based on the current rotation
	virtual void updateMinimumValue();
	/// Updates the maximum value based on the current rotation
	virtual void updateMaximumValue();
	/// Handles the vertical axis control being rotated.
	void onVerticalAxisRotated(double value);
protected:

	/// Creates and returns a move action which will move to the provided setpoint
	/// taking into account the current rotation of the system.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Helper function which calculates the rotation of the coordinate system
	void rotateSystem();

	/// Helper function which converts the provided global vector to the rotated
	/// prime coordinate system
	QVector3D globalSystemToPrime(const QVector3D& globalVector) const;

	/// Helper function which converts the provided rotated prime vector to the
	/// global coordinate system
	QVector3D primeSystemToGlobal(const QVector3D& primeVector) const;

	/// Updates the minimum and maximum value of the control based on the current
	/// coordinate system rotation.
	void updateMinimumAndMaximum();

	/// Calculates the bound for a given quadrant of a cartesian system, given the
	/// first and second bounds of that quadrant within the global system
	/// and the rotation angle of the prime system.
	double boundsForQuardrant(double firstBound,
	                          double secondBound,
	                          double rotationAngle);

	/// Helper function which returns the value from the vector based on the current
	/// motion direction.
	double valueForDirection(const QVector3D& vector);

	/// Helper function which converts radians to degrees
	double radiansToDegrees(double radians);

	/// Helper function which converts degrees to randians
	double degreesToRadians(double degrees);

	AMMotorGroupObject::MotionDirection direction_;
	AMControl* horizontalTranslationControl_;
	AMControl* normalTranslationControl_;
	AMControl* verticalRotationControl_;
	double angleOffset_;

	QQuaternion rotationQuaternion_;

	bool connectedOnce_;
};

#endif // REIXSSAMPLEMOTOR_H
