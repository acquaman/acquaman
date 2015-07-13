#include "AMPIC887ConsoleCommandParser.h"
#include "GCS2Commands/AMGCS2Command.h"
#include "AMGCS2Support.h"
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
	commandList.append("DRR? <offset point> <number of data points> <record table id> : Queries the specified number of data points from the provided record table, starting at the offset point.\n");
	commandList.append("DRT <trigger source> : Sets the data record trigger source for all data record tables.\n");
	commandList.append("DRT? <record table id1> <record table id2> ... : Queries the current recording trigger sources for the provided data record tables.\n");
	commandList.append("FRF <axis1> <axis2> ... : Instructs the controller to perform a reference move on the provided axes. If none are provided reference moves will be performed on all axes.\n");
	commandList.append("FRF? <axis1> <axis2> ... : Queries whether the provided axes have been referenced. If no axes are provided the reference status of all axes will be queries.\n");
	commandList.append("HDR? : Queries the controller's recording options.\n");
	commandList.append("HLT <axis1> <axis2> ... : Halts the motions of the provided axes smoothly. If no axes are provided all axes are halted.\n");
	commandList.append("HPA? : Queries the controller's available parameters.\n");
	commandList.append("IDN? : Queries the controller's device identification.\n");
	commandList.append("LIM? <axis1> <axis2> ... : Queries whether limit switches are active for the provided axes. If none are provided all axes will be queried.\n");
	commandList.append("MOV <axis1> <position1> <axis2> <position2> ... : Moves the provided axes to the provided positions. At least one axes/position combination must be provided.\n");
	commandList.append("MOV? <axis1> <axis2> ... : Queries the current position which the provided axes are moving towards. If none are provided all axes will be queried.\n");
	commandList.append("MVR <axis1> <position1> <axis2> <position2> ... : Moves the provided axes to the provided relative positions. At least one axes/position combination must be provided.\n");
	commandList.append("MST? : Queries the movement statuses of the controller axes.\n");
	commandList.append("NLM <axis1> <lowlimit1> <axis2> <lowlimit2> ... : Sets the soft lower limit for the provided axes to the provided values.\n");
	commandList.append("NLM? <axis1> <axis2> ... : Queries the current soft lower limits of the provided axes. If none are provided the soft lower limits of all axes will be queried.\n");
	commandList.append("ONT? <axis1> <axis2> ... : Queries whether the provided axes reached their target positions. If no axes are provided the on target state of all axes will be queried.\n");
	commandList.append("PML <axis1> <highlimit1> <axis2> <highlimit2> ... : Sets the soft high limit for the provided axes to the provided values.\n");
	commandList.append("PLM? <axis1> <axis2> ... : Queries the current soft high limit for the provided axes. If none are provided the soft high limits of all axes will be queried.\n");
	commandList.append("POS? <axis1> <axis2> ... : Queries the current position of the provided axes. If none are provided all axes positions are displayed.\n");
	commandList.append("PUN? <axis1> <axis2> ... : Queries the position units of the provided axes. If none are provided all axes position units are queried.\n");
	commandList.append("RDY? : Queries the ready status of the controller.\n");
	commandList.append("SCT <value> : Sets the cycle time for running a defined motion profile.\n");
	commandList.append("SCT? : Queries the cycle time for running a defined motion profile.\n");
	commandList.append("SPI <axis1> <position1> <axis2> <position2> ... : Sets the pivot point of rotation to the values provided. Only X, Y and Z axes can be specified.\n");
	commandList.append("SPI? <axis1> <axis2> ... : Queries the current pivot point of rotation. Only X,Y and Z axes can be queried. If no axes are provided all valid axes are queried.\n");
	commandList.append("SSL <axis1> <true/false> <axis2> <true/false> ... : Sets the active status of the soft limits to the provided values (true = active, false = inactive).\n");
	commandList.append("SSL? <axis1> <axis2> ... : Queries the active status of the soft limits on the provided axes. If no axes are provided the soft limit statuses of all axes are provided.\n");
	commandList.append("SST <axis1> <stepSize1> <axis2> <stepSize2> ... : Sets the step size for the provided axes to the provided values.\n");
	commandList.append("SST? <axis1> <axis2> ... : Queries the step size for the provided axes. If no axes are provided the step sizes of all axes are queried.\n");
	commandList.append("STP : Performs an emergency stop of all axes.\n");
	commandList.append("SVO? <axis1> <axis2> ... : Queries the servo mode status of the provided axes. If none are provided the servo mode status of all axes is queried.\n");
	commandList.append("SVO <true/false> : Activates/deactivates servo mode across all axes of the controller (true = activate, false = desactivate).\n");
	commandList.append("TMN? <axis1> <axis2> ... : Queries the minimum commandable position of the provided axes. If none are provided the min commandable position of all axes is queried.\n");
	commandList.append("TMX? <axis1> <axis2> ... : Queries the maximum commandable position of the provided axes. If none are provided the max commandable position of all axes is queried.\n");
	commandList.append("VLS? : Queries the system velocity.\n");
	commandList.append("VLS <value> : Sets the system velocity to the provided value.\n");
	commandList.append("VMO? <axis1> <position1> <axis2> <position2> ... : Queries whether it is safe to apprach the provided axis positions.");
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
	} else if (command.startsWith("STP")) {
		emit stopCommandIssued();
	} else if(command.startsWith("HLT")) {
		emit haltCommandIssued(axesFromCommandString(command));
	} else if(command.startsWith("HPA?")) {
		emit availableParametersCommandIssued();
	} else if(command.startsWith("MST?")) {
		emit motionStatusCommandIssued();
	} else if (command.startsWith("CCL?")) {
		emit commandLevelCommandIssued();
	} else if (command.startsWith("POS?")) {
		emit currentPositionCommandIssued(axesFromCommandString(command));
	} else if (command.startsWith("SCT?")) {
		emit cycleTimeCommandIssued();
	} else if (command.startsWith("IDN?")) {
		emit deviceIdentificationCommandIssued();
	} else if (command.startsWith("NLM?")) {
		emit lowSoftLimitCommandIssued(axesFromCommandString(command));
	} else if (command.startsWith("PLM?")) {
		emit highSoftLimitCommandIssued(axesFromCommandString(command));
	} else if (command.startsWith("LIM?")) {
		emit softLimitStatusCommandIssued(axesFromCommandString(command));
	} else if (command.startsWith("TMN?")) {
		emit minPositionCommandIssued(axesFromCommandString(command));
	} else if (command.startsWith("TMX?")) {
		emit maxPositionCommandIssued(axesFromCommandString(command));
	} else if (command.startsWith("ONT?")) {
		emit onTargetCommandIssued(axesFromCommandString(command));
	} else if (command.startsWith("SPI?")) {
		emit pivotPointCommandIssued(axesFromCommandString(command, AMPIC887AxisCollection::LinearAxes));
	} else if (command.startsWith("MOV?")) {
		emit targetPositionCommandIssued(axesFromCommandString(command));
	} else if (command.startsWith("PUN?")) {
		emit positionUnitsCommandIssued(axesFromCommandString(command));
	} else if (command.startsWith("HDR?")) {
		emit availableRecorderOptionsCommandIssued();
	} else if (command.startsWith("DRT?")) {
		emit recordTriggerCommandIssued();
	} else if (command.startsWith("FRF?")) {
		emit referencedStateCommandIssued(axesFromCommandString(command));
	} else if (command.startsWith("SVO?")) {
		emit servoModeCommandIssued();
	} else if (command.startsWith("SST?")) {
		emit stepSizeCommandIssued(axesFromCommandString(command));
	} else if (command.startsWith("VLS?")) {
		emit systemVelocityCommandIssued();
	} else if(command.startsWith("MOV")){
		bool parseSuccess = false;
		QHash<AMGCS2::Axis, double> parsedArguments = axesDoublePairFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit moveCommandIssued(parsedArguments);
		}
	} else if(command.startsWith("FRF")) {
		emit referenceMoveCommandIssued(axesFromCommandString(command));
	} else if(command.startsWith("MVR")) {
		bool parseSuccess = false;
		QHash<AMGCS2::Axis, double> parsedArguments = axesDoublePairFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit moveRelativeCommandIssued(parsedArguments);
		}
	} else {

		emit otherCommandIssued(command);
	}

	if(command != "quit") {
		emit parseComplete();
	}
}

