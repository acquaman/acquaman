#ifndef AMGCS2SETSYNTAXVERSIONCOMMAND_H
#define AMGCS2SETSYNTAXVERSIONCOMMAND_H

#include "AMGCS2Command.h"
/*!
  * \brief A command representing an instruction for a controller to set its syntax
  * version to GCS2.0.
  *
  * GCS1.0 is only provided on the controller for backwards compatibility use. These
  * drivers do not support interfacing with GCS1.0. As such this command exists
  * only to ensure that if, for some reason, the controller is manually set to
  * syntax version 1.0, we can detect this during initialization with an
  * AMGCS2SyntaxVersionQueryCommand, and use this command to set it back to 2.0.
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

	/*!
	  * Returns the success status of the attempt to set the syntax version.
	  */
	virtual QString outputString() const;
protected:
	/*!
	  * Defines the steps required to set a controller's syntax version.
	  */
	virtual bool runImplementation();
};

#endif // AMGCS2SETSYNTAXVERSIONCOMMAND_H
