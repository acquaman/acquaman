#include "BioXASSSRLMonochromatorMask.h"

BioXASSSRLMonochromatorMask::BioXASSSRLMonochromatorMask(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	upperSlitBlade_ = 0;
	lowerSlitBlade_ = 0;
	state_ = 0;
}

BioXASSSRLMonochromatorMask::~BioXASSSRLMonochromatorMask()
{

}

void BioXASSSRLMonochromatorMask::setUpperSlitBlade(AMControl *newControl)
{
	if (upperSlitBlade_ != newControl) {

		if (upperSlitBlade_)
			removeChildControl(upperSlitBlade_);

		upperSlitBlade_ = newControl;

		if (upperSlitBlade_)
			addChildControl(upperSlitBlade_);

		emit upperSlitBladeChanged(upperSlitBlade_);
	}
}

void BioXASSSRLMonochromatorMask::setLowerSlitBlade(AMControl *newControl)
{
	if (lowerSlitBlade_ != newControl) {

		if (lowerSlitBlade_)
			removeChildControl(lowerSlitBlade_);

		lowerSlitBlade_ = newControl;

		if (lowerSlitBlade_)
			addChildControl(lowerSlitBlade_);

		emit lowerSlitBladeChanged(lowerSlitBlade_);
	}
}

void BioXASSSRLMonochromatorMask::setState(AMControl *newControl)
{
	if (state_ != newControl) {

		if (state_)
			removeChildControl(state_);

		state_ = newControl;

		if (state_)
			addChildControl(state_);

		emit stateChanged(state_);
	}
}
