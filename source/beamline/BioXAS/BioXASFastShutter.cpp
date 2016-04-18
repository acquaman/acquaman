#include "BioXASFastShutter.h"

BioXASFastShutter::BioXASFastShutter(const QString &name, QObject *parent) :
	AMExclusiveStatesEnumeratedControl(name, "", parent)
{
	// Initialize class variables.

	operator_ = 0;

	// Initialize inherited variables.

	setContextKnownDescription("Fast shutter");
	setAllowsMovesWhileMoving(false);
}

BioXASFastShutter::~BioXASFastShutter()
{

}

bool BioXASFastShutter::isOpen() const
{
	return (int(value()) == Open);
}

bool BioXASFastShutter::isClosed() const
{
	return (int(value()) == Closed);
}

AMControl::FailureExplanation BioXASFastShutter::open()
{
	return move(Open);
}

AMControl::FailureExplanation BioXASFastShutter::close()
{
	return move(Closed);
}

void BioXASFastShutter::setOperatorControl(AMControl *newOperator, double openStatusValue, double openTrigger, double closedStatusValue, double closeTrigger)
{
	if (operator_ != newOperator) {

		if (operator_)
			clearStates();

		operator_ = newOperator;

		if (operator_) {
			addState(Open, "Open", openStatusValue, operator_, openTrigger);
			addState(Closed, "Closed", closedStatusValue, operator_, closeTrigger);
		}

		emit operatorChanged(operator_);
	}
}
