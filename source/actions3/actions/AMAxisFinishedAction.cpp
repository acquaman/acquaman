#include "AMAxisFinishedAction.h"

#include "acquaman/AMAgnosticDataAPI.h"

 AMAxisFinishedAction::~AMAxisFinishedAction(){}
AMAxisFinishedAction::AMAxisFinishedAction(AMAxisFinishedActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	generateScanActionMessages_ = true;
}

AMAxisFinishedAction::AMAxisFinishedAction(const AMAxisFinishedAction &other) :
	AMAction3(other)
{
	generateScanActionMessages_ = other.generateScanActionMessages();
}

void AMAxisFinishedAction::startImplementation(){
	setStarted();

	if(generateScanActionMessages_){
		AMAgnosticDataAPIFinishAxisMessage finishAxisMessage(axisFinishedInfo()->axisName());
		AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions")->postMessage(finishAxisMessage);
	}

	setSucceeded();
}

void AMAxisFinishedAction::cancelImplementation(){
	setCancelled();
}
