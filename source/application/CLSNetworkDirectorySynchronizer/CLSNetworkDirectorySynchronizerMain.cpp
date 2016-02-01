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
#include <QDesktopWidget>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QMap>

#include "CLSNetworkDirectorySynchronizer.h"
#include "ui/util/AMDirectorySynchronizerDialog.h"

int main(int argc, char *argv[])
{
	QStringList arguments;
	QMap<QString, QString> argumentValuePairs;

	QString localDirectory;
	QString networkDirectory;

	QCoreApplication *application = 0;

	QString firstArgument;
	if(argc > 1)
		firstArgument = argv[1];

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

	for(int x = 0, size = arguments.count(); x < size; x++){
		if(arguments.at(x).contains('='))
			argumentValuePairs.insert(arguments.at(x).section('=', 0, 0), arguments.at(x).section('=', 1, 1));
		else
			argumentValuePairs.insert(arguments.at(x), QString());
	}

	QString errorTitle;
	QString errorMessage;

	if(argumentValuePairs.contains("--local") && !argumentValuePairs.value("--local").isEmpty() && argumentValuePairs.contains("--remote") && !argumentValuePairs.value("--remote").isEmpty()){
		localDirectory = argumentValuePairs.value("--local");
		networkDirectory = argumentValuePairs.value("--remote");
	}
	else{
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Acquaman", "Acquaman");

		if(settings.contains("userDataFolder") && settings.contains("remoteDataFolder")){
			localDirectory = settings.value("userDataFolder").toString();
			networkDirectory = settings.value("remoteDataFolder").toString();
		}
		else if(settings.contains("remoteDataFolder")){
			errorTitle = "A Serious Error Occured";
			errorMessage = "The syncrhonizer can only find information for the remoteDataFolder but not the userDataFolder.\nThis is bad enough that we need to stop right now.\nFind an Acquaman programmer to help.";
		}
		else if(settings.contains("userDataFolder")){
			QString currentUserDataFolder = settings.value("userDataFolder").toString();
			QString localBaseDirectory = "/AcquamanLocalData";

			QString dataFolderExtension = currentUserDataFolder;
			if(currentUserDataFolder.contains("/home"))
				dataFolderExtension.remove("/home");
			else if(currentUserDataFolder.contains("/experiments"))
				dataFolderExtension.remove("/experiments");
			else
				dataFolderExtension.remove(currentUserDataFolder.section('/', 0, 1));

			QString newUserDataFolder = localBaseDirectory+dataFolderExtension;
			QString newRemoteDataFolder = currentUserDataFolder;

			settings.setValue("remoteDataFolder", newRemoteDataFolder);
			settings.setValue("userDataFolder", newUserDataFolder);

			localDirectory = settings.value("userDataFolder").toString();
			networkDirectory = settings.value("remoteDataFolder").toString();
		}
		else{
			errorTitle = "First Time User Detected";
			errorMessage = "Looks like this is a first time user.\nIt will be much simpler to run Acquaman to get things set up.";
		}
	}

	QStringList excludePatterns;
	excludePatterns.append("*.db.bk.*");
	excludePatterns.append("*.BACKUPS");
	excludePatterns.append("*.db-journal");
	excludePatterns.append("*exportData*");

	if(firstArgument == "-i"){
		if(!errorTitle.isEmpty() || !errorMessage.isEmpty()){
			QMessageBox::critical(0, errorTitle, errorMessage, QMessageBox::Ok);
			return -1;
		}

		if(argumentValuePairs.contains("--checkForJournals")){
			QDir localDir(localDirectory);
			QFileInfoList localDirEntries = localDir.entryInfoList();
			for(int x = 0, size = localDirEntries.count(); x < size; x++){
				if(localDirEntries.at(x).absoluteFilePath().contains(".db-journal")){
					QMessageBox::critical(0, "A database journal file was detected", "A database journal file was detected.\n\n Please run Acquaman to handle this problem.", QMessageBox::Ok);
					return -1;
				}

			}
		}

		AMDirectorySynchronizerDialog *synchronizerDialog = new AMDirectorySynchronizerDialog(localDirectory, networkDirectory, "Local", "Network");
		synchronizerDialog->setCloseOnCompletion(true);
		if(!argumentValuePairs.contains("--noWarningTimeout"))
			synchronizerDialog->setTimedWarningOnCompletion(true);
		for(int x = 0, size = excludePatterns.size(); x < size; x++)
			synchronizerDialog->appendExcludePattern(excludePatterns.at(x));
		synchronizerDialog->raise();

		QDesktopWidget *desktop = QApplication::desktop();
		QRect ourScreen = desktop->screenGeometry(desktop->primaryScreen());
		synchronizerDialog->move(ourScreen.width()/4 + ourScreen.left(), ourScreen.height()/2 + ourScreen.top());

		if(argumentValuePairs.contains("--delayedStart")){
			synchronizerDialog->delayedStart(argumentValuePairs.value("--delayedStart").toDouble());
			synchronizerDialog->exec();
		}
		else if(argumentValuePairs.contains("--autoPrepare"))
			synchronizerDialog->autoPrepare();
		else if(argumentValuePairs.contains("--autoStart"))
			synchronizerDialog->autoStart();
		else
			synchronizerDialog->exec();
	}
	else{
		if(!errorTitle.isEmpty() || !errorMessage.isEmpty()){
			qDebug() << "\n" << errorTitle;
			qDebug() << errorMessage << "\n";
			return -1;
		}

		if(argumentValuePairs.contains("--checkForJournals")){
			QDir localDir(localDirectory);
			QFileInfoList localDirEntries = localDir.entryInfoList();
			for(int x = 0, size = localDirEntries.count(); x < size; x++){
				if(localDirEntries.at(x).absoluteFilePath().contains(".db-journal")){
					qDebug() << "A database journal file was detected.";
					qDebug() << "Please run Acquaman to handle this problem.";
					return -1;
				}

			}
		}

		AMDirectorySynchronizer *synchronizer = new AMDirectorySynchronizer(localDirectory, networkDirectory);
		synchronizer->setSide1DirectorName("Local");
		synchronizer->setSide2DirectorName("Network");
		for(int x = 0, size = excludePatterns.size(); x < size; x++)
			synchronizer->appendExcludePattern(excludePatterns.at(x));

		if(argumentValuePairs.contains("--dryRun")){
			AMRecursiveDirectoryCompare::DirectoryCompareResult directoryCompareResult = synchronizer->prepare();

			switch(directoryCompareResult){
			case AMRecursiveDirectoryCompare::FullyMatchingResult:
				qDebug() << QString("The local directory %1 is identical to the network directory %2.").arg(synchronizer->side1Directory()).arg(synchronizer->side2Directory());
				break;
			case AMRecursiveDirectoryCompare::Side1ModifiedResult:
				qDebug() << QString("The local directory %1 is has been modified and is newer than the network directory %2.").arg(synchronizer->side1Directory()).arg(synchronizer->side2Directory());
				break;
			case AMRecursiveDirectoryCompare::Side2ModifiedResult:
				qDebug() << QString("The network directory %2 is has been modified and is newer than the local directory %1.").arg(synchronizer->side1Directory()).arg(synchronizer->side2Directory());
				break;
			case AMRecursiveDirectoryCompare::BothSidesModifiedResult:
				qDebug() << QString("Both the local directory %1 and the network directory %2 have been modified, this is a serious problem.").arg(synchronizer->side1Directory()).arg(synchronizer->side2Directory());
				break;
			case AMRecursiveDirectoryCompare::UnableToDetermineResult:
				qDebug() << QString("Unable to determine if the local directory %1 and the network directory %2 have been modified, this is a serious problem.").arg(synchronizer->side1Directory()).arg(synchronizer->side2Directory());
				break;
			case AMRecursiveDirectoryCompare::InvalidResult:
				qDebug() << QString("A serious unknown error happened comparing the local directory %1 and the network directory %2 have been modified.").arg(synchronizer->side1Directory()).arg(synchronizer->side2Directory());
				break;
			default:
				qDebug() << QString("There must be a compiling error in the program, cannot compare the local directory %1 and the network directory %2.").arg(synchronizer->side1Directory()).arg(synchronizer->side2Directory());
				break;
			}

			return 0;
		}

		if(!synchronizer->start()){
			qDebug() << synchronizer->errorMessages();
			return -1;
		}
	}

	return application->exec();
}
