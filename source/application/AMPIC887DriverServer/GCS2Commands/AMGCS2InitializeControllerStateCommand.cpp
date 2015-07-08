#include "AMGCS2InitializeControllerStateCommand.h"

#include "AMGCS2GetMovingStatusCommand.h"
#include "AMGCS2GetSystemVelocityCommand.h"
#include "AMGCS2GetCycleTimeCommand.h"
#include "AMGCS2GetServoModeCommand.h"
#include "AMGCS2GetCurrentPositionCommand.h"
#include "AMGCS2GetReferenceResultCommand.h"
#include "AMGCS2GetLowSoftLimitsCommand.h"
#include "AMGCS2GetHighSoftLimitsCommand.h"
#include "AMGCS2GetLimitSwitchStatusCommand.h"
#include "AMGCS2GetPositionUnitsCommand.h"
#include "AMGCS2GetStepSizeCommand.h"
#include "AMGCS2GetMinCommandablePositionCommand.h"
#include "AMGCS2GetMaxCommandablePositionCommand.h"
#include "AMGCS2GetPivotPointCommand.h"
#include "AMGCS2GetRecordTriggerSourceCommand.h"
#include "AMGCS2GetDataRecorderConfigurationCommand.h"


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

	QFlags<AMGCS2::AxisMovementStatus> axisMovementStatuses =
			movingStatusCommand.movementStatuses();

	if(axisMovementStatuses) {

		lastError_ = "Cannot load data for a controller while its hexapod is in motion";
		return false;
	}

	return true;
}

