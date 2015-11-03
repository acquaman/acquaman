#include "AMGCS2InitializeControllerStateCommand.h"

#include "AMGCS2GetMovingStatusCommand.h"
#include "AMGCS2GetSystemVelocityCommand.h"
#include "AMGCS2GetCycleTimeCommand.h"
#include "AMGCS2GetServoModeCommand.h"
#include "AMGCS2GetCurrentPositionCommand.h"
#include "AMGCS2GetReferenceResultCommand.h"
#include "AMGCS2GetLowSoftLimitsCommand.h"
#include "AMGCS2GetHighSoftLimitsCommand.h"
#include "AMGCS2GetSoftLimitsStatusCommand.h"
#include "AMGCS2GetLimitSwitchStatusCommand.h"
#include "AMGCS2GetPositionUnitsCommand.h"
#include "AMGCS2GetStepSizeCommand.h"
#include "AMGCS2GetMinCommandablePositionCommand.h"
#include "AMGCS2GetMaxCommandablePositionCommand.h"
#include "AMGCS2GetPivotPointCommand.h"
#include "AMGCS2GetRecordTriggerSourceCommand.h"
#include "AMGCS2GetRecordConfigurationCommand.h"
#include "AMGCS2GetCommandLevelCommand.h"
#include "AMGCS2GetAvailableParametersCommand.h"
#include "AMGCS2GetRecorderOptionsCommand.h"
#include "AMGCS2GetDeviceIdentificationCommand.h"
#include "AMGCS2GetRecordRateCommand.h"


AMGCS2InitializeControllerStateCommand::AMGCS2InitializeControllerStateCommand(AMPIC887ControllerState* controllerState)
{
	controllerState_ = controllerState;
}

AMGCS2InitializeControllerStateCommand::~AMGCS2InitializeControllerStateCommand()
{

}

bool AMGCS2InitializeControllerStateCommand::validateArguments()
{

	if(!controllerState_) {
		lastError_ = "Could not initialize state of controller. No controller state provided";
		return false;
	}

	AMGCS2GetMovingStatusCommand movingStatusCommand;

	movingStatusCommand.setController(controller_);
	movingStatusCommand.run();

	if(movingStatusCommand.runningState() != Succeeded) {
		lastError_ = "Could not determin motion status of the controller";
		return false;
	}

	AMGCS2::AxisMovementStatuses axisMovementStatuses =
			movingStatusCommand.movementStatuses();

	if(axisMovementStatuses) {

		lastError_ = "Cannot load data for a controller while its hexapod is in motion";
		return false;
	}

	return true;
}


