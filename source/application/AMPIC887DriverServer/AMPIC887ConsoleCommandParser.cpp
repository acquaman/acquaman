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
	commandList.append("CCL? : Queries the controller's current command level\n");
	commandList.append("CCL <command level> <password> : Sets the controller's current command level. A password is only required for setting to the Advanced (1) command level. Command level's available are Normal (0) and Advanced (1).\n");
	commandList.append("CSV : Sets the controller's syntax version to GCS v2.0.\n");
	commandList.append("DRC? : Queries the controller's data record configuration.\n");
	commandList.append("DRC <record table id1> <source1> <option1> <record table id2> <source2> <option2> ... : Sets the record configuration for the provided record table.\n");
	commandList.append("DRL? <record table id1> <record table id2> ... : Queries the number of recorded data points acquired since the last recording was triggered for each of the provided record tables.\n");
	commandList.append("IDN? : Queries the controller's device identification.\n");
	commandList.append("MOV <axis1> <position1> <axis2> <position2> ... : Moves the provided axes to the provided positions. At least one axes/position combination must be provided.\n");
	commandList.append("MST? : Queries the movement statuses of the controller axes.\n");
	commandList.append("POS? <axis1> <axis2> ... : Queries the current position of the provided axes. If none are provided all axes positions are displayed.\n");
	commandList.append("RDY? : Queries the ready status of the controller.\n");
	commandList.append("STP : Performs an emergency stop of all axes.\n");

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
