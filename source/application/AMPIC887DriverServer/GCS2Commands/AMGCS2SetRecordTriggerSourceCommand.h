#ifndef AMGCS2SETRECORDTRIGGERSOURCECOMMAND_H
#define AMGCS2SETRECORDTRIGGERSOURCECOMMAND_H

#include <QHash>
#include "AMGCS2Command.h"
#include "../AMGCS2.h"
/*!
  * A command representing an instruction for a controller to set the recording
  * trigger it uses to record in the tables.
  */
class AMGCS2SetRecordTriggerSourceCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set record trigger source command which will
	  * set the trigger which will commence recording.
	  * \param tableTrigger ~ The new data recording trigger which the controller
	  * will be set to when the command succeeds.
	  */
	AMGCS2SetRecordTriggerSourceCommand(AMGCS2::DataRecordTrigger tableTrigger);

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


	AMGCS2::DataRecordTrigger tableTrigger_;
};

#endif // AMGCS2SETRECORDTRIGGERSOURCECOMMAND_H
