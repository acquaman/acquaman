#include "AMPIC887ConsoleApplication.h"

#include <QTimer>

#include "AMConsoleInputHandler.h"
#include "AMPIC887ConsoleCommandParser.h"
#include "AMGCS2Support.h"

AMPIC887ConsoleApplication::AMPIC887ConsoleApplication(int argc, char *argv[]) :
	QCoreApplication(argc, argv)
{

	// Set up the console and command set.
	commandParser_ = new AMPIC887ConsoleCommandParser();
	consoleInputHandler_ = new AMConsoleInputHandler(commandParser_, this);


	if(startup()) {

		// Print a welcome message
		consoleInputHandler_->setCommandPrompt(
					QString("%1 > ")
					.arg(controllerCollection_.activeController()->name()));

		consoleInputHandler_->writeLineToStandardOutput("Welcome to the PI C887 Server Application.");
		consoleInputHandler_->writeLineToStandardOutput(controllerCollection_.connectionStatuses());
		consoleInputHandler_->writeLineToStandardOutput("Type 'help' for a list of commands.");
		consoleInputHandler_->displayCommandPrompt();

		controllerCoordinator_ = new AMPIC887EpicsCoordinator(controllerCollection_.activeController(), this);
		makeConnections();
	} else {

		// Startup failed. Print an error message, then quit the application.
		consoleInputHandler_->writeLineToStandardError("Failed to start hexapod driver application:");
		if(!controllerCollection_.activeController()) {
			consoleInputHandler_->writeLineToStandardError("Could not initialize any controllers");
		} else {
			consoleInputHandler_->writeLineToStandardError("Could not establish connection to active controller");
		}

		QTimer quitTimer(this);
		quitTimer.singleShot(10, this, SLOT(quit()));
	}
}

void AMPIC887ConsoleApplication::onHelpCommandIssued()
{
	consoleInputHandler_->writeLineToStandardOutput(commandParser_->commandList());
}

void AMPIC887ConsoleApplication::onQuitCommandIssued()
{
	consoleInputHandler_->writeLineToStandardOutput("Exiting...");
	quit();
}


void AMPIC887ConsoleApplication::onStatusCommandIssued()
{
	consoleInputHandler_->writeLineToStandardOutput(controllerCollection_.connectionStatuses());
}

void AMPIC887ConsoleApplication::onInitControllerCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else {
		activeController->initializeControllerStateData();
	}
}

void AMPIC887ConsoleApplication::onActiveControllerChangeRequested(const QString &controllerName)
{
	if(controllerCollection_.activeController()->name() != controllerName) {
		controllerCollection_.setActiveController(controllerName);
		if(controllerCollection_.activeController()->name() == controllerName) {
			consoleInputHandler_->setCommandPrompt(QString("%1 >").arg(controllerCollection_.activeController()->name()));
		} else {
			consoleInputHandler_->writeLineToStandardError(QString("No controller found with name %1").arg(controllerName));
		}
	}
}

void AMPIC887ConsoleApplication::onControllerOutputReceived(const QString &outputString)
{
	consoleInputHandler_->writeLineToStandardOutput(outputString);
}

void AMPIC887ConsoleApplication::onControllerErrorEncountered(const QString &errorMessage)
{
	consoleInputHandler_->writeLineToStandardError(errorMessage);
}

void AMPIC887ConsoleApplication::onSetCommandLevelCommandIssued(AMGCS2::ControllerCommandLevel commandLevel,
																const QString &password)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->setCommandLevel(commandLevel, password);
	}
}

void AMPIC887ConsoleApplication::onSetCycleTimeCommandIssued(double cycleTime)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->setCycleTime(cycleTime);
	}
}

void AMPIC887ConsoleApplication::onSetHighSoftLimitsCommandIssued(const AMPIC887AxisMap<double> &highSoftLimits)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->setHighSoftLimits(highSoftLimits);
	}
}

void AMPIC887ConsoleApplication::onSetLowSoftLimitsCommandIssued(const AMPIC887AxisMap<double> &lowSoftLimits)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else  {
		activeController->setLowSoftLimits(lowSoftLimits);
	}
}

void AMPIC887ConsoleApplication::onSetPivotPointCommandIssued(const AMPIC887AxisMap<double> &pivotPoints)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->setPivotPoints(pivotPoints);
	}
}