bool AMGCS2InitializeControllerStateCommand::runImplementation()
{

	// Controller Statuses
	//////////////////////

	// Command Level
	AMGCS2GetCommandLevelCommand commandLevelCommand;
	commandLevelCommand.setController(controller_);
	commandLevelCommand.run();

	if(commandLevelCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain controller's command level";
		return false;
	}

	AMGCS2::ControllerCommandLevel commandLevel =
			commandLevelCommand.commandLevel();

	// Available Parameters String
	AMGCS2GetAvailableParametersCommand availableParametersCommand;
	availableParametersCommand.setController(controller_);
	availableParametersCommand.run();

	if(availableParametersCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain controller's available parameters";
		return false;
	}

	QString availableParameters =
			availableParametersCommand.availableParameters();

	// Device identification
	AMGCS2GetDeviceIdentificationCommand deviceIdentificationCommand;
	deviceIdentificationCommand.setController(controller_);
	deviceIdentificationCommand.run();

	if(deviceIdentificationCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain controller's device identification";
		return false;
	}

	QString deviceIdentification =
			deviceIdentificationCommand.deviceIdentification();

	// Hexapod Level Statuses
	////////////////////////////

	// System Velocity
	AMGCS2GetSystemVelocityCommand systemVelocityCommand;
	systemVelocityCommand.setController(controller_);
	systemVelocityCommand.run();

	if(systemVelocityCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the system velocity of the controller";
		return false;
	}

	double systemVelocity = systemVelocityCommand.systemVelocity();

	// Servo Mode
	AMGCS2GetServoModeCommand servoModeCommand;
	servoModeCommand.setController(controller_);
	servoModeCommand.run();

	if(servoModeCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the servo mode of the controller";
		return false;
	}

	bool servoMode = servoModeCommand.servoModeState();

	// Cycle Time
	AMGCS2GetCycleTimeCommand cycleTimeCommand;
	cycleTimeCommand.setController(controller_);
	cycleTimeCommand.run();

	if(cycleTimeCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain cycle time of the controller";
		return false;
	}

	double cycleTime = cycleTimeCommand.cycleTime();

	// Axis Level Statuses
	//////////////////////

	// The referenced state of the axes
	AMGCS2GetReferenceResultCommand referencedResultsCommand;
	referencedResultsCommand.setController(controller_);
	referencedResultsCommand.run();

	if(referencedResultsCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the referenced statuses of the controller's hexapod";
		return false;
	}

	// Current position & target position (same as we're not moving)
	AMGCS2GetCurrentPositionCommand currentPositionCommand;
	currentPositionCommand.setController(controller_);
	currentPositionCommand.run();

	if(currentPositionCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain position of controller's hexapod";
		return false;
	}

	// The low soft limit values of the axes
	AMGCS2GetLowSoftLimitsCommand lowSoftLimitsCommand;
	lowSoftLimitsCommand.setController(controller_);
	lowSoftLimitsCommand.run();

	if(lowSoftLimitsCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the low soft limit values of the controller's hexapod";
		return false;
	}

	// The high soft limit values of the axes
	AMGCS2GetHighSoftLimitsCommand highSoftLimitsCommand;
	highSoftLimitsCommand.setController(controller_);
	highSoftLimitsCommand.run();

	if(highSoftLimitsCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the high soft limit values of the controller's hexapod";
		return false;
	}

	// Whether the soft limits are active for the axes
	AMGCS2GetSoftLimitsStatusCommand softLimitStatusCommand;
	softLimitStatusCommand.setController(controller_);
	softLimitStatusCommand.run();

	if(softLimitStatusCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the soft limit statuses of the controller's hexapod";
		return false;
	}

	// Limit switch states for the axes
	AMGCS2GetLimitSwitchStatusCommand limitSwitchStatesCommand;
	limitSwitchStatesCommand.setController(controller_);
	limitSwitchStatesCommand.run();

	if(limitSwitchStatesCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the limit switch state of the controller's hexapod";
		return false;
	}

	// The position units of the axes
	AMGCS2GetPositionUnitsCommand positionUnitsCommand;
	positionUnitsCommand.setController(controller_);
	positionUnitsCommand.run();

	if(positionUnitsCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the position units of the controller's hexapod";
		return false;
	}

	// The step size values of the axes
	AMGCS2GetStepSizeCommand stepSizeCommand;
	stepSizeCommand.setController(controller_);
	stepSizeCommand.run();

	if(stepSizeCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the step size values of the controller's hexapod";
		return false;
	}

	// The min commandable positions of the axes
	AMGCS2GetMinCommandablePositionCommand minCommandablePositionCommand;
	minCommandablePositionCommand.setController(controller_);
	minCommandablePositionCommand.run();

	if(minCommandablePositionCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the min commandable positions of the controller's hexapod";
		return false;
	}

	// The max commandable position of the axes
	AMGCS2GetMaxCommandablePositionCommand maxCommandablePositionCommand;
	maxCommandablePositionCommand.setController(controller_);
	maxCommandablePositionCommand.run();

	if(maxCommandablePositionCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the max commandable positions of the controller's hexapod";
		return false;
	}

	// The pivot points of the linear axes (X, Y, Z)
	AMGCS2GetPivotPointCommand pivotPointCommand;
	pivotPointCommand.setController(controller_);
	pivotPointCommand.run();

	if(pivotPointCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the pivot point positions of the controller's hexapod";
		return false;
	}


	// Data Recorder Statuses
	//////////////////////////
	QList<int> tableIdsList;
	for(int iTableId = 1; iTableId <= 16; ++iTableId) {
		tableIdsList.append(iTableId);
	}

	// Record Trigger

	AMGCS2GetRecordTriggerSourceCommand triggerSourceCommand;
	triggerSourceCommand.setController(controller_);
	triggerSourceCommand.run();

	if(triggerSourceCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain recorder trigger source";
		return false;
	}

	// Record options

	AMGCS2GetRecorderOptionsCommand recorderOptionsCommand;
	recorderOptionsCommand.setController(controller_);
	recorderOptionsCommand.run();

	if(recorderOptionsCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain record options";
		return false;
	}

	// Record rate

	AMGCS2GetRecordRateCommand recordRateCommand;
	recordRateCommand.setController(controller_);
	recordRateCommand.run();

	if(recordRateCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain record rate";
		return false;
	}


	// Data Recorder Table Statuses
	////////////////////////////////

	// Record configurations
	AMGCS2GetRecordConfigurationsCommand recordConfigurationsCommand;
	recordConfigurationsCommand.setController(controller_);
	recordConfigurationsCommand.run();

	if(recordConfigurationsCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain table recorder configurations";
		return false;
	}

	// Initialize controller state data
	////////////////////////////////////

	controllerState_->initialize(commandLevel, availableParameters, deviceIdentification);

	// Initialize Hexapod data
	///////////////////////////
	controllerState_->hexapodState()->initialize(servoMode,
												 cycleTime,
												 systemVelocity,
												 referencedResultsCommand.axesReferenceResults(),
												 currentPositionCommand.axisPositions(),
												 lowSoftLimitsCommand.axesLowSoftLimits(),
												 highSoftLimitsCommand.axesHighSoftLimits(),
												 softLimitStatusCommand.softLimitStatuses(),
												 limitSwitchStatesCommand.limitSwitchStatuses(),
												 positionUnitsCommand.axesUnits(),
												 stepSizeCommand.axisStepSizes(),
												 minCommandablePositionCommand.minCommandablePositions(),
												 maxCommandablePositionCommand.maxCommandablePositions(),
												 pivotPointCommand.axisPivotPoints());

	// Initialize data recorder data
	/////////////////////////////////

	controllerState_->dataRecorderState()->initialize(triggerSourceCommand.recordTrigger(),
													  recorderOptionsCommand.recordOptions(),
													  recordRateCommand.recordRate(),
													  recordConfigurationsCommand.recordConfigs());


	return true;
}


