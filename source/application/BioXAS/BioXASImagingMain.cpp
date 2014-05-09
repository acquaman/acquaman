#include <QApplication>
#include "application/BioXAS/BioXASImagingAppController.h"

int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");


	BioXASImagingAppController* appController = new BioXASImagingAppController();


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

