#ifndef AMGCS2GETDATARECORDERCONFIGURATIONCOMMAND_H
#define AMGCS2GETDATARECORDERCONFIGURATIONCOMMAND_H

#include <QList>
#include <QStringList>

#include "AMGCS2Command.h"
#include "../AMPIC887DataRecorderConfiguration.h"
/*!
  * A command representing a query for the data recorder configuration of a PI
  * C887.11 controller.
  */
class AMGCS2GetDataRecorderConfigurationCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get data recorder configuration command, ready
	  * to run on a controller.
	  */
	AMGCS2GetDataRecorderConfigurationCommand();

	/*!
	  * Virtual destructor for a get data recorder configuration command
	  */
	virtual ~AMGCS2GetDataRecorderConfigurationCommand() {}

	/*!
	  * The reported data recorder configuration in a human readable format.
	  * \returns Stringified form of the data recorder configuration if the command
	  * was run successfully, the empty string otherwise.
	  */
	virtual QString outputString() const;

	/*!
	  * The returned data recorder configurations for each recorder table of the
	  * controller.
	  * \returns A list of data recorder configurations for the tables of the controller
	  * if the command was run successfully, an empty list otherwise.
	  */
	QList<AMPIC887DataRecorderConfiguration*> dataRecorderConfigurations() const;
protected:

	/*!
	  * Defines the steps taken to query the data recorder configurations from
	  * a controller.
	  */
	bool runImplementation();

	QList<AMPIC887DataRecorderConfiguration*> dataRecorderConfigurations_;
};

#endif // AMGCS2GETDATARECORDERCONFIGURATIONCOMMAND_H
