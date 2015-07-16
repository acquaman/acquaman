#include "AMGCS2CommandFactory.h"
#include <QStringList>

#include "AMGCS2Support.h"
#include "GCS2Commands/AMGCS2AsyncMoveCommand.h"
#include "GCS2Commands/AMGCS2GetCurrentPositionCommand.h"
#include "GCS2Commands/AMGCS2StopCommand.h"
#include "GCS2Commands/AMGCS2HaltSmoothlyCommand.h"
#include "GCS2Commands/AMGCS2GetMovingStatusCommand.h"
#include "GCS2Commands/AMGCS2CompositeCommand.h"
#include "GCS2Commands/AMGCS2GetControllerReadyStatusCommand.h"
#include "GCS2Commands/AMGCS2GetDeviceIdentificationCommand.h"
#include "GCS2Commands/AMGCS2GetCommandLevelCommand.h"
#include "GCS2Commands/AMGCS2SetCommandLevelCommand.h"
#include "GCS2Commands/AMGCS2SetSyntaxVersionCommand.h"
#include "GCS2Commands/AMGCS2GetRecordConfigurationCommand.h"
#include "AMPIC887DataRecorderConfiguration.h"
#include "GCS2Commands/AMGCS2SetDataRecorderConfigurationCommand.h"
#include "GCS2Commands/AMGCS2GetNumberOfRecordedPointsCommand.h"
#include "GCS2Commands/AMGCS2GetRecordedDataValuesCommand.h"
#include "GCS2Commands/AMGCS2SetDataRecorderConfigurationCommand.h"
#include "GCS2Commands/AMGCS2SetRecordTriggerSourceCommand.h"
#include "GCS2Commands/AMGCS2GetRecordTriggerSourceCommand.h"
#include "GCS2Commands/AMGCS2AsyncReferenceMoveCommand.h"
#include "GCS2Commands/AMGCS2GetReferenceResultCommand.h"
#include "GCS2Commands/AMGCS2GetRecorderOptionsCommand.h"
#include "GCS2Commands/AMGCS2GetAvailableParametersCommand.h"
#include "GCS2Commands/AMGCS2GetLimitSwitchStatusCommand.h"
#include "GCS2Commands/AMGCS2GetTargetPositionCommand.h"
#include "GCS2Commands/AMGCS2AsyncMoveRelativeCommand.h"
#include "GCS2Commands/AMGCS2GetLowSoftLimitsCommand.h"
#include "GCS2Commands/AMGCS2SetLowSoftLimitsCommand.h"
#include "GCS2Commands/AMGCS2GetOnTargetStateCommand.h"
#include "GCS2Commands/AMGCS2GetHighSoftLimitsCommand.h"
#include "GCS2Commands/AMGCS2SetHighSoftLimitsCommand.h"
#include "GCS2Commands/AMGCS2GetPositionUnitsCommand.h"
#include "GCS2Commands/AMGCS2SetCycleTimeCommand.h"
#include "GCS2Commands/AMGCS2GetCycleTimeCommand.h"
#include "GCS2Commands/AMGCS2GetPivotPointCommand.h"
#include "GCS2Commands/AMGCS2SetPivotPointCommand.h"
#include "GCS2Commands/AMGCS2GetSoftLimitsStatusCommand.h"
#include "GCS2Commands/AMGCS2SetSoftLimitsStatusCommand.h"
#include "GCS2Commands/AMGCS2GetServoModeCommand.h"
#include "GCS2Commands/AMGCS2SetServoModeCommand.h"
#include "GCS2Commands/AMGCS2GetStepSizeCommand.h"
#include "GCS2Commands/AMGCS2SetStepSizeCommand.h"
#include "GCS2Commands/AMGCS2GetMinCommandablePositionCommand.h"
#include "GCS2Commands/AMGCS2GetMaxCommandablePositionCommand.h"
#include "GCS2Commands/AMGCS2GetSystemVelocityCommand.h"
#include "GCS2Commands/AMGCS2SetSystemVelocityCommand.h"
#include "GCS2Commands/AMGCS2VirtualMoveCommand.h"