void AMPIC887ConsoleApplication::onSetRecorderTriggerCommandIssued(AMGCS2::DataRecordTrigger recorderTrigger)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->setRecordTrigger(recorderTrigger);
	}
}

void AMPIC887ConsoleApplication::onSetRecordConfigCommandIssued(const QHash<int, AMPIC887DataRecorderConfiguration> &recordConfigs)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->setRecordConfigs(recordConfigs);
	}
}

void AMPIC887ConsoleApplication::onSetServoModeCommandIssued(bool servoMode)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->setServoMode(servoMode);
	}
}

void AMPIC887ConsoleApplication::onSetSoftLimitStatesCommandIssued(const AMPIC887AxisMap<bool> &softLimitStates)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->setSoftLimitStatuses(softLimitStates);
	}
}

void AMPIC887ConsoleApplication::onSetStepSizeCommandIssued(const AMPIC887AxisMap<double> &stepSizes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->setStepSizes(stepSizes);
	}
}

void AMPIC887ConsoleApplication::onSetSystemVelocityCommandIssued(double systemVelocity)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->setSystemVelocity(systemVelocity);
	}
}

void AMPIC887ConsoleApplication::onMoveCommandIssued(const AMPIC887AxisMap<double> &axisPositions)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->move(axisPositions);
	}
}

void AMPIC887ConsoleApplication::onMoveRelativeCommandIssued(const AMPIC887AxisMap<double> &relativePositions)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->moveRelative(relativePositions);
	}
}

void AMPIC887ConsoleApplication::onMotionStatusCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {


		AMGCS2::AxisMovementStatuses movementStatuses =
				activeController->movementStatuses();

		if(movementStatuses.testFlag(AMGCS2::UnknownAxisMovementStatus)) {
			consoleInputHandler_->writeLineToStandardOutput("Axis movement status not known.");
		}

		QString movementStatusesText;
		bool anyMovement = false;

		if(movementStatuses.testFlag(AMGCS2::XAxisIsMoving)) {
			anyMovement = true;
			movementStatusesText.append("X is moving\n");
		} else {
			movementStatusesText.append("X is not moving\n");
		}

		if(movementStatuses.testFlag(AMGCS2::YAxisIsMoving)) {
			anyMovement = true;
			movementStatusesText.append("Y is moving\n");
		} else {
			movementStatusesText.append("Y is not moving\n");
		}

		if(movementStatuses.testFlag(AMGCS2::ZAxisIsMoving)) {
			anyMovement = true;
			movementStatusesText.append("Z is moving\n");
		} else {
			movementStatusesText.append("Z is not moving\n");
		}

		if(movementStatuses.testFlag(AMGCS2::UAxisIsMoving)) {
			anyMovement = true;
			movementStatusesText.append("U is moving\n");
		} else {
			movementStatusesText.append("U is not moving\n");
		}

		if(movementStatuses.testFlag(AMGCS2::VAxisIsMoving)) {
			anyMovement = true;
			movementStatusesText.append("V is moving\n");
		} else {
			movementStatusesText.append("V is not moving\n");
		}

		if(movementStatuses.testFlag(AMGCS2::WAxisIsMoving)) {
			anyMovement = true;
			movementStatusesText.append("W is moving\n");
		} else {
			movementStatusesText.append("W is not moving\n");
		}

		if(anyMovement)	{
			consoleInputHandler_->writeLineToStandardOutput(movementStatusesText);
		} else {
			consoleInputHandler_->writeLineToStandardOutput("No axes movement");
		}

	}
}

void AMPIC887ConsoleApplication::onReferenceMoveCommandIssued(const AMPIC887AxisCollection& axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		controllerCollection_.activeController()->referenceMove(axes);
	}
}

void AMPIC887ConsoleApplication::onAvailableParametersCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->availableParametersString());
	}
}

void AMPIC887ConsoleApplication::onCommandLevelCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
				AMGCS2Support::commandLevelToString(activeController->commandLevel()));
	}

}

void AMPIC887ConsoleApplication::onCurrentPositionCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->currentPositions(axes).toString("Current Position"));
	}
}

void AMPIC887ConsoleApplication::onCycleTimeCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		QString currentCycleTime = QString("Cycle Time: %1 ms")
				.arg(activeController->cycleTime());

		consoleInputHandler_->writeLineToStandardOutput(currentCycleTime);
	}
}

