#include "AMPIC887ConsoleApplication.h"

#include <QTimer>

#include "AMConsoleInputHandler.h"
#include "AMPIC887ConsoleCommandParser.h"
#include "AMGCS2Support.h"
#include "AMGCS2CurrentPositionCommand.h"

AMPIC887ConsoleApplication::AMPIC887ConsoleApplication(int argc, char *argv[]) :
	QCoreApplication(argc, argv)
{

	// Set up the console and command set.
	commandParser_ = new AMPIC887ConsoleCommandParser();
	consoleApplication_ = new AMConsoleInputHandler(commandParser_, this);


	if(startup()) {

		// Print a welcome message
		consoleApplication_->setCommandPrompt(
					QString("%1 > ")
					.arg(controllerCollection_.activeController()->name()));

		consoleApplication_->writeLineToStandardOutput("Welcome to the PI C887 Server Application.");
		consoleApplication_->writeLineToStandardOutput(controllerCollection_.connectionStatuses());
		consoleApplication_->writeLineToStandardOutput("Type 'help' for a list of commands.");
		consoleApplication_->displayCommandPrompt();
		connect(commandParser_, SIGNAL(help()), this, SLOT(onHelpCommandIssued()));
		connect(commandParser_, SIGNAL(quit()), this, SLOT(onQuitCommandIssued()));
		connect(commandParser_, SIGNAL(GCS2CommandIssued(AMGCS2Command*)), this, SLOT(onGCS2CommandIssued(AMGCS2Command*)));

	} else {

		// Startup failed. Print an error message, then startup a timer to close
		// the application after a few seconds.
		consoleApplication_->writeLineToStandardError("Could not startup the hexapod driver application.");

		QTimer* exitTimer = new QTimer(this);
		exitTimer->setInterval(500);

		connect(exitTimer, SIGNAL(timeout()), this, SLOT(quit()));
		exitTimer->start();
	}
}

void AMPIC887ConsoleApplication::onHelpCommandIssued()
{
	consoleApplication_->writeLineToStandardOutput(commandParser_->commandList());
}

void AMPIC887ConsoleApplication::onQuitCommandIssued()
{
	consoleApplication_->writeLineToStandardOutput("Exiting...");
	quit();
}

void AMPIC887ConsoleApplication::onGCS2CommandIssued(AMGCS2Command* command)
{

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

	if(controllerCollection_.activeController()) {
		return true;
	} else {
		return false;
	}
}
