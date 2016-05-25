#include "CLSSSRLMonochromatorMaskState.h"

#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

CLSSSRLMonochromatorMaskState::CLSSSRLMonochromatorMaskState(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("SSRL Monochromator Mask State");
	setAllowsMovesWhileMoving(false);

	// Setup value options.

	addValueOption(Closed, "Closed", 1);
	addValueOption(Open, "Open", 0, true);
}

CLSSSRLMonochromatorMaskState::~CLSSSRLMonochromatorMaskState()
{

}

bool CLSSSRLMonochromatorMaskState::canMove() const
{
	bool result = false;

	if (isConnected())
		result = ( upperBlade_->canMove() && lowerBlade_->canMove() );

	return result;
}

bool CLSSSRLMonochromatorMaskState::isConnected() const
{
	bool connected = (
				AMSingleEnumeratedControl::isConnected() &&
				upperBlade_ && upperBlade_->isConnected() &&
				lowerBlade_ && lowerBlade_->isConnected()
				);

	return connected;
}

void CLSSSRLMonochromatorMaskState::setBladesState(AMControl *newControl)
{
	if (setBaseControl(newControl))
		emit bladesStateChanged(newControl);
}

void CLSSSRLMonochromatorMaskState::setUpperBlade(AMControl *newControl)
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

void CLSSSRLMonochromatorMaskState::setLowerBlade(AMControl *newControl)
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

AMAction3* CLSSSRLMonochromatorMaskState::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (int(setpoint) == Closed)
		result = createMoveToClosedAction(); // This control only supports closing, for now.

	return result;
}

AMAction3* CLSSSRLMonochromatorMaskState::createMoveToClosedAction()
{
	AMListAction3 *closeMask = new AMListAction3(new AMListActionInfo3("Closing mono mask.", "Closing mono mask."), AMListAction3::Parallel);
	closeMask->addSubAction(AMActionSupport::buildControlMoveAction(upperBlade_, 0));
	closeMask->addSubAction(AMActionSupport::buildControlMoveAction(lowerBlade_, 0));

	return closeMask;
}
