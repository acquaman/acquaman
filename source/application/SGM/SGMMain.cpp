/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

//#include <QFile>
//#include <QProcess>
//#include <QDir>
#include <QHostInfo>
#include "application/SGM/SGMAppController.h"
#include "application/AMCrashMonitorSupport.h"

#include <QDebug>

#ifndef Q_WS_MAC
//#include <signal.h>
//#include <execinfo.h>

//void handle_signal(int signum);
//qint64 crashMonitorPID;
//QFile *errorFile;
#endif

int main(int argc, char *argv[])
{
#ifndef Q_WS_MAC
//	signal(SIGSEGV, handle_signal);

//	QFile localErrorFile(QString("/tmp/ErrorFile%1.txt").arg(getpid()));
//	localErrorFile.open(QIODevice::WriteOnly | QIODevice::Text);
//	errorFile = &localErrorFile;
#endif

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");

#ifndef Q_WS_MAC
//	QString applicationPath = app.arguments().at(0);
//	QFileInfo applicationPathInfo(applicationPath);
//	QString applicationRootPath;
//	if(applicationPathInfo.isSymLink())
//		applicationRootPath = applicationPathInfo.symLinkTarget().section('/', 0, -2);
//	else
//		applicationRootPath = applicationPathInfo.absoluteDir().path();


//	//applicationRootPath.replace("SGMAcquaman.app", "AMCrashReporter.app");

//	QStringList arguments;
//	arguments << "-m";
//	arguments << app.applicationFilePath();
//	arguments << "/home/acquaman/AcquamanApplicationCrashReports";
//	arguments << QString("%1").arg(getpid());
//	QProcess::startDetached(applicationRootPath+"/AMCrashReporter", arguments, QDir::currentPath(), &crashMonitorPID);
#endif

	SGMAppController* appController = new SGMAppController();

#ifndef Q_WS_MAC
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
	AMCrashMonitorSupport::s()->report();
//	kill(crashMonitorPID, SIGUSR2);
#endif
	delete appController;

	return retVal;
}


#ifndef Q_WS_MAC
//void handle_signal(int signum){
//	void *array[100];
//	size_t size;

//	size = backtrace(array, 100);
//	backtrace_symbols_fd(array, size, errorFile->handle());

//	kill(crashMonitorPID, SIGUSR1);
//	signal(signum, SIG_DFL);
//	kill(getpid(), signum);
//}
#endif
