#include "AMTimeoutAction.h"

AMTimeoutAction::AMTimeoutAction(AMWaitActionInfo *info, QObject *parent):
	AMWaitAction(info, parent)
{

}

AMTimeoutAction::AMTimeoutAction(const AMInputTimeoutAction &original) :
	AMWaitAction(original)
{

}

AMTimeoutAction::~AMTimeoutAction()
{

}

void AMTimeoutAction::proceed()
{
	if (state_ == AMAction3::Running)
		setSucceeded();
}

void AMTimeoutAction::onWaitTimerTimeout()
{
	setFailed("Failed to provide input before the timeout expired. Action failed.");
}
