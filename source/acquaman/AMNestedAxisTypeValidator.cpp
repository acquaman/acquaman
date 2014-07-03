/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