AMGCS2Command * AMGCS2CommandFactory::buildCommand(const QString &commandString)
{
	if(commandString.startsWith("OPOS?")) {
		return new AMGCS2GetCurrentPositionCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OMST?")) {
		return new AMGCS2GetMovingStatusCommand();
	} else if (commandString.startsWith("OSTP")){
		return new AMGCS2StopCommand();
	} else if (commandString.startsWith("OHLT")) {
		return new AMGCS2HaltSmoothlyCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OMOV?")) {
		return new AMGCS2GetTargetPositionCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OMOV")) {
		return new AMGCS2AsyncMoveCommand(axesDoublePairFromCommandString(commandString));
	} else if(commandString.startsWith("ORDY?")) {
		return new AMGCS2GetControllerReadyStatusCommand();
	} else if(commandString.startsWith("OIDN?")) {
		return new AMGCS2GetDeviceIdentificationCommand();
	} else if(commandString.startsWith("OCCL?")) {
		return new AMGCS2GetCommandLevelCommand();
	} else if(commandString.startsWith("OCCL")) {
		return buildSetCommandLevelCommand(commandArguments(commandString));
	} else if (commandString.startsWith("OCSV")) {
		return new AMGCS2SetSyntaxVersionCommand();
	} else if(commandString.startsWith("ODRC?")) {
		return new AMGCS2GetRecordConfigurationsCommand();
	} else if(commandString.startsWith("ODRC")) {
		return buildSetDataRecorderConfigurationCommand(commandArguments(commandString));
	} else if(commandString.startsWith("ODRL?")) {
		return buildGetNumberOfRecordedPointsCommand(commandArguments(commandString));
	} else if(commandString.startsWith("ODRR?")) {
		return buildGetRecordedDataValuesCommand(commandArguments(commandString));
	} else if(commandString.startsWith("ODRT?")) {
		return new AMGCS2GetRecordTriggerSourceCommand();
	} else if(commandString.startsWith("ODRT")) {
		return buildSetRecordTriggerSourceCommand(commandArguments(commandString));
	} else if (commandString.startsWith("OFRF?")){
		return new AMGCS2GetReferenceResultCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OFRF")) {
		return new AMGCS2AsyncReferenceMoveCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OHDR?")) {
		return new AMGCS2GetRecorderOptionsCommand();
	} else if(commandString.startsWith("OHPA?")) {
		return new AMGCS2GetAvailableParametersCommand();
	} else if(commandString.startsWith("OLIM?")) {
		return new AMGCS2GetLimitSwitchStatusCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OMVR")) {
		return new AMGCS2AsyncMoveRelativeCommand(axesDoublePairFromCommandString(commandString));
	} else if(commandString.startsWith("ONLM?")) {
		return new AMGCS2GetLowSoftLimitsCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("ONLM")) {
		return new AMGCS2SetLowSoftLimitsCommand(axesDoublePairFromCommandString(commandString));
	} else if(commandString.startsWith("OONT?")) {
		return new AMGCS2GetOnTargetStateCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OPLM?")) {
		return new AMGCS2GetHighSoftLimitsCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OPLM")) {
		return new AMGCS2SetHighSoftLimitsCommand(axesDoublePairFromCommandString(commandString));
	} else if(commandString.startsWith("OPUN?")) {
		return new AMGCS2GetPositionUnitsCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OSCT?")) {
		return new AMGCS2GetCycleTimeCommand();
	} else if(commandString.startsWith("OSCT")) {
		return buildSetCycleTimeCommand(commandArguments(commandString));
	} else if(commandString.startsWith("OSPI?")) {
		return new AMGCS2GetPivotPointCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OSPI")) {
		return new AMGCS2SetPivotPointCommand(axesDoublePairFromCommandString(commandString));
	} else if(commandString.startsWith("OSSL?")) {
		return new AMGCS2GetSoftLimitsStatusCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OSSL")) {
		return buildSetSoftLimitStatusesCommand(commandArguments(commandString));
	} else if(commandString.startsWith("OSVO?")) {
		return new AMGCS2GetServoModeCommand();
	} else if(commandString.startsWith("OSVO")) {
		return buildSetServoModeCommand(commandArguments(commandString));
	} else if(commandString.startsWith("OSST?")) {
		return new AMGCS2GetStepSizeCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OSST")) {
		return new AMGCS2SetStepSizeCommand(axesDoublePairFromCommandString(commandString));
	} else if(commandString.startsWith("OTMN?")) {
		return new AMGCS2GetMinCommandablePositionCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OTMX?")) {
		return new AMGCS2GetMaxCommandablePositionCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("OVLS?")) {
		return new AMGCS2GetSystemVelocityCommand();
	} else if(commandString.startsWith("OVLS")) {
		return buildSetSystemVelocityCommand(commandArguments(commandString));
	} else if(commandString.startsWith("OVMO?")) {
		return new AMGCS2VirtualMoveCommand(axesDoublePairFromCommandString(commandString));
	}

	return 0;
}

