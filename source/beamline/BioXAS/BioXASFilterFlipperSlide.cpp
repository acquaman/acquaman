#include "BioXASFilterFlipperSlide.h"

BioXASFilterFlipperSlide::BioXASFilterFlipperSlide(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize member variables.

	filter_ = 0;
	enabled_ = false;
	cartridgeStatus_ = 0;
}

BioXASFilterFlipperSlide::~BioXASFilterFlipperSlide()
{

}

bool BioXASFilterFlipperSlide::isConnected() const
{
	bool connected = (
				cartridgeStatus_ && cartridgeStatus_->isConnected()
				);

	return connected;
}

void BioXASFilterFlipperSlide::setFilter(BioXASFilterFlipperFilter *newFilter)
{
	if (filter_ != newFilter) {
		filter_ = newFilter;
		emit filterChanged(filter_);
	}
}

void BioXASFilterFlipperSlide::setEnabled(bool isEnabled)
{
	if (enabled_ != isEnabled) {
		enabled_ = isEnabled;
		emit enabledChanged(enabled_);
	}
}

void BioXASFilterFlipperSlide::setCartridgeStatusControl(AMControl *newControl)
{
	if (cartridgeStatus_ != newControl) {
		cartridgeStatus_ = newControl;
		emit cartridgeStatusControlChanged(cartridgeStatus_);
	}
}
