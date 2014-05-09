#include "AMAxisStartedAction.h"

#include "acquaman/AMAgnosticDataAPI.h"

AMAxisStartedAction::~AMAxisStartedAction(){}

AMAxisStartedAction::AMAxisStartedAction(AMAxisStartedActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	generateScanActionMessages_ = true;
}

AMAxisStartedAction::AMAxisStartedAction(const AMAxisStartedAction &other) :
	AMAction3(other)
{
	generateScanActionMessages_ = other.generateScanActionMessages();
}

void AMAxisStartedAction::startImplementation(){
	setStarted();

	if(generateScanActionMessages_){
		AMAgnosticDataAPIStartAxisMessage startAxisMessage(axisStartedInfo()->axisName());
		AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions")->postMessage(startAxisMessage);
	}

	setSucceeded();
}

void AMAxisStartedAction::cancelImplementation(){
	setCancelled();
}
