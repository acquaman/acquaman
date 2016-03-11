#ifndef BIOXASSOLLERSLIT_H
#define BIOXASSOLLERSLIT_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class CLSMAXvMotor;

class BioXASSollerSlit : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASSollerSlit(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSollerSlit();

	/// Returns the curent x value.
	double xValue() const;
	/// Returns the current z value.
	double zValue() const;

	/// Returns the x motor.
	CLSMAXvMotor* xMotor() const { return xMotor_; }
	/// Returns the z motor.
	CLSMAXvMotor* zMotor() const { return zMotor_; }

signals:
	/// Notifier that the x motor value has changed.
	void xValueChanged(double newValue);
	/// Notifier that the z motor control has changed.
	void xMotorChanged(CLSMAXvMotor *newControl);
	/// Notifier that the z motor value has changed.
	void zValueChanged(double newValue);
	/// Notifier tha the z motor control has changed.
	void zMotorChanged(CLSMAXvMotor *newControl);

public slots:
	/// Sets the x motor control.
	void setXMotor(CLSMAXvMotor *newControl);
	/// Sets the z motor control.
	void setZMotor(CLSMAXvMotor *newControl);

protected:
	/// The x motor.
	CLSMAXvMotor *xMotor_;
	/// The z motor.
	CLSMAXvMotor *zMotor_;
};

#endif // BIOXASSOLLERSLIT_H
