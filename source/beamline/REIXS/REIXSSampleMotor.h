#ifndef REIXSSAMPLEMOTOR_H
#define REIXSSAMPLEMOTOR_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/AMMotorGroup.h"

#include <QVector3D>
#include <QQuaternion>

// Limits within the global axes. Used to calculate limits in the prime axes
#define REIXS_SAMPLE_HORIZONTAL_MAX 29
#define REIXS_SAMPLE_HORIZONTAL_MIN 1
#define REIXS_SAMPLE_NORMAL_MAX 29
#define REIXS_SAMPLE_NORMAL_MIN 1

// Error Codes
#define REIXS_SAMPLE_MOTOR_INVALID_DIRECTION 199840



/*!
  * A class which represents a single motor for performing moves within the plane
  * of the sample plate.
  *
  * As the sample plate rotates about the vertical axis, the global plane of the
  * motion is also rotated to produce a new 'prime' coordinate system. Moves within
  * this new system will then be translated to parallel moves of the normal and
  * horizontal controls of the global system producing in-plane moves of the sample
  * plate.
  *
  * NOTE: To an extent the choice of mapping between vertical, normal, horizontal
  * and X, Y, Z is arbitrary (and differs per beamline). In this case we choose
  * Y = Normal
  * X = Horizontal
  * Z = Vertical
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

	/// The current total rotation of the system
	double totalRotation() const;

	/// The current direction of motion.
	AMMotorGroupObject::MotionDirection direction() const;

	/// Indicates that this control \em can currently take measurements.
	virtual bool canMeasure() const;
	/// Indicates that this control type \em should (assuming it's connected) be able to measure values.
	virtual bool shouldMeasure() const;
	/// Indicates thatthis control \em can (currently) move to setpoints:
	virtual bool canMove() const;
	/// Indicates that this control \em should (assuming it's connected) be able to move to setpoints:
	virtual bool shouldMove() const;
	/// Indicates that this control \em can (currently) issue stop() commands while moves are in progress.
	virtual bool canStop() const;
	/// Indicates that this control \em shoule (assuming it's connected) be able to issue stop() commands while moves are in progress.
	virtual bool shouldStop() const;

	/// Returns true if the given value is a valid value for the sample motor by
	/// translating the value into the global axes. False otherwise.
	virtual bool validValue(double value) const;

signals:

public slots:
	/// Sets the angle offset used for rotating the coordinate system
	void setAngleOffset(double offset);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving status
	virtual void updateMoving();
	/// Handles the vertical axis control being rotated.
	void onVerticalAxisRotated(double value);

	/// Handles clean up of the action, emits signals and updates the min and max
	/// based on the new current position.
	virtual void onMoveFailed(QObject *action);

	/// Handles clean up of the action, emits signals and updates the min and max
	/// based on the new current position.
	virtual void onMoveSucceeded(QObject *action);
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

	/// Creates a movement vector in the global axes from the provided setpoint
	/// based on the current motion axis, and the current value of the other axis.
	QVector3D createGlobalMovementVector(double primeSetpoint) const;

	/// Helper function which returns the value from the vector based on the current
	/// motion direction.
	double valueForDirection(const QVector3D& vector);

	/// Helper function which converts radians to degrees
	double radiansToDegrees(double radians);

	/// Helper function which converts degrees to randians
	double degreesToRadians(double degrees);

	/// Helper function which recalculates the minimum and maximum values for
	/// the sample motor, taking into account the current vertical rotation,
	/// the horizontal prime position and the normal prime position.
	void updateMinimumAndMaximum();

	AMMotorGroupObject::MotionDirection direction_;
	AMControl* horizontalTranslationControl_;
	AMControl* normalTranslationControl_;
	AMControl* verticalRotationControl_;
	double angleOffset_;

	QQuaternion rotationQuaternion_;

	bool connectedOnce_;
};

#endif // REIXSSAMPLEMOTOR_H
