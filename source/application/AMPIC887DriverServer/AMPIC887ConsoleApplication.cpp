#include "AMPIC887ConsoleApplication.h"

#include <QTimer>

#include "AMConsoleInputHandler.h"
#include "AMPIC887ConsoleCommandParser.h"
#include "AMGCS2Support.h"

AMPIC887ConsoleApplication::AMPIC887ConsoleApplication(int argc, char *argv[]) :
	QCoreApplication(argc, argv)
{

	// Set up the console and command set.
	commandParser_ = new AMPIC887ConsoleCommandParser();
	consoleInputHandler_ = new AMConsoleInputHandler(commandParser_, this);


	if(startup()) {

		// Print a welcome message
		consoleInputHandler_->setCommandPrompt(
					QString("%1 > ")
					.arg(controllerCollection_.activeController()->name()));

		consoleInputHandler_->writeLineToStandardOutput("Welcome to the PI C887 Server Application.");
		consoleInputHandler_->writeLineToStandardOutput(controllerCollection_.connectionStatuses());
		consoleInputHandler_->writeLineToStandardOutput("Type 'help' for a list of commands.");
		consoleInputHandler_->displayCommandPrompt();
		connect(commandParser_, SIGNAL(help()), this, SLOT(onHelpCommandIssued()));
		connect(commandParser_, SIGNAL(quit()), this, SLOT(onQuitCommandIssued()));
		connect(commandParser_, SIGNAL(status()), this, SLOT(onStatusCommandIssued()));
		connect(commandParser_, SIGNAL(changeActiveController(QString)), this, SLOT(onActiveControllerChangeIssued(QString)));
		connect(commandParser_, SIGNAL(GCS2CommandIssued(AMGCS2Command*)), this, SLOT(onGCS2CommandIssued(AMGCS2Command*)));

	} else {

		// Startup failed. Print an error message, then quit the application.
		consoleInputHandler_->writeLineToStandardError("Could not startup the hexapod driver application.");

		quit();
	}
}

void AMPIC887ConsoleApplication::onHelpCommandIssued()
{
	consoleInputHandler_->writeLineToStandardOutput(commandParser_->commandList());
}

void AMPIC887ConsoleApplication::onQuitCommandIssued()
{
	consoleInputHandler_->writeLineToStandardOutput("Exiting...");
	quit();
}


void AMPIC887ConsoleApplication::onStatusCommandIssued()
{
	consoleInputHandler_->writeLineToStandardOutput(controllerCollection_.connectionStatuses());
}

void AMPIC887ConsoleApplication::onActiveControllerChangeIssued(const QString &controllerName)
{
	if(controllerCollection_.activeController()->name() != controllerName) {
		controllerCollection_.setActiveController(controllerName);
		if(controllerCollection_.activeController()->name() == controllerName) {
			consoleInputHandler_->setCommandPrompt(QString("%1 >").arg(controllerCollection_.activeController()->name()));
		} else {
			consoleInputHandler_->writeLineToStandardError(QString("No controller found with name %1").arg(controllerName));
		}
	}
}

void AMPIC887ConsoleApplication::onGCS2CommandIssued(AMGCS2Command* command)
{
	if(!controllerCollection_.activeController()) {
		return;
	}

	controllerCollection_.activeController()->runCommand(command);

	if(!command->wasSuccessful()) {
		consoleInputHandler_->writeLineToStandardError(
					QString("Error - Command was not run, with message: %1.")
					.arg(command->lastError()));
	} else if(!command->outputString().isEmpty()) {
		consoleInputHandler_->writeLineToStandardOutput(command->outputString());
	}

	delete command;
}

bool AMPIC887ConsoleApplication::startup()
{
	bool successfulLoad = true;

	successfulLoad &= startupControllers();
	return successfulLoad;
}

bool AMPIC887ConsoleApplication::startupControllers()
{
	controllerCollection_.append(new AMPIC887Controller("MC1611-401", "10.52.48.26"));

	return (controllerCollection_.activeController() != 0);
}



