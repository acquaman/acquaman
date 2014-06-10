#include <QApplication>
#include "application/BioXAS/BioXASSideAppController.h"

int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");


	BioXASSideAppController* appController = new BioXASSideAppController();


	/// Program Run-loop:
	// =================================
	int retVal = -1;
	if(appController->startup())
		retVal = app.exec();

	/// Program Shutdown:
	// =================================
	if (appController->isRunning())
		appController->shutdown();

	delete appController;

	return retVal;
}