bool AMGCS2InitializeControllerStateCommand::runImplementation()
{

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

	// Servo mode applies consistently across all axes, so we only need the value
	// for X.
	bool servoMode = servoModeCommand.servoModeStatuses().value(AMGCS2::XAxis);

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

	// Current position & target position (same as we're not moving)
	AMGCS2GetCurrentPositionCommand currentPositionCommand;
	currentPositionCommand.setController(controller_);
	currentPositionCommand.run();

	if(currentPositionCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain position of controller's hexapod";
		return false;
	}

	QHash<AMGCS2::Axis, double> currentPositions =
			currentPositionCommand.axisPositions();

	// The referenced state of the axes
	AMGCS2GetReferenceResultCommand referencedResultsCommand;
	referencedResultsCommand.setController(controller_);
	referencedResultsCommand.run();

	if(referencedResultsCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the referenced statuses of the controller's hexapod";
		return false;
	}

	QHash<AMGCS2::Axis, bool> referencedResults =
			referencedResultsCommand.axesReferenceResults();

	// The low soft limit values of the axes
	AMGCS2GetLowSoftLimitsCommand lowSoftLimitsCommand;
	lowSoftLimitsCommand.setController(controller_);
	lowSoftLimitsCommand.run();

	if(lowSoftLimitsCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the low soft limit values of the controller's hexapod";
		return false;
	}

	QHash<AMGCS2::Axis, double> lowSoftLimts =
			lowSoftLimitsCommand.axesLowSoftLimits();

	// The high soft limit values of the axes
	AMGCS2GetHighSoftLimitsCommand highSoftLimitsCommand;
	highSoftLimitsCommand.setController(controller_);
	highSoftLimitsCommand.run();

	if(highSoftLimitsCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the high soft limit values of the controller's hexapod";
		return false;
	}

	QHash<AMGCS2::Axis, double> highSoftLimts =
			highSoftLimitsCommand.axesHighSoftLimits();

	// Whether the soft limits are active for the axes
	AMGCS2GetLimitSwitchStatusCommand softLimitStatusCommand;
	softLimitStatusCommand.setController(controller_);
	softLimitStatusCommand.run();

	if(softLimitStatusCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the soft limit statuses of the controller's hexapod";
		return false;
	}

	QHash<AMGCS2::Axis, bool> softLimitStatuses =
			softLimitStatusCommand.limitSwitchStatuses();

	// The position units of the axes
	AMGCS2GetPositionUnitsCommand positionUnitsCommand;
	positionUnitsCommand.setController(controller_);
	positionUnitsCommand.run();

	if(positionUnitsCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the position units of the controller's hexapod";
		return false;
	}

	QHash<AMGCS2::Axis, AMGCS2::PositionUnits> positionUnits =
			positionUnitsCommand.axesUnits();

	// The step size values of the axes
	AMGCS2GetStepSizeCommand stepSizeCommand;
	stepSizeCommand.setController(controller_);
	stepSizeCommand.run();

	if(stepSizeCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the step size values of the controller's hexapod";
		return false;
	}

	QHash<AMGCS2::Axis, double> stepSizes =
			stepSizeCommand.axisStepSizes();

	// The min commandable positions of the axes
	AMGCS2GetMinCommandablePositionCommand minCommandablePositionCommand;
	minCommandablePositionCommand.setController(controller_);
	minCommandablePositionCommand.run();

	if(minCommandablePositionCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the min commandable positions of the controller's hexapod";
		return false;
	}

	QHash<AMGCS2::Axis, double> minPositions =
			minCommandablePositionCommand.minCommandablePositions();


	// The max commandable position of the axes
	AMGCS2GetMaxCommandablePositionCommand maxCommandablePositionCommand;
	maxCommandablePositionCommand.setController(controller_);
	maxCommandablePositionCommand.run();

	if(maxCommandablePositionCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the max commandable positions of the controller's hexapod";
		return false;
	}

	QHash<AMGCS2::Axis, double> maxPositions =
			maxCommandablePositionCommand.maxCommandablePositions();

	// The pivot points of the linear axes (X, Y, Z)
	AMGCS2GetPivotPointCommand pivotPointCommand;
	pivotPointCommand.setController(controller_);
	pivotPointCommand.run();

	if(pivotPointCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain the pivot point positions of the controller's hexapod";
		return false;
	}

	QHash<AMGCS2::Axis, double> pivotPoints =
			pivotPointCommand.axisPivotPoints();

	// Data Recorder Statuses
	//////////////////////////
	QList<int> tableIdsList;
	for(int iTableId = 1; iTableId <= 16; ++iTableId) {
		tableIdsList.append(iTableId);
	}

	// Record Trigger
	// Note command syntax is set up for this to run on each table, but changing
	// value on one table changes them all. As such this is stored once at the
	// general recorder state leve, not once per table.

	AMGCS2GetRecordTriggerSourceCommand triggerSourceCommand(tableIdsList);
	triggerSourceCommand.setController(controller_);
	triggerSourceCommand.run();

	if(triggerSourceCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain recorder trigger source";
		return false;
	}

	QHash<int, AMGCS2::DataRecordTrigger> recordTriggers =
			triggerSourceCommand.tableRecordTriggers();

	// Data Recorder Table Statuses
	////////////////////////////////

	// Record configurations (option)
	AMGCS2GetDataRecorderConfigurationCommand recordConfigurationsCommand;
	recordConfigurationsCommand.setController(controller_);
	recordConfigurationsCommand.run();

	if(recordConfigurationsCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain table recorder configurations";
		return false;
	}

	QList<AMPIC887DataRecorderConfiguration*> recordConfigurations =
			recordConfigurationsCommand.dataRecorderConfigurations();

	// Initialize controller state data
	////////////////////////////////////

	// Initialize Hexapod data
	///////////////////////////

	controllerState_->hexapodState()->initialize(servoMode, cycleTime, systemVelocity);

	// Initialize Hexapod Axes data
	////////////////////////////////

	controllerState_->hexapodState()->xAxisState()->initialize(
				referencedResults.value(AMGCS2::XAxis),
				currentPositions.value(AMGCS2::XAxis),
				lowSoftLimts.value(AMGCS2::XAxis),
				highSoftLimts.value(AMGCS2::XAxis),
				softLimitStatuses.value(AMGCS2::XAxis),
				positionUnits.value(AMGCS2::XAxis),
				stepSizes.value(AMGCS2::XAxis),
				maxPositions.value(AMGCS2::XAxis),
				minPositions.value(AMGCS2::XAxis),
				pivotPoints.value(AMGCS2::XAxis));

	controllerState_->hexapodState()->yAxisState()->initialize(
				referencedResults.value(AMGCS2::YAxis),
				currentPositions.value(AMGCS2::YAxis),
				lowSoftLimts.value(AMGCS2::YAxis),
				highSoftLimts.value(AMGCS2::YAxis),
				softLimitStatuses.value(AMGCS2::YAxis),
				positionUnits.value(AMGCS2::YAxis),
				stepSizes.value(AMGCS2::YAxis),
				maxPositions.value(AMGCS2::YAxis),
				minPositions.value(AMGCS2::YAxis),
				pivotPoints.value(AMGCS2::YAxis));

	controllerState_->hexapodState()->zAxisState()->initialize(
				referencedResults.value(AMGCS2::ZAxis),
				currentPositions.value(AMGCS2::ZAxis),
				lowSoftLimts.value(AMGCS2::ZAxis),
				highSoftLimts.value(AMGCS2::ZAxis),
				softLimitStatuses.value(AMGCS2::ZAxis),
				positionUnits.value(AMGCS2::ZAxis),
				stepSizes.value(AMGCS2::ZAxis),
				maxPositions.value(AMGCS2::ZAxis),
				minPositions.value(AMGCS2::ZAxis),
				pivotPoints.value(AMGCS2::ZAxis));

	controllerState_->hexapodState()->uAxisState()->initialize(
				referencedResults.value(AMGCS2::UAxis),
				currentPositions.value(AMGCS2::UAxis),
				lowSoftLimts.value(AMGCS2::UAxis),
				highSoftLimts.value(AMGCS2::UAxis),
				softLimitStatuses.value(AMGCS2::UAxis),
				positionUnits.value(AMGCS2::UAxis),
				stepSizes.value(AMGCS2::UAxis),
				maxPositions.value(AMGCS2::UAxis),
				minPositions.value(AMGCS2::UAxis));

	controllerState_->hexapodState()->vAxisState()->initialize(
				referencedResults.value(AMGCS2::VAxis),
				currentPositions.value(AMGCS2::VAxis),
				lowSoftLimts.value(AMGCS2::VAxis),
				highSoftLimts.value(AMGCS2::VAxis),
				softLimitStatuses.value(AMGCS2::VAxis),
				positionUnits.value(AMGCS2::VAxis),
				stepSizes.value(AMGCS2::VAxis),
				maxPositions.value(AMGCS2::VAxis),
				minPositions.value(AMGCS2::VAxis));

	controllerState_->hexapodState()->wAxisState()->initialize(
				referencedResults.value(AMGCS2::WAxis),
				currentPositions.value(AMGCS2::WAxis),
				lowSoftLimts.value(AMGCS2::WAxis),
				highSoftLimts.value(AMGCS2::WAxis),
				softLimitStatuses.value(AMGCS2::WAxis),
				positionUnits.value(AMGCS2::WAxis),
				stepSizes.value(AMGCS2::WAxis),
				maxPositions.value(AMGCS2::WAxis),
				minPositions.value(AMGCS2::WAxis));

	// Initialize data recorder data
	/////////////////////////////////

	// Can use the value for 1 table, as this applies to all tables.
	controllerState_->dataRecorderState()->initialize(recordTriggers.value(1));

	// Initialize data for recorder tables
	///////////////////////////////////////
	for(int iRecordTable = 1; iRecordTable <= 16; ++iRecordTable) {
		AMPIC887DataRecorderConfiguration* currentConfig =
				recordConfigurations.at(iRecordTable -1);

		controllerState_->dataRecorderState()->stateAt(iRecordTable)
				->initialize(currentConfig->recordSource(), currentConfig->recordOption());
	}

	if(!controllerState_->isAllInitialized()) {
		lastError_ = "Unknown error initializing the controller state";
		return false;
	}

	return true;
}


