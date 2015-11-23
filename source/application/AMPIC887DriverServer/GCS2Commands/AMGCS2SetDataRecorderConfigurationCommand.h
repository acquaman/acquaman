#ifndef AMGCS2SETDATARECORDERCONFIGURATIONCOMMAND_H
#define AMGCS2SETDATARECORDERCONFIGURATIONCOMMAND_H

#include <QHash>

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887DataRecorderConfiguration.h"

/*!
  * A command representing an instruction for a controller to change its record
  * configuration.
  */
class AMGCS2SetDataRecorderConfigurationCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set data recorder configuration command which will
	  * instruct a controller to alter its record configuration to the provided
	  * record configuration.
	  */
	AMGCS2SetDataRecorderConfigurationCommand(const QHash<int, AMPIC887DataRecorderConfiguration>& tableRecordConfigs);

	/*!
	  * Virtual destructor for the set data recorder configuration command.
	  * Cleans up the list of passed configurations.
	  */
	virtual ~AMGCS2SetDataRecorderConfigurationCommand() {}
protected:
	/*!
	  * Ensures that the passed list of configurations is valid (i.e. is not empty
	  * and contains only valid configurations).
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to set a controller's data recorder configuration.
	  */
	virtual bool runImplementation();

	QHash<int, AMPIC887DataRecorderConfiguration> tableRecordConfigs_;
};

#endif // AMGCS2SETDATARECORDERCONFIGURATIONCOMMAND_H
