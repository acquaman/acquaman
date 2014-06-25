#include <QApplication>
#include <QFile>

#include "application/BioXAS/BioXASImagingAppController.h"
#include "application/AMCrashMonitorSupport.h"

int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");


	BioXASImagingAppController* appController = new BioXASImagingAppController();

#ifndef Q_WS_MAC
	// Make a local QFile for the error file. It needs to be in this scope and get passed into AMCrashMonitorSupport, otherwise it won't work properly
	// After doing so, star the monitor
	// Ignore all of this for Mac OSX, it has it's own crash reporter and the two seem to compete
	QFile localErrorFile(QString("/tmp/ErrorFile%1.txt").arg(getpid()));
	localErrorFile.open(QIODevice::WriteOnly | QIODevice::Text);
	AMCrashMonitorSupport::s()->setErrorFile(&localErrorFile);
	AMCrashMonitorSupport::s()->setPathToCrashReportFiles("/home/acquaman/AcquamanApplicationCrashReports/BioXAS/");
	AMCrashMonitorSupport::s()->monitor();
#endif

	/// Program Run-loop:
	// =================================
	int retVal = -1;
	if(appController->startup())
		retVal = app.exec();

	/// Program Shutdown:
	// =================================
	if (appController->isRunning())
		appController->shutdown();

#ifndef Q_WS_MAC
	// Make sure we have the crash reporter system actually generate a report
	// Ignore all of this for Mac OSX, it has it's own crash reporter and the two seem to compete
	AMCrashMonitorSupport::s()->report();
#endif

	delete appController;

	return retVal;
}

