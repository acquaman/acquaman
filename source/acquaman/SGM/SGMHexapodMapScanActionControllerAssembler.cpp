#include "SGMHexapodMapScanActionControllerAssembler.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMAxisStartedAction.h"
#include "actions3/actions/AMAxisFinishedAction.h"
#include "actions3/actions/AMAxisValueFinishedAction.h"
#include "actions3/actions/AMDetectorTriggerAction.h"
#include "beamline/AMDetectorTriggerSource.h"

#include "beamline/CLS/CLSAMDSScalerChannelDetector.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMHexapodTransformedAxis.h"

#include <QDebug>
#include <QVector3D>

SGMHexapodMapScanActionControllerAssembler::SGMHexapodMapScanActionControllerAssembler(QObject *parent)
	: AMScanActionControllerScanAssembler(parent)
{

}

bool SGMHexapodMapScanActionControllerAssembler::generateActionTreeImplmentation()
{
	if (axes_.size() != 2)
		return false;

	// Grab the axes individually for conceptual ease later.
	AMScanAxis *stepAxis = axes_.at(0);
	AMScanAxis *continuousAxis = axes_.at(1);

	// Grab the controls individually for conceptual ease later.
	AMControl *stepControl = controls_->at(0);
	AMControl *continuousControl = controls_->at(1);

	// First we need to make the list for the first axis.
	AMListAction3 *mapActions = new AMListAction3(new AMListActionInfo3(QString("Axis %1").arg(stepAxis->name()), QString("Axis %1").arg(stepAxis->name())), AMListAction3::Sequential);

	AMAxisStartedAction *axisStartAction = new AMAxisStartedAction(new AMAxisStartedActionInfo(QString("%1 Axis").arg(stepAxis->name()), AMScanAxis::StepAxis));
	mapActions->addSubAction(axisStartAction);

	// Get the initialization actions for the step axis.
	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3(QString("Initializing %1").arg(stepControl->name()), QString("Initializing Axis with Control %1").arg(stepControl->name())), AMListAction3::Sequential);

	AMAction3 *initializeControlPosition = AMActionSupport::buildControlMoveAction(stepControl, stepAxis->axisStart());
	initializeControlPosition->setGenerateScanActionMessage(true);
	initializationActions->addSubAction(initializeControlPosition);
	mapActions->addSubAction(initializationActions);

	// There will be only 1 region in these step scans because it is a map.
	AMScanAxisRegion *stepRegion = stepAxis->regionAt(0);
	QList<double> stepRegionPositions = generateStepAxisPositions(stepRegion);
	SGMHexapodTransformedAxis *hexapod = qobject_cast<SGMHexapodTransformedAxis *>(continuousControl);
	// Grabbing current position so we know what the z value is.
	QVector3D currentPosition = QVector3D(SGMBeamline::sgm()->exposedControlByName("Hexapod Global X Axis")->setpoint(),
					      SGMBeamline::sgm()->exposedControlByName("Hexapod Global Y Axis")->setpoint(),
					      SGMBeamline::sgm()->exposedControlByName("Hexapod Global Z Axis")->setpoint());
	currentPosition = hexapod->globalAxisToPrime(currentPosition);

	QList<QVector3D> startTrajectories = generateContinuousAxisStartTrajectories(double(continuousAxis->axisStart()),
										     stepRegionPositions,
										     currentPosition);
	QList<QVector3D> endTrajectories = generateContinuousAxisEndTrajectories(double(continuousAxis->axisEnd()),
										     stepRegionPositions,
										     currentPosition);

	// Generate all the continuous actions and add them to the list.  It's a pseudo-loop.
	for (int i = 0, size = stepAxis->numberOfPoints(); i < size; i++)
		mapActions->addSubAction(generateContinuousAxisActions(continuousAxis, continuousControl, startTrajectories.at(i), endTrajectories.at(i)));

	// Get the cleanup actions for the step axis.
	AMListAction3 *cleanupActions = new AMListAction3(
				new AMListActionInfo3(
					QString("Cleaning Up %1").arg(stepControl->name()),
					QString("Cleaning Up Axis with Control %1").arg(stepControl->name())),
				AMListAction3::Sequential);
	mapActions->addSubAction(cleanupActions);

	// Finally, the axis finished action for the step axis.
	AMAxisFinishedAction *axisFinishAction = new AMAxisFinishedAction(new AMAxisFinishedActionInfo(QString("%1 Axis").arg(stepAxis->name())));
	mapActions->addSubAction(axisFinishAction);

	actionTree_ = mapActions;

	return true;
}

QList<double> SGMHexapodMapScanActionControllerAssembler::generateStepAxisPositions(AMScanAxisRegion *region) const
{
	QList<double> stepPositions;

	for (int i = 0, size = region->numberOfPoints(); i < size; i++)
		stepPositions << (double(region->regionStart()) + i*double(region->regionStep()));

	return stepPositions;
}

QList<QVector3D> SGMHexapodMapScanActionControllerAssembler::generateContinuousAxisStartTrajectories(double continuousStart, const QList<double> &yAxisSetpoints, const QVector3D &templatePosition) const
{
	QList<QVector3D> startTrajectories;

	foreach (double yValue, yAxisSetpoints){

		QVector3D position = templatePosition;
		position.setX(continuousStart);
		position.setY(yValue);
		startTrajectories << position;
	}

	return startTrajectories;
}

