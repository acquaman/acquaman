#include "VESPERSTimedLineScanActionControllerAssembler.h"

#include "actions3/AMTimeoutLoopAction.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMAxisStartedAction.h"
#include "actions3/actions/AMAxisFinishedAction.h"
#include "actions3/actions/AMAxisValueFinishedAction.h"

VESPERSTimedLineScanActionControllerAssembler::VESPERSTimedLineScanActionControllerAssembler(int iterations, double timeoutTime, QObject *parent)
	: AMGenericScanActionControllerAssembler(false, AMScanConfiguration::Increase, parent)
{
	iterations_ = iterations;
	timeoutTime_ = timeoutTime;
}

bool VESPERSTimedLineScanActionControllerAssembler::generateActionTreeImplmentation()
{
	AMTimeoutLoopAction *timeoutLoopTree = new AMTimeoutLoopAction(iterations_, timeoutTime_);
	timeoutLoopTree->addSubAction(generateActionTreeForAxis(controls_->at(0), axes_.at(0)));
	AMAxisValueFinishedActionInfo *axisValueFinishedInfo = new AMAxisValueFinishedActionInfo;
	axisValueFinishedInfo->setShortDescription(QString("Time axis value finshed"));
	AMAxisValueFinishedAction *axisValueFinishedAction = new AMAxisValueFinishedAction(axisValueFinishedInfo);
	timeoutLoopTree->addSubAction(axisValueFinishedAction);

	QList<AMAction3*> detectorInsertionPoints = findInsertionPoints(timeoutLoopTree);

	for(int x = 0; x < detectorInsertionPoints.count(); x++){

		AMListAction3 *castParentToListAction = qobject_cast<AMListAction3*>(detectorInsertionPoints.at(x)->parentAction());

		if(castParentToListAction){

			int indexOfAction = castParentToListAction->indexOfSubAction(detectorInsertionPoints.at(x));
			castParentToListAction->insertSubAction(generateActionListForDetectorAcquisition(), indexOfAction);
			castParentToListAction->deleteSubAction(indexOfAction+1);
		}
	}

	AMListAction3 *scanActions = new AMSequentialListAction3(new AMSequentialListActionInfo3("Timed Line Scan Actions", "Timed Line Scan Actions"));
	scanActions->addSubAction(new AMAxisStartedAction(new AMAxisStartedActionInfo(QString("Time Axis"), AMScanAxis::StepAxis)));
	scanActions->addSubAction(generateActionListForDetectorInitialization());
	scanActions->addSubAction(timeoutLoopTree);
	scanActions->addSubAction(generateActionListForDetectorCleanup());
	scanActions->addSubAction(new AMAxisFinishedAction(new AMAxisFinishedActionInfo(QString("Time Axis"))));

	actionTree_ = scanActions;

	return true;
}
