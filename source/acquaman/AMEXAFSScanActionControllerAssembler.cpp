#include "AMEXAFSScanActionControllerAssembler.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMLoopAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMAxisStartedAction.h"
#include "actions3/actions/AMAxisFinishedAction.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "util/AMErrorMonitor.h"
#include "util/AMVariableIntegrationTime.h"

AMEXAFSScanActionControllerAssembler::AMEXAFSScanActionControllerAssembler(QObject *parent)
	: AMGenericScanActionControllerAssembler(parent)
{
}

bool AMEXAFSScanActionControllerAssembler::generateActionTreeImplmentation()
{
	// There is only one axis, but there can be many regions in an EXAFS scan.
	QList<AMScanAxisEXAFSRegion *> exafsRegions;

	foreach (AMScanAxisRegion *region, axes_.at(0)->regions().toList()){

		AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);

		if (exafsRegion)
			exafsRegions << exafsRegion;

		else {

			AMErrorMon::alert(this, AMEXAFSSCANACTIONCONTROLLERASSEMBLER_AXES_INVALID, "An axis region was invalid.");
			return false;
		}
	}

	// First, if there are no special extended regions of any kind, just use the generic assembler.
	bool hasExtendedRegion = false;

	foreach (AMScanAxisEXAFSRegion *region, exafsRegions)
		if (region->inKSpace())
			hasExtendedRegion = true;

	// If no EXAFS specific regions are required.  Just make it a normal step scan.
	if (!hasExtendedRegion)
		return AMGenericScanActionControllerAssembler::generateActionTreeImplmentation();

	// Build an EXAFS scan.
	AMScanAxis *axis = axes_.at(0);
	AMControl *axisControl = controls_->at(0);
	AMListAction3 *axisActions = new AMListAction3(new AMListActionInfo3(QString("Axis %1").arg(axis->name()), QString("Axis %1").arg(axis->name())), AMListAction3::Sequential);

	AMAxisStartedAction *axisStartAction = new AMAxisStartedAction(new AMAxisStartedActionInfo(QString("%1 Axis").arg(axis->name()), AMScanAxis::StepAxis));
	axisActions->addSubAction(axisStartAction);

	if (axisControl){

		// generate axis initialization list
		AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3(QString("Initializing %1").arg(axisControl->name()), QString("Initializing Axis with Control %1").arg(axisControl->name())), AMListAction3::Sequential);
		AMControlInfo initializeControlPositionSetpoint = axisControl->toInfo();
		initializeControlPositionSetpoint.setValue(axis->axisStart());
		AMControlMoveAction3 *initializeControlPosition = new AMControlMoveAction3(new AMControlMoveActionInfo3(initializeControlPositionSetpoint), axisControl);
		initializeControlPosition->setGenerateScanActionMessage(true);
		initializationActions->addSubAction(initializeControlPosition);
		axisActions->addSubAction(initializationActions);
	}

	AMListAction3 *allRegionsList = new AMListAction3(new AMListActionInfo3(QString("%1 Regions for %2 Axis").arg(axis->regionCount()).arg(axis->name()), QString("%1 Regions for %2 Axis").arg(axis->regionCount()).arg(axis->name())), AMListAction3::Sequential);

	for(int i = 0, size = exafsRegions.size(); i < size; i++){

		AMScanAxisEXAFSRegion *exafsRegion = exafsRegions.at(i);

		if (exafsRegion->inKSpace())
			allRegionsList->addSubAction(generateActionTreeForEXAFSStepAxisRegion(axisControl, exafsRegion, (i == size-1) ));

		else
			allRegionsList->addSubAction(generateActionTreeForStepAxisRegion(axisControl, exafsRegion, (i == size-1) ));
	}

	axisActions->addSubAction(allRegionsList);

	if (axisControl){

		// generate axis cleanup list
		AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3(QString("Cleaning Up %1").arg(axisControl->name()), QString("Cleaning Up Axis with Control %1").arg(axisControl->name())), AMListAction3::Sequential);
		axisActions->addSubAction(cleanupActions);
	}

	AMAxisFinishedAction *axisFinishAction = new AMAxisFinishedAction(new AMAxisFinishedActionInfo(QString("%1 Axis").arg(axis->name())));
	axisActions->addSubAction(axisFinishAction);

	return axisActions;
}

