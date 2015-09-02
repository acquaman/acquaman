#ifndef AMGCS2INITIALIZECONTROLLERSTATECOMMAND_H
#define AMGCS2INITIALIZECONTROLLERSTATECOMMAND_H

#include "AMGCS2Command.h"
#include "../AMPIC887ControllerState.h"

/*!
  * A command representing the initialization of a controller state class.
  *
  * NOTE: This does not correspond to a command in the GCS2 set. It is simply
  * a means of loading all the required data relating to a controller on startup
  * so that it can be cached in the relevant state classes for future reference.
  */
class AMGCS2InitializeControllerStateCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of an initialize controller state command which will
	  * initialize data within the provided controller state.
	  * \param controllerState ~ The controller state whose data is to be initialized.
	  */
	AMGCS2InitializeControllerStateCommand(AMPIC887ControllerState* controllerState);

	/*!
	  * Virtual destructor for an initialize controller state command.
	  */
	~AMGCS2InitializeControllerStateCommand();

protected:

	/*!
	  * Ensures that the arguments for the command are valid:
	  *  - Ensures the controllerState that the command was initialized with is
	  *    not null.
	  *  - Ensures that no axes of the hexapod which is being controlled are
	  *    moving.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps required to query all the required data to initialize
	  * the controller state for the actual controller it represents the state of.
	  */
	virtual bool runImplementation();

	AMPIC887ControllerState* controllerState_;
};

#endif // AMGCS2INITIALIZECONTROLLERSTATECOMMAND_H
