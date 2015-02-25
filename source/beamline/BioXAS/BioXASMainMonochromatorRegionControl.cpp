#include "BioXASMainMonochromatorRegionControl.h"

BioXASMainMonochromatorRegionControl::BioXASMainMonochromatorRegionControl(const QString &name, BioXASMainMonochromator *mono, QObject *parent) :
	AMControl(name, "", parent, "")
{
	// Initialize variables.

	mono_ = 0;
	region_ = Region::None;
	regionSetpoint_ = Region::None;

	// Current settings.

	setMono(mono);
}

BioXASMainMonochromatorRegionControl::~BioXASMainMonochromatorRegionControl()
{

}

bool BioXASMainMonochromatorRegionControl::isConnected() const
{
	if (mono_)
		return mono_->isConnected();

	return false;
}

void BioXASMainMonochromatorRegionControl::setMono(BioXASMainMonochromator *newMono)
{
	if (mono_ != newMono) {
		mono_ = newMono;
		emit monoChanged(mono_);
	}
}

void BioXASMainMonochromatorRegionControl::setRegion(Region::State newState)
{
	if (region_ != newState) {
		region_ = newState;
		emit regionChanged(region_);
	}
}
