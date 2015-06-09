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
};

#endif // AMGCS2COMMANDFACTORY_H
