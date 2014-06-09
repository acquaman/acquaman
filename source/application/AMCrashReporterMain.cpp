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
#include "AMCrashReporter.h"
#include <QDebug>

#include <signal.h>


int main(int argc, char *argv[])
{
	if(argc == 1)
		return -1;

	QString firstArgument = argv[1];
	if(firstArgument == "-m"){
		signal(SIGUSR1, AMCrashReporterUnixSignalHandler::sigusr1SignalHandler);
		signal(SIGUSR2, AMCrashReporterUnixSignalHandler::sigusr2SignalHandler);

		QCoreApplication coreApp(argc, argv);
		coreApp.setApplicationName("Acquaman Crash Monitor");

		AMCrashMonitor *crashMonitor = new AMCrashMonitor(coreApp.arguments().at(2), coreApp.arguments().at(3), coreApp.arguments().at(4).toInt());
		Q_UNUSED(crashMonitor)

		return coreApp.exec();
	}
	else{
		signal(SIGUSR1, AMCrashReporterUnixSignalHandler::sigusr1SignalHandler);

		QApplication app(argc, argv);
		app.setApplicationName("Acquaman Crash Reporter");

		AMCrashReporter *crashReporter = new AMCrashReporter(app.arguments().at(1), app.arguments().at(2), app.arguments().at(3).toInt(), app.arguments().at(4).toInt());
		crashReporter->show();

		return app.exec();
	}
}
