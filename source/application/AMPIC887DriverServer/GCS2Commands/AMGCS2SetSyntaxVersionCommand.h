#ifndef AMGCS2SETSYNTAXVERSIONCOMMAND_H
#define AMGCS2SETSYNTAXVERSIONCOMMAND_H

#include "AMGCS2Command.h"
/*!
  * \brief A command representing an instruction for a controller to set its syntax
  * version to GCS2.0.
  *
  * GCS1.0 is only provided on the controller for backwards compatibility use. These
  * drivers do not support interfacing with GCS1.0. A such this command exists only
  * to ensure that a controller is on the correct syntax level on initialization.
  *
  * NOTE: It is not possible to check the syntax version if the syntax version is
  * set to 1.0. As such the only method to ensure that the controller is set to
  * syntax version 2.0 without risk of crashing the drivers (which occurs if a call
  * to qCSV() is made when set to syntax version 1.0) is to run this command.
  */
class AMGCS2SetSyntaxVersionCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set syntax version command, ready to be run on
	  * a controller.
	  */
    AMGCS2SetSyntaxVersionCommand();

	/*!
	  * Virtual destructor for a set syntax version command
	  */
	virtual ~AMGCS2SetSyntaxVersionCommand() {}

protected:
	/*!
	  * Defines the steps required to set a controller's syntax version.
	  */
	virtual bool runImplementation();
};

#endif // AMGCS2SETSYNTAXVERSIONCOMMAND_H
