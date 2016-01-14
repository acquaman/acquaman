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

void BioXASFastShutter::setStatus(AMControl *newStatus)
{
	if (setStatusControl(newStatus))
		emit statusChanged(newStatus);
}

void BioXASFastShutter::setOperator(AMControl *newOperator)
{
	if (operator_ != newOperator) {

		if (operator_)
			clearStates();

		operator_ = newOperator;

		if (operator_) {
			addState(0, "Open", 0, operator_, 1);
			addState(1, "Closed", 1, operator_, 0);
		}

		emit operatorChanged(operator_);
	}
}
