#include "VESPERSMonochomatorControl.h"

 VESPERSMonochomatorControl::~VESPERSMonochomatorControl(){}
VESPERSMonochomatorControl::VESPERSMonochomatorControl(const QString &name, const QString &readPVname, const QString &writePVname, const QString &movingPVname, const QString &stopPVname, QObject *parent, double tolerance, double moveStartTimeoutSeconds, AMAbstractControlStatusChecker *statusChecker, int stopValue, const QString &description)
	: AMPVwStatusControl(name, readPVname, writePVname, movingPVname, stopPVname, parent, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description)
{
}

AMControl::FailureExplanation VESPERSMonochomatorControl::move(double setpoint)
{
	if ((setpoint-setpoint_) < 0.001){

		emit moveSucceeded();
		return AMControl::NoFailure;
	}

	return AMPVwStatusControl::move(setpoint);
}
