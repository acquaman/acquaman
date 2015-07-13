#ifndef AMPIC887CONSOLECOMMANDPARSER_H
#define AMPIC887CONSOLECOMMANDPARSER_H

#include <QStringList>

#include "AMAbstractConsoleCommandParser.h"
#include "AMGCS2.h"
#include "AMPIC887AxisCollection.h"
#include <QHash>
/*!
  * A class which represents the commands which a user can input to the Hexapod
  * C887 Controller driver server system.
  */
class AMGCS2Command;
class AMPIC887ConsoleCommandParser : public AMAbstractConsoleCommandParser
{
	Q_OBJECT
public:
	/*!
	  * Creates a new instance of hexapod driver console command set.
	  */
	AMPIC887ConsoleCommandParser(QObject* parent = 0);

	/*!
	  * Standard virtual destructor for a hexapod driver console command set.
	  */
	virtual ~AMPIC887ConsoleCommandParser() {}

	/*!
	  * Helper method which lists the current available commands.
	  */
	QString commandList();
signals:
	/*!
	  * Signal indicating that a stop command has been issued.
	  */
	void stopCommandIssued();

	/*!
	  * Signal indicating that a halt motions smoothly command has been issued.
	  */
	void haltCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get available parameters command has been issued.
	  */
	void availableParametersCommandIssued();

	/*!
	  * Signal indicating that a get command level command has been issued.
	  */
	void commandLevelCommandIssued();

	/*!
	  * Signal indicating that a get current position command has been issued.
	  */
	void currentPositionCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get cycle time command has been issued.
	  */
	void cycleTimeCommandIssued();

	/*!
	  * Signal indicating that a get record option command has been issued.
	  */
	void recordOptionCommandIssued(const QList<int>& tableIds);

	/*!
	  * Signal indicating that a get record source command has been issued.
	  */
	void recordSourceCommandIssued(const QList<int>& tableIds);

	/*!
	  * Signal indicating that a get device identification command has been issued.
	  */
	void deviceIdentificationCommandIssued();

	/*!
	  * Signal indicating that a get low soft limit command has been issued.
	  */
	void lowSoftLimitCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get high soft limit command has been issued.
	  */
	void highSoftLimitCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get soft limit statuses command has been issued.
	  */
	void softLimitStatusCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get min position command has been issued.
	  */
	void minPositionCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get max position command has been issued.
	  */
	void maxPositionCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get on target states command has been issued.
	  */
	void onTargetCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get pivot point command has been issued.
	  */
	void pivotPointCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get target position command has been issued.
	  */
	void targetPositionCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get position units command has been issued.
	  */
	void positionUnitsCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get available recorder options command has been
	  * issued.
	  */
	void availableRecorderOptionsCommandIssued();

	/*!
	  * Signal indicating that a get record trigger command has been issued.
	  */
	void recordTriggerCommandIssued();

	/*!
	  * Signal indicating that a get referenced state command has been issued.
	  */
	void referencedStateCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get servo mode status command has been issued.
	  */
	void servoModeCommandIssued();

	/*!
	  * Signal indicating that a get step size command has been issued.
	  */
	void stepSizeCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a get system velocity command has been issued.
	  */
	void systemVelocityCommandIssued();

	/*!
	  * Signal indicating that a move command has been issued
	  */
	void moveCommandIssued(const QHash<AMGCS2::Axis, double>& targetPositions);

	/*!
	  * Signal indicating that a move relative command has been issued.
	  */
	void moveRelativeCommandIssued(const QHash<AMGCS2::Axis, double>& relativeTargetPositions);

	/*!
	  * Signal indicating that a reference move command has been issued.
	  */
	void referenceMoveCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a motion status command has been issued.
	  */
	void motionStatusCommandIssued();

	/*!
	  * Signals that the quit command has been issued.
	  */
	void quit();

	/*!
	  * Signals that the help command has been issued.
	  */
	void help();

	/*!
	  * Signals that the staus command has been issued.
	  */
	void status();

	/*!
	  * Signals that the command to switch to a different active controller has
	  * been issued.
	  * \param controllerName ~ The name of the controller to make the active
	  * controller.
	  */
	void changeActiveController(const QString& controllerName);

	/*!
	  * Signals that a command other than the standard ones handled by the parser
	  * has been issued.
	  */
	void otherCommandIssued(const QString& commandString);
protected:
	/*!
	  * Interprets the provided command using the list of acceptable commands which
	  * can be issued to the hexapod driver console.
	  * \param command ~ The command which was issued to the hexapod driver console.
	  */
	virtual void interpretCommandImplementation(const QString &command);

	/*!
	  * Helper method for obtaining a list of passed argument from a command string.
	  * \param commandString ~ The string from which the arguments are to be
	  * retrieved.
	  */
	QStringList commandArguments(const QString& commandString);

	/*!
	  * Helper method for converting a command arguments in string format into a
	  * collection of Axes.
	  * \param axisArguments ~ The QString argument to be converted to a collection
	  * of axes.
	  * \param stateIfEmpty ~ An optional parameter which governs how the collection
	  * should be initialized if the command arguments are empty.
	  */
	AMPIC887AxisCollection axesFromCommandString(
			const QString &axisArguments,
			AMPIC887AxisCollection::InitializationState stateIfEmpty = AMPIC887AxisCollection::AllAxes);

	/*!
	  * Helper method for converting a command arguments into a mapping of axis
	  * to double value. Assumes that the arguments are provided as:
	  *    axis1 value1 axis2 value2 ... axisN valueN
	  * \param arguments ~ The QString argument to be converted to a mapping of
	  * axis to value.
	  * \param parseSuccess ~ Output parameter which indicates the success of the
	  * parse. The parse will only fail if it cannot convert a passed character
	  * to a double value where it is required.
	  */
	QHash<AMGCS2::Axis, double> axesDoublePairFromCommandString(const QString& arguments, bool* parseSuccess);
};

#endif // AMPIC887CONSOLECOMMANDSET_H
