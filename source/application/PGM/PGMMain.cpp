/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/





#include <QApplication>
#include <QFile>
#include "application/PGM/PGMAppController.h"
#include "application/AMCrashMonitorSupport.h"

int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");


	PGMAppController* appController = new PGMAppController();

#ifndef Q_WS_MAC
	// Make a local QFile for the error file. It needs to be in this scope and get passed into AMCrashMonitorSupport, otherwise it won't work properly
	// After doing so, star the monitor
	// Ignore all of this for Mac OSX, it has it's own crash reporter and the two seem to compete
	QFile localErrorFile(QString("/tmp/ErrorFile%1.txt").arg(getpid()));
	localErrorFile.open(QIODevice::WriteOnly | QIODevice::Text);
	AMCrashMonitorSupport::s()->setErrorFile(&localErrorFile);
	AMCrashMonitorSupport::s()->setPathToCrashReportFiles("/home/acquaman/AcquamanApplicationCrashReports/IDEAS");
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

