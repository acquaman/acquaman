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

		makeConnections();
	} else {

		// Startup failed. Print an error message, then quit the application.
		consoleInputHandler_->writeLineToStandardError("Could not startup the hexapod driver application.");

		quit();
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

void AMPIC887ConsoleApplication::onActiveControllerChangeIssued(const QString &controllerName)
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

void AMPIC887ConsoleApplication::onOtherCommandIssued(const QString& commandString)
{
	AMPIC887Controller* activeController = controllerCollection_.activeController();
	if(!activeController) {
		return;
	}

	activeController->interpretAndRunCommand(commandString);
}

void AMPIC887ConsoleApplication::onControllerOutputReceived(const QString &outputString)
{
	consoleInputHandler_->writeLineToStandardOutput(outputString);
}

void AMPIC887ConsoleApplication::onControllerErrorEncountered(const QString &errorMessage)
{
	consoleInputHandler_->writeLineToStandardError(errorMessage);
}

void AMPIC887ConsoleApplication::onMoveCommandIssued(const QHash<AMGCS2::Axis, double> &axisPositions)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	controllerCollection_.activeController()->move(axisPositions);
}

void AMPIC887ConsoleApplication::onMoveRelativeCommandIssued(const QHash<AMGCS2::Axis, double> &relativePositions)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	controllerCollection_.activeController()->moveRelative(relativePositions);
}

void AMPIC887ConsoleApplication::onMotionStatusCommandIssued()
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	QFlags<AMGCS2::AxisMovementStatus> movementStatuses =
			controllerCollection_.activeController()->movementStatus();

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

void AMPIC887ConsoleApplication::onReferenceMoveCommandIssued(const QList<AMGCS2::Axis> &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	controllerCollection_.activeController()->referenceMove(axes);
}

void AMPIC887ConsoleApplication::onAvailableParametersCommandIssued()
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput(controllerCollection_.activeController()->availableParametersString());
}

void AMPIC887ConsoleApplication::onCommandLevelCommandIssued()
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput(
			AMGCS2Support::commandLevelToString(controllerCollection_.activeController()->commandLevel()));
}

void AMPIC887ConsoleApplication::onCurrentPositionCommandIssued(const AMPIC887AxisCollection &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tCurrent Position");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allAxes;
		allAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis
				<< AMGCS2::UAxis
				<< AMGCS2::VAxis
				<< AMGCS2::WAxis;

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->currentPosition(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->currentPosition(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}
	}
}

void AMPIC887ConsoleApplication::onCycleTimeCommandIssued()
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	QString currentCycleTime = QString("Cycle Time: %1 ms")
			.arg(controllerCollection_.activeController()->cycleTime());
	consoleInputHandler_->writeLineToStandardOutput(currentCycleTime);
}

void AMPIC887ConsoleApplication::onDeviceIdentificationCommandIssued()
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput(
				controllerCollection_.activeController()->identificationString());
}

void AMPIC887ConsoleApplication::onLowSoftLimitCommandIssued(const QList<AMGCS2::Axis> &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tLow Soft Limit");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allAxes;
		allAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis
				<< AMGCS2::UAxis
				<< AMGCS2::VAxis
				<< AMGCS2::WAxis;

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->lowSoftLimit(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->lowSoftLimit(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}
	}
}

void AMPIC887ConsoleApplication::onHighSoftLimitCommandIssued(const QList<AMGCS2::Axis> &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tHigh Soft Limit");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allAxes;
		allAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis
				<< AMGCS2::UAxis
				<< AMGCS2::VAxis
				<< AMGCS2::WAxis;

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->highSoftLimit(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->highSoftLimit(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}
	}
}

void AMPIC887ConsoleApplication::onSoftLimitStatusesCommandIssued(const QList<AMGCS2::Axis> &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tSoft Limits Active");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allAxes;
		allAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis
				<< AMGCS2::UAxis
				<< AMGCS2::VAxis
				<< AMGCS2::WAxis;

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			QString isActiveString;
			if(controllerCollection_.activeController()->softLimitStatus(currentAxis)) {
				isActiveString = "Yes";
			} else {
				isActiveString = "No";
			}

			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(isActiveString));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			QString isActiveString;
			if(controllerCollection_.activeController()->softLimitStatus(currentAxis)) {
				isActiveString = "Yes";
			} else {
				isActiveString = "No";
			}

			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(isActiveString));
		}
	}
}

