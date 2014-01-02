#include "AMNestedAxisTypeValidator.h"

 AMNestedAxisTypeValidator::~AMNestedAxisTypeValidator(){}
AMNestedAxisTypeValidator::AMNestedAxisTypeValidator(AMAction3 *scanActionTree, QObject *parent) :
	AMScanActionControllerScanValidator(scanActionTree, parent)
{
}

#include "actions3/actions/AMAxisStartedAction.h"
bool AMNestedAxisTypeValidator::validateImplementation(AMAction3 *scanActionTree){
	AMAxisStartedAction *templateStartedAction = new AMAxisStartedAction(new AMAxisStartedActionInfo("Fake Axis", AMScanAxis::StepAxis));

	QList<AMAction3*> allStartsAsActions = AMScanActionTreeSupport::findActionsOfType(scanActionTree, templateStartedAction);
	QList<AMAxisStartedAction*> allStarts;
	AMAxisStartedAction *tempAxisStarted;
	for(int x = 0; x < allStartsAsActions.count(); x++){
		tempAxisStarted = qobject_cast<AMAxisStartedAction*>(allStartsAsActions.at(x));
		if(tempAxisStarted)
			allStarts.append(tempAxisStarted);
	}

	bool foundOneStepAxis = false;
	bool foundOneNonStepAxis = false;
	AMAxisStartedActionInfo *tempAxisStartedInfo;
	for(int x = allStarts.count()-1; x >= 0; x--){
		tempAxisStartedInfo = qobject_cast<AMAxisStartedActionInfo*>(allStarts.at(x)->info());
		if(!tempAxisStartedInfo)
			return false;
		if(foundOneStepAxis && tempAxisStartedInfo->axisType() != AMScanAxis::StepAxis)
			return false;
		if(foundOneNonStepAxis && tempAxisStartedInfo->axisType() != AMScanAxis::StepAxis)
			return false;
		if(!foundOneStepAxis && tempAxisStartedInfo->axisType() == AMScanAxis::StepAxis)
			foundOneStepAxis = true;
		if(!foundOneNonStepAxis && tempAxisStartedInfo->axisType() != AMScanAxis::StepAxis)
			foundOneNonStepAxis = true;
	}
	return true;
}
