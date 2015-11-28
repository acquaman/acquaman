#include "SGMEnergyPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/actions/AMChangeToleranceAction.h"
#include "actions3/AMAction3.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/energy/SGMEnergyControlSet.h"
#include "beamline/SGM/energy/SGMExitSlitSupport.h"
SGMEnergyPVControl::SGMEnergyPVControl(QObject *parent) :
    AMPVwStatusControl("Energy",
		       "AM1611-4-I10:energy:eV:fbk",
		       "AM1611-4-I10:energy:eV",
                       "AM1611-4-I10:energy:status",
                       "AM1611-4-I10:energy:stop",
		       parent,
		       SGMENERGYPVCONTROL_CLOSEDLOOP_TOLERANCE,
                       2.0,
                       new CLSMAXvControlStatusChecker())
{

	 coordinatedTarget_ = new AMSinglePVControl("Energy Trajectory Target",
							"AM1611-4-I10:energy:trajectory:target:eV",
	                                                this,
	                                                0.5,
	                                                2);

	 coordinatedDeltaTime_ = new AMSinglePVControl("Energy Trajectory Time",
	                                               "AM1611-4-I10:energy:trajectory:time:s",
	                                               this,
	                                               0.5,
	                                               2);

	 coordinatedStart_ = new AMSinglePVControl("Energy Trajectory Start",
	                                           "AM1611-4-I10:energy:trajectory:start",
	                                           this,
	                                           0.5,
	                                           2);

	 setAttemptMoveWhenWithinTolerance(true);
}

bool SGMEnergyPVControl::shouldPerformCoordinatedMovement() const
{
	return true;
}

bool SGMEnergyPVControl::canPerformCoordinatedMovement() const
{
	return true;
}

AMAction3 * SGMEnergyPVControl::createSetParametersActions(double startPoint, double endPoint, double deltaTime)
{
	savedStartpoint_ = startPoint;
	savedEndpoint_ = endPoint;
	savedDeltaTime_ = deltaTime;
	AMListAction3* setParameterActions = new AMListAction3(new AMListActionInfo3("Set energy trajectory parameters",
	                                                                             "Set energy trajectory parameters"),
	                                                       AMListAction3::Sequential);

	setParameterActions->addSubAction(AMActionSupport::buildControlMoveAction(coordinatedTarget_, savedEndpoint_));
	setParameterActions->addSubAction(AMActionSupport::buildControlMoveAction(coordinatedDeltaTime_, deltaTime));
	setParameterActions->addSubAction(AMActionSupport::buildControlWaitAction(coordinatedTarget_, savedEndpoint_, 2, AMControlWaitActionInfo::MatchWithinTolerance));
	setParameterActions->addSubAction(AMActionSupport::buildControlWaitAction(coordinatedDeltaTime_, deltaTime, 2, AMControlWaitActionInfo::MatchWithinTolerance));



	return setParameterActions;
}