void AMPIC887ConsoleApplication::onMinPositionCommandIssued(const QList<AMGCS2::Axis> &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tMin Position");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allAxes;
		allAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis
				<< AMGCS2::UAxis
				<< AMGCS2::VAxis
				<< AMGCS2::WAxis;

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->minCommandablePosition(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->minCommandablePosition(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}
	}
}

void AMPIC887ConsoleApplication::onMaxPositionCommandIssued(const QList<AMGCS2::Axis> &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tMax Position");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allAxes;
		allAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis
				<< AMGCS2::UAxis
				<< AMGCS2::VAxis
				<< AMGCS2::WAxis;

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->maxCommandablePosition(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->maxCommandablePosition(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}
	}
}

void AMPIC887ConsoleApplication::onOnTargetCommandIssued(const QList<AMGCS2::Axis> &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tOn Target");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allAxes;
		allAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis
				<< AMGCS2::UAxis
				<< AMGCS2::VAxis
				<< AMGCS2::WAxis;

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			QString onTargetString;
			if(controllerCollection_.activeController()->onTargetState(currentAxis)) {
				onTargetString = "Yes";
			} else {
				onTargetString = "No";
			}

			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(onTargetString));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			QString onTargetString;
			if(controllerCollection_.activeController()->onTargetState(currentAxis)) {
				onTargetString = "Yes";
			} else {
				onTargetString = "No";
			}

			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(onTargetString));
		}
	}
}

void AMPIC887ConsoleApplication::onPivotPointCommandIssued(const QList<AMGCS2::Axis> &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tPivot Point");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allLinearAxes;
		allLinearAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis;

		foreach(AMGCS2::Axis currentAxis, allLinearAxes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->pivotPoint(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->pivotPoint(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}
	}
}

void AMPIC887ConsoleApplication::onTargetPositionCommandIssued(const QList<AMGCS2::Axis> &axes)
{

	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tTarget Position");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allAxes;
		allAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis
				<< AMGCS2::UAxis
				<< AMGCS2::VAxis
				<< AMGCS2::WAxis;

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->targetPosition(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->targetPosition(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}
	}
}

void AMPIC887ConsoleApplication::onPositionUnitsCommandIssued(const QList<AMGCS2::Axis> &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tPosition Units");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allAxes;
		allAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis
				<< AMGCS2::UAxis
				<< AMGCS2::VAxis
				<< AMGCS2::WAxis;

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}
	}
}

void AMPIC887ConsoleApplication::onAvailableRecorderOptionsCommandIssued()
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput(
				controllerCollection_.activeController()->recorderOptionsString());
}

void AMPIC887ConsoleApplication::onRecordTriggerCommandIssued()
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput(QString("Record Trigger: %1").arg(
				AMGCS2Support::dataRecordTriggerToString(controllerCollection_.activeController()->recordTrigger())));
}

void AMPIC887ConsoleApplication::onReferencedStateCommandIssued(const QList<AMGCS2::Axis> &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tReferenced");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allAxes;
		allAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis
				<< AMGCS2::UAxis
				<< AMGCS2::VAxis
				<< AMGCS2::WAxis;

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			QString referencedStateString;
			if(controllerCollection_.activeController()->isAxisReferenced(currentAxis)) {
				referencedStateString = "Yes";
			} else {
				referencedStateString = "No";
			}

			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(referencedStateString));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			QString referencedStateString;
			if(controllerCollection_.activeController()->isAxisReferenced(currentAxis)) {
				referencedStateString = "Yes";
			} else {
				referencedStateString = "No";
			}

			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(referencedStateString));
		}
	}
}

void AMPIC887ConsoleApplication::onServoModeStateCommandIssued()
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	QString servoStateString = "Servo Mode: ";

	if(controllerCollection_.activeController()->isInServoMode()) {
		servoStateString.append("Yes");
	} else {
		servoStateString.append(("No"));
	}

	consoleInputHandler_->writeLineToStandardOutput(servoStateString);
}

