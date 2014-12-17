#include "BioXASPseudoMotorControl.h"

BioXASPseudoMotorControl::BioXASPseudoMotorControl(const QString &name, const QString& pvBaseName, const QString &readPVname, const QString &writePVname, const QString &movingPVname, const QString &enabledPVname, const QString &stopPVname, double tolerance, double moveStartTimeoutSeconds, AMAbstractControlStatusChecker *statusChecker, int stopValue, const QString &description, QObject *parent)
	: CLSPseudoMotorControl(name, pvBaseName+readPVname, pvBaseName+writePVname, pvBaseName+movingPVname, pvBaseName+stopPVname, parent, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description)
{
	pvBaseName_ = pvBaseName;

	statusPVControl_ = new AMReadOnlyPVControl("PseudoMotor moving status", pvBaseName+movingPVname, this);
	enabledPVControl_ = new AMReadOnlyPVControl("PseudoMotor enabled status", pvBaseName+enabledPVname, this);

	connect(enabledPVControl_, SIGNAL(valueChanged(double)), this, SIGNAL(enabledPVValueChanged(double)));
}

QString BioXASPseudoMotorControl::pvBaseName() const
{
	return pvBaseName_;
}

QString BioXASPseudoMotorControl::enabledPVName() const
{
	return enabledPVControl_->readPVName();
}

QString BioXASPseudoMotorControl::statusPVName() const
{
	return statusPVControl_->readPVName();
}

AMReadOnlyPVControl * BioXASPseudoMotorControl::statusPVControl()
{
	return statusPVControl_;
}

double BioXASPseudoMotorControl::enabledPVStatus()
{
	double status = -1;
	if (enabledPVControl_->isConnected())
		status = enabledPVControl_->value();
	return status;
}

bool BioXASPseudoMotorControl::canMove() const
{
	return enabledPVControl_->isConnected() ? (writePV_->canWrite() && (enabledPVControl_->value() == 1)) : false;
}
