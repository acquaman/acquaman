#include "AMPIC887ConsoleCommandParser.h"

#include "AMGCS2CommandFactory.h"
#include "AMGCS2Command.h"
AMPIC887ConsoleCommandParser::AMPIC887ConsoleCommandParser(QObject *parent) :
	AMAbstractConsoleCommandParser(parent)
{
}

QString AMPIC887ConsoleCommandParser::commandList()
{
	QString commandList;
	commandList.append("\nServer commands:\n");
	commandList.append("help : Gets a list of available commands\n");
	commandList.append("quit : Exits the system\n");
	commandList.append("status: Gets the status of the current connected controllers\n");
	commandList.append("activate <controller_name> : Sets the current active controller.\n");

	commandList.append("\nDriver GCS2 Commands:\n");
	commandList.append("MOV <axis1> <position1> <axis2> <position2> ... : Moves the provided axes to the provided positions. At least one axes/position combination must be provided.\n");

	return commandList;
}

void AMPIC887ConsoleCommandParser::interpretCommandImplementation(const QString &command)
{
	if(command == "quit") {
		emit quit();
	} else if (command == "help") {
		emit help();
	} else if( command == "status") {
		emit status();
	} else if(command.startsWith("activate")) {
		QStringList commandParts = command.split(" ");
		if(commandParts.count() == 1) {
			emit unknownCommand(QString("Cannot active a controller with no name specified."));
		} else if(commandParts.count() > 2) {
			emit unknownCommand(QString("Cannot active more than one controller at a time."));
		} else {
			emit changeActiveController(commandParts.at(1));
		}
	} else {

		AMGCS2Command* gcs2Command = AMGCS2CommandFactory::buildCommand(command);

		if(gcs2Command) {
			emit GCS2CommandIssued(gcs2Command);
		} else {
			emit unknownCommand(QString("Command not recognized. Type 'help' for a list of commands."));
		}
	}

	if(command != "quit") {
		emit parseComplete();
	}
}