void AMPIC887ConsoleApplication::onStepSizeCommandIssued(const QList<AMGCS2::Axis> &axes)
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput("Axis\t\tStep Size");
	if(axes.isEmpty()) {
		QList<AMGCS2::Axis> allAxes;
		allAxes << AMGCS2::XAxis
				<< AMGCS2::YAxis
				<< AMGCS2::ZAxis
				<< AMGCS2::UAxis
				<< AMGCS2::VAxis
				<< AMGCS2::WAxis;

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->stepSize(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}

	} else {

		foreach(AMGCS2::Axis currentAxis, axes) {
			consoleInputHandler_->writeLineToStandardOutput(
						QString("%1\t\t%2 %3")
						.arg(AMGCS2Support::axisToCharacter(currentAxis))
						.arg(controllerCollection_.activeController()->stepSize(currentAxis))
						.arg(AMGCS2Support::positionUnitsToString(controllerCollection_.activeController()->positionUnits(currentAxis))));
		}
	}
}

void AMPIC887ConsoleApplication::onSystemVelocityCommandIssued()
{
	if(!controllerCollection_.activeController() && !controllerCollection_.activeController()->isInitialized()) {
		return;
	}

	consoleInputHandler_->writeLineToStandardOutput(
				QString("System Velocity: %1 mm/s").arg(
					controllerCollection_.activeController()->systemVelocity()));

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

	return (controllerCollection_.activeController() != 0);
}

void AMPIC887ConsoleApplication::makeConnections()
{
	connect(commandParser_, SIGNAL(help()), this, SLOT(onHelpCommandIssued()));
	connect(commandParser_, SIGNAL(quit()), this, SLOT(onQuitCommandIssued()));
	connect(commandParser_, SIGNAL(status()), this, SLOT(onStatusCommandIssued()));
	connect(commandParser_, SIGNAL(changeActiveController(QString)), this, SLOT(onActiveControllerChangeIssued(QString)));
	connect(commandParser_, SIGNAL(otherCommandIssued(QString)), this, SLOT(onOtherCommandIssued(QString)));
	connect(commandParser_, SIGNAL(moveCommandIssued(QHash<AMGCS2::Axis,double>)), this, SLOT(onMoveCommandIssued(QHash<AMGCS2::Axis,double>)));
	connect(commandParser_, SIGNAL(moveRelativeCommandIssued(QHash<AMGCS2::Axis,double>)), this, SLOT(onMoveRelativeCommandIssued(QHash<AMGCS2::Axis,double>)));
	connect(commandParser_, SIGNAL(motionStatusCommandIssued()), this, SLOT(onMotionStatusCommandIssued()));
	connect(commandParser_, SIGNAL(referenceMoveCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onReferenceMoveCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(availableParametersCommandIssued()), this, SLOT(onAvailableParametersCommandIssued()));
	connect(commandParser_, SIGNAL(commandLevelCommandIssued()), this, SLOT(onCommandLevelCommandIssued()));
	connect(commandParser_, SIGNAL(currentPositionCommandIssued(AMPIC887AxisCollection)), this, SLOT(onCurrentPositionCommandIssued(AMPIC887AxisCollection)));
	connect(commandParser_, SIGNAL(cycleTimeCommandIssued()), this, SLOT(onCycleTimeCommandIssued()));

	connect(commandParser_, SIGNAL(deviceIdentificationCommandIssued()), this, SLOT(onDeviceIdentificationCommandIssued()));
	connect(commandParser_, SIGNAL(lowSoftLimitCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onLowSoftLimitCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(highSoftLimitCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onHighSoftLimitCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(softLimitStatusCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onSoftLimitStatusesCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(minPositionCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onMinPositionCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(maxPositionCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onMaxPositionCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(onTargetCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onOnTargetCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(pivotPointCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onPivotPointCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(targetPositionCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onTargetPositionCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(positionUnitsCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onPositionUnitsCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(availableRecorderOptionsCommandIssued()), this, SLOT(onAvailableRecorderOptionsCommandIssued()));
	connect(commandParser_, SIGNAL(recordTriggerCommandIssued()), this, SLOT(onRecordTriggerCommandIssued()));
	connect(commandParser_, SIGNAL(referencedStateCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onReferencedStateCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(servoModeCommandIssued()), this, SLOT(onServoModeStateCommandIssued()));
	connect(commandParser_, SIGNAL(stepSizeCommandIssued(QList<AMGCS2::Axis>)), this, SLOT(onStepSizeCommandIssued(QList<AMGCS2::Axis>)));
	connect(commandParser_, SIGNAL(systemVelocityCommandIssued()), this, SLOT(onSystemVelocityCommandIssued()));
}








