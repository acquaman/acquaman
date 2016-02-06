#include "BioXASCarbonFilterFarm.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent) :
	BioXASBeamlineComponent(deviceName, parent)
{
	// Create upstream actuator.

	upstreamActuator_ = new BioXASCarbonFilterFarmActuator(QString("%1%2").arg(deviceName).arg("UpstreamActuator"), this);
	addChildControl(upstreamActuator_);

	// Create downstream actuator.

	downstreamActuator_ = new BioXASCarbonFilterFarmActuator(QString("%1%2").arg(deviceName).arg("DownstreamActuator"), this);
	addChildControl(downstreamActuator_);

	// Create filter control.

	filter_ = new BioXASCarbonFilterFarmFilterControl(QString("%1%2").arg(name()).arg("Filter"), "um", this);
	addChildControl(filter_);

	filter_->setUpstreamFilter(upstreamActuator_->filter());
	filter_->setDownstreamFilter(downstreamActuator_->filter());
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}

double BioXASCarbonFilterFarm::filterValue() const
{
	double result = -1;

	if (filter_ && filter_->canMeasure())
		result = filter_->value();

	return result;
}

CLSMAXvMotor* BioXASCarbonFilterFarm::upstreamActuatorMotor() const
{
	CLSMAXvMotor *motor = 0;

	if (upstreamActuator_)
		motor = upstreamActuator_->motor();

	return motor;
}

AMControl* BioXASCarbonFilterFarm::upstreamActuatorPositionStatus() const
{
	AMControl *positionStatus = 0;

	if (upstreamActuator_)
		positionStatus = upstreamActuator_->positionStatus();

	return positionStatus;
}

CLSMAXvMotor* BioXASCarbonFilterFarm::downstreamActuatorMotor() const
{
	CLSMAXvMotor *motor = 0;

	if (downstreamActuator_)
		motor = downstreamActuator_->motor();

	return motor;
}

AMControl* BioXASCarbonFilterFarm::downstreamActuatorPositionStatus() const
{
	AMControl *positionStatus = 0;

	if (downstreamActuator_)
		positionStatus = downstreamActuator_->positionStatus();

	return positionStatus;
}

QString BioXASCarbonFilterFarm::windowToString(double window) const
{
	QString result;

	switch (int(window)) {
	case Window::None:
		result = "None";
		break;
	case Window::Bottom:
		result = "Bottom";
		break;
	case Window::Top:
		result = "Top";
		break;
	default:
		break;
	}

	return result;
}

void BioXASCarbonFilterFarm::setUpstreamActuatorMotor(CLSMAXvMotor *newControl)
{
	if (upstreamActuator_)
		upstreamActuator_->setMotor(newControl);
}

void BioXASCarbonFilterFarm::setUpstreamActuatorPositionStatus(AMControl *newControl)
{
	if (upstreamActuator_)
		upstreamActuator_->setPositionStatus(newControl);
}

void BioXASCarbonFilterFarm::addUpstreamActuatorWindow(int windowIndex, double positionSetpoint, double positionMin, double positionMax, double filter)
{
	if (upstreamActuator_)
		upstreamActuator_->addWindow(windowIndex, windowToString(windowIndex), positionSetpoint, positionMin, positionMax, filter);
}

void BioXASCarbonFilterFarm::setUpstreamActuatorWindowPreference(double filter, int windowIndex)
{
	if (upstreamActuator_)
		upstreamActuator_->setWindowPreference(filter, windowIndex);
}

void BioXASCarbonFilterFarm::setDownstreamActuatorMotor(CLSMAXvMotor *newControl)
{
	if (downstreamActuator_)
		downstreamActuator_->setMotor(newControl);
}

void BioXASCarbonFilterFarm::setDownstreamActuatorPositionStatus(AMControl *newControl)
{
	if (downstreamActuator_)
		downstreamActuator_->setPositionStatus(newControl);
}

void BioXASCarbonFilterFarm::setDownstreamActuatorWindowPreference(double filter, int windowIndex)
{
	if (downstreamActuator_)
		downstreamActuator_->setWindowPreference(filter, windowIndex);
}
