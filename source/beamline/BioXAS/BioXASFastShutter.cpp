#include "BioXASFastShutter.h"

BioXASFastShutter::BioXASFastShutter(const QString &name, QObject *parent) :
	AMBiStateControl(name, parent)
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

void BioXASFastShutter::setOperator(AMControl *newOperator, double openStatusValue, double openTrigger, double closedStatusValue, double closeTrigger)
{
	if (operator_ != newOperator) {

		if (operator_)
			clearStates();

		operator_ = newOperator;

		if (operator_) {
			addOpenState(operator_, openStatusValue, openTrigger);
			addClosedState(operator_, closedStatusValue, closeTrigger);
		}

		emit operatorChanged(operator_);
	}
}