void AMPIC887ConsoleApplication::onDeviceIdentificationCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->identificationString());
	}
}

void AMPIC887ConsoleApplication::onLowSoftLimitCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->lowSoftLimits(axes).toString("Low Soft Limits"));
	}
}

void AMPIC887ConsoleApplication::onHighSoftLimitCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->highSoftLimits(axes).toString("High Soft Limits"));
	}
}

void AMPIC887ConsoleApplication::onSoftLimitStatusesCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->softLimitStatuses(axes).toString("Soft Limit Active"));
	}
}

void AMPIC887ConsoleApplication::onLimitSwitchStatusesCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardError(
					activeController->limitSwitchStatuses(axes).toString("Limit Switch Active"));
	}
}

void AMPIC887ConsoleApplication::onMinPositionCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->minCommandablePositions(axes).toString("Min Position"));
	}
}

void AMPIC887ConsoleApplication::onMaxPositionCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->maxCommandablePositions(axes).toString("Max Position"));
	}
}

void AMPIC887ConsoleApplication::onOnTargetCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->onTargetStates(axes).toString("On Target"));
	}
}

void AMPIC887ConsoleApplication::onPivotPointCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->pivotPoints(axes).toString("Pivot Points"));
	}
}

void AMPIC887ConsoleApplication::onTargetPositionCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->targetPositions(axes).toString("Target Position"));
	}
}

void AMPIC887ConsoleApplication::onPositionUnitsCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->positionUnits(axes).toString("Position Units"));
	}
}

void AMPIC887ConsoleApplication::onAvailableRecorderOptionsCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->recorderOptionsString());
	}
}

void AMPIC887ConsoleApplication::onRecordTriggerCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(QString("Record Trigger: %1").arg(
					AMGCS2Support::dataRecordTriggerToString(activeController->recordTrigger())));
	}
}

void AMPIC887ConsoleApplication::onRecordConfigCommandIssued(const QList<int> &tableIds)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		QString resultsString = "Table Id\tRecord Source\t\tRecord Option\n";
		QHash<int, AMPIC887DataRecorderConfiguration> recordConfigs =
				activeController->recordConfigs();

		if(!tableIds.isEmpty()) {
			foreach(int tableId, tableIds) {
				resultsString.append(QString("%1\t\t%2\n")
									 .arg(tableId)
									 .arg(recordConfigs.value(tableId).toString()));
			}
		} else {
			for(int tableId = 1, tableIdCount = 16;
				tableId <= tableIdCount;
				++tableId) {
				resultsString.append(QString("%1\t\t%2\n")
									 .arg(tableId)
									 .arg(recordConfigs.value(tableId).toString()));
			}
		}

		consoleInputHandler_->writeLineToStandardOutput(resultsString);
	}
}

void AMPIC887ConsoleApplication::onReferencedStateCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->axisReferencedStates(axes).toString("Is Referenced"));
	}
}

void AMPIC887ConsoleApplication::onServoModeStateCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		QString servoStateString = "Servo Mode: ";

		if(activeController->isInServoMode()) {
			servoStateString.append("Yes");
		} else {
			servoStateString.append(("No"));
		}

		consoleInputHandler_->writeLineToStandardOutput(servoStateString);
	}
}

void AMPIC887ConsoleApplication::onStepSizeCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					activeController->stepSizes(axes).toString("Step Sizes"));
	}
}

void AMPIC887ConsoleApplication::onSystemVelocityCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					QString("System Velocity: %1 mm/s").arg(
						activeController->systemVelocity()));
	}
}

void AMPIC887ConsoleApplication::onRecordRateCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		consoleInputHandler_->writeLineToStandardOutput(
					QString("Record Rate: %1 Hz").arg(activeController->recordRate()));
	}
}

void AMPIC887ConsoleApplication::onStopCommandIssued()
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->stop();
	}
}

void AMPIC887ConsoleApplication::onHaltCommandIssued(const AMPIC887AxisCollection &axes)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if (!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		activeController->haltSmoothly(axes);
	}
}

