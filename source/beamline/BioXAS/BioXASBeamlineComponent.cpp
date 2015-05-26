#include "BioXASBeamlineComponent.h"

BioXASBeamlineComponent::BioXASBeamlineComponent(const QString &name, QObject *parent) :
    QObject(parent)
{
	connected_ = false;
	name_ = name;
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

void BioXASBeamlineComponent::updateConnected()
{
	setConnected( isConnected() );
}