QStringList AMGCS2CommandFactory::commandArguments(const QString& commandString)
{
	return commandString.trimmed().split(" ").mid(1, -1);
}

AMPIC887AxisCollection AMGCS2CommandFactory::axesFromCommandString(const QString &axisArguments)
{
	QStringList axisArgumentList = commandArguments(axisArguments);

	AMPIC887AxisCollection axes;

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

AMPIC887AxisMap<double> AMGCS2CommandFactory::axesDoublePairFromCommandString(const QString &arguments)
{
	QStringList argumentList = commandArguments(arguments);

	AMPIC887AxisMap<double> axisValueMap;
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
		bool parseSuccess = false;
		double parsedValue = valueString.toDouble(&parseSuccess);

		if(!parseSuccess) {
			// If parse fails we have to return an empty map
			axisValueMap.clear();
			return axisValueMap;
		}

		axisValueMap.insert(axis, parsedValue);
	}

	return axisValueMap;
}

AMGCS2Command * AMGCS2CommandFactory::buildSetCommandLevelCommand(const QStringList &argumentList)
{
	if(argumentList.isEmpty()) {
		return 0;
	}

	bool parseIntSuccessful = false;
	int commandLevelCode = argumentList.at(0).toInt(&parseIntSuccessful);

	if(!parseIntSuccessful) {
		return 0;
	}

	QString password;
	if(argumentList.count() > 1) {
		password = argumentList.at(1);
	}


	return new AMGCS2SetCommandLevelCommand(AMGCS2Support::intCodeToCommandLevel(
												commandLevelCode), password);
}

AMGCS2Command * AMGCS2CommandFactory::buildSetDataRecorderConfigurationCommand(const QStringList &argumentList)
{

	QHash<int, AMPIC887DataRecorderConfiguration> recordConfigurations;

	int argumentCount = argumentList.count();

	// Ensure arguments are provided and come in sets of 3
	if((argumentCount == 0) || (argumentCount % 3 != 0)) {
		return 0;
	}

	/*
	  The arguments to set data recorder configuration come in sets of 3, so we
	  iterate through the collection three at a time.
	  */	
	for(int iTableId = 0, iSource = 1, iOption = 2, argCount = argumentList.count();
		iTableId < argCount && iSource < argCount && iOption < argCount;
		iTableId += 3, iSource += 3, iOption += 3) {

		bool parseSuccess = false;
		int recordTableId = -1;

		recordTableId = argumentList.at(iTableId).toInt(&parseSuccess);
		if(!parseSuccess) {
			return 0;
		}

		QString sourceString = argumentList.at(iSource);
		if(sourceString.length() != 1) {
			return 0;
		}

		AMGCS2::DataRecordSource recordSource =
				AMGCS2Support::charCodeToDataRecordSource(sourceString.at(0));

		int recordOptionCode = argumentList.at(iOption).toInt(&parseSuccess);
		if(!parseSuccess) {
			return 0;
		}

		AMGCS2::DataRecordOption recordOption =
				AMGCS2Support::intCodeToDataRecordOption(recordOptionCode);

		recordConfigurations.insert(recordTableId,
									AMPIC887DataRecorderConfiguration(recordSource, recordOption));

	}

	return new AMGCS2SetDataRecorderConfigurationCommand(recordConfigurations);
}

AMGCS2Command * AMGCS2CommandFactory::buildGetNumberOfRecordedPointsCommand(const QStringList &argumentList)
{
	if(argumentList.isEmpty()) {
		return 0;
	}

	QList<int> recordTableIds;

	foreach(QString argument, argumentList) {
		bool parseSuccess = false;
		int recordTableId = argument.toInt(&parseSuccess);

		if(!parseSuccess) {
			return 0;
		}

		recordTableIds.append(recordTableId);
	}

	return new AMGCS2GetNumberOfRecordedPointsCommand(recordTableIds);
}

