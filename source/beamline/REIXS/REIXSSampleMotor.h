#ifndef REIXSSAMPLEMOTOR_H
#define REIXSSAMPLEMOTOR_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/AMMotorGroup.h"

#include <QVector3D>
#include <QQuaternion>

class REIXSSampleMotor : public AMPseudoMotorControl
{
    Q_OBJECT
public:
    explicit REIXSSampleMotor(const QString& name,
	                          const QString& units,
	                          AMControl* horizontalTranslationControl,
	                          AMControl* normalTranslationControl,
	                          AMControl* verticalRotationControl,
	                          QObject *parent = 0,
	                          const QString& description = QString());

	double angleOffset() const;

	void setAngleOffset(double offset);

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

	QVector3D globalSystemToPrime(const QVector3D& globalVector) const;

	QVector3D primeSystemToGlobal(const QVector3D& primeVector) const;

	void updateMinimumAndMaximum();

	AMControl* horizontalTranslationControl_;
	AMControl* normalTranslationControl_;
	AMControl* verticalRotationControl_;
	double angleOffset_;

	QQuaternion rotationQuaternion_;

	bool connectedOnce_;
};

#endif // REIXSSAMPLEMOTOR_H
