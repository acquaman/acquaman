#ifndef AMGCS2GETRECORDEROPTIONSCOMMAND_H
#define AMGCS2GETRECORDEROPTIONSCOMMAND_H
#include "AMGCS2Command.h"

/*!
  * A command representing a query to a C887.11 controller for all its information
  * relating to data recording  (record options, trigger options, information about
  * additional parameters and command regarding data recording).
  */
class AMGCS2GetRecorderOptionsCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get recorder options command which is ready to
	  * be run on a controller.
	  */
    AMGCS2GetRecorderOptionsCommand();

	/*!
	  * Virtual destructor for a get recorder options command
	  */
	virtual ~AMGCS2GetRecorderOptionsCommand() {}

	/*!
	  * The obtained recorder options if the command was run successfully, the
	  * empty string otherwise.
	  */
	QString recordOptions() const;

protected:

	/*!
	  * Defines the steps taken to request the record options from a controller.
	  */
	virtual bool runImplementation();


	QString optionResults_;
};

#endif // AMGCS2GETRECORDEROPTIONSCOMMAND_H
