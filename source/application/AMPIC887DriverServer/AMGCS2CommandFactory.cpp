#include "AMGCS2CommandFactory.h"
#include <QHash>
#include <QStringList>

#include "AMGCS2.h"
#include "AMGCS2Support.h"
#include "AMGCS2MoveCommand.h"
#include "AMGCS2CurrentPositionCommand.h"
#include "AMGCS2StopCommand.h"
#include "AMGCS2HaltSmoothlyCommand.h"
#include "AMGCS2MovingStatusCommand.h"
#include "AMGCS2CompositeCommand.h"
#include "AMGCS2ControllerReadyStatusCommand.h"
#include "AMGCS2DeviceIdentificationCommand.h"
#include "AMGCS2CommandLevelQueryCommand.h"
#include "AMGCS2SetCommandLevelCommand.h"
#include "AMGCS2SetSyntaxVersionCommand.h"
#include "AMGCS2DataRecorderConfigurationQueryCommand.h"
#include "AMPIC887DataRecorderConfiguration.h"
#include "AMGCS2SetDataRecorderConfigurationCommand.h"

AMGCS2Command * AMGCS2CommandFactory::buildCommand(const QString &commandString)
{
	if(commandString.startsWith("POS?")) {
		return buildCurrentPositionCommand(commandArguments(commandString));
	} else if(commandString.startsWith("MST?")) {
		return new AMGCS2MovingStatusCommand();
	} else if (commandString.startsWith("STP")){
		return new AMGCS2StopCommand();
	} else if (commandString.startsWith("HLT")) {
		return buildHaltSmoothlyCommand(commandArguments(commandString));
	} else if(commandString.startsWith("MOV")) {
		return buildMoveCommand(commandArguments(commandString));
	} else if(commandString.startsWith("RDY?")) {
		return new AMGCS2ControllerReadyStatusCommand();
	} else if(commandString.startsWith("IDN?")) {
		return new AMGCS2DeviceIdentificationCommand();
	} else if(commandString.startsWith("CCL?")) {
		return new AMGCS2CommandLevelQueryCommand();
	} else if(commandString.startsWith("CCL")) {
		return buildSetCommandLevelCommand(commandArguments(commandString));
	} else if (commandString.startsWith("CSV")) {
		return new AMGCS2SetSyntaxVersionCommand();
	} else if(commandString.startsWith("DRC")) {
		return buildSetDataRecorderConfigurationCommand(commandArguments(commandString));
	} else if(commandString.startsWith("DRC?")) {
		return new AMGCS2DataRecorderConfigurationQueryCommand();
	}

	return 0;
}

QStringList AMGCS2CommandFactory::commandArguments(const QString& commandString)
{
	QStringList argumentsList = commandString.split(" ");
	argumentsList.removeAt(0);
	return argumentsList;
}

AMGCS2Command * AMGCS2CommandFactory::buildMoveCommand(const QStringList &argumentList)
{
	QHash<AMGCS2::Axis, double> axisPositions;

	// Axis positions are provided in arguments in pairs of Axis double. As such
	// we interate through the arguments two at a time, which is why this looks odd.
	for(int iAxisPositionPair = 0, argumentCount = argumentList.count();
		iAxisPositionPair < argumentCount;) {

		QString axisString = argumentList.at(iAxisPositionPair);

		++iAxisPositionPair;

		if(iAxisPositionPair >= argumentCount) {
			// We must have an axis that doesn't have a position, this is an error.
			return 0;
		}

		QString positionString = argumentList.at(iAxisPositionPair);

		AMGCS2::Axis axis = AMGCS2Support::characterToAxis(axisString.at(0));

		bool canParse = false;
		double position = positionString.toDouble(&canParse);

		if(!canParse || axis == AMGCS2::UnknownAxis) {
			return 0;
		}

		axisPositions.insert(axis, position);

		++iAxisPositionPair;
	}

	return new AMGCS2MoveCommand(axisPositions);
}


AMGCS2Command * AMGCS2CommandFactory::buildCurrentPositionCommand(const QStringList &argumentList)
{
	if(argumentList.isEmpty()) {
		return new AMGCS2CurrentPositionCommand();
	}

	QList<AMGCS2::Axis> axes;

	foreach (QString argument, argumentList) {

		if(argument.length() != 1) {
			return 0;
		}

		AMGCS2::Axis axis = AMGCS2Support::characterToAxis(argument.at(0));
		if(axis == AMGCS2::UnknownAxis) {
			return 0;
		}

		axes.append(axis);
	}

	return new AMGCS2CurrentPositionCommand(axes);
}

AMGCS2Command * AMGCS2CommandFactory::buildHaltSmoothlyCommand(const QStringList &argumentList)
{
	if(argumentList.isEmpty()) {
		return new AMGCS2HaltSmoothlyCommand();
	}

	QList<AMGCS2::Axis> axes;

	foreach(QString argument, argumentList) {

		if(argument.length() != 1) {
			return 0;
		}

		AMGCS2::Axis axis = AMGCS2Support::characterToAxis(argument.at(0));
		if(axis == AMGCS2::UnknownAxis) {
			return 0;
		}

		axes.append(axis);
	}

	return new AMGCS2HaltSmoothlyCommand(axes);
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

	if(argumentList.count() < 3) {
		return 0;
	}

	QList<AMPIC887DataRecorderConfiguration*> recordConfigurationList;

	/*
	  The arguments to set data recorder configuration come in sets of 3, so we
	  iterate through the collection three at a time.
	  */
	for (int iConfigurationSet = 0, argCount = argumentList.count();
		 iConfigurationSet < argCount;
		 ) {

		// Arg1: Record Table Id
		bool parseSuccess = false;
		int recordTableId = -1;

		recordTableId = argumentList.at(iConfigurationSet).toInt(&parseSuccess);
		if(!parseSuccess) {
			return 0;
		}

		++iConfigurationSet;
		// Arg2: RecordSource

		if(iConfigurationSet >= argCount) {
			return 0;
		}

		QString sourceString = argumentList.at(iConfigurationSet);
		if(sourceString.length() != 1) {
			return 0;
		}
		AMGCS2::DataRecordSource recordSource =
				AMGCS2Support::charCodeToDataRecordSource(sourceString.at(0));

		++iConfigurationSet;
		// Arg3: Record Option

		if(iConfigurationSet >= argCount) {
			return 0;
		}

		int recordOptionCode = argumentList.at(iConfigurationSet).toInt(&parseSuccess);
		if(!parseSuccess) {
			return 0;
		}

		AMGCS2::DataRecordOption recordOption =
				AMGCS2Support::intCodeToDataRecordOption(recordOptionCode);

		// Add the produced record config to the list.
		recordConfigurationList.append(
					new AMPIC887DataRecorderConfiguration(
						recordTableId,
						recordSource,
						recordOption));

		++iConfigurationSet;
	}

	return new AMGCS2SetDataRecorderConfigurationCommand(recordConfigurationList);
}