void AMPIC887ConsoleApplication::onDataRecordValuesIssues(int offset, int numberOfElements, int tableId)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {
		QList<double> dataValues = activeController->recordedData(offset, numberOfElements, tableId);

		QString output = "[";
		foreach (double value, dataValues) {

			output.append(QString("%1, ").arg(value));
		}

		output.append("]");

		consoleInputHandler_->writeLineToStandardOutput(output);
	}
}

void AMPIC887ConsoleApplication::onSetRecordRateCommandIssued(double recordRate)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();

	if(!activeController) {
		consoleInputHandler_->writeLineToStandardError("No active controller.");
	} else if(!activeController->isInValidState()) {
		consoleInputHandler_->writeLineToStandardError("Active controller not in a valid state. Reinitialize the controller with an init command..");
	} else {

		activeController->setRecordRate(recordRate);
	}

}

bool AMPIC887ConsoleApplication::startup()
{
	bool successfulLoad = true;

	successfulLoad &= startupControllers();
	return successfulLoad;
}

bool AMPIC887ConsoleApplication::startupControllers()
{
	AMPIC887Controller* mc1611_401 = new AMPIC887Controller("MC1611-401", "10.52.48.26");
	connect(mc1611_401, SIGNAL(output(QString)), this, SLOT(onControllerOutputReceived(QString)));
	connect(mc1611_401, SIGNAL(errorEncountered(QString)), this, SLOT(onControllerErrorEncountered(QString)));

	controllerCollection_.append(mc1611_401);

	return (controllerCollection_.activeController() != 0 &&
			controllerCollection_.activeController()->connectionEstablished());
}

