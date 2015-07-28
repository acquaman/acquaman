#ifndef AMGCS2GETNUMBEROFRECORDEDPOINTSCOMMAND_H
#define AMGCS2GETNUMBEROFRECORDEDPOINTSCOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include <QList>
#include <QHash>
/*!
  * A command representing a query to a C887.11 controller for the number of recorded
  * points acquired since data recording was last triggered.
  */
class AMGCS2GetNumberOfRecordedPointsCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of a get number of recorded points for the list of
	  * provided record table ids, which is ready to run on a controller.
	  * \param recordTables ~ A list of the table ids whose number of recorded
	  * data points the command will query.
	  */
	AMGCS2GetNumberOfRecordedPointsCommand(const QList<int>& recordTables);

	/*!
	  * Virtual destructor for a get number of recorded points command.
	  */
	virtual ~AMGCS2GetNumberOfRecordedPointsCommand() {}

	/*!
	  * A mapping of table ids which the command was initialized with, to that
	  * table's number of recorded data points. If the command has not yet been
	  * run successfully, this map will be empty.
	  */
	QHash<int, int> numberOfDataPointsRecorded() const;

protected:
	/*!
	  * Ensures that the list of record tables the command was initialized with
	  * is not empty, and that each of the record table ids is valid.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the number of recorded data points for
	  * each of the tables the command was initailized with from a C887.11 controller.
	  */
	virtual bool runImplementation();

	QList<int> recordTablesToQuery_;
	QHash<int, int> numberOfDataPointsRecorded_;
};

#endif // AMGCS2GETNUMBEROFRECORDEDPOINTSCOMMAND_H
