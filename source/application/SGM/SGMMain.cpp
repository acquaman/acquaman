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
#include <QDebug>
#include <QHostInfo>
#include <QProcess>
#include <QDir>
#include "application/SGM/SGMAppController.h"

#include <signal.h>

void handle_signal(int signum);
qint64 crashReporterPID;
QFile *errorFile;

int main(int argc, char *argv[])
{
//	signal(SIGSEGV, handle_signal);

//	QFile localErrorFile("ErrorFile.txt");
//	localErrorFile.open(QIODevice::WriteOnly | QIODevice::Text);
//	errorFile = &localErrorFile;

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");

//	QStringList arguments;
//	arguments << "-m";
//	arguments << app.applicationFilePath();
//	//arguments << "/home/chevrid/testingCoreDumps/runAcquamanCoreBackTraceCommands.txt";
//	arguments << QString("%1").arg(getpid());
//	QProcess::startDetached("/home/sgm/beamline/programming/acquamanTestSandbox/build/AMCrashReporter", arguments, QDir::currentPath(), &crashReporterPID);

	qDebug() << "Local host? " << QHostInfo::localHostName();
	QString pathEnv = getenv(QString("PATH").toAscii().data());
	if(QHostInfo::localHostName() == "OPI1611-408" || QHostInfo::localHostName() == "OPI1611-412"){
		QString ldLibraryPathEnv = getenv(QString("LD_LIBRARY_PATH").toAscii().data());
		QString newPath = QString("%1:%2").arg("/home/beamline/tools/gstreamer-0.10.35/gstreamer-install/bin:/home/beamline/tools/gstreamer-0.10.35/deps/bin").arg(pathEnv);
		QString newLdLibraryPath = QString("%1:%2").arg("/home/beamline/tools/gstreamer-0.10.35/gstreamer-install/lib:/home/beamline/tools/gstreamer-0.10.35/deps/lib").arg(ldLibraryPathEnv);
		setenv(QString("PATH").toAscii().data(), newPath.toAscii().data(), 1);
		setenv(QString("LD_LIBRARY_PATH").toAscii().data(), newLdLibraryPath.toAscii().data(), 1);

		QString pathEnvAfter = getenv(QString("PATH").toAscii().data());
		QString ldLibraryPathEnvAfter = getenv(QString("LD_LIBRARY_PATH").toAscii().data());
		qDebug() << "New path is " << pathEnvAfter;
		qDebug() << "New ld library path is " << ldLibraryPathEnvAfter;
	}
	else if(QHostInfo::localHostName() == "nbk-m001074.clsi.ca"){
		QString dyldLibraryPathEnv = getenv(QString("DYLD_LIBRARY_PATH").toAscii().data());
		QString newPath = QString("%1:%2").arg("/Users/fawkes/dev/gstreamer-0.10.35/gstreamer-install/bin:/Users/fawkes/dev/gstreamer-0.10.35/deps/bin").arg(pathEnv);
		QString newDyldLibraryPath = QString("%1:%2").arg("/Users/fawkes/dev/gstreamer-0.10.35/gstreamer-install/lib:/Users/fawkes/dev/gstreamer-0.10.35/deps/lib").arg(dyldLibraryPathEnv);
		setenv(QString("PATH").toAscii().data(), newPath.toAscii().data(), 1);
		setenv(QString("DYLD_LIBRARY_PATH").toAscii().data(), newDyldLibraryPath.toAscii().data(), 1);

		QString pathEnvAfter = getenv(QString("PATH").toAscii().data());
		QString dyldLibraryPathEnvAfter = getenv(QString("DYLD_LIBRARY_PATH").toAscii().data());
		qDebug() << "New path is " << pathEnvAfter;
		qDebug() << "New dyld library path is " << dyldLibraryPathEnvAfter;
	}



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

	delete appController;

	return retVal;
}

#include <execinfo.h>

void handle_signal(int signum){
	void *array[100];
	size_t size;

	size = backtrace(array, 100);
	//fprintf(stderr, "Error: ");
	//backtrace_symbols_fd(array, size, STDERR_FILENO);
	backtrace_symbols_fd(array, size, errorFile->handle());
	//exit(1);

	qDebug() << "Handling SIGSEV, hopefully on PID " << getpid() << " need to notify PID " << crashReporterPID;

	kill(crashReporterPID, SIGUSR1);
	signal(signum, SIG_DFL);
	kill(getpid(), signum);
}
