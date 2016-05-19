#include "BioXASMAXvMotorMoveToLimitActionInfo.h"

BioXASMAXvMotorMoveToLimitActionInfo::BioXASMAXvMotorMoveToLimitActionInfo(const AMControlInfo &motor, CLSMAXvMotor::Limit limitSetpoint, QObject *parent) :
	AMActionInfo3(QString(), QString(), QString(), parent)
{
	controlInfo_ = motor;
	limitSetpoint_ = limitSetpoint;
	updateDescriptionText();
}

BioXASMAXvMotorMoveToLimitActionInfo::BioXASMAXvMotorMoveToLimitActionInfo(const BioXASMAXvMotorMoveToLimitActionInfo &original) :
	AMActionInfo3(original)
{
	controlInfo_ = original.controlInfo();
	limitSetpoint_ = original.limitSetpoint();
	updateDescriptionText();
}

BioXASMAXvMotorMoveToLimitActionInfo::~BioXASMAXvMotorMoveToLimitActionInfo()
{

}

AMActionInfo3* BioXASMAXvMotorMoveToLimitActionInfo::createCopy() const
{
	AMActionInfo3 *info = new BioXASMAXvMotorMoveToLimitActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

QString BioXASMAXvMotorMoveToLimitActionInfo::limitSetpointToString(CLSMAXvMotor::Limit limit) const
{
	switch (limit) {
	case CLSMAXvMotor::LimitNone:
		return "None";
	case CLSMAXvMotor::LimitCW:
		return "Clockwise";
	case CLSMAXvMotor::LimitCCW:
		return "Counter-clockwise";
	case CLSMAXvMotor::LimitError:
		return "Error";
	default:
		return "Unknown";
	}
}

void BioXASMAXvMotorMoveToLimitActionInfo::setControlInfo(const AMControlInfo &info)
{
	controlInfo_.setValuesFrom(info);
	setModified(true);
	updateDescriptionText();
}

void BioXASMAXvMotorMoveToLimitActionInfo::setLimitSetpoint(CLSMAXvMotor::Limit setpoint)
{
	limitSetpoint_ = setpoint;
	setModified(true);
	updateDescriptionText();
}

void BioXASMAXvMotorMoveToLimitActionInfo::updateDescriptionText()
{
	QString d = QString("Moving %1 to '%2' limit.").arg(controlInfo_.description().isEmpty() ? controlInfo_.name() : controlInfo_.description()).arg(limitSetpointToString(limitSetpoint_));

	setShortDescription(d);
	setLongDescription(d);
}