AMAction3 * SGMEnergyPVControl::createInitializeCoordinatedMovementActions()
{
	AMListAction3* initializeActions = new AMListAction3(new AMListActionInfo3("Initialize energy motion",
	                                                                           "Initialize energy motion"),
	                                                     AMListAction3::Sequential);


	AMControl* exitSlitPositionControl = SGMBeamline::sgm()->energyControlSet()->exitSlitPosition();
	AMControl* exitSlitTrackingControl = SGMBeamline::sgm()->energyControlSet()->exitSlitPositionTracking();
	double savedExitSlitTrackingStateValue = exitSlitTrackingControl->value();
	bool savedExitSlitTrackingState = exitSlitTrackingControl->withinTolerance(1.0);

	// #1 Turn off tracking exit slit.
	initializeActions->addSubAction(AMActionSupport::buildControlMoveAction(exitSlitTrackingControl, 0));
	initializeActions->addSubAction(AMActionSupport::buildControlWaitAction(exitSlitTrackingControl, 0, 2, AMControlWaitActionInfo::MatchWithinTolerance));

	// #2 Move controls to their starting positions.
	AMListAction3* controlMoveActions = new AMListAction3(new AMListActionInfo3("Moving energy controls to starting positions",
	                                                                            "Moving energy controls to starting positions"),
	                                                      AMListAction3::Parallel);

	// #2a Move exit slit to its static position for the motion (if tracking was set to on at the start).
	SGMGratingSupport::GratingTranslation currentGratingTranslation =
	        SGMGratingSupport::GratingTranslation(int(SGMBeamline::sgm()->energyControlSet()->gratingTranslation()->value()));

	double meanExitSlitPosition = SGMExitSlitSupport::exitSlitPositionForScan(savedStartpoint_, savedEndpoint_, currentGratingTranslation);
	if(savedExitSlitTrackingState) {
		if(!exitSlitPositionControl->withinTolerance(meanExitSlitPosition)) {

			controlMoveActions->addSubAction(AMActionSupport::buildControlMoveAction(exitSlitPositionControl, meanExitSlitPosition));
		}
	}

	// #2b Move energy to the start point for the coodinated motion
	controlMoveActions->addSubAction(AMActionSupport::buildControlMoveAction(this, savedStartpoint_));
	initializeActions->addSubAction(controlMoveActions);


	// #3 Wait for controls to reach their starting positions
	AMListAction3* controlWaitActions = new AMListAction3(new AMListActionInfo3("Waiting for controls to reach starting positions",
	                                                                            "Waiting for controls to reach starting positions"),
	                                                      AMListAction3::Parallel);


	// #3a Wait for exit slit to reach its static position (if tracking was set to on)
	if(savedExitSlitTrackingState) {
		controlWaitActions->addSubAction(AMActionSupport::buildControlWaitAction(exitSlitPositionControl, meanExitSlitPosition, 60, AMControlWaitActionInfo::MatchWithinTolerance));
	}

	// #3b Wait for energy to reach its starting position
	controlWaitActions->addSubAction(AMActionSupport::buildControlWaitAction(this, savedStartpoint_, 60, AMControlWaitActionInfo::MatchWithinTolerance));

	initializeActions->addSubAction(controlWaitActions);

	// #4 Re-enable exit slit tracking.
	initializeActions->addSubAction(AMActionSupport::buildControlMoveAction(exitSlitTrackingControl, savedExitSlitTrackingStateValue));
	initializeActions->addSubAction(AMActionSupport::buildControlWaitAction(exitSlitTrackingControl, savedExitSlitTrackingStateValue, 2, AMControlWaitActionInfo::MatchWithinTolerance));

	// Set the tolerance of this control to less, so that it succeeds within the accuracy of the stepper target
	initializeActions->addSubAction(new AMChangeToleranceAction(new AMChangeToleranceActionInfo(toInfo(), SGMENERGYPVCONTROL_COORDINATED_TOLERANCE),this));

	// #6 Read the detector emulator for the grating encoder. This will give us a start position that's accurate for this scan.
	AMAction3 *gratingEncoderDetectorReadAction = SGMBeamline::sgm()->exposedDetectorByName("GratingEncoderFeedback")->createReadAction();
	gratingEncoderDetectorReadAction->setGenerateScanActionMessage(true);
	initializeActions->addSubAction(gratingEncoderDetectorReadAction);

	return initializeActions;
}

AMAction3 * SGMEnergyPVControl::createStartCoordinatedMovementActions()
{
	AMListAction3* startAction = new AMListAction3(new AMListActionInfo3("Trigger start of coordinated energy motion",
	                                                                     "Trigger start of coordinated energy motion"),
	                                               AMListAction3::Sequential);

	startAction->addSubAction(AMActionSupport::buildControlMoveAction(coordinatedStart_, 1));
	startAction->addSubAction(AMActionSupport::buildControlWaitAction(coordinatedStart_, 1, 2, AMControlWaitActionInfo::MatchWithinTolerance));
	startAction->setGenerateScanActionMessage(true);

	return startAction;
}

AMAction3 * SGMEnergyPVControl::createWaitForCompletionActions()
{
	AMListAction3* completionWaitAction = new AMListAction3(new AMListActionInfo3("Wait for coordinated energy motion",
	                                                                              "Wait for coordinated energy motion"),
	                                                        AMListAction3::Sequential);

	completionWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(this, savedEndpoint_, savedDeltaTime_*1.5, AMControlWaitActionInfo::MatchWithinTolerance));
	completionWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(SGMBeamline::sgm()->energyControlSet()->energyStatus(), 0, savedDeltaTime_*1.5, AMControlWaitActionInfo::MatchWithinTolerance));
	// Change the tolerance back to its default for closed loop motions
	completionWaitAction->addSubAction(new AMChangeToleranceAction(new AMChangeToleranceActionInfo(toInfo(), SGMENERGYPVCONTROL_CLOSEDLOOP_TOLERANCE),this));
	return completionWaitAction;
}
