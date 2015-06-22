#include "AMGCS2CommandFactory.h"
#include <QHash>
#include <QStringList>

#include "AMGCS2Support.h"
#include "AMGCS2MoveCommand.h"
#include "AMGCS2GetCurrentPositionCommand.h"
#include "AMGCS2StopCommand.h"
#include "AMGCS2HaltSmoothlyCommand.h"
#include "AMGCS2GetMovingStatusCommand.h"
#include "AMGCS2CompositeCommand.h"
#include "AMGCS2GetControllerReadyStatusCommand.h"
#include "AMGCS2GetDeviceIdentificationCommand.h"
#include "AMGCS2GetCommandLevelCommand.h"
#include "AMGCS2SetCommandLevelCommand.h"
#include "AMGCS2SetSyntaxVersionCommand.h"
#include "AMGCS2GetDataRecorderConfigurationCommand.h"
#include "AMPIC887DataRecorderConfiguration.h"
#include "AMGCS2SetDataRecorderConfigurationCommand.h"
#include "AMGCS2GetNumberOfRecordedPointsCommand.h"
#include "AMGCS2GetRecordedDataValuesCommand.h"
#include "AMGCS2SetDataRecorderConfigurationCommand.h"
#include "AMGCS2SetRecordTriggerSourceCommand.h"
#include "AMGCS2GetRecordTriggerSourceCommand.h"
#include "AMGCS2ReferenceMoveCommand.h"
#include "AMGCS2GetReferenceResultCommand.h"
#include "AMGCS2GetRecorderOptionsCommand.h"
#include "AMGCS2GetAvailableParametersCommand.h"
#include "AMGCS2GetLimitSwitchStatusCommand.h"
#include "AMGCS2GetTargetPositionCommand.h"
#include "AMGCS2MoveRelativeCommand.h"
#include "AMGCS2GetLowSoftLimitsCommand.h"
#include "AMGCS2SetLowSoftLimitsCommand.h"

AMGCS2Command * AMGCS2CommandFactory::buildCommand(const QString &commandString)
{
	if(commandString.startsWith("POS?")) {
		return new AMGCS2GetCurrentPositionCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("MST?")) {
		return new AMGCS2GetMovingStatusCommand();
	} else if (commandString.startsWith("STP")){
		return new AMGCS2StopCommand();
	} else if (commandString.startsWith("HLT")) {
		return new AMGCS2HaltSmoothlyCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("MOV?")) {
		return new AMGCS2GetTargetPositionCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("MOV")) {
		return buildMoveCommand(commandArguments(commandString));
	} else if(commandString.startsWith("RDY?")) {
		return new AMGCS2GetControllerReadyStatusCommand();
	} else if(commandString.startsWith("IDN?")) {
		return new AMGCS2GetDeviceIdentificationCommand();
	} else if(commandString.startsWith("CCL?")) {
		return new AMGCS2GetCommandLevelCommand();
	} else if(commandString.startsWith("CCL")) {
		return buildSetCommandLevelCommand(commandArguments(commandString));
	} else if (commandString.startsWith("CSV")) {
		return new AMGCS2SetSyntaxVersionCommand();
	} else if(commandString.startsWith("DRC")) {
		return buildSetDataRecorderConfigurationCommand(commandArguments(commandString));
	} else if(commandString.startsWith("DRC?")) {
		return new AMGCS2GetDataRecorderConfigurationCommand();
	} else if(commandString.startsWith("DRL?")) {
		return buildGetNumberOfRecordedPointsCommand(commandArguments(commandString));
	} else if(commandString.startsWith("DRR?")) {
		return buildGetRecordedDataValuesCommand(commandArguments(commandString));
	} else if(commandString.startsWith("DRT?")) {
		return buildGetRecordTriggerSourceCommand(commandArguments(commandString));
	} else if (commandString.startsWith("FRF?")){
		return new AMGCS2GetReferenceResultCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("FRF")) {
		return new AMGCS2ReferenceMoveCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("HDR?")) {
		return new AMGCS2GetRecorderOptionsCommand();
	} else if(commandString.startsWith("HPA?")) {
		return new AMGCS2GetAvailableParametersCommand();
	} else if(commandString.startsWith("LIM?")) {
		return new AMGCS2GetLimitSwitchStatusCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("MVR")) {
		return buildMoveRelativeCommand(commandArguments(commandString));
	} else if(commandString.startsWith("NLM?")) {
		return new AMGCS2GetLowSoftLimitsCommand(axesFromCommandString(commandString));
	} else if(commandString.startsWith("NLM")) {
		return buildSetLowSoftLimitsCommand(commandArguments(commandString));
	}

	return 0;
}

QStringList AMGCS2CommandFactory::commandArguments(const QString& commandString)
{
	QStringList argumentsList = commandString.split(" ");
	argumentsList.removeAt(0);
	return argumentsList;
}

