#ifndef AMGCS2SETRECORDTRIGGERSOURCECOMMAND_H
#define AMGCS2SETRECORDTRIGGERSOURCECOMMAND_H

#include <QHash>
#include "AMGCS2Command.h"
#include "AMGCS2.h"
/*!
  * A command representing an instruction for a controller to set the recording
  * triggers for specified record tables.
  */
class AMGCS2SetRecordTriggerSourceCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set record trigger source command which will
	  * set the triggers of the provided record table ids to the values associated
	  * with them in the provided map.
	  * \param tableTriggers ~ A mapping of record table id to the trigger source
	  * to which the command will set it.
	  */
	AMGCS2SetRecordTriggerSourceCommand(const QHash<int, AMGCS2::DataRecordTrigger> tableTriggers);

	/*!
	  * Virtual destructor for a set record trigger source command.
	  */
	virtual ~AMGCS2SetRecordTriggerSourceCommand() {}

protected:
	/*!
	  * Ensures that the mapping of record table ids to record triggers is valid:
	  *  - The tableTriggers map cannot be empty.
	  *  - None of the trigger values in the map can be UnknownRecordTrigger
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to set the record table trigger source for the
	  * provided record tables.
	  */
	virtual bool runImplementation();


	QHash<int, AMGCS2::DataRecordTrigger> tableTriggers_;
};

#endif // AMGCS2SETRECORDTRIGGERSOURCECOMMAND_H
