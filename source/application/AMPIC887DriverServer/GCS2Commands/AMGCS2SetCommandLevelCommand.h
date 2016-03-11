#ifndef AMGCS2SETCOMMANDLEVELCOMMAND_H
#define AMGCS2SETCOMMANDLEVELCOMMAND_H
#include "AMGCS2Command.h"
#include "../AMGCS2.h"
/*!
  * \brief A command representing an instruction for the controller to update its
  * current command level to the provided command level. A password is required
  * only when setting the command level to Advanced.
  *
  * Normal is the default command level, and allows access to most commands, and
  * read-only access to all parameters.
  * Advanced adds additional commands and write access to level-1 parameters.
  * Access to higher than level-1 parameters are only available to PI support staff.
  */
class AMGCS2SetCommandLevelCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set command level command to set a controller
	  * to the provided command level using the provided password.
	  * \param commandLevel ~ The new command level that the controller will be
	  * set to.
	  * \param password ~ Optional password which is required for setting the controller
	  * to the Advanced command level.
	  */
	AMGCS2SetCommandLevelCommand(AMGCS2::ControllerCommandLevel commandLevel,
								 const QString& password = QString());

	/*!
	  * Virtual destructor for the set command level command.
	  */
	virtual ~AMGCS2SetCommandLevelCommand() {}

protected:
	/*!
	  * Ensures that a password has been provided if the new command level is
	  * Advanced, and if the command level is Unknown.
	  * \returns False if no password has been provided to set to the advanced
	  * command level or if the command level is Unknown, true otherwise.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to set the command level of the controller.
	  */
	virtual bool runImplementation();

	AMGCS2::ControllerCommandLevel newCommandLevel_;
	QString password_;
};

#endif // AMGCS2SETCOMMANDLEVELCOMMAND_H
