#ifndef AMGCS2GETRECORDTRIGGERSOURCECOMMAND_H
#define AMGCS2GETRECORDTRIGGERSOURCECOMMAND_H

#include "AMGCS2Command.h"
#include "AMGCS2.h"
#include <QList>
#include <QHash>
/*!
  * A command representing a query to request the current triggers set for the
  * provided record tables from a C887.11 controller.
  */
class AMGCS2GetRecordTriggerSourceCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of a get record trigger source command which will
	  * obtain the record triggers for the provided list of record tables.
	  * \param recordTableIds ~ A list of the record tables whose current record
	  * triggers are to be returned.
	  */
	AMGCS2GetRecordTriggerSourceCommand(const QList<int> recordTableIds);

	/*!
	  * Virtual destructor for a get record trigger source command.
	  */
	virtual ~AMGCS2GetRecordTriggerSourceCommand() {}

	/*!
	  * Outputs a human readable string version of the mapping from record table
	  * to record trigger which was obtained if the command was run successfully,
	  * the empty string otherwise.
	  */
	virtual QString outputString() const;

	/*!
	  * A mapping of the provided record table ids that the command was initialzied
	  * with to the result record triggers obtained when the command was successfully
	  * run.
	  * \returns A mapping of record table id to its record trigger if the command
	  * has been run successfully, and empty map otherwise.
	  */
	QHash<int, AMGCS2::DataRecordTrigger> tableRecordTriggers() const;

protected:
	/*!
	  * Ensures that the arguments passed to the get record trigger source command
	  * on initialization are valid:
	  *  - The list of record table ids to query cannot be empty.
	  *  - Each record table id in the list must be between 1 and 16
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the record trigger sources for the record
	  * tables which the command was initialized with from the controller.
	  */
	virtual bool runImplementation();

	QList<int> recordTablesToQuery_;
	QHash<int, AMGCS2::DataRecordTrigger> tableRecordTriggers_;
};

#endif // AMGCS2GETRECORDTRIGGERSOURCECOMMAND_H
