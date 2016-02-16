#include "BioXASCarbonFilterFarm.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent) :
	BioXASBeamlineComponent(deviceName, parent)
{
	// Create upstream actuator.

	upstreamActuator_ = new BioXASCarbonFilterFarmActuator(QString("%1%2").arg(deviceName).arg("UpstreamActuator"), this);
	addChildControl(upstreamActuator_);

	connect( upstreamActuator_, SIGNAL(motorChanged(CLSMAXvMotor*)), this, SIGNAL(upstreamActuatorMotorChanged(CLSMAXvMotor*)) );
	connect( upstreamActuator_, SIGNAL(positionStatusChanged(AMControl*)), this, SIGNAL(upstreamActuatorPositionStatusChanged(AMControl*)) );
	connect( upstreamActuator_, SIGNAL(windowsChanged()), this, SIGNAL(upstreamActuatorWindowsChanged()) );
	connect( upstreamActuator_, SIGNAL(windowPreferencesChanged()), this, SIGNAL(upstreamActuatorWindowPreferencesChanged()) );

	// Create downstream actuator.

	downstreamActuator_ = new BioXASCarbonFilterFarmActuator(QString("%1%2").arg(deviceName).arg("DownstreamActuator"), this);
	addChildControl(downstreamActuator_);

	connect( downstreamActuator_, SIGNAL(motorChanged(CLSMAXvMotor*)), this, SIGNAL(downstreamActuatorMotorChanged(CLSMAXvMotor*)) );
	connect( downstreamActuator_, SIGNAL(positionStatusChanged(AMControl*)), this, SIGNAL(downstreamActuatorPositionStatusChanged(AMControl*)) );
	connect( downstreamActuator_, SIGNAL(windowsChanged()), this, SIGNAL(downstreamActuatorWindowsChanged()) );
	connect( downstreamActuator_, SIGNAL(windowPreferencesChanged()), this, SIGNAL(downstreamActuatorWindowPreferencesChanged()) );

	// Create filter control.

	filter_ = new BioXASCarbonFilterFarmFilterControl(QString("%1%2").arg(name()).arg("Filter"), "um", this);
	addChildControl(filter_);

	filter_->setUpstreamFilter(upstreamActuator_->filter());
	filter_->setDownstreamFilter(downstreamActuator_->filter());

	connect( filter_, SIGNAL(valueChanged(double)), this, SIGNAL(filterValueChanged(double)) );
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}

bool BioXASCarbonFilterFarm::isConnected() const
{
	bool connected = (
				upstreamActuator_ && upstreamActuator_->isConnected() &&
				downstreamActuator_ && downstreamActuator_->isConnected() &&
				filter_ && filter_->isConnected()
				);

	return connected;
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

void BioXASCarbonFilterFarm::removeUpstreamActuatorWindow(int windowIndex)
{
	if (upstreamActuator_)
		upstreamActuator_->removeWindow(windowIndex);
}

void BioXASCarbonFilterFarm::clearUpstreamActuatorWindows()
{
	if (upstreamActuator_)
		upstreamActuator_->clearWindows();
}

void BioXASCarbonFilterFarm::setUpstreamActuatorWindowPreference(double filter, int windowIndex)
{
	if (upstreamActuator_)
		upstreamActuator_->setWindowPreference(filter, windowIndex);
}

void BioXASCarbonFilterFarm::removeUpstreamActuatorWindowPreference(double filter)
{
	if (upstreamActuator_)
		upstreamActuator_->removeWindowPreference(filter);
}

void BioXASCarbonFilterFarm::clearUpstreamActuatorWindowPreferences()
{
	if (upstreamActuator_)
		upstreamActuator_->clearWindowPreferences();
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

void BioXASCarbonFilterFarm::addDownstreamActuatorWindow(int windowIndex, double positionSetpoint, double positionMin, double positionMax, double filter)
{
	if (downstreamActuator_)
		downstreamActuator_->addWindow(windowIndex, windowToString(windowIndex), positionSetpoint, positionMin, positionMax, filter);
}

void BioXASCarbonFilterFarm::removeDownstreamActuatorWindow(int windowIndex)
{
	if (downstreamActuator_)
		downstreamActuator_->removeWindow(windowIndex);
}

void BioXASCarbonFilterFarm::clearDownstreamActuatorWindows()
{
	if (downstreamActuator_)
		downstreamActuator_->clearWindows();
}

void BioXASCarbonFilterFarm::setDownstreamActuatorWindowPreference(double filter, int windowIndex)
{
	if (downstreamActuator_)
		downstreamActuator_->setWindowPreference(filter, windowIndex);
}

void BioXASCarbonFilterFarm::removeDownstreamActuatorWindowPreference(double filter)
{
	if (downstreamActuator_)
		downstreamActuator_->removeWindowPreference(filter);
}

void BioXASCarbonFilterFarm::clearDownstreamActuatorWindowPreferences()
{
	if (downstreamActuator_)
		downstreamActuator_->clearWindowPreferences();
}

void BioXASCarbonFilterFarm::clearWindows()
{
	clearUpstreamActuatorWindows();
	clearDownstreamActuatorWindows();
}

void BioXASCarbonFilterFarm::clearWindowPreferences()
{
	clearUpstreamActuatorWindowPreferences();
	clearDownstreamActuatorWindowPreferences();
}
