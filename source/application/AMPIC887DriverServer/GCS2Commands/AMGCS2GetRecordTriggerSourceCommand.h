#ifndef AMGCS2GETRECORDTRIGGERSOURCECOMMAND_H
#define AMGCS2GETRECORDTRIGGERSOURCECOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"

/*!
  * A command representing a query to request the current triggers set for the
  * provided record tables from a C887.11 controller.
  */
class AMGCS2GetRecordTriggerSourceCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of a get record trigger source command which will
	  * obtain the record trigger of the controller.
	  */
	AMGCS2GetRecordTriggerSourceCommand();

	/*!
	  * Virtual destructor for a get record trigger source command.
	  */
	virtual ~AMGCS2GetRecordTriggerSourceCommand() {}

	/*!
	  * The record trigger obtained by the command, or UnknownRecordTrigger if
	  * the command has not been run successfully.
	  */
	AMGCS2::DataRecordTrigger recordTrigger() const;

protected:

	/*!
	  * Defines the steps taken to query the record trigger from the controller.
	  */
	virtual bool runImplementation();

	AMGCS2::DataRecordTrigger recordTrigger_;
};

#endif // AMGCS2GETRECORDTRIGGERSOURCECOMMAND_H
