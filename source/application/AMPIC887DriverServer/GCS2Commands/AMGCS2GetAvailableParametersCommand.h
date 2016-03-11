#ifndef AMGCS2GETAVAILABLEPARAMETERSCOMMAND_H
#define AMGCS2GETAVAILABLEPARAMETERSCOMMAND_H

#include "AMGCS2Command.h"
/*!
  * A command representing a query to a C887.11 controller for its available
  * parameters and a short destription.
  */
class AMGCS2GetAvailableParametersCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of a get available parameters command which is
	  * ready to be run on a controller.
	  */
    AMGCS2GetAvailableParametersCommand();

	/*!
	  * Virtual destructor for a get available parameters command.
	  */
	virtual ~AMGCS2GetAvailableParametersCommand() {}

	/*!
	  * The returned available parameters if the command was run successfully,
	  * the empty string otherwise.
	  */
	QString availableParameters() const;

protected:

	/*!
	  * Defines the steps taken to query a controller for its available parameters.
	  */
	virtual bool runImplementation();

	QString parameterResults_;
};

#endif // AMGCS2GETAVAILABLEPARAMETERSCOMMAND_H
