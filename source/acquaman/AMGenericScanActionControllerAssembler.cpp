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


#include "AMGenericScanActionControllerAssembler.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMLoopAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMAxisStartedAction.h"
#include "actions3/actions/AMAxisFinishedAction.h"
#include "actions3/actions/AMAxisValueFinishedAction.h"
#include "beamline/AMDetectorTriggerSource.h"

AMGenericScanActionControllerAssembler::AMGenericScanActionControllerAssembler(QObject *parent)
	: AMScanActionControllerScanAssembler(parent)
{
}

bool AMGenericScanActionControllerAssembler::generateActionTreeImplmentation()
{
	actionTree_ = generateActionTreeForAxis(controls_->at(0), axes_.at(0));

	QList<AMAction3*> insertionPoints = findInsertionPoints(actionTree_);

	for(int x = 1; x < axes_.count(); x++){

		QList<AMAction3*> newInsertionPoints;
		newInsertionPoints.clear();

		for(int y = 0; y < insertionPoints.count(); y++){

			AMListAction3 *castParentToListAction = qobject_cast<AMListAction3*>(insertionPoints.at(y)->parentAction());

			if(castParentToListAction){

				int indexOfAction = castParentToListAction->indexOfSubAction(insertionPoints.at(y));
				castParentToListAction->insertSubAction(generateActionTreeForAxis(controls_->at(x), axes_.at(x)), indexOfAction);
				castParentToListAction->deleteSubAction(indexOfAction+1);
				newInsertionPoints.append(findInsertionPoints(castParentToListAction->subActionAt(indexOfAction)));
			}
		}

		insertionPoints.clear();
		insertionPoints = newInsertionPoints;
	}

	QList<AMAction3*> detectorInsertionPoints = findInsertionPoints(actionTree_);

	for(int x = 0; x < detectorInsertionPoints.count(); x++){

		AMListAction3 *castParentToListAction = qobject_cast<AMListAction3*>(detectorInsertionPoints.at(x)->parentAction());

		if(castParentToListAction){

			int indexOfAction = castParentToListAction->indexOfSubAction(detectorInsertionPoints.at(x));
			castParentToListAction->insertSubAction(generateActionListForDetectorAcquisition(), indexOfAction);
			castParentToListAction->deleteSubAction(indexOfAction+1);
		}
	}

	AMListAction3 *castRetValToListAction = qobject_cast<AMListAction3*>(actionTree_);

	if(castRetValToListAction){

		AMListAction3 *castFirstToListAction = qobject_cast<AMListAction3*>(castRetValToListAction->subActionAt(1));

		if(castFirstToListAction)
			castFirstToListAction->addSubAction(generateActionListForDetectorInitialization());

		AMListAction3 *castLastToListAction = qobject_cast<AMListAction3*>(castRetValToListAction->subActionAt(castRetValToListAction->subActionCount()-2));

		if(castLastToListAction)
			castLastToListAction->addSubAction(generateActionListForDetectorCleanup());
	}

	return true;
}

AMAction3* AMGenericScanActionControllerAssembler::generateActionTreeForAxis(AMControl *axisControl, AMScanAxis *scanAxis)
{
	if(scanAxis->axisType() == AMScanAxis::StepAxis)
		return generateActionTreeForStepAxis(axisControl, scanAxis);

	else if(scanAxis->axisType() == AMScanAxis::ContinuousMoveAxis)
		return generateActionTreeForContinuousMoveAxis(axisControl, scanAxis);

	else if(scanAxis->axisType() == AMScanAxis::ContinuousDwellAxis)
		return generateActionTreeForContinuousDwellAxis(axisControl, scanAxis);

	else
		return 0; //NULL
}

