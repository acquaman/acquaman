#include "AMScanActionControllerScanAssembler.h"

AMScanActionControllerScanAssembler::AMScanActionControllerScanAssembler(QObject *parent) :
	QObject(parent)
{
	detectors_ = new AMDetectorSet(this);
	controls_ = new AMControlSet(this);
	actionTree_ = 0; //NULL
}

bool AMScanActionControllerScanAssembler::insertAxis(int index, AMControl *axisControl, AMScanAxis *scanAxis){
	if(!axisControl || !scanAxis)
		return false;
	if(controls_->contains(axisControl->name()) || axes_.contains(scanAxis))
		return false;
	if(index < 0 || index > controls_->count())
		return false;
	if(controls_->count() != axes_.count())
		return false;

	controls_->insert(index, axisControl, axisControl->name());
	axes_.insert(index, scanAxis);
	return true;
}

bool AMScanActionControllerScanAssembler::appendAxis(AMControl *axisControl, AMScanAxis *scanAxis){
	return insertAxis(controls_->count(), axisControl, scanAxis);
}

bool AMScanActionControllerScanAssembler::addDetector(AMDetector *detector){
	return detectors_->addDetector(detector);
}

#include "actions3/AMListAction3.h"
void AMScanActionControllerScanAssembler::generateActionTree(){
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

	emit actionTreeGenerated(actionTree_);
}

AMAction3* AMScanActionControllerScanAssembler::generateActionTreeForAxis(AMControl *axisControl, AMScanAxis *scanAxis){
	if(scanAxis->axisType() == AMScanAxis::StepAxis)
		return generateActionTreeForStepAxis(axisControl, scanAxis);
	else if(scanAxis->axisType() == AMScanAxis::ContinuousMoveAxis)
		return generateActionTreeForContinuousMoveAxis(axisControl, scanAxis);
	else if(scanAxis->axisType() == AMScanAxis::ContinuousDwellAxis)
		return generateActionTreeForContinuousDwellAxis(axisControl, scanAxis);
	else
		return 0; //NULL
}

#include "actions3/AMLoopAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMAxisStartedAction.h"
#include "actions3/actions/AMAxisFinishedAction.h"
AMAction3* AMScanActionControllerScanAssembler::generateActionTreeForStepAxis(AMControl *axisControl, AMScanAxis *stepScanAxis){
	AMListAction3 *axisActions = new AMListAction3(new AMListActionInfo3(QString("Axis %1").arg(axisControl->name()), QString("Axis %1").arg(axisControl->name())), AMListAction3::Sequential);

	// generate axis initialization list
	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3(QString("Initializing %1").arg(axisControl->name()), QString("Initializing Axis with Control %1").arg(axisControl->name())), AMListAction3::Sequential);
	AMControlInfo initializeControlPositionSetpoint = axisControl->toInfo();
	initializeControlPositionSetpoint.setValue(stepScanAxis->axisStart());
	AMControlMoveAction3 *initializeControlPosition = new AMControlMoveAction3(new AMControlMoveActionInfo3(initializeControlPositionSetpoint), axisControl);
	initializeControlPosition->setGenerateScanActionMessage(true);
	initializationActions->addSubAction(initializeControlPosition);

	AMListAction3 *allRegionsList = new AMListAction3(new AMListActionInfo3(QString("%1 Regions for %2 Axis").arg(stepScanAxis->regionCount()).arg(axisControl->name()), QString("%1 Regions for %2 Axis").arg(stepScanAxis->regionCount()).arg(axisControl->name())), AMListAction3::Sequential);
	for(int x = 0; x < stepScanAxis->regionCount(); x++)
		allRegionsList->addSubAction(generateActionTreeForStepAxisRegion(axisControl, stepScanAxis->regionAt(x), (x == stepScanAxis->regionCount()-1) ));

	// generate axis cleanup list
	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3(QString("Cleaning Up %1").arg(axisControl->name()), QString("Cleaning Up Axis with Control %1").arg(axisControl->name())), AMListAction3::Sequential);

	AMAxisStartedAction *axisStartAction = new AMAxisStartedAction(new AMAxisStartedActionInfo(QString("%1 Axis").arg(axisControl->name()), AMScanAxis::StepAxis));
	AMAxisFinishedAction *axisFinishAction = new AMAxisFinishedAction(new AMAxisFinishedActionInfo(QString("%1 Axis").arg(axisControl->name())));

	axisActions->addSubAction(axisStartAction);
	axisActions->addSubAction(initializationActions);
	axisActions->addSubAction(allRegionsList);
	axisActions->addSubAction(cleanupActions);
	axisActions->addSubAction(axisFinishAction);
	return axisActions;
}

