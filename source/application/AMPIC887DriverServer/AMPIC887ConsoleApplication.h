#ifndef AMPIC887CONSOLEAPPLICATION_H
#define AMPIC887CONSOLEAPPLICATION_H

#include <QCoreApplication>

#include "AMPIC887Controller.h"
#include "AMPIC887ControllerCollection.h"

class AMConsoleInputHandler;
class AMPIC887ConsoleCommandParser;
/*!
  * A QCoreApplication derived class which represents the Qt Application entry
  * point for the driver server. Customized to initialize the controllers, the
  * input handler and the command set.
  */
class AMPIC887ConsoleApplication : public QCoreApplication
{
    Q_OBJECT
public:
	/*!
	  * Creates a new instance of an AMPIC887ConsoleApplication, with the provided
	  * command line arguments.
	  * \param argc ~ The number of command line arguments passed to the application.
	  * \param argv ~ A collection of the command line arguments passed to the
	  * application.
	  */
	explicit AMPIC887ConsoleApplication(int argc, char *argv[]);

signals:

public slots:
protected slots:
	/*!
	  * Handles signals indicating that the help command has been issued. Writes
	  * the command parsers list of commands to the console.
	  */
	void onHelpCommandIssued();

	/*!
	  * Handles signals indicating that the quit command has been issued. Prints
	  * an exit message then closes the application.
	  */
	void onQuitCommandIssued();

	/*!
	  * Handles signals indicating that the status command has been issued. Prints
	  * the controller collections current status.
	  */
	void onStatusCommandIssued();

	/*!
	  * Handles signals indicating that the active controller command has been issued.
	  * Sets the active controller to the first located controller in the collection
	  * which matches the passed name.
	  * \param controllerName ~ The name of the controller to be made active.
	  */
	void onActiveControllerChangeIssued(const QString& controllerName);

	/*!
	  * Handles signals indicating that a command which does not form part of the
	  * usual command parsers list (ie. help, quit, activate etc). In our case
	  * this needs to be passed along to the active controller to see if it is
	  * a GCS2 command
	  * \param commandString ~ The text input to the console which is to be
	  * interpreted by a controller.
	  */
	void onOtherCommandIssued(const QString& commandString);

protected:
	/*!
	  * Runs the initialization procedure for the application.
	  * \returns True if startup was successful, false otherwise.
	  */
	bool startup();

	/*!
	  * Initializes all the controllers which the application is to connect to.
	  * \returns True if at least one controller could be connected to, false otherwise.
	  */
	bool startupControllers();

	AMConsoleInputHandler* consoleInputHandler_;
	AMPIC887ConsoleCommandParser* commandParser_;
	AMPIC887ControllerCollection controllerCollection_;
};

#endif // AMPIC887CONSOLEAPPLICATION_H
