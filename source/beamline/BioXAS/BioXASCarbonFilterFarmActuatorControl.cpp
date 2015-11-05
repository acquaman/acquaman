#include "BioXASCarbonFilterFarmActuatorControl.h"

BioXASCarbonFilterFarmActuatorControl::BioXASCarbonFilterFarmActuatorControl(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class varibles.

	currentPosition_ = 0;

	currentWindow_ = new BioXASCarbonFilterFarmActuatorWindowControl(objectName()+"Window", this);

	currentFilter_ = new BioXASCarbonFilterFarmActuatorFilterControl(objectName()+"Filter", "mm", this);
	currentFilter_->setCurrentWindow(currentWindow_);
}

BioXASCarbonFilterFarmActuatorControl::~BioXASCarbonFilterFarmActuatorControl()
{

}

bool BioXASCarbonFilterFarmActuatorControl::isConnected() const
{
	bool connected = (
				currentPosition_ && currentPosition_->isConnected() &&
				currentWindow_ && currentWindow_->isConnected() &&
				currentFilter_ && currentFilter_->isConnected()
				);

	return connected;
}

void BioXASCarbonFilterFarmActuatorControl::addWindowOption(BioXASCarbonFilterFarmWindowOption *newOption)
{
	if (!windowOptions_.contains(newOption)) {
		windowOptions_.append(newOption);

		if (currentWindow_)
			currentWindow_->addWindow(newOption);

		emit windowOptionsChanged();
	}
}

void BioXASCarbonFilterFarmActuatorControl::removeWindowOption(BioXASCarbonFilterFarmWindowOption *option)
{
	if (windowOptions_.contains(option)) {
		windowOptions_.removeOne(option);

		if (currentWindow_)
			currentWindow_->removeWindow(option);

		emit windowOptionsChanged();
	}
}

void BioXASCarbonFilterFarmActuatorControl::clearWindowOptions()
{
	foreach (BioXASCarbonFilterFarmWindowOption *option, windowOptions_)
		removeWindowOption(option);
}

void BioXASCarbonFilterFarmActuatorControl::setCurrentPosition(AMControl *newControl)
{
	if (currentPosition_ != newControl) {

		if (currentPosition_) {
			disconnect( currentPosition_, 0, this, 0 );

			if (currentWindow_)
				currentWindow_->setCurrentPosition(0);
		}

		currentPosition_ = newControl;

		if (currentPosition_) {
			connect( currentPosition_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

			if (currentWindow_)
				currentWindow_->setCurrentPosition(currentPosition_);
		}

		updateConnected();

		emit currentPositionChanged(currentPosition_);
	}
}
