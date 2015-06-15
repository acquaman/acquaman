#ifndef AMGCS2COMMANDFACTORY_H
#define AMGCS2COMMANDFACTORY_H

#include <QString>

#include "AMGCS2Command.h"
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
	  * Static helper method for parsing move command strings.
	  * \param argumentList ~ A list of the arguments provided to the command.
	  * \returns An initialized AMGCS2MoveCommand if the arguments could be parsed,
	  * 0 otherwise.
	  */
	static AMGCS2Command* buildMoveCommand(const QStringList& argumentList);

	/*!
	  * Static helper method for parsing position query command strings.
	  * \param argumentList ~ A list of the arguments provided to the command.
	  * \returns An initialized AMGCS2CurrentPositionCommand if the arguments could
	  * be parsed, 0 otherwise.
	  */
	static AMGCS2Command* buildCurrentPositionCommand(const QStringList& argumentList);

	/*!
	  * Static helper method for parsing halt smoothly command string.
	  * \param argumentList ~ A list of the arguments provided to the command.
	  * \returns An initialized AMGCS2HaltSmoothlyCommand if the arguments could
	  * be parsed, 0 otherwise.
	  */
	static AMGCS2Command* buildHaltSmoothlyCommand(const QStringList& argumentList);

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
};

#endif // AMGCS2COMMANDFACTORY_H