QList<AMGCS2::Axis> AMGCS2CommandFactory::axesFromCommandString(const QString &axisArguments)
{
	QStringList axisArgumentList = commandArguments(axisArguments);

	QList<AMGCS2::Axis> axes;

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

AMGCS2Command * AMGCS2CommandFactory::buildMoveCommand(const QStringList &argumentList)
{
	QHash<AMGCS2::Axis, double> axisPositions;
	int argumentCount = argumentList.count();

	// Ensure argument list isn't empty and has arguments in groups of two.
	if((argumentCount == 0) || (argumentCount % 2) != 0) {
		return 0;
	}

	// Axis positions are provided in pairs, and are not valid unless both are
	// there. As such we iterate through two at a time.
	for(int iAxis = 0, iPosition = 1;
		iAxis < argumentCount && iPosition < argumentCount;
		iAxis += 2, iPosition += 2) {

		QString axisString = argumentList.at(iAxis);
		QString positionString = argumentList.at(iPosition);

		if(axisString.length() != 1) {
			return 0;
		}

		AMGCS2::Axis axis = AMGCS2Support::characterToAxis(axisString.at(0));
		bool parseSuccess = false;
		double position = positionString.toDouble(&parseSuccess);

		if(!parseSuccess) {
			return 0;
		}

		axisPositions.insert(axis, position);
	}

	return new AMGCS2MoveCommand(axisPositions);
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

	QList<AMPIC887DataRecorderConfiguration*> recordConfigurationList;
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

		recordConfigurationList.append(
					new AMPIC887DataRecorderConfiguration(
						recordTableId,
						recordSource,
						recordOption));
	}

	return new AMGCS2SetDataRecorderConfigurationCommand(recordConfigurationList);
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
	if(argumentList.isEmpty()) {
		return 0;
	}

	QHash<int, AMGCS2::DataRecordTrigger> tableTriggerMap;

	for(int iTableId = 0, iTrigger = 1, argCount = argumentList.count();
		iTableId < argCount && iTrigger < argCount;
		iTableId += 2, iTrigger += 2) {

		bool parseSuccess = false;
		int recordTableId = argumentList.at(iTableId).toInt(&parseSuccess);

		if(!parseSuccess) {
			return 0;
		}

		int triggerCode = argumentList.at(iTrigger).toInt(&parseSuccess);

		if(!parseSuccess) {
			return 0;
		}

		AMGCS2::DataRecordTrigger triggerValue =
				AMGCS2Support::intCodeToDataRecordTrigger(triggerCode);

		tableTriggerMap.insert(recordTableId, triggerValue);
	}

	return new AMGCS2SetRecordTriggerSourceCommand(tableTriggerMap);
}

AMGCS2Command * AMGCS2CommandFactory::buildGetRecordTriggerSourceCommand(const QStringList &argumentList)
{
	if(argumentList.isEmpty()) {
		return 0;
	}

	QList<int> recordTableIds;

	for(int iTableId = 0, argCount = argumentList.count();
		iTableId < argCount;
		++iTableId) {

		bool parseSuccess = false;
		int tableId = argumentList.at(iTableId).toInt(&parseSuccess);

		if(!parseSuccess) {
			return 0;
		}

		recordTableIds.append(tableId);
	}

	return new AMGCS2GetRecordTriggerSourceCommand(recordTableIds);
}

AMGCS2Command * AMGCS2CommandFactory::buildMoveRelativeCommand(const QStringList &argumentList)
{
	QHash<AMGCS2::Axis, double> axisPositions;
	int argumentCount = argumentList.count();

	// Ensure argument list isn't empty and has arguments in groups of two.
	if((argumentCount == 0) || (argumentCount % 2) != 0) {
		return 0;
	}

	// Axis positions are provided in pairs, and are not valid unless both are
	// there. As such we iterate through two at a time.
	for(int iAxis = 0, iPosition = 1;
		iAxis < argumentCount && iPosition < argumentCount;
		iAxis += 2, iPosition += 2) {

		QString axisString = argumentList.at(iAxis);
		QString positionString = argumentList.at(iPosition);

		if(axisString.length() != 1) {
			return 0;
		}

		AMGCS2::Axis axis = AMGCS2Support::characterToAxis(axisString.at(0));
		bool parseSuccess = false;
		double position = positionString.toDouble(&parseSuccess);

		if(!parseSuccess) {
			return 0;
		}

		axisPositions.insert(axis, position);
	}

	return new AMGCS2MoveRelativeCommand(axisPositions);
}

AMGCS2Command * AMGCS2CommandFactory::buildSetLowSoftLimitsCommand(const QStringList &argumentList)
{
	QHash<AMGCS2::Axis, double> axisPositions;
	int argumentCount = argumentList.count();

	// Ensure argument list isn't empty and has arguments in groups of two.
	if((argumentCount == 0) || (argumentCount % 2) != 0) {
		return 0;
	}

	// Axis limits are provided in pairs, and are not valid unless both are
	// there. As such we iterate through two at a time.
	for(int iAxis = 0, iPosition = 1;
		iAxis < argumentCount && iPosition < argumentCount;
		iAxis += 2, iPosition += 2) {

		QString axisString = argumentList.at(iAxis);
		QString positionString = argumentList.at(iPosition);

		if(axisString.length() != 1) {
			return 0;
		}

		AMGCS2::Axis axis = AMGCS2Support::characterToAxis(axisString.at(0));
		bool parseSuccess = false;
		double position = positionString.toDouble(&parseSuccess);

		if(!parseSuccess) {
			return 0;
		}

		axisPositions.insert(axis, position);
	}

	return new AMGCS2SetLowSoftLimitsCommand(axisPositions);
}




