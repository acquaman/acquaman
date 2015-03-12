#include <QApplication>

#include "CSRDataVisualization/CSRAppController.h"

int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("CSR Data Visualization");


	CSRAppController *appController = new CSRAppController();

	/// Program Run-loop:
	// =================================
	int retVal = -1;

	if (appController->start())
		retVal = app.exec();

	delete appController;

	return retVal;
}
