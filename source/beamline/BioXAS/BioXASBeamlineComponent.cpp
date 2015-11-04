#include "BioXASBeamlineComponent.h"

BioXASBeamlineComponent::BioXASBeamlineComponent(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	connected_ = false;
}

BioXASBeamlineComponent::~BioXASBeamlineComponent()
{

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
