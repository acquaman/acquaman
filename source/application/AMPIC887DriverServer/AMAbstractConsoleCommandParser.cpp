#include "AMAbstractConsoleCommandParser.h"

AMAbstractConsoleCommandParser::AMAbstractConsoleCommandParser(QObject *parent) :
	QObject(parent)
{

}

void AMAbstractConsoleCommandParser::interpretCommand(const QString &command)
{
	interpretCommandImplementation(command);
}
