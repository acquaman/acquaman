#ifndef AMPIC887CONSOLECOMMANDPARSER_H
#define AMPIC887CONSOLECOMMANDPARSER_H

#include <QStringList>

#include "AMAbstractConsoleCommandParser.h"
#include "AMPIC887DataRecorderConfiguration.h"
#include "AMGCS2.h"
#include "AMPIC887AxisCollection.h"
#include "AMPIC887AxisMap.h"
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
	  * Signal indicating that a set command level command has been issued.
	  */
	void setCommandLevelCommandIssued(AMGCS2::ControllerCommandLevel commandLevel,
										const QString& password);

	/*!
	  * Signal indicating that a set cycle time command has been issued.
	  */
	void setCycleTimeCommandIssued(double cycleTime);

	/*!
	  * Signal indicating that a set hight soft limits command has been issued.
	  */
	void setHighSoftLimitsCommandIssued(const AMPIC887AxisMap<double>& highSoftLimits);

	/*!
	  * Signal indicating that a set low soft limits command has been issued.
	  */
	void setLowSoftLimitsCommandIssued(const AMPIC887AxisMap<double>& lowSoftLimits);

	/*!
	  * Signal indicating that a set pivot points command has been issued.
	  */
	void setPivotPointsCommandIssued(const AMPIC887AxisMap<double>& pivotPoints);

	/*!
	  * Signal indicating that a set record trigger command has been issued.
	  */
	void setRecordTriggerCommandIssued(AMGCS2::DataRecordTrigger recordTrigger);

	/*!
	  * Signa indicating that a set record rate command has been issued.
	  */
	void setRecordRateCommandIssued(double recordRate);

	/*!
	  * Signal indicating that a set record config command has been issued.
	  */
	void setRecordConfigCommandIssued(const QHash<int, AMPIC887DataRecorderConfiguration>& recordConfigs);

	/*!
	  * Signal indicating that a set servo mode command has been issued.
	  */
	void setServoModeCommandIssued(bool servoMode);

	/*!
	  * Signal indicating that a set soft limit states command has been issued.
	  */
	void setSoftLimitStatesCommandIssued(const AMPIC887AxisMap<bool>& softLimitStates);

	/*!
	  * Signal indicating that a set step sizes command has been issued.
	  */
	void setStepSizesCommandIssued(const AMPIC887AxisMap<double>& stepSizes);

	/*!
	  * Signal indicating thata a set system velocity command has been issued.
	  */
	void setSystemVelocityCommandIssued(double systemVelocity);

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
	  * Signal indicating that a get record config command has been issued.
	  */
	void recordConfigCommandIssued(const QList<int>& tableIds);

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
	  * Signal indicating that a get limit switch status command has been issued.
	  */
	void limitSwitchStatusCommandIssued(const AMPIC887AxisCollection& axes);

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
	  * Signal indicating that a get data record rate command has been issued.
	  */
	void recordRateCommandIssued();

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
	void moveCommandIssued(const AMPIC887AxisMap<double>& targetPositions);

	/*!
	  * Signal indicating that a move relative command has been issued.
	  */
	void moveRelativeCommandIssued(const AMPIC887AxisMap<double>& relativeTargetPositions);

	/*!
	  * Signal indicating that a reference move command has been issued.
	  */
	void referenceMoveCommandIssued(const AMPIC887AxisCollection& axes);

	/*!
	  * Signal indicating that a motion status command has been issued.
	  */
	void motionStatusCommandIssued();

	/*!
	  * Signal indicating that a command to initialize the state of the current
	  * controller has been issued.
	  */
	void initializeControllerCommandIssued();

	/*!
	  * Signal indicating that a command to obtain the recorded data vales from
	  * the controller's data recorder has been issued.
	  * \param offset ~ The starting index of the values to be retrieved from the
	  * data recorder.
	  * \param numberOfDataPoints ~ The number of values to be retrieved.
	  * \param tableId ~ The table from which to retrieve the values.
	  */
	void dataRecorderValuesCommandIssued(int offset, int numberOfDataPoints, int tableId);

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
	void activeControllerChangeRequested(const QString& controllerName);

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
	  * \param commandString ~ The command string issued.
	  * \param parseSuccess ~ Output parameter which indicates the success of the
	  * parse. The parse will only fail if it cannot convert a passed character
	  * to a double value where it is required.
	  */
	AMPIC887AxisMap<double> axisMapFromCommandString(const QString& commandString, bool* parseSuccess);

	/*!
	  * Helper method for converting a command string into a double value.
	  * \param commandString ~ The full command issued to the parser, whose
	  * arguments are to be parsed into a double value.
	  * \param parseSuccess ~ An optional out parameter which will be set to true
	  * if the parse succeeded, or false if it failed. If none is provided no
	  * parse success state will be set.
	  */
	double doubleValueFromCommandString(const QString& commandString, bool* parseSuccess = 0);

	/*!
	  * Helper method for converting a command string into a boolean value.
	  * \param commandString ~ The full command issued to the parser, whose
	  * arguments are to be parsed into a boolean value.
	  * \param parseSuccess ~ An optional out parameter which will be set to true
	  * if the parse succeeded, or false if it failed. If none is provided no
	  * parse success state will be set.
	  */
	bool boolValueFromCommandString(const QString& commandString, bool* parseSuccess = 0);

	/*!
	  * Helper method for parsing the command string passed along with the
	  * set command level command (CCL)
	  * \param commandString ~ The command string which was entered, and must be
	  * parsed.
	  */
	void handleSetCommandLevelInput(const QString& commandString);

	/*!
	  * Helper method for parsing the command string passed along with the set
	  * record trigger command (DRT)
	  * \param commandString ~ The command string which was entered, and must be
	  * parsed.
	  */
	void handleSetRecordTriggerInput(const QString& commandString);

	/*!
	  * Helper method for parsing the command string passed along with the set
	  * soft limit status command (SSL)
	  * \param commandString ~ The command string which was entered, and must be
	  * parsed.
	  */
	void handleSetSoftLimitStatusInput(const QString& commandString);

	/*!
	  * Helper method for parsing the command string passed along with the
	  * get record configuration command (DRC?)
	  * \param commandString ~ The command string which was entered, and must be
	  * parsed.
	  */
	void handleRecordConfigInput(const QString& commandString);

	/*!
	  * Helper methpod for parsing the command string passed along with the
	  * set record configuration command (DRC).
	  * \param commandString ~ The command string which was entered, and must be
	  * parsed.
	  */
	void handleSetRecordConfigInput(const QString& commandString);

	/*!
	  * Helper method for parsing the command string passed along with the get
	  * data record values command (DRR?)
	  * \param commandString ~ The command string which was entered, and must be
	  * parsed.
	  */
	void handleDataRecordValuesInput(const QString& commandString);
};

#endif // AMPIC887CONSOLECOMMANDSET_H