QList<QVector3D> SGMHexapodMapScanActionControllerAssembler::generateContinuousAxisEndTrajectories(double continuousEnd, const QList<double> &yAxisSetpoints, const QVector3D &templatePosition) const
{
	QList<QVector3D> endTrajectories;

	foreach (double yValue, yAxisSetpoints){

		QVector3D position = templatePosition;
		position.setX(continuousEnd);
		position.setY(yValue);
		endTrajectories << position;
	}

	return endTrajectories;
}

#include "actions3/actions/AMWaitAction.h"

AMAction3 *SGMHexapodMapScanActionControllerAssembler::generateContinuousAxisActions(AMScanAxis *continuousAxis, AMControl *hexapodControl, const QVector3D &startTrajectory, const QVector3D &endTrajectory) const
{
	AMListAction3 *axisActions = new AMSequentialListAction3(
				new AMSequentialListActionInfo3(QString("Axis %1").arg(continuousAxis->name()),
								QString("Axis %1").arg(continuousAxis->name())));

	AMAxisStartedAction *axisStartAction = new AMAxisStartedAction(
				new AMAxisStartedActionInfo(QString("%1 Axis").arg(continuousAxis->name()),
							    AMScanAxis::ContinuousMoveAxis));
	axisActions->addSubAction(axisStartAction);

	// Generate axis initialization list //////////////
	AMListAction3 *initializationActions = new AMSequentialListAction3(
				new AMSequentialListActionInfo3(QString("Initializing %1").arg(hexapodControl->name()),
								QString("Initializing Axis with Control %1").arg(hexapodControl->name())));

	SGMHexapodTransformedAxis *hexapod = qobject_cast<SGMHexapodTransformedAxis *>(hexapodControl);
	double time = double(continuousAxis->regionAt(0)->regionTime());

	initializationActions->addSubAction(new AMWaitAction(new AMWaitActionInfo(0.1)));
	initializationActions->addSubAction(hexapod->createSetParametersActions(startTrajectory, endTrajectory, time));
	initializationActions->addSubAction(hexapodControl->createInitializeCoordinatedMovementActions());

	axisActions->addSubAction(initializationActions);
	// End Initialization /////////////////////////////


	// ACTION GENERATION: Coordinated Movement and Wait to Finish
	// This is where the control is told to go and detectors to acquire.
	axisActions->addSubAction(hexapodControl->createStartCoordinatedMovementActions());

	// The move should auto-trigger the detectors, but if there is more stuff, it will probably go here somewhere.

	// End control actions ////////////////////////////
	// Wait for the energy control to arrive within tolerance of the destination
	axisActions->addSubAction(hexapodControl->createWaitForCompletionActions());

	// END OF ACTION GENERATION: Coordinated Movement and Wait to Finish

	// ACTION GENERATION: Detectors
	QList<AMDetector*> detectorsToConfigure;
	bool foundOneScaler = false;
	for(int x = 0, size = detectors_->count(); x < size; x++){
		CLSAMDSScalerChannelDetector *asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(detectors_->at(x));
		if(asScalerChannelDetector && !foundOneScaler){
			foundOneScaler = true;
			detectorsToConfigure.append(detectors_->at(x));
		}
		else if(!asScalerChannelDetector)
			detectorsToConfigure.append(detectors_->at(x));
	}

	// Generate two lists - one parallel for triggering the other parallel for reading
	AMListAction3 *continuousDetectorTriggerList = new AMParallelListAction3(new AMParallelListActionInfo3(QString("Triggering Continuous Detectors"), QString("Triggering Continuous Detectors")));
	AMListAction3 *continuousDetectorReadList = new AMParallelListAction3(new AMParallelListActionInfo3(QString("Reading Continuous Detectors"), QString("Reading Continuous Detectors")));
	for(int x = 0, size = detectorsToConfigure.count(); x < size; x++){
		// Get each detector to give us a trigger action and set the continuousWindowSeconds parameter
		AMAction3 *continuousDetectorTrigger = detectorsToConfigure.at(x)->createTriggerAction(AMDetectorDefinitions::ContinuousRead);
		AMDetectorTriggerActionInfo *asTriggerActionInfo = qobject_cast<AMDetectorTriggerActionInfo*>(continuousDetectorTrigger->info());
		if(asTriggerActionInfo)
			asTriggerActionInfo->setContinuousWindowSeconds(time+4.0);
		continuousDetectorTriggerList->addSubAction(continuousDetectorTrigger);

		// Get each detector to give us a read action
		AMAction3 *continuousDetectorRead = detectorsToConfigure.at(x)->createReadAction();
		continuousDetectorRead->setGenerateScanActionMessage(true);
		continuousDetectorReadList->addSubAction(continuousDetectorRead);
	}
	// First add the parallel list of triggers
	axisActions->addSubAction(continuousDetectorTriggerList);
	// Once all triggers are confirmed (ie, data is back and ready) do all of the reads in parallel
	axisActions->addSubAction(continuousDetectorReadList);
	// END OF ACTION GENERATION: Detectors

	AMAxisFinishedAction *axisFinishAction = new AMAxisFinishedAction(new AMAxisFinishedActionInfo(QString("%1 Axis").arg(continuousAxis->name())));
	axisActions->addSubAction(axisFinishAction);

	return axisActions;
}
