#include <QApplication>
#include "application/VESPERS/VESPERSDatamanAppController.h"


int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Dataman");


	VESPERSDatamanAppController* appController = new VESPERSDatamanAppController();


	/// Program Run-loop:
	// =================================
	int retVal = -1;
	if(appController->startup())
		retVal = app.exec();

	/// Program Shutdown:
	// =================================
	delete appController;

	return retVal;
}
