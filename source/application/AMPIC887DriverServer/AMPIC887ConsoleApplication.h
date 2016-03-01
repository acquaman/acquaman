#ifndef AMPIC887CONSOLEAPPLICATION_H
#define AMPIC887CONSOLEAPPLICATION_H

#include <QCoreApplication>

#include "AMPIC887Controller.h"
#include "AMPIC887ControllerCollection.h"
#include "AMPIC887AxisCollection.h"
#include "AMPIC887EpicsCoordinator.h"

class AMConsoleInputHandler;
class AMPIC887ConsoleCommandParser;
/*!
  * A QCoreApplication derived class which represents the Qt Application entry
  * point for the driver server. Customized to initialize the controllers, the
  * input handler and the command set.
  */
class AMPIC887ConsoleApplication : public QCoreApplication
{
    Q_OBJECT
public:
	/*!
	  * Creates a new instance of an AMPIC887ConsoleApplication, with the provided
	  * command line arguments.
	  * \param argc ~ The number of command line arguments passed to the application.
	  * \param argv ~ A collection of the command line arguments passed to the
	  * application.
	  */
	explicit AMPIC887ConsoleApplication(int argc, char *argv[]);

signals:

public slots:
protected slots:
	/*!
	  * Handles signals indicating that the help command has been issued. Writes
	  * the command parsers list of commands to the console.
	  */
	void onHelpCommandIssued();

	/*!
	  * Handles signals indicating that the quit command has been issued. Prints
	  * an exit message then closes the application.
	  */
	void onQuitCommandIssued();

	/*!
	  * Handles signals indicating that the status command has been issued. Prints
	  * the controller collections current status.
	  */
	void onStatusCommandIssued();

	/*!
	  * Handles signals indicating that an initialize active controller command
	  * has been issued.
	  */
	void onInitControllerCommandIssued();

	/*!
	  * Handles signals indicating that a request to change the active controller
	  * has been issued. Attempts to locate a controller with the provided name,
	  * and makes it the active controller if one is found.
	  * \param controllerName ~ The name of the controller to be made active.
	  */
	void onActiveControllerChangeRequested(const QString& controllerName);

	/*!
	  * Handles signals indicating that one of the controllers in the application
	  * has some text it wishes to output to the console terminal.
	  * \param outputString ~ The text that the controller wishes to output.
	  */
	void onControllerOutputReceived(const QString& outputString);

	/*!
	  * Handles signals indicating that one of the controller in the application
	  * has encountered an error.
	  * \param errorMessage ~ An message describing the error.
	  */
	void onControllerErrorEncountered(const QString& errorMessage);

	/*!
	  * Handles signals indicating that a set command level command has been issued.
	  */
	void onSetCommandLevelCommandIssued(AMGCS2::ControllerCommandLevel commandLevel,
										const QString& password);

	/*!
	  * Handles signals indicating that a set cycle time command has been issued.
	  */
	void onSetCycleTimeCommandIssued(double cycleTime);

	/*!
	  * Handles signals indicating that a set high soft limits command has been
	  * issued.
	  */
	void onSetHighSoftLimitsCommandIssued(const AMPIC887AxisMap<double>& highSoftLimits);

	/*!
	  * Handles signals indicating that a set low soft limits command has been
	  * issued.
	  */
	void onSetLowSoftLimitsCommandIssued(const AMPIC887AxisMap<double>& lowSoftLimits);

	/*!
	  * Handles signals indicating that a set pivot point command has been issued.
	  */
	void onSetPivotPointCommandIssued(const AMPIC887AxisMap<double>& pivotPoints);

	/*!
	  * Handles signals indicating that a set record trigger command has been issued.
	  */
	void onSetRecorderTriggerCommandIssued(AMGCS2::DataRecordTrigger recorderTrigger);

	/*!
	  * Handles signals indicating that a set record config command has been issued.
	  */
	void onSetRecordConfigCommandIssued(const QHash<int, AMPIC887DataRecorderConfiguration>& recordConfigs);

	/*!
	  * Handles signals indicating that a set servo mode command has been issued.
	  */
	void onSetServoModeCommandIssued(bool servoMode);

	/*!
	  * Handles signals indicating that a set soft limit states command has been
	  * issued.
	  */
	void onSetSoftLimitStatesCommandIssued(const AMPIC887AxisMap<bool>& softLimitStates);

