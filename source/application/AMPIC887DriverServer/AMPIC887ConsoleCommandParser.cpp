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
	commandList.append("init : Initializes data for the current active controller.\n");

	commandList.append("\nDriver GCS2 Commands:\n");
	commandList.append("CCL? : Queries the controller's current command level\n");
	commandList.append("CCL <command level> <password> : Sets the controller's current command level. A password is only required for setting to the Advanced (1) command level. Command level's available are Normal (0) and Advanced (1).\n");
	commandList.append("CSV : Sets the controller's syntax version to GCS v2.0.\n");
	commandList.append("DRC? : Queries the controller's data record configuration.\n");
	commandList.append("DRC <record table id1> <source1> <option1> <record table id2> <source2> <option2> ... : Sets the record configuration for the provided record table.\n");
	commandList.append("DRL? <record table id1> <record table id2> ... : Queries the number of recorded data points acquired since the last recording was triggered for each of the provided record tables.\n");
	commandList.append("DRR? <offset point> <number of data points> <record table id> : Queries the specified number of data points from the provided record table, starting at the offset point.\n");
	commandList.append("DRT <trigger> : Sets the recording trigger for the controller.\n");
	commandList.append("DRT? : Queries the current recording trigger for the controller.\n");
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
	commandList.append("RTR <integer> : Sets the record rate for the controller's data recorder.\n");
	commandList.append("RTR? : Queries the record rate for the controller's data recorder.\n");
	commandList.append("SCT <value> : Sets the cycle time for running a defined motion profile.\n");
	commandList.append("SCT? : Queries the cycle time for running a defined motion profile.\n");
	commandList.append("SPI <axis1> <position1> <axis2> <position2> ... : Sets the pivot point of rotation to the values provided. Only X, Y and Z axes can be specified.\n");
	commandList.append("SPI? <axis1> <axis2> ... : Queries the current pivot point of rotation. Only X,Y and Z axes can be queried. If no axes are provided all valid axes are queried.\n");
	commandList.append("SSL <axis1> <true/false> <axis2> <true/false> ... : Sets the active status of the soft limits to the provided values (true = active, false = inactive).\n");
	commandList.append("SSL? <axis1> <axis2> ... : Queries the active status of the soft limits on the provided axes. If no axes are provided the soft limit statuses of all axes are provided.\n");
	commandList.append("SST <axis1> <stepSize1> <axis2> <stepSize2> ... : Sets the step size for the provided axes to the provided values.\n");
	commandList.append("SST? <axis1> <axis2> ... : Queries the step size for the provided axes. If no axes are provided the step sizes of all axes are queried.\n");
	commandList.append("STP : Performs an emergency stop of all axes.\n");
	commandList.append("SVO? : Queries the servo mode status controller.\n");
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
	} else if(command.startsWith("init")) {

		emit initializeControllerCommandIssued();
	} else if(command.startsWith("activate")) {

		QStringList commandParts = command.split(" ");
		if(commandParts.count() == 1) {
			emit unknownCommand(QString("Cannot active a controller with no name specified."));
		} else if(commandParts.count() > 2) {
			emit unknownCommand(QString("Cannot active more than one controller at a time."));
		} else {
			emit activeControllerChangeRequested(commandParts.at(1));
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

		emit limitSwitchStatusCommandIssued(axesFromCommandString(command));

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

	} else if (command.startsWith("DRR?")) {

		handleDataRecordValuesInput(command);

	} else if (command.startsWith("DRC?")) {

		handleRecordConfigInput(command);

	} else if (command.startsWith("FRF?")) {

		emit referencedStateCommandIssued(axesFromCommandString(command));

	} else if (command.startsWith("RTR?")) {

		emit recordRateCommandIssued();

	} else if (command.startsWith("SVO?")) {

		emit servoModeCommandIssued();

	} else if(command.startsWith("SSL?")) {

		emit softLimitStatusCommandIssued(axesFromCommandString(command));

	} else if (command.startsWith("SST?")) {

		emit stepSizeCommandIssued(axesFromCommandString(command));

	} else if (command.startsWith("VLS?")) {

		emit systemVelocityCommandIssued();

	} else if(command.startsWith("MOV")){

		bool parseSuccess = false;
		AMPIC887AxisMap<double> parsedArguments = axisMapFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit moveCommandIssued(parsedArguments);
		}

	} else if(command.startsWith("FRF")) {

		emit referenceMoveCommandIssued(axesFromCommandString(command));

	} else if(command.startsWith("MVR")) {

		bool parseSuccess = false;
		AMPIC887AxisMap<double> parsedArguments = axisMapFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit moveRelativeCommandIssued(parsedArguments);
		}

	} else if(command.startsWith("CCL")) {

		handleSetCommandLevelInput(command);

	} else if (command.startsWith("SCT")) {

		bool parseSuccess = false;
		double parsedArgument = doubleValueFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit setCycleTimeCommandIssued(parsedArgument);
		}

	} else if (command.startsWith("PLM")) {

		bool parseSuccess = false;
		AMPIC887AxisMap<double> parsedArguments = axisMapFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit setHighSoftLimitsCommandIssued(parsedArguments);
		}

	} else if (command.startsWith("NLM")) {

		bool parseSuccess = false;
		AMPIC887AxisMap<double> parsedArguments = axisMapFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit setLowSoftLimitsCommandIssued(parsedArguments);
		}

	} else if (command.startsWith("SPI")) {

		bool parseSuccess = false;
		AMPIC887AxisMap<double> parsedArguments = axisMapFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit setPivotPointsCommandIssued(parsedArguments);
		}

	} else if (command.startsWith("DRT")) {

		handleSetRecordTriggerInput(command);

	} else if (command.startsWith("DRC")) {

		handleSetRecordConfigInput(command);

	} else if(command.startsWith("RTR")) {

		bool parseSuccess = false;
		double value = doubleValueFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit setRecordRateCommandIssued(value);
		}

	} else if (command.startsWith("SVO")) {

		bool parseSuccess = false;
		bool value = boolValueFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit setServoModeCommandIssued(value);
		}

	} else if (command.startsWith("SSL")) {

		handleSetSoftLimitStatusInput(command);

	} else if (command.startsWith("SST")) {

		bool parseSuccess = false;
		AMPIC887AxisMap<double> parsedArguments = axisMapFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit setStepSizesCommandIssued(parsedArguments);
		}

	} else if (command.startsWith("VLS")) {

		bool parseSuccess = false;
		double parsedArgument = doubleValueFromCommandString(command, &parseSuccess);
		if(parseSuccess) {
			emit setSystemVelocityCommandIssued(parsedArgument);
		}

	} else {
		emit unknownCommand("Command not recognized. Type 'help' to see a list of commands");
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

AMPIC887AxisMap<double> AMPIC887ConsoleCommandParser::axisMapFromCommandString(
		const QString &commandString, bool* parseSuccess)
{
	QStringList argumentList = commandArguments(commandString);

	AMPIC887AxisMap<double> axisValueMap;
	int argumentCount = argumentList.count();

	// Ensure argument list isn't empty and has arguments in groups of two.
	if((argumentCount == 0) || (argumentCount % 2) != 0) {
		if(parseSuccess) {
			(*parseSuccess) = false;
		}
		return axisValueMap;
	}

	// Axis values are provided in pairs, and are not valid unless both are
	// there. As such we iterate through two at a time.
	for(int iAxis = 0, iValue = 1;
		iAxis < argumentCount && iValue < argumentCount;
		iAxis += 2, iValue += 2) {

		QString axisString = argumentList.at(iAxis);
		QString valueString = argumentList.at(iValue);

		AMGCS2::Axis axis;
		if(axisString.length() == 1) {
			axis = AMGCS2Support::characterToAxis(axisString.at(0));
		} else {
			axis = AMGCS2::UnknownAxis;
		}

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

double AMPIC887ConsoleCommandParser::doubleValueFromCommandString(
		const QString &commandString, bool *parseSuccess)
{
	QStringList arguments = commandArguments(commandString);

	if(arguments.isEmpty()) {
		if(parseSuccess) {
			(*parseSuccess) = false;
		}
		return 0;
	}

	QString stringValue = arguments.at(0);
	double doubleValue = stringValue.toDouble(parseSuccess);

	return doubleValue;
}

bool AMPIC887ConsoleCommandParser::boolValueFromCommandString(const QString &commandString, bool *parseSuccess)
{
	QStringList arguments = commandArguments(commandString);

	if(arguments.isEmpty()) {
		if(parseSuccess) {
			(*parseSuccess) = false;
		}
		return false;
	}

	QString stringValue = arguments.at(0);
	if(stringValue == "0" ||
			stringValue.toLower() == "false") {

		if(parseSuccess) {
			(*parseSuccess) = true;
			return false;
		}

	} else if(stringValue == "1" ||
			  stringValue.toLower() == "true") {

		if(parseSuccess) {
			(*parseSuccess) = true;
			return true;
		}
	}


	if(parseSuccess) {
		(*parseSuccess) = false;
	}

	return false;
}

void AMPIC887ConsoleCommandParser::handleSetCommandLevelInput(const QString &commandString)
{
	QStringList argsList = commandArguments(commandString);

	QString commandLevelString;
	if(argsList.count() >= 1) {
		commandLevelString = argsList.at(0);
	}

	bool parseSuccess = false;
	int commandLevelCode = commandLevelString.toInt(&parseSuccess);
	if(!parseSuccess) {
		commandLevelCode = -1;
	}

	QString passwordString;
	if(argsList.count() >= 2) {
			passwordString = argsList.at(1);
	}

	emit setCommandLevelCommandIssued(AMGCS2Support::intCodeToCommandLevel(commandLevelCode),
									  passwordString);
}

void AMPIC887ConsoleCommandParser::handleSetRecordTriggerInput(const QString &commandString)
{
	QStringList argsList = commandArguments(commandString);

	QString recordTriggerString;
	if(argsList.count() >= 1) {
		recordTriggerString = argsList.at(0);
	}

	bool parseSuccess = false;
	int recordTriggerCode = recordTriggerString.toInt(&parseSuccess);
	if(!parseSuccess) {
		recordTriggerCode = -1;
	}

	emit setRecordTriggerCommandIssued(AMGCS2Support::intCodeToDataRecordTrigger(recordTriggerCode));
}

void AMPIC887ConsoleCommandParser::handleSetSoftLimitStatusInput(const QString &commandString)
{
	QStringList argumentList = commandArguments(commandString);

	AMPIC887AxisMap<bool> axisValueMap;
	int argumentCount = argumentList.count();

	// Ensure argument list isn't empty and has arguments in groups of two.
	if((argumentCount == 0) || (argumentCount % 2) != 0) {
		return;
	}

	// Axis values are provided in pairs, and are not valid unless both are
	// there. As such we iterate through two at a time.
	for(int iAxis = 0, iValue = 1;
		iAxis < argumentCount && iValue < argumentCount;
		iAxis += 2, iValue += 2) {

		QString axisString = argumentList.at(iAxis);
		QString valueString = argumentList.at(iValue);

		AMGCS2::Axis axis;
		if(axisString.length() == 1) {
			axis = AMGCS2Support::characterToAxis(axisString.at(0));
		} else {
			axis = AMGCS2::UnknownAxis;
		}

		bool parsedValue;
		if(valueString == "0" ||
				valueString.toLower() == "false") {
			parsedValue = false;
		} else if (valueString == "1" ||
				   valueString.toLower() == "true") {
			parsedValue = true;
		} else {

			return;
		}

		axisValueMap.insert(axis, parsedValue);
	}

	emit setSoftLimitStatesCommandIssued(axisValueMap);
}

void AMPIC887ConsoleCommandParser::handleRecordConfigInput(const QString &commandString)
{
	QStringList argumentList = commandArguments(commandString);

	QList<int> tableIdList;

	foreach(QString argumentString, argumentList) {

		bool parseSuccess = false;
		int currentArgumentTableId = argumentString.toInt(&parseSuccess);
		if(parseSuccess) {
			tableIdList.append(currentArgumentTableId);
		} else {
			tableIdList.append(-1);
		}
	}

	emit recordConfigCommandIssued(tableIdList);
}

void AMPIC887ConsoleCommandParser::handleSetRecordConfigInput(const QString &commandString)
{
	QStringList argumentList = commandArguments(commandString);

	QHash<int, AMPIC887DataRecorderConfiguration> recordConfigurations;

	int argumentCount = argumentList.count();

	// Ensure arguments are provided and come in sets of 3
	if(argumentCount < 0 || argumentCount % 3 != 0) {
		emit setRecordConfigCommandIssued(recordConfigurations);
		return;
	}

	/*
	  The arguments to set data recorder configuration come in sets of 3, so we
	  iterate through the collection three at a time.
	  */
	for(int iTableId = 0, iSource = 1, iOption = 2, argumentCount = argumentList.count();
		iTableId < argumentCount && iSource < argumentCount && iOption < argumentCount;
		iTableId += 3, iSource += 3, iOption += 3) {

		bool parseSuccess = false;
		int recordTableId = -1;

		recordTableId = argumentList.at(iTableId).toInt(&parseSuccess);
		if(!parseSuccess) {
			recordTableId = -1;
		}


		QString sourceString = argumentList.at(iSource);
		AMGCS2::DataRecordSource recordSource = AMGCS2::UnknownDataRecordSource;
		if(sourceString.length() == 1) {
			recordSource =
					AMGCS2Support::charCodeToDataRecordSource(sourceString.at(0));
		}

		int recordOptionCode = argumentList.at(iOption).toInt(&parseSuccess);
		AMGCS2::DataRecordOption recordOption = AMGCS2::UnknownRecordOption;
		if(parseSuccess) {
			recordOption =
					AMGCS2Support::intCodeToDataRecordOption(recordOptionCode);
		}

		recordConfigurations.insert(recordTableId,
									AMPIC887DataRecorderConfiguration(recordSource, recordOption));

	}

	emit setRecordConfigCommandIssued(recordConfigurations);
}

void AMPIC887ConsoleCommandParser::handleDataRecordValuesInput(const QString &commandString)
{
	//offset, number, tableId
	QStringList argumentList = commandArguments(commandString);
	if(argumentList.length() == 3) {

		bool parseDataOffset = false;
		int dataOffset = argumentList.at(0).toInt(&parseDataOffset);

		bool parseNumberOfElements = false;
		int numberOfElements = argumentList.at(1).toInt(&parseNumberOfElements);

		bool parseTableId = false;
		int tableId = argumentList.at(2).toInt(&parseTableId);

		if(parseDataOffset && parseNumberOfElements && parseTableId) {

			emit dataRecorderValuesCommandIssued(dataOffset, numberOfElements, tableId);
		}
	}
}