AMAction3* AMGenericScanActionControllerAssembler::generateActionTreeForStepAxis(AMControl *axisControl, AMScanAxis *stepScanAxis)
{
	AMListAction3 *axisActions = new AMListAction3(new AMListActionInfo3(QString("Axis %1").arg(stepScanAxis->name()), QString("Axis %1").arg(stepScanAxis->name())), AMListAction3::Sequential);

	AMAxisStartedAction *axisStartAction = new AMAxisStartedAction(new AMAxisStartedActionInfo(QString("%1 Axis").arg(stepScanAxis->name()), AMScanAxis::StepAxis));
	axisActions->addSubAction(axisStartAction);

	if (axisControl){

		// generate axis initialization list
		AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3(QString("Initializing %1").arg(axisControl->name()), QString("Initializing Axis with Control %1").arg(axisControl->name())), AMListAction3::Sequential);

		AMAction3 *initializeControlPosition = AMActionSupport::buildControlMoveAction(axisControl, stepScanAxis->axisStart());
		initializeControlPosition->setGenerateScanActionMessage(true);
		initializationActions->addSubAction(initializeControlPosition);
		axisActions->addSubAction(initializationActions);
	}

	AMListAction3 *allRegionsList = new AMListAction3(new AMListActionInfo3(QString("%1 Regions for %2 Axis").arg(stepScanAxis->regionCount()).arg(stepScanAxis->name()), QString("%1 Regions for %2 Axis").arg(stepScanAxis->regionCount()).arg(stepScanAxis->name())), AMListAction3::Sequential);

	for(int x = 0; x < stepScanAxis->regionCount(); x++)
		allRegionsList->addSubAction(generateActionTreeForStepAxisRegion(axisControl, stepScanAxis->regionAt(x), (x == stepScanAxis->regionCount()-1) ));

	axisActions->addSubAction(allRegionsList);

	if (axisControl){

		// generate axis cleanup list
		AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3(QString("Cleaning Up %1").arg(axisControl->name()), QString("Cleaning Up Axis with Control %1").arg(axisControl->name())), AMListAction3::Sequential);
		axisActions->addSubAction(cleanupActions);
	}

	AMAxisFinishedAction *axisFinishAction = new AMAxisFinishedAction(new AMAxisFinishedActionInfo(QString("%1 Axis").arg(stepScanAxis->name())));
	axisActions->addSubAction(axisFinishAction);

	return axisActions;
}