	/*!
	  * Handles signals indicating that a set step size command has been issued.
	  */
	void onSetStepSizeCommandIssued(const AMPIC887AxisMap<double>& stepSizes);

	/*!
	  * Handles signals indicating that a set system velocity command has been
	  * issued.
	  */
	void onSetSystemVelocityCommandIssued(double systemVelocity);

	/*!
	  * Handles signals indicating that a move command has been issued.
	  */
	void onMoveCommandIssued(const AMPIC887AxisMap<double>& axisPositions);

	/*!
	  * Handles signals indicating that a move relative command has been issued.
	  */
	void onMoveRelativeCommandIssued(const AMPIC887AxisMap<double>& relativePositions);

	/*!
	  * Handles signals indicating that a motion status command has been issued.
	  */
	void onMotionStatusCommandIssued();

	/*!
	  * Handles signals indicating that a reference moce command has been issued.
	  */
	void onReferenceMoveCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get available parameters command has
	  * been issued.
	  */
	void onAvailableParametersCommandIssued();

	/*!
	  * Handles signals indicating that a get command level command has been
	  * issued.
	  */
	void onCommandLevelCommandIssued();

	/*!
	  * Handles signals indicating that a get current position command has been
	  * issued.
	  */
	void onCurrentPositionCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get cycle time command has been issued.
	  */
	void onCycleTimeCommandIssued();

	/*!
	  * Handles signals indicating that a get device identification command has
	  * been issued.
	  */
	void onDeviceIdentificationCommandIssued();

	/*!
	  * Handles signals indicating that a get low soft limit command has been issued.
	  */
	void onLowSoftLimitCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get high soft limit command has been issued.
	  */
	void onHighSoftLimitCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get soft limit statuses command has been
	  * issued.
	  */
	void onSoftLimitStatusesCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get limit switch statuses command has
	  * been issued.
	  */
	void onLimitSwitchStatusesCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get min position command has been issued.
	  */
	void onMinPositionCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get max position command has been issued.
	  */
	void onMaxPositionCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get on target state command has been
	  * issued.
	  */
	void onOnTargetCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get pivot point command has been issued.
	  */
	void onPivotPointCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get target position command has been
	  * issued.
	  */
	void onTargetPositionCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get position units command has been
	  * issued.
	  */
	void onPositionUnitsCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get available recorder options command
	  * has been issued.
	  */
	void onAvailableRecorderOptionsCommandIssued();

	/*!
	  * Handles signals indicating that a get record trigger command has been issued.
	  */
	void onRecordTriggerCommandIssued();

	/*!
	  * Handles signals indicating that a get record config command has been issued.
	  */
	void onRecordConfigCommandIssued(const QList<int>& tableIds);

	/*!
	  * Handles signals indicating that a get referenced state command has been
	  * issued.
	  */
	void onReferencedStateCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get servo mode state command has been
	  * issued.
	  */
	void onServoModeStateCommandIssued();

	/*!
	  * Handles signals indicating that a get step size command has been issued.
	  */
	void onStepSizeCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a get system velocity command has been issued.
	  */
	void onSystemVelocityCommandIssued();

	/*!
	  * Handles signals indicating that a get record rate command has been issued.
	  */
	void onRecordRateCommandIssued();

	/*!
	  * Handles signals indicating that a stop command has been issued.
	  */
	void onStopCommandIssued();

	/*!
	  * Handles signals indicating that a halt command has been issued.
	  */
	void onHaltCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Handles signals indicating that a data record values command has been
	  * issued.
	  */
	void onDataRecordValuesIssues(int offset, int numberOfElements, int tableId);

	/*!
	  * Handles signals indicating that a set record rate command has been issued.
	  */
	void onSetRecordRateCommandIssued(double recordRate);
protected:
	/*!
	  * Runs the initialization procedure for the application.
	  * \returns True if startup was successful, false otherwise.
	  */
	bool startup();

	/*!
	  * Initializes all the controllers which the application is to connect to.
	  * \returns True if at least one controller could be connected to, false otherwise.
	  */
	bool startupControllers();

	/*!
	  * Connects signals from the command parser to this class' slots.
	  */
	void makeConnections();

	AMConsoleInputHandler* consoleInputHandler_;
	AMPIC887ConsoleCommandParser* commandParser_;
	AMPIC887ControllerCollection controllerCollection_;
	AMPIC887EpicsCoordinator* controllerCoordinator_;
};

#endif // AMPIC887CONSOLEAPPLICATION_H
