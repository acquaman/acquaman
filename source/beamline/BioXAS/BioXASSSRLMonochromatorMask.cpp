#include "BioXASSSRLMonochromatorMask.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

BioXASSSRLMonochromatorMask::BioXASSSRLMonochromatorMask(const QString &deviceName, QObject *parent) :
	BioXASBeamlineComponent(deviceName, parent)
{
	// Initialize class variables.

	upperBlade_ = 0;
	lowerBlade_ = 0;
	bladesState_ = 0;
	state_ = 0;

	// Current settings.

	setState(new BioXASSSRLMonochromatorMaskState(name()+"State", this));
}

BioXASSSRLMonochromatorMask::~BioXASSSRLMonochromatorMask()
{

}

bool BioXASSSRLMonochromatorMask::isConnected() const
{
	bool connected = (
				upperBlade_ && upperBlade_->isConnected() &&
				lowerBlade_ && lowerBlade_->isConnected() &&
				bladesState_ && bladesState_->isConnected() &&
				state_ && state_->isConnected()
				);

	return connected;
}

void BioXASSSRLMonochromatorMask::setUpperBlade(BioXASMAXvMotor *newControl)
{
	if (upperBlade_ != newControl) {

		if (upperBlade_)
			removeChildControl(upperBlade_);

		upperBlade_ = newControl;

		if (upperBlade_)
			addChildControl(upperBlade_);

		updateState();

		emit upperBladeChanged(upperBlade_);
	}
}

void BioXASSSRLMonochromatorMask::setLowerBlade(BioXASMAXvMotor *newControl)
{
	if (lowerBlade_ != newControl) {

		if (lowerBlade_)
			removeChildControl(lowerBlade_);

		lowerBlade_ = newControl;

		if (lowerBlade_)
			addChildControl(lowerBlade_);

		updateState();

		emit lowerBladeChanged(lowerBlade_);
	}
}

void BioXASSSRLMonochromatorMask::setBladesState(AMControl *newControl)
{
	if (bladesState_ != newControl) {

		if (bladesState_)
			removeChildControl(bladesState_);

		bladesState_ = newControl;

		if (bladesState_)
			addChildControl(bladesState_);

		updateState();

		emit bladesStateChanged(bladesState_);
	}
}

void BioXASSSRLMonochromatorMask::setState(BioXASSSRLMonochromatorMaskState *newControl)
{
	if (state_ != newControl) {

		if (state_) {
			disconnect( state_, 0, this, 0 );
			removeChildControl(state_);
		}

		state_ = newControl;

		if (state_) {
			addChildControl(state_);

			connect( state_, SIGNAL(upperBladeChanged(AMControl*)), this, SLOT(setUpperBlade(AMControl*)) );
			connect( state_, SIGNAL(lowerBladeChanged(AMControl*)), this, SLOT(setLowerBlade(AMControl*)) );
			connect( state_, SIGNAL(bladesStateChanged(AMControl*)), this, SLOT(setBladesState(AMControl*)) );
		}

		updateState(); // The state adopts the mask's controls, initially.

		emit stateChanged(state_);
	}
}

void BioXASSSRLMonochromatorMask::updateState()
{
	if (state_) {
		state_->setUpperBlade(upperBlade_);
		state_->setLowerBlade(lowerBlade_);
		state_->setBladesState(bladesState_);
	}
}
