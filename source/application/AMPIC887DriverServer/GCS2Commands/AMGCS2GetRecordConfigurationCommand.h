#ifndef AMGCS2GETRECORDCONFIGURATIONSCOMMAND_H
#define AMGCS2GETRECORDCONFIGURATIONSCOMMAND_H


#include <QList>
#include <QHash>

#include "AMGCS2Command.h"
#include "../AMPIC887DataRecorderConfiguration.h"
/*!
  * A command representing a query for the data recorder configuration of a PI
  * C887.11 controller.
  */
class AMGCS2GetRecordConfigurationsCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get data recorder configuration command, ready
	  * to run on a controller.

	  */
	AMGCS2GetRecordConfigurationsCommand(const QList<int>& tableIds = QList<int>());

	/*!
	  * Virtual destructor for a get data recorder configuration command
	  */
	virtual ~AMGCS2GetRecordConfigurationsCommand() {}

	/*!
	  * The returned data recorder configurations for each recorder table of the
	  * controller.
	  * \returns A list of data recorder configurations for the tables of the controller
	  * if the command was run successfully, an empty list otherwise.
	  */
	QHash<int, AMPIC887DataRecorderConfiguration> recordConfigs() const;
protected:

	/*!
	  * Ensures that the arguments with which the command was initialized are valid:
	  *  - Ensures that all the tableIds in the list are between 1 and 16.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the data recorder configurations from
	  * a controller.
	  */
	virtual bool runImplementation();

	QList<int> tablesIdsToQuery_;
	QHash<int, AMPIC887DataRecorderConfiguration> recordConfigs_;
};

#endif // AMGCS2GETRECORDCONFIGURATIONSCOMMAND_H
