#ifndef AMGCS2COMMANDFACTORY_H
#define AMGCS2COMMANDFACTORY_H

#include <QString>

#include "AMGCS2Command.h"
class AMGCS2CommandFactory
{
public:
	static AMGCS2Command* buildCommand(const QString& commandString);
protected:
	AMGCS2CommandFactory() {}
};

#endif // AMGCS2COMMANDFACTORY_H
