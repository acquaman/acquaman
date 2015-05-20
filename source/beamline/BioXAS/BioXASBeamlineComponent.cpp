#include "BioXASBeamlineComponent.h"

BioXASBeamlineComponent::BioXASBeamlineComponent(QObject *parent) :
    QObject(parent)
{
	// Initialize member variables.

	connected_ = false;
}

BioXASBeamlineComponent::~BioXASBeamlineComponent()
{

}

void BioXASBeamlineComponent::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}
