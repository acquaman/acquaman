#ifndef AMGCS2GETCOMMANDLEVELCOMMAND_H
#define AMGCS2GETCOMMANDLEVELCOMMAND_H
#include "AMGCS2Command.h"
#include "../AMGCS2.h"
/*
  * A command representing a query to a controller for its current command level.
  */
class AMGCS2GetCommandLevelCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a command level query command, ready to be run on
	  * a controller.
	  */
	AMGCS2GetCommandLevelCommand();

	/*!
	  * Virtual destructor for a command level query command
	  */
	virtual ~AMGCS2GetCommandLevelCommand() {}

	/*!
	  * The controller's command level, if this command was run successfully, otherwise,
	  * UnknownCommandLevel.
	  */
	AMGCS2::ControllerCommandLevel commandLevel();
protected:
	/*!
	  * Defines the steps necessary to query a controller's command level.
	  * \returns True if the controller's command level was determined successfully,
	  * false otherwise.
	  */
	virtual bool runImplementation();

	AMGCS2::ControllerCommandLevel commandLevel_;
};

#endif // AMGCS2GETCOMMANDLEVELCOMMAND_H
