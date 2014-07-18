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
#include <QStringList>
#include <QDir>
#include <QDebug>

#include "CLSNetworkDirectorySynchronizer.h"
#include "ui/util/AMDirectorySynchronizerDialog.h"

int main(int argc, char *argv[])
{
	if(argc == 1)
		return -1;

	QStringList arguments;
	QStringList keyArguments;

	QString localDirectory;
	QString networkDirectory;

	QCoreApplication *application = 0;

	QString firstArgument = argv[1];
	if(firstArgument == "-i"){
		QApplication *app = new QApplication(argc, argv);
		app->setApplicationName("Synchronize Directories");

		application = app;
	}
	else{
		QCoreApplication *coreApp = new QCoreApplication(argc, argv);
		coreApp->setApplicationName("Synchronize Directories");

		application = coreApp;
	}

	arguments = application->arguments();
	for(int x = 0, size = arguments.count(); x < size; x++)
		if(!arguments.at(x).contains("--"))
			keyArguments.append(arguments.at(x));

	if(keyArguments.count() < 3){
		qDebug() << "Invalid use. Expected CLSNetworkDirectorySynchronizer <local directory> <network directory>";
		return -1;
	}

	if(firstArgument == "-i"){
		localDirectory = keyArguments.at(2);
		networkDirectory = keyArguments.at(3);
	}
	else{
		localDirectory = keyArguments.at(1);
		networkDirectory = keyArguments.at(2);
	}

	if(firstArgument == "-i"){
		AMDirectorySynchronizerDialog *synchronizerDialog = new AMDirectorySynchronizerDialog(localDirectory, networkDirectory, "Local", "Network");
		synchronizerDialog->setCloseOnCompletion(true);
		synchronizerDialog->setTimedWarningOnCompletion(true);
		synchronizerDialog->raise();

		if(arguments.contains("--autoPrepare"))
			synchronizerDialog->autoPrepare();
		else if(arguments.contains("--autoStart"))
			synchronizerDialog->autoStart();
		else
			synchronizerDialog->exec();
	}
	else{

		CLSNetworkDirectorySynchronizer *synchronizer = new CLSNetworkDirectorySynchronizer(localDirectory, networkDirectory);

		if(arguments.contains("--dryRun")){
			AMRecursiveDirectoryCompare::DirectoryCompareResult directoryCompareResult = synchronizer->compareDirectories();

			switch(directoryCompareResult){
			case AMRecursiveDirectoryCompare::FullyMatchingResult:
				qDebug() << QString("The local directory %1 is identical to the network directory %2.").arg(synchronizer->localPath()).arg(synchronizer->networkPath());
				break;
			case AMRecursiveDirectoryCompare::Side1ModifiedResult:
				qDebug() << QString("The local directory %1 is has been modified and is newer than the network directory %2.").arg(synchronizer->localPath()).arg(synchronizer->networkPath());
				break;
			case AMRecursiveDirectoryCompare::Side2ModifiedResult:
				qDebug() << QString("The network directory %2 is has been modified and is newer than the local directory %1.").arg(synchronizer->localPath()).arg(synchronizer->networkPath());
				break;
			case AMRecursiveDirectoryCompare::BothSidesModifiedResult:
				qDebug() << QString("Both the local directory %1 and the network directory %2 have been modified, this is a serious problem.").arg(synchronizer->localPath()).arg(synchronizer->networkPath());
				break;
			case AMRecursiveDirectoryCompare::UnableToDetermineResult:
				qDebug() << QString("Unable to determine if the local directory %1 and the network directory %2 have been modified, this is a serious problem.").arg(synchronizer->localPath()).arg(synchronizer->networkPath());
				break;
			case AMRecursiveDirectoryCompare::InvalidResult:
				qDebug() << QString("A serious unknown error happened comparing the local directory %1 and the network directory %2 have been modified.").arg(synchronizer->localPath()).arg(synchronizer->networkPath());
				break;
			default:
				qDebug() << QString("There must be a compiling error in the program, cannot compare the local directory %1 and the network directory %2.").arg(synchronizer->localPath()).arg(synchronizer->networkPath());
				break;
			}

			return 0;
		}

		if(!synchronizer->startSynchronization()){
			qDebug() << synchronizer->errorMessages();
			return -1;
		}
	}

	return application->exec();
}