QStringList AMPIC887ConsoleCommandParser::commandArguments(const QString &commandString)
{
	return commandString.trimmed().split(" ").mid(1, -1);
}

AMPIC887AxisCollection AMPIC887ConsoleCommandParser::axesFromCommandString(const QString &axisArguments,
											  AMPIC887AxisCollection::InitializationState stateIfEmpty)
{

	QStringList axisArgumentList = commandArguments(axisArguments);

	if(axisArgumentList.isEmpty()) {
		return AMPIC887AxisCollection(stateIfEmpty);

	}

	AMPIC887AxisCollection axes(AMPIC887AxisCollection::EmptyCollection);

	for(int iAxis = 0, argCount = axisArgumentList.count();
		iAxis < argCount;
		++iAxis) {

		QString axisString = axisArgumentList.at(iAxis);

		if(axisString.length() != 1) {
			axes.append(AMGCS2::UnknownAxis);
		} else {
			axes.append(AMGCS2Support::characterToAxis(axisString.at(0)));
		}
	}

	return axes;
}

QHash<AMGCS2::Axis, double> AMPIC887ConsoleCommandParser::axesDoublePairFromCommandString(const QString &arguments, bool* parseSuccess)
{
	QStringList argumentList = commandArguments(arguments);

	QHash<AMGCS2::Axis, double> axisValueMap;
	int argumentCount = argumentList.count();

	// Ensure argument list isn't empty and has arguments in groups of two.
	if((argumentCount == 0) || (argumentCount % 2) != 0) {
		return axisValueMap;
	}

	// Axis values are provided in pairs, and are not valid unless both are
	// there. As such we iterate through two at a time.
	for(int iAxis = 0, iValue = 1;
		iAxis < argumentCount && iValue < argumentCount;
		iAxis += 2, iValue += 2) {

		QString axisString = argumentList.at(iAxis);
		QString valueString = argumentList.at(iValue);

		if(axisString.length() != 1) {
			axisValueMap.clear();
			return axisValueMap;
		}

		AMGCS2::Axis axis = AMGCS2Support::characterToAxis(axisString.at(0));

		bool localParseSuccess = false;
		double parsedValue = valueString.toDouble(&localParseSuccess);

		if(!localParseSuccess) {
			// If parse fails we have to return an empty map
			if(parseSuccess) {
				(*parseSuccess) = false;
			}
			axisValueMap.clear();
			return axisValueMap;
		}

		axisValueMap.insert(axis, parsedValue);
	}

	if(parseSuccess) {
		(*parseSuccess) = true;
	}
	return axisValueMap;
}
