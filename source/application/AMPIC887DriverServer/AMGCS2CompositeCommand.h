#ifndef AMGCS2COMPOSITECOMMAND_H
#define AMGCS2COMPOSITECOMMAND_H

#include <QList>

#include "AMGCS2Command.h"
/*!
  * A command class which stores a sublist of commands to be executed sequentially.
  */
class AMGCS2CompositeCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an empty composite command containing no sub commands.
	  */
    AMGCS2CompositeCommand();

	/*!
	  * Virtual destructor for a composite command
	  */
	virtual ~AMGCS2CompositeCommand();

	/*!
	  * Adds a command to the end of the list of contained sub commands.
	  * \param command ~ The command to add to the end of the list of sub commands.
	  */
	void addCommand(AMGCS2Command* command);

	/*!
	  * The output of all the contained subcommands output when run. If the commands
	  * have not yet been run, or were not run successfully, this will return the
	  * empty string.
	  */
	virtual QString outputString() const;
protected:
	/*!
	  * Ensures that this composite command contains at least one subcommand.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps to be taken to run the list of subcommands. Will halt
	  * if any of the sub commands fails to run successfully.
	  */
	virtual bool runImplementation();

	QList<AMGCS2Command*> commandList_;
	QString commandOutputs_;
};

#endif // AMGCS2COMPOSITECOMMAND_H