AMGCS2Command * AMGCS2CommandFactory::buildGetRecordedDataValuesCommand(const QStringList &argumentList)
{
	if(argumentList.count() < 3) {
		return 0;
	}

	bool parseSuccess = false;
	int offsetPoint = argumentList.at(0).toInt(&parseSuccess);

	if(!parseSuccess) {
		return 0;
	}

	int numberOfPoints = argumentList.at(1).toInt(&parseSuccess);

	if(!parseSuccess) {
		return 0;
	}

	int recordTableId = argumentList.at(2).toInt(&parseSuccess);

	if(!parseSuccess) {
		return 0;
	}

	return new AMGCS2GetRecordedDataValuesCommand(recordTableId, numberOfPoints, offsetPoint);
}

AMGCS2Command * AMGCS2CommandFactory::buildSetRecordTriggerSourceCommand(const QStringList &argumentList)
{
	if(argumentList.count() != 1) {
		return 0;
	}

	bool parseSuccess = false;
	int triggerCode = argumentList.at(0).toInt(&parseSuccess);

	if(!parseSuccess) {
		return 0;
	}

	AMGCS2::DataRecordTrigger recordTrigger =
			AMGCS2Support::intCodeToDataRecordTrigger(triggerCode);

	return new AMGCS2SetRecordTriggerSourceCommand(recordTrigger);
}

AMGCS2Command * AMGCS2CommandFactory::buildSetCycleTimeCommand(const QStringList &argumentList)
{
	if(argumentList.length() != 1) {
		return 0;
	}

	bool parseSuccess = false;
	double cycleTimeValue = argumentList.at(0).toDouble(&parseSuccess);

	if(!parseSuccess) {
		return 0;
	}

	return new AMGCS2SetCycleTimeCommand(cycleTimeValue);
}

AMGCS2Command * AMGCS2CommandFactory::buildSetSoftLimitStatusesCommand(const QStringList &argumentList)
{
	AMPIC887AxisMap<bool> axisValueMap;
	int argumentCount = argumentList.count();

	// Ensure argument list isn't empty and has arguments in groups of two.
	if((argumentCount == 0) || (argumentCount % 2) != 0) {
		return 0;
	}

	// Axis values are provided in pairs, and are not valid unless both are
	// there. As such we iterate through two at a time.
	for(int iAxis = 0, iValue = 1;
		iAxis < argumentCount && iValue < argumentCount;
		iAxis += 2, iValue += 2) {

		QString axisString = argumentList.at(iAxis);
		QString valueString = argumentList.at(iValue);

		if(axisString.length() != 1) {
			return 0;
		}

		AMGCS2::Axis axis = AMGCS2Support::characterToAxis(axisString.at(0));

		bool activateSoftLimit;

		if(valueString == "false") {
			activateSoftLimit = false;
		} else if(valueString == "true") {
			activateSoftLimit = true;
		} else {
			return 0;
		}

		axisValueMap.insert(axis, activateSoftLimit);
	}

	return new AMGCS2SetSoftLimitsStatusCommand(axisValueMap);
}

AMGCS2Command * AMGCS2CommandFactory::buildSetServoModeCommand(const QStringList &argumentList)
{
	if(argumentList.count() != 1) {
		return 0;
	}

	bool activateServoMode;

	if(argumentList.at(0) == "false") {
		activateServoMode = false;
	} else if(argumentList.at(0) == "true") {
		activateServoMode = true;
	} else {
		return 0;
	}

	return new AMGCS2SetServoModeCommand(activateServoMode);
}

AMGCS2Command * AMGCS2CommandFactory::buildSetSystemVelocityCommand(const QStringList &argumentList)
{
	if(argumentList.count() != 1) {
		return 0;
	}

	bool parseSuccess = false;
	double systemVelocityValue = argumentList.at(0).toDouble(&parseSuccess);

	if(!parseSuccess) {
		return 0;
	}

	return new AMGCS2SetSystemVelocityCommand(systemVelocityValue);
}





