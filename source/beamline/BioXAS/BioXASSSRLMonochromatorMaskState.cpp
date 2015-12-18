#include "BioXASSSRLMonochromatorMaskState.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASSSRLMonochromatorMaskState::BioXASSSRLMonochromatorMaskState(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("SSRL Monochromator Mask State");
	setAllowsMovesWhileMoving(false);

	// Setup value options.

	addValueOption(Open, "Open", Open);
	addValueOption(Closed, "Closed", Closed);
}

BioXASSSRLMonochromatorMaskState::~BioXASSSRLMonochromatorMaskState()
{

}

bool BioXASSSRLMonochromatorMaskState::canMove() const
{
	bool result = false;

	if (isConnected())
		result = ( upperBlade_->canMove() && lowerBlade_->canMove() );

	return result;
}

bool BioXASSSRLMonochromatorMaskState::isConnected() const
{
	bool connected = (
				AMSingleEnumeratedControl::isConnected() &&
				upperBlade_ && upperBlade_->isConnected() &&
				lowerBlade_ && lowerBlade_->isConnected()
				);

	return connected;
}

void BioXASSSRLMonochromatorMaskState::setBladesState(AMControl *newControl)
{
	if (setBaseControl(newControl))
		emit bladesStateChanged(newControl);
}

void BioXASSSRLMonochromatorMaskState::setUpperBlade(AMControl *newControl)
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

void BioXASSSRLMonochromatorMaskState::setLowerBlade(AMControl *newControl)
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

AMAction3* BioXASSSRLMonochromatorMaskState::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (int(setpoint) == Closed)
		result = createMoveToClosedAction(); // This control only supports closing, for now.

	return result;
}

AMAction3* BioXASSSRLMonochromatorMaskState::createMoveToClosedAction()
{
	AMListAction3 *closeMask = new AMListAction3(new AMListActionInfo3("Closing mono mask.", "Closing mono mask."), AMListAction3::Parallel);
	closeMask->addSubAction(AMActionSupport::buildControlMoveAction(upperBlade_, 0));
	closeMask->addSubAction(AMActionSupport::buildControlMoveAction(lowerBlade_, 0));

	return closeMask;
}
