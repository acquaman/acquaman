#include "AMTimeoutAction.h"

AMTimeoutAction::AMTimeoutAction(AMTimeoutActionInfo *info, QObject *parent):
	AMWaitAction(info, parent)
{

}

AMTimeoutAction::AMTimeoutAction(const AMTimeoutAction &original) :
	AMWaitAction(original)
{

}

AMTimeoutAction::~AMTimeoutAction()
{

}

void AMTimeoutAction::proceed()
{
	if (state() == AMAction3::Running)
		setSucceeded();
}

void AMTimeoutAction::onWaitTimerTimeout()
{
	setFailed("Failed to provide input before the timeout expired. Action failed.");
}
