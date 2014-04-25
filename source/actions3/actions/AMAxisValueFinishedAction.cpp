#include "AMAxisValueFinishedAction.h"

#include "acquaman/AMAgnosticDataAPI.h"

AMAxisValueFinishedAction::AMAxisValueFinishedAction(AMAxisValueFinishedActionInfo *info, QObject *parent)
	: AMAction3(info, parent)
{
	generateScanActionMessages_ = true;
}

AMAxisValueFinishedAction::AMAxisValueFinishedAction(const AMAxisValueFinishedAction &other)
	: AMAction3(other)
{
	generateScanActionMessages_ = other.generateScanActionMessages();
}

void AMAxisValueFinishedAction::startImplementation()
{
	setStarted();

	if (generateScanActionMessages_){

		AMAgnosticDataAPIAxisValueFinishedMessage axisValueFinishedMessage(info()->shortDescription());
		AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions")->postMessage(axisValueFinishedMessage);
	}

	setSucceeded();
}

void AMAxisValueFinishedAction::cancelImplementation()
{
	setCancelled();
}
