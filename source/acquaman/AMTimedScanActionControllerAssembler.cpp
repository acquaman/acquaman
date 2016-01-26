#include "AMTimedScanActionControllerAssembler.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMLoopAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMAxisStartedAction.h"
#include "actions3/actions/AMAxisFinishedAction.h"
#include "actions3/actions/AMAxisValueFinishedAction.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "actions3/actions/AMWaitAction.h"

AMTimedScanActionControllerAssembler::AMTimedScanActionControllerAssembler(double acquisitionTime, double timePerAcquisition, int iterations, QObject *parent)
	: AMScanActionControllerScanAssembler(parent)
{
	acquisitionTime_ = acquisitionTime;
	timePerAcquisition_ = timePerAcquisition;
	iterations_ = iterations;
}

AMTimedScanActionControllerAssembler::~AMTimedScanActionControllerAssembler()
{

}

bool AMTimedScanActionControllerAssembler::generateActionTreeImplmentation()
{
	AMLoopAction3 *valueLoopTree = new AMLoopAction3(iterations_);
	valueLoopTree->addSubAction(generateActionListForDetectorAcquisition());
	valueLoopTree->addSubAction(new AMWaitAction(new AMWaitActionInfo(timePerAcquisition_-acquisitionTime_)));
	AMAxisValueFinishedActionInfo *axisValueFinishedInfo = new AMAxisValueFinishedActionInfo;
	axisValueFinishedInfo->setShortDescription(QString("Time axis value finshed"));
	AMAxisValueFinishedAction *axisValueFinishedAction = new AMAxisValueFinishedAction(axisValueFinishedInfo);
	valueLoopTree->addSubAction(axisValueFinishedAction);

	AMListAction3 *detectorSetDwellList = new AMListAction3(new AMListActionInfo3(QString("Set All Detectors Dwell Times"), QString("Set %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++){

		if(detectors_->at(x)->readMode() == AMDetectorDefinitions::RequestRead) {
			AMAction3 *detectorSetDwellAction = detectors_->at(x)->createSetAcquisitionTimeAction(acquisitionTime_);

			if(detectorSetDwellAction)
				detectorSetDwellList->addSubAction(detectorSetDwellAction);
		}
	}

	AMListAction3 *scanActions = new AMSequentialListAction3(new AMSequentialListActionInfo3("Timed Scan Actions", "Timed Scan Actions"));
	scanActions->addSubAction(new AMAxisStartedAction(new AMAxisStartedActionInfo(QString("Time Axis"), AMScanAxis::StepAxis)));
	scanActions->addSubAction(generateActionListForDetectorInitialization());
	scanActions->addSubAction(detectorSetDwellList);
	scanActions->addSubAction(valueLoopTree);
	scanActions->addSubAction(generateActionListForDetectorCleanup());
	scanActions->addSubAction(new AMAxisFinishedAction(new AMAxisFinishedActionInfo(QString("Time Axis"))));

	actionTree_ = scanActions;

	return true;
}

AMAction3* AMTimedScanActionControllerAssembler::generateActionListForDetectorAcquisition()
{
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

AMAction3* AMTimedScanActionControllerAssembler::generateActionListForDetectorInitialization()
{
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Initialize Detectors"), QString("Initialize %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createInitializationActions());

	return retVal;
}

AMAction3* AMTimedScanActionControllerAssembler::generateActionListForDetectorCleanup()
{
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Cleanup Detectors"), QString("Cleanup %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createCleanupActions());

	return retVal;
}
