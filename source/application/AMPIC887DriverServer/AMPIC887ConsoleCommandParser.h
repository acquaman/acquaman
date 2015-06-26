#ifndef AMPIC887CONSOLECOMMANDPARSER_H
#define AMPIC887CONSOLECOMMANDPARSER_H

#include <QStringList>

#include "AMAbstractConsoleCommandParser.h"

/*!
  * A class which represents the commands which a user can input to the Hexapod
  * C887 Controller driver server system.
  */
class AMGCS2Command;
class AMPIC887ConsoleCommandParser : public AMAbstractConsoleCommandParser
{
	Q_OBJECT
public:
	/*!
	  * Creates a new instance of hexapod driver console command set.
	  */
	AMPIC887ConsoleCommandParser(QObject* parent = 0);

	/*!
	  * Standard virtual destructor for a hexapod driver console command set.
	  */
	virtual ~AMPIC887ConsoleCommandParser() {}

	/*!
	  * Helper method which lists the current available commands.
	  */
	QString commandList();
signals:
	/*!
	  * Signals that the quit command has been issued.
	  */
	void quit();

	/*!
	  * Signals that the help command has been issued.
	  */
	void help();

	/*!
	  * Signals that the staus command has been issued.
	  */
	void status();

	/*!
	  * Signals that the command to switch to a different active controller has
	  * been issued.
	  * \param controllerName ~ The name of the controller to make the active
	  * controller.
	  */
	void changeActiveController(const QString& controllerName);

	/*!
	  * Signals that a command other than the standard ones handled by the parser
	  * has been issued.
	  */
	void otherCommandIssued(const QString& commandString);
protected:
	/*!
	  * Interprets the provided command using the list of acceptable commands which
	  * can be issued to the hexapod driver console.
	  * \param command ~ The command which was issued to the hexapod driver console.
	  */
	virtual void interpretCommandImplementation(const QString &command);
};

#endif // AMPIC887CONSOLECOMMANDSET_H