AMAction3 *AMEXAFSScanActionControllerAssembler::generateActionTreeForEXAFSStepAxisRegion(AMControl *axisControl, AMScanAxisEXAFSRegion *exafsRegion, bool isFinalRegion)
{
	AMListAction3 *regionList = new AMListAction3(new AMListActionInfo3(QString("Region on %1").arg(exafsRegion->name()), QString("Region from %1 to %2 by %3 on %4").arg(exafsRegion->regionStart().toString()).arg(exafsRegion->regionEnd().toString()).arg(exafsRegion->regionStep().toString()).arg(exafsRegion->name())), AMListAction3::Sequential);

	if (axisControl){

		AMControlInfo regionStartSetpoint = axisControl->toInfo();
		regionStartSetpoint.setValue(exafsRegion->regionStart());
		AMControlMoveAction3 *regionStart = new AMControlMoveAction3(new AMControlMoveActionInfo3(regionStartSetpoint), axisControl);
		regionStart->setGenerateScanActionMessage(true);
		regionList->addSubAction(regionStart);
	}

	// If there is no difference in time and maximumTime or if maximumTime is invalid then we only need to set the time once.
	if (!exafsRegion->maximumTime().isValid() || double(exafsRegion->regionTime()) == double(exafsRegion->maximumTime())){

		AMListAction3 *detectorSetDwellList = new AMListAction3(new AMListActionInfo3(QString("Set All Detectors Dwell Times"), QString("Set %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

		for(int x = 0; x < detectors_->count(); x++){

			AMAction3 *detectorSetDwellAction = detectors_->at(x)->createSetAcquisitionTimeAction(exafsRegion->regionTime());

			if(detectorSetDwellAction)
				detectorSetDwellList->addSubAction(detectorSetDwellAction);
		}

		regionList->addSubAction(detectorSetDwellList);

		int extendedPoints = round(( ((double)exafsRegion->regionEnd()) - ((double)exafsRegion->regionStart()) )/ ((double)exafsRegion->regionStep()) );

		for (int i = 0; i < extendedPoints; i++){

			AMControlInfo controlLoopMoveInfoSetpoint = axisControl->toInfo();
			controlLoopMoveInfoSetpoint.setValue(exafsRegion->regionStep());
			AMControlMoveActionInfo3 *controlLoopMoveInfo = new AMControlMoveActionInfo3(controlLoopMoveInfoSetpoint);
			controlLoopMoveInfo->setIsRelativeMove(true);
			controlLoopMoveInfo->setIsRelativeFromSetpoint(true);
			AMControlMoveAction3 *controlLoopMove = new AMControlMoveAction3(controlLoopMoveInfo, axisControl);
			controlLoopMove->setGenerateScanActionMessage(true);
//			axisLoop->addSubAction(controlLoopMove);
		}
	}

	// Otherwise, we need a detector dwell set action and a control move position for each point in the region.
	else {


	}
//	AMListAction3 *detectorSetDwellList = new AMListAction3(new AMListActionInfo3(QString("Set All Detectors Dwell Times"), QString("Set %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

//	for(int x = 0; x < detectors_->count(); x++){

//		AMAction3 *detectorSetDwellAction = detectors_->at(x)->createSetAcquisitionTimeAction(exafsRegion->regionTime());

//		if(detectorSetDwellAction)
//			detectorSetDwellList->addSubAction(detectorSetDwellAction);
//	}

//	regionList->addSubAction(detectorSetDwellList);

//	// generate axis loop for region
//	int extendedPoints = round(( ((double)exafsRegion->regionEnd()) - ((double)exafsRegion->regionStart()) )/ ((double)exafsRegion->regionStep()) );
//	AMLoopAction3 *axisLoop = new AMLoopAction3(new AMLoopActionInfo3(extendedPoints, QString("Loop %1").arg(exafsRegion->name()), QString("Looping from %1 to %2 by %3 on %4").arg(exafsRegion->regionStart().toString()).arg(exafsRegion->regionEnd().toString()).arg(exafsRegion->regionStep().toString()).arg(exafsRegion->name())));
//	axisLoop->setGenerateScanActionMessage(true);
//	AMListAction3 *nextLevelHolderAction = new AMListAction3(new AMListActionInfo3("Holder Action for the Next Sublevel", "Holder Action for the Next Sublevel"));
//	axisLoop->addSubAction(nextLevelHolderAction);

//	if (axisControl){

//		AMControlInfo controlLoopMoveInfoSetpoint = axisControl->toInfo();
//		controlLoopMoveInfoSetpoint.setValue(exafsRegion->regionStep());
//		AMControlMoveActionInfo3 *controlLoopMoveInfo = new AMControlMoveActionInfo3(controlLoopMoveInfoSetpoint);
//		controlLoopMoveInfo->setIsRelativeMove(true);
//		controlLoopMoveInfo->setIsRelativeFromSetpoint(true);
//		AMControlMoveAction3 *controlLoopMove = new AMControlMoveAction3(controlLoopMoveInfo, axisControl);
//		controlLoopMove->setGenerateScanActionMessage(true);
//		axisLoop->addSubAction(controlLoopMove);
//	}

//	regionList->addSubAction(axisLoop);

	if(isFinalRegion){
		AMListAction3 *nextLevelFinalHolderAction = new AMListAction3(new AMListActionInfo3("Holder Action for the Next Sublevel", "Holder Action for the Next Sublevel"));
		regionList->addSubAction(nextLevelFinalHolderAction);
	}

	return regionList;}