void AMPIC887ConsoleApplication::makeConnections()
{
	connect(commandParser_, SIGNAL(help()), this, SLOT(onHelpCommandIssued()));
	connect(commandParser_, SIGNAL(quit()), this, SLOT(onQuitCommandIssued()));
	connect(commandParser_, SIGNAL(status()), this, SLOT(onStatusCommandIssued()));
	connect(commandParser_, SIGNAL(activeControllerChangeRequested(QString)), this, SLOT(onActiveControllerChangeRequested(QString)));
	connect(commandParser_, SIGNAL(moveCommandIssued(AMPIC887AxisMap<double>)), this, SLOT(onMoveCommandIssued(AMPIC887AxisMap<double>)));
	connect(commandParser_, SIGNAL(moveRelativeCommandIssued(AMPIC887AxisMap<double>)), this, SLOT(onMoveRelativeCommandIssued(AMPIC887AxisMap<double>)));
	connect(commandParser_, SIGNAL(motionStatusCommandIssued()), this, SLOT(onMotionStatusCommandIssued()));
	connect(commandParser_, SIGNAL(referenceMoveCommandIssued(AMPIC887AxisCollection)), this, SLOT(onReferenceMoveCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(availableParametersCommandIssued()), this, SLOT(onAvailableParametersCommandIssued()));
	connect(commandParser_, SIGNAL(commandLevelCommandIssued()), this, SLOT(onCommandLevelCommandIssued()));
	connect(commandParser_, SIGNAL(currentPositionCommandIssued(AMPIC887AxisCollection)), this, SLOT(onCurrentPositionCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(cycleTimeCommandIssued()), this, SLOT(onCycleTimeCommandIssued()));

	connect(commandParser_, SIGNAL(deviceIdentificationCommandIssued()), this, SLOT(onDeviceIdentificationCommandIssued()));
	connect(commandParser_, SIGNAL(lowSoftLimitCommandIssued(AMPIC887AxisCollection)), this, SLOT(onLowSoftLimitCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(highSoftLimitCommandIssued(AMPIC887AxisCollection)), this, SLOT(onHighSoftLimitCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(softLimitStatusCommandIssued(AMPIC887AxisCollection)), this, SLOT(onSoftLimitStatusesCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(limitSwitchStatusCommandIssued(AMPIC887AxisCollection)), this, SLOT(onLimitSwitchStatusesCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(minPositionCommandIssued(AMPIC887AxisCollection)), this, SLOT(onMinPositionCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(maxPositionCommandIssued(AMPIC887AxisCollection)), this, SLOT(onMaxPositionCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(onTargetCommandIssued(AMPIC887AxisCollection)), this, SLOT(onOnTargetCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(pivotPointCommandIssued(AMPIC887AxisCollection)), this, SLOT(onPivotPointCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(targetPositionCommandIssued(AMPIC887AxisCollection)), this, SLOT(onTargetPositionCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(positionUnitsCommandIssued(AMPIC887AxisCollection)), this, SLOT(onPositionUnitsCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(availableRecorderOptionsCommandIssued()), this, SLOT(onAvailableRecorderOptionsCommandIssued()));
	connect(commandParser_, SIGNAL(recordTriggerCommandIssued()), this, SLOT(onRecordTriggerCommandIssued()));
	connect(commandParser_, SIGNAL(referencedStateCommandIssued(AMPIC887AxisCollection)), this, SLOT(onReferencedStateCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(servoModeCommandIssued()), this, SLOT(onServoModeStateCommandIssued()));
	connect(commandParser_, SIGNAL(stepSizeCommandIssued(AMPIC887AxisCollection)), this, SLOT(onStepSizeCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(systemVelocityCommandIssued()), this, SLOT(onSystemVelocityCommandIssued()));
	connect(commandParser_, SIGNAL(recordRateCommandIssued()), this, SLOT(onRecordRateCommandIssued()));

	connect(commandParser_, SIGNAL(stopCommandIssued()), this, SLOT(onStopCommandIssued()));
	connect(commandParser_, SIGNAL(haltCommandIssued(AMPIC887AxisCollection)), this, SLOT(onHaltCommandIssued(AMPIC887AxisCollection)));

	connect(commandParser_, SIGNAL(dataRecorderValuesCommandIssued(int,int,int)), this, SLOT(onDataRecordValuesIssues(int,int,int)));

	connect(commandParser_, SIGNAL(setCommandLevelCommandIssued(AMGCS2::ControllerCommandLevel,QString)),
			this, SLOT(onSetCommandLevelCommandIssued(AMGCS2::ControllerCommandLevel,QString)));
	connect(commandParser_, SIGNAL(setCycleTimeCommandIssued(double)),
			this, SLOT(onSetCycleTimeCommandIssued(double)));
	connect(commandParser_, SIGNAL(setHighSoftLimitsCommandIssued(AMPIC887AxisMap<double>)),
			this, SLOT(onSetHighSoftLimitsCommandIssued(AMPIC887AxisMap<double>)));
	connect(commandParser_, SIGNAL(setLowSoftLimitsCommandIssued(AMPIC887AxisMap<double>)),
			this, SLOT(onSetLowSoftLimitsCommandIssued(AMPIC887AxisMap<double>)));
	connect(commandParser_, SIGNAL(setPivotPointsCommandIssued(AMPIC887AxisMap<double>)),
			this, SLOT(onSetPivotPointCommandIssued(AMPIC887AxisMap<double>)));
	connect(commandParser_, SIGNAL(setRecordTriggerCommandIssued(AMGCS2::DataRecordTrigger)),
			this, SLOT(onSetRecorderTriggerCommandIssued(AMGCS2::DataRecordTrigger)));
	connect(commandParser_, SIGNAL(setServoModeCommandIssued(bool)),
			this, SLOT(onSetServoModeCommandIssued(bool)));
	connect(commandParser_, SIGNAL(setSoftLimitStatesCommandIssued(AMPIC887AxisMap<bool>)),
			this, SLOT(onSetSoftLimitStatesCommandIssued(AMPIC887AxisMap<bool>)));
	connect(commandParser_, SIGNAL(setStepSizesCommandIssued(AMPIC887AxisMap<double>)),
			this, SLOT(onSetStepSizeCommandIssued(AMPIC887AxisMap<double>)));
	connect(commandParser_, SIGNAL(setSystemVelocityCommandIssued(double)),
			this, SLOT(onSetSystemVelocityCommandIssued(double)));
	connect(commandParser_, SIGNAL(setRecordRateCommandIssued(double)),
			this, SLOT(onSetRecordRateCommandIssued(double)));

	connect(commandParser_, SIGNAL(recordConfigCommandIssued(QList<int>)),
			this, SLOT(onRecordConfigCommandIssued(QList<int>)));

	connect(commandParser_,SIGNAL(setRecordConfigCommandIssued(QHash<int,AMPIC887DataRecorderConfiguration>)),
			this, SLOT(onSetRecordConfigCommandIssued(QHash<int,AMPIC887DataRecorderConfiguration>)));
}







