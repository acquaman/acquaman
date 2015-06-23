#ifndef AMGCS2COMMANDFACTORY_H
#define AMGCS2COMMANDFACTORY_H

#include <QString>

#include "AMGCS2Command.h"
#include "AMGCS2.h"
#include <QHash>
/*!
  * Factory class which constructs AMGCS2Commands from the provided input command
  * string.
  */
class AMGCS2CommandFactory
{
public:
	/*!
	  * Static method to build an AMGCS2Command from the provided commandString
	  * \param commandString ~ The input command text. Includes the command keyword
	  * as well as any arguments provided to it.
	  * \returns An AMGCS2Command initialized to run based on the provided command
	  * string, if the commandString can be parsed, or 0 otherwise.
	  */
	static AMGCS2Command* buildCommand(const QString& commandString);
protected:

	/*!
	  * Static class, so we protect the contstructor to prevent initialization.
	  */
	AMGCS2CommandFactory() {}

	/*!
	  * Helper method for obtaining a list of passed argument from a command string.
	  * \param commandString ~ The string from which the arguments are to be
	  * retrieved.
	  */
	static QStringList commandArguments(const QString& commandString);

	/*!
	  * Helper method for converting a command arguments in string format into a
	  * list of Axes.
	  * \param axisArguments ~ The QString argument to be converted to a list of
	  * axes.
	  */
	static QList<AMGCS2::Axis> axesFromCommandString(const QString& axisArguments);

	/*!
	  * Helper method for converting a command arguments into a mapping of axis
	  * to double value. Assumes that the arguments are provided as:
	  *    axis1 value1 axis2 value2 ... axisN valueN
	  * \param arguments ~ The QString argument to be converted to a mapping of
	  * axis to value.
	  */
	static QHash<AMGCS2::Axis, double> axesDoublePairFromCommandString(const QString& arguments);

	/*!
	  * Static helper method for parsing the set command level command string.
	  * \param argumentList ~ a list of the arguments provided to the command.
	  * \returns An initialized AMGCS2SetCommandLevelCommand if the arguments could
	  * be parsed, 0 otherwise.
	  */
	static AMGCS2Command* buildSetCommandLevelCommand(const QStringList& argumentList);

	/*!
	  * Static helper method for parsing the set data recorder confiuration command
	  * string arguments.
	  * \param argumentList ~ A list of the arguments provided to the command.
	  * \returns An initialized AMGCS2SetDataRecorderConfigurationCommand if
	  * the arguments could be parsed, 0 otherwise.
	  */
	static AMGCS2Command* buildSetDataRecorderConfigurationCommand(const QStringList& argumentList);

	/*!
	  * Static helper method for parsing the get number of recorded data points
	  * command string arguments.
	  * \param argumentList ~ A list of the arguments provided to the command.
	  * \returns An initialized AMGCS2GetNumberOfRecordedPointsCommand if the
	  * arguments could be parsed, 0 otherwise.
	  */
	static AMGCS2Command* buildGetNumberOfRecordedPointsCommand(const QStringList& argumentList);

	/*!
	  * Static helper method for parsing the get recorded data values command
	  * string arguments.
	  * \param argumentList ~ A list of the arguments provided to the command.
	  * \returns An initialized AMGCS2GetRecordedDataValuesCommand if the arguments
	  * could be parsed, 0 otherwise.
	  */
	static AMGCS2Command* buildGetRecordedDataValuesCommand(const QStringList& argumentList);

	/*!
	  * Static helper method for parsing the set data record trigger source command
	  * string arguments.
	  * \param argumentList ~ A list of the arguments provided to the command.
	  * \returns An initialized AMGCS2SetRecordTriggerSourceCommand if the arguments
	  * can be parsed, 0 otherwise.
	  */
	static AMGCS2Command* buildSetRecordTriggerSourceCommand(const QStringList& argumentList);

	/*!
	  * Static helper method for parsing the get data record trigger source command
	  * string arguments.
	  * \param argumentList ~ A list of the arguments provided to the command.
	  * \returns An initialized AMGCS2GetRecordTriggerSourceCommand if the arguments
	  * can be parsed, 0 otherwise.
	  */
	static AMGCS2Command* buildGetRecordTriggerSourceCommand(const QStringList& argumentList);

	/*!
	  * Static helper method for parsing the set cycle time command string arguments.
	  * \param argumentList ~ A list of the arguments provided to the command.
	  * \returns An initialized AMGCS2SetCycleTimeCommand if the arguments can be
	  * parsed, 0 otherwise.
	  */
	static AMGCS2Command* buildSetCycleTimeCommand(const QStringList& argumentList);

	/*!
	  * Static helper method for parsing the set soft limit statuses command string
	  * arguments.
	  * \param argumentList ~ A list of the arguments provided to the command.
	  * \returns An initialized AMGCS2SetSoftLimitStatusesCommand if the arguments
	  * can be parsed, 0 otherwise.
	  */
	static AMGCS2Command* buildSetSoftLimitStatusesCommand(const QStringList& argumentList);

	/*!
	  * Static helper method for parsing the set servo mode command string arguments.
	  * \param argumentList ~ A list of the arguments provided to the command.
	  * \returns An initialized AMGCS2SetServoModeCommand if the arguments can be
	  * parsed, 0 otherwise.
	  */
	static AMGCS2Command* buildSetServoModeCommand(const QStringList& argumentList);
};

#endif // AMGCS2COMMANDFACTORY_H
