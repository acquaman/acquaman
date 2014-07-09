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


#include <QCoreApplication>
#include <QStringList>
#include <QDir>
#include <QDebug>

#include "CLSNetworkDirectorySynchronizer.h"

int main(int argc, char *argv[])
{
	QCoreApplication coreApp(argc, argv);
	coreApp.setApplicationName("Synchronize Directories");

	QStringList arguments = coreApp.arguments();
	QStringList keyArguments;
	for(int x = 0, size = arguments.count(); x < size; x++)
		if(!arguments.at(x).contains("--"))
			keyArguments.append(arguments.at(x));

	if(keyArguments.count() != 3){
		qDebug() << "Invalid use. Expected CLSNetworkDirectorySynchronizer <local directory> <network directory>";
		return -1;
	}

	QString localDirectory = keyArguments.at(1);
	QString networkDirectory = keyArguments.at(2);

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

	//return coreApp.exec();
	return 0;
}
