#include "BioXASBeamlineComponent.h"

BioXASBeamlineComponent::BioXASBeamlineComponent(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	connected_ = false;
}

BioXASBeamlineComponent::~BioXASBeamlineComponent()
{

}

bool BioXASBeamlineComponent::stop() const
{
	bool result = true;

	foreach (AMControl *control, children_) {
		if (control && control->stop())
			result = true;
		else
			result = false;
	}

	return result;
}

void BioXASBeamlineComponent::addChildControl(AMControl *control)
{
	if (control) {
		children_ << control;

		connect( control, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	}
}

void BioXASBeamlineComponent::removeChildControl(AMControl *control)
{
	if (control) {
		disconnect( control, 0, this, 0 );
		children_.removeOne(control);
	}
}

void BioXASBeamlineComponent::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void BioXASBeamlineComponent::updateConnected()
{
	setConnected( isConnected() );
}