AMAction3* AMScanActionControllerScanAssembler::generateActionTreeForStepAxisRegion(AMControl *axisControl, const AMScanAxisRegion &stepScanAxisRegion, bool isFinalRegion){
	AMListAction3 *regionList = new AMListAction3(new AMListActionInfo3(QString("Region on %1").arg(axisControl->name()), QString("Region from %1 to %2 by %3 on %4").arg(stepScanAxisRegion.regionStart().toString()).arg(stepScanAxisRegion.regionEnd().toString()).arg(stepScanAxisRegion.regionStep().toString()).arg(axisControl->name())), AMListAction3::Sequential);
	AMControlInfo regionStartSetpoint = axisControl->toInfo();
	regionStartSetpoint.setValue(stepScanAxisRegion.regionStart());
	AMControlMoveAction3 *regionStart = new AMControlMoveAction3(new AMControlMoveActionInfo3(regionStartSetpoint), axisControl);
	regionStart->setGenerateScanActionMessage(true);

	AMListAction3 *detectorSetDwellList = new AMListAction3(new AMListActionInfo3(QString("Set All Detectors Dwell Times"), QString("Set %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);
	AMAction3 *detectorSetDwellAction;
	for(int x = 0; x < detectors_->count(); x++){
		detectorSetDwellAction = detectors_->at(x)->createSetAcquisitionTimeAction(stepScanAxisRegion.regionTime());
		if(detectorSetDwellAction)
			detectorSetDwellList->addSubAction(detectorSetDwellAction);
	}

	// generate axis loop for region
	int loopIterations = ceil(( ((double)stepScanAxisRegion.regionEnd()) - ((double)stepScanAxisRegion.regionStart()) )/ ((double)stepScanAxisRegion.regionStep()) );
	AMLoopAction3 *axisLoop = new AMLoopAction3(new AMLoopActionInfo3(loopIterations, QString("Loop %1").arg(axisControl->name()), QString("Looping from %1 to %2 by %3 on %4").arg(stepScanAxisRegion.regionStart().toString()).arg(stepScanAxisRegion.regionEnd().toString()).arg(stepScanAxisRegion.regionStep().toString()).arg(axisControl->name())));
	axisLoop->setGenerateScanActionMessage(true);
	AMListAction3 *nextLevelHolderAction = new AMListAction3(new AMListActionInfo3("Holder Action for the Next Sublevel", "Holder Action for the Next Sublevel"));
	AMControlInfo controlLoopMoveInfoSetpoint = axisControl->toInfo();
	controlLoopMoveInfoSetpoint.setValue(stepScanAxisRegion.regionStep());
	AMControlMoveActionInfo3 *controlLoopMoveInfo = new AMControlMoveActionInfo3(controlLoopMoveInfoSetpoint);
	controlLoopMoveInfo->setIsRelativeMove(true);
	controlLoopMoveInfo->setIsRelativeFromSetpoint(true);
	AMControlMoveAction3 *controlLoopMove = new AMControlMoveAction3(controlLoopMoveInfo, axisControl);
	controlLoopMove->setGenerateScanActionMessage(true);
	axisLoop->addSubAction(nextLevelHolderAction);
	axisLoop->addSubAction(controlLoopMove);

	regionList->addSubAction(regionStart);
	regionList->addSubAction(detectorSetDwellList);
	regionList->addSubAction(axisLoop);
	if(isFinalRegion){
		AMListAction3 *nextLevelFinalHolderAction = new AMListAction3(new AMListActionInfo3("Holder Action for the Next Sublevel", "Holder Action for the Next Sublevel"));
		regionList->addSubAction(nextLevelFinalHolderAction);
	}

	return regionList;
}

AMAction3* AMScanActionControllerScanAssembler::generateActionTreeForContinuousMoveAxis(AMControl *axisControl, AMScanAxis *continuiousMoveScanAxis){
	Q_UNUSED(axisControl)
	Q_UNUSED(continuiousMoveScanAxis)
	return 0; //NULL
}

AMAction3* AMScanActionControllerScanAssembler::generateActionTreeForContinuousDwellAxis(AMControl *axisControl, AMScanAxis *continuousDwellScanAxis){
	Q_UNUSED(axisControl)
	Q_UNUSED(continuousDwellScanAxis)
	return 0; //NULL
}

AMAction3* AMScanActionControllerScanAssembler::generateActionListForDetectorAcquisition(){
	if(axes_.at(axes_.count()-1)->axisType() == AMScanAxis::StepAxis)
		return generateActionListForStepDetectorAcquisition();
	else if( (axes_.at(axes_.count()-1)->axisType() == AMScanAxis::ContinuousMoveAxis) || (axes_.at(axes_.count()-1)->axisType() == AMScanAxis::ContinuousDwellAxis) )
		return generateActionListForContinuousDetectorAcquisition();
	else
		return 0; //NULL
}

#include "beamline/AMDetectorTriggerSource.h"
AMAction3* AMScanActionControllerScanAssembler::generateActionListForStepDetectorAcquisition(){
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

AMAction3* AMScanActionControllerScanAssembler::generateActionListForContinuousDetectorAcquisition(){
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Acquire Detectors"), QString("Acquire %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createAcquisitionAction(AMDetectorDefinitions::ContinuousRead));

	return retVal;
}

AMAction3* AMScanActionControllerScanAssembler::generateActionListForDetectorInitialization(){
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Initialize Detectors"), QString("Initialize %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createInitializationActions());

	return retVal;
}

AMAction3* AMScanActionControllerScanAssembler::generateActionListForDetectorCleanup(){
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Cleanup Detectors"), QString("Cleanup %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createCleanupActions());

	return retVal;
}

QList<AMAction3*> AMScanActionControllerScanAssembler::findInsertionPoints(AMAction3 *action){
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
