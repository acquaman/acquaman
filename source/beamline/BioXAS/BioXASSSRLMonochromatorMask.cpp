#include "BioXASSSRLMonochromatorMask.h"

BioXASSSRLMonochromatorMask::BioXASSSRLMonochromatorMask(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	upperBlade_ = 0;
	lowerBlade_ = 0;
	state_ = 0;
}

BioXASSSRLMonochromatorMask::~BioXASSSRLMonochromatorMask()
{

}

void BioXASSSRLMonochromatorMask::setUpperBlade(AMControl *newControl)
{
	if (upperBlade_ != newControl) {

		if (upperBlade_)
			removeChildControl(upperBlade_);

		upperBlade_ = newControl;

		if (upperBlade_)
			addChildControl(upperBlade_);

		emit upperBladeChanged(upperBlade_);
	}
}

void BioXASSSRLMonochromatorMask::setLowerBlade(AMControl *newControl)
{
	if (lowerBlade_ != newControl) {

		if (lowerBlade_)
			removeChildControl(lowerBlade_);

		lowerBlade_ = newControl;

		if (lowerBlade_)
			addChildControl(lowerBlade_);

		emit lowerBladeChanged(lowerBlade_);
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
