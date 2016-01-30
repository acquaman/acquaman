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

void BioXASFastShutter::setOperatorControl(AMControl *newOperator, double openStatusValue, double openTrigger, double closedStatusValue, double closeTrigger)
{
	if (operator_ != newOperator) {

		if (operator_)
			clearStates();

		operator_ = newOperator;

		if (operator_) {
			addState(openStatusValue, "Open", openStatusValue, operator_, openTrigger);
			addState(closedStatusValue, "Closed", closedStatusValue, operator_, closeTrigger);
		}

		emit operatorChanged(operator_);
	}
}
