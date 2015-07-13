#ifndef AMPIC887CONSOLEAPPLICATION_H
#define AMPIC887CONSOLEAPPLICATION_H

#include <QCoreApplication>

#include "AMPIC887Controller.h"
#include "AMPIC887ControllerCollection.h"
#include "AMPIC887AxisCollection.h"

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
	  * Handles signals indicating that the active controller command has been issued.
	  * Sets the active controller to the first located controller in the collection
	  * which matches the passed name.
	  * \param controllerName ~ The name of the controller to be made active.
	  */
	void onActiveControllerChangeIssued(const QString& controllerName);

	/*!
	  * Handles signals indicating that a command which does not form part of the
	  * usual command parsers list (ie. help, quit, activate etc). In our case
	  * this needs to be passed along to the active controller to see if it is
	  * a GCS2 command
	  * \param commandString ~ The text input to the console which is to be
	  * interpreted by a controller.
	  */
	void onOtherCommandIssued(const QString& commandString);

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
	  * Handles signals indicating that a move command has been issued.
	  */
	void onMoveCommandIssued(const QHash<AMGCS2::Axis, double>& axisPositions);

	/*!
	  * Handles signals indicating that a move relative command has been issued.
	  */
	void onMoveRelativeCommandIssued(const QHash<AMGCS2::Axis, double>& relativePositions);

	/*!
	  * Handles signals indicating that a motion status command has been issued.
	  */
	void onMotionStatusCommandIssued();

	/*!
	  * Handles signals indicating that a reference moce command has been issued.
	  */
	void onReferenceMoveCommandIssued(const QList<AMGCS2::Axis>& axes);

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
	void onLowSoftLimitCommandIssued(const QList<AMGCS2::Axis>& axes);

	/*!
	  * Handles signals indicating that a get high soft limit command has been issued.
	  */
	void onHighSoftLimitCommandIssued(const QList<AMGCS2::Axis>& axes);

	/*!
	  * Handles signals indicating that a get soft limit statuses command has been
	  * issued.
	  */
	void onSoftLimitStatusesCommandIssued(const QList<AMGCS2::Axis>& axes);

	/*!
	  * Handles signals indicating that a get min position command has been issued.
	  */
	void onMinPositionCommandIssued(const QList<AMGCS2::Axis>& axes);

	/*!
	  * Handles signals indicating that a get max position command has been issued.
	  */
	void onMaxPositionCommandIssued(const QList<AMGCS2::Axis>& axes);

	/*!
	  * Handles signals indicating that a get on target state command has been
	  * issued.
	  */
	void onOnTargetCommandIssued(const QList<AMGCS2::Axis>& axes);

	/*!
	  * Handles signals indicating that a get pivot point command has been issued.
	  */
	void onPivotPointCommandIssued(const QList<AMGCS2::Axis>& axes);

	/*!
	  * Handles signals indicating that a get target position command has been
	  * issued.
	  */
	void onTargetPositionCommandIssued(const QList<AMGCS2::Axis>& axes);

	/*!
	  * Handles signals indicating that a get position units command has been
	  * issued.
	  */
	void onPositionUnitsCommandIssued(const QList<AMGCS2::Axis>& axes);

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
	  * Handles signals indicating that a get referenced state command has been
	  * issued.
	  */
	void onReferencedStateCommandIssued(const QList<AMGCS2::Axis>& axes);

	/*!
	  * Handles signals indicating that a get servo mode state command has been
	  * issued.
	  */
	void onServoModeStateCommandIssued();

	/*!
	  * Handles signals indicating that a get step size command has been issued.
	  */
	void onStepSizeCommandIssued(const QList<AMGCS2::Axis>& axes);

	/*!
	  * Handles signals indicating that a get system velocity command has been issued.
	  */
	void onSystemVelocityCommandIssued();

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
};

#endif // AMPIC887CONSOLEAPPLICATION_H
