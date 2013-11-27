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
#include <QFile>
#include <QProcess>
#include <QDir>
#include "application/SGM/SGMAppController.h"

#include <QDebug>

#include <signal.h>
#include <execinfo.h>

void handle_signal(int signum);
qint64 crashMonitorPID;
QFile *errorFile;

int main(int argc, char *argv[])
{
	signal(SIGSEGV, handle_signal);

	QFile localErrorFile(QString("/tmp/ErrorFile%1.txt").arg(getpid()));
	localErrorFile.open(QIODevice::WriteOnly | QIODevice::Text);
	errorFile = &localErrorFile;

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");

	QString applicationPath = app.arguments().at(0);
	QFileInfo applicationPathInfo(applicationPath);
	QString applicationRootPath;
	if(applicationPathInfo.isSymLink())
		applicationRootPath = applicationPathInfo.symLinkTarget().section('/', 0, -2);
	else
		applicationRootPath = applicationPathInfo.absoluteDir().path();

	//qDebug() << "Going to try to run " << applicationRootPath+"/AMCrashReporter";

	QStringList arguments;
	arguments << "-m";
	arguments << app.applicationFilePath();
	arguments << QString("%1").arg(getpid());
	QProcess::startDetached(applicationRootPath+"/AMCrashReporter", arguments, QDir::currentPath(), &crashMonitorPID);


	SGMAppController* appController = new SGMAppController();


	/// Program Run-loop:
	// =================================
	int retVal = -1;
	if(appController->startup())
		retVal = app.exec();

	/// Program Shutdown:
	// =================================
	if (appController->isRunning())
		appController->shutdown();

	kill(crashMonitorPID, SIGUSR2);
	delete appController;

	return retVal;
}

void handle_signal(int signum){
	void *array[100];
	size_t size;

	size = backtrace(array, 100);
	backtrace_symbols_fd(array, size, errorFile->handle());

	//qDebug() << "Handling SIGSEV, hopefully on PID " << getpid() << " need to notify PID " << crashMonitorPID;

	kill(crashMonitorPID, SIGUSR1);
	signal(signum, SIG_DFL);
	kill(getpid(), signum);
}
