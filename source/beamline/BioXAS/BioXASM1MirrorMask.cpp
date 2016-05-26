#include "BioXASM1MirrorMask.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"

BioXASM1MirrorMask::BioXASM1MirrorMask(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	upperSlitBlade_ = 0;
	state_ = 0;

	// Current settings.

	setState(new BioXASM1MirrorMaskState(name+"State", this));
}

BioXASM1MirrorMask::~BioXASM1MirrorMask()
{

}

bool BioXASM1MirrorMask::isConnected() const
{
	bool connected = (
				upperSlitBlade_ && upperSlitBlade_->isConnected() &&
				state_ && state_->isConnected()
				);

	return connected;
}

void BioXASM1MirrorMask::setUpperSlitBlade(AMControl *newControl)
{
	if (upperSlitBlade_ != newControl) {

                removeChildControl(upperSlitBlade_);
		upperSlitBlade_ = newControl;
                addChildControl(upperSlitBlade_);

		updateState();

		emit upperSlitBladeChanged(upperSlitBlade_);
	}
}

void BioXASM1MirrorMask::setState(BioXASM1MirrorMaskState *newControl)
{
	if (state_ != newControl) {

                removeChildControl(state_);
		state_ = newControl;

                if (addChildControl(state_))
			connect( state_, SIGNAL(upperSlitBladeChanged(AMControl*)), this, SLOT(updateMask()) );

		updateMask();

		emit stateChanged(state_);
	}
}

void BioXASM1MirrorMask::updateState()
{
	if (state_)
		state_->setUpperSlitBlade(upperSlitBlade_);
}

void BioXASM1MirrorMask::updateMask()
{
	AMControl *upperSlitBlade = 0;

	if (state_) { // we do not change the upper slit blade if state_ is not valid.
		upperSlitBlade = state_->upperSlitBlade();
		setUpperSlitBlade(upperSlitBlade);
	}
}