AMAction3* AMGenericScanActionControllerAssembler::generateActionTreeForStepAxisRegion(AMControl *axisControl, AMScanAxisRegion *stepScanAxisRegion, bool isFinalRegion)
{
	AMListAction3 *regionList = new AMListAction3(new AMListActionInfo3(QString("Region on %1").arg(stepScanAxisRegion->name()), QString("Region from %1 to %2 by %3 on %4").arg(stepScanAxisRegion->regionStart().toString()).arg(stepScanAxisRegion->regionEnd().toString()).arg(stepScanAxisRegion->regionStep().toString()).arg(stepScanAxisRegion->name())), AMListAction3::Sequential);

	if (axisControl){

		AMAction3 *regionStart = AMActionSupport::buildControlMoveAction(axisControl, stepScanAxisRegion->regionStart());

		regionStart->setGenerateScanActionMessage(true);
		regionList->addSubAction(regionStart);
	}

	AMListAction3 *detectorSetDwellList = new AMListAction3(new AMListActionInfo3(QString("Set All Detectors Dwell Times"), QString("Set %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++){

		if(detectors_->at(x)->readMode() == AMDetectorDefinitions::RequestRead) {
			AMAction3 *detectorSetDwellAction = detectors_->at(x)->createSetAcquisitionTimeAction(stepScanAxisRegion->regionTime());

			if(detectorSetDwellAction)
				detectorSetDwellList->addSubAction(detectorSetDwellAction);
		}
	}

	regionList->addSubAction(detectorSetDwellList);

	// generate axis loop for region
	int loopIterations = int(round(( ((double)stepScanAxisRegion->regionEnd()) - ((double)stepScanAxisRegion->regionStart()) )/ ((double)stepScanAxisRegion->regionStep()) ));
	AMLoopAction3 *axisLoop = new AMLoopAction3(new AMLoopActionInfo3(loopIterations, QString("Loop %1").arg(stepScanAxisRegion->name()), QString("Looping from %1 to %2 by %3 on %4").arg(stepScanAxisRegion->regionStart().toString()).arg(stepScanAxisRegion->regionEnd().toString()).arg(stepScanAxisRegion->regionStep().toString()).arg(stepScanAxisRegion->name())));
	AMListAction3 *nextLevelHolderAction = new AMListAction3(new AMListActionInfo3("Holder Action for the Next Sublevel", "Holder Action for the Next Sublevel"));
	axisLoop->addSubAction(nextLevelHolderAction);

	if (axisControl){

		//setIsRelativeMove(true), setIsRelativeFromSetpoint(true)
		AMAction3 *controlLoopMove = AMActionSupport::buildControlMoveAction(axisControl, stepScanAxisRegion->regionStep(), true, true);

		controlLoopMove->setGenerateScanActionMessage(true);
		axisLoop->addSubAction(controlLoopMove);
		AMAxisValueFinishedActionInfo *axisValueFinishedInfo = new AMAxisValueFinishedActionInfo;
		axisValueFinishedInfo->setShortDescription(QString("%1 axis value finshed").arg(stepScanAxisRegion->name()));
		AMAxisValueFinishedAction *axisValueFinishedAction = new AMAxisValueFinishedAction(axisValueFinishedInfo);
		axisLoop->addSubAction(axisValueFinishedAction);
	}

	regionList->addSubAction(axisLoop);

	if(isFinalRegion){
		AMListAction3 *nextLevelFinalHolderAction = new AMListAction3(new AMListActionInfo3("Holder Action for the Next Sublevel", "Holder Action for the Next Sublevel"));
		regionList->addSubAction(nextLevelFinalHolderAction);
		AMAxisValueFinishedActionInfo *axisValueFinishedInfo = new AMAxisValueFinishedActionInfo;
		axisValueFinishedInfo->setShortDescription(QString("%1 axis value finshed").arg(stepScanAxisRegion->name()));
		AMAxisValueFinishedAction *axisValueFinishedAction = new AMAxisValueFinishedAction(axisValueFinishedInfo);
		regionList->addSubAction(axisValueFinishedAction);
	}

	return regionList;
}

AMAction3* AMGenericScanActionControllerAssembler::generateActionTreeForContinuousMoveAxis(AMControl *axisControl, AMScanAxis *continuiousMoveScanAxis){
	Q_UNUSED(axisControl)
	Q_UNUSED(continuiousMoveScanAxis)
	return 0; //NULL
}

AMAction3* AMGenericScanActionControllerAssembler::generateActionTreeForContinuousDwellAxis(AMControl *axisControl, AMScanAxis *continuousDwellScanAxis){
	Q_UNUSED(axisControl)
	Q_UNUSED(continuousDwellScanAxis)
	return 0; //NULL
}

AMAction3* AMGenericScanActionControllerAssembler::generateActionListForDetectorAcquisition(){
	if(axes_.at(axes_.count()-1)->axisType() == AMScanAxis::StepAxis)
		return generateActionListForStepDetectorAcquisition();
	else if( (axes_.at(axes_.count()-1)->axisType() == AMScanAxis::ContinuousMoveAxis) || (axes_.at(axes_.count()-1)->axisType() == AMScanAxis::ContinuousDwellAxis) )
		return generateActionListForContinuousDetectorAcquisition();
	else
		return 0; //NULL
}

AMAction3* AMGenericScanActionControllerAssembler::generateActionListForStepDetectorAcquisition(){
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Acquire All Detectors"), QString("Acquire %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	AMAction3 *detectorAction;
	AMListAction3 *detectorAcquisitionListAction;
	for(int x = 0; x < detectors_->count(); x++){
		detectorAcquisitionListAction = new AMListAction3(new AMListActionInfo3(QString("Acquire One Detector"), QString("Acquire %1 Detector").arg(detectors_->at(x)->name())), AMListAction3::Sequential);
		detectorAction = detectors_->at(x)->createTriggerAction(AMDetectorDefinitions::SingleRead);
		if(detectorAction){
			detectorAction->setGenerateScanActionMessage(true);
			detectorAcquisitionListAction->addSubAction(detectorAction);
		}
		detectorAction = detectors_->at(x)->createReadAction();
		detectorAction->setGenerateScanActionMessage(true);
		detectorAcquisitionListAction->addSubAction(detectorAction);
		retVal->addSubAction(detectorAcquisitionListAction);
	}

	return retVal;
}

AMAction3* AMGenericScanActionControllerAssembler::generateActionListForContinuousDetectorAcquisition(){
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Acquire Detectors"), QString("Acquire %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createAcquisitionAction(AMDetectorDefinitions::ContinuousRead));

	return retVal;
}

AMAction3* AMGenericScanActionControllerAssembler::generateActionListForDetectorInitialization(){
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Initialize Detectors"), QString("Initialize %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createInitializationActions());

	return retVal;
}

AMAction3* AMGenericScanActionControllerAssembler::generateActionListForDetectorCleanup(){
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Cleanup Detectors"), QString("Cleanup %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createCleanupActions());

	return retVal;
}

QList<AMAction3*> AMGenericScanActionControllerAssembler::findInsertionPoints(AMAction3 *action){
	QList<AMAction3*> retVal;
	AMListAction3 *castToListAction = qobject_cast<AMListAction3*>(action);
	if(castToListAction){
		for(int x = 0; x < castToListAction->subActionCount(); x++){
			AMListAction3 *subActionToListAction = qobject_cast<AMListAction3*>(castToListAction->subActionAt(x));
			if(subActionToListAction && subActionToListAction->info()->shortDescription() == "Holder Action for the Next Sublevel")
				retVal.append(subActionToListAction);
			retVal.append((findInsertionPoints(castToListAction->subActionAt(x))));
		}
	}
	return retVal;
}
