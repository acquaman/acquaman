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


#include <QCoreApplication>
#include <QStringList>
#include <QDir>

#include <QDebug>

#include "AMDeploy.h"

int main(int argc, char *argv[])
{
	QCoreApplication coreApp(argc, argv);
	coreApp.setApplicationName("Deploy Acquaman");

	QStringList arguments = coreApp.arguments();
	if(arguments.count() < 2){
		qDebug() << "Invalid use. Expected either AMDeploy <project file> or AMDeploy <project file> <working directory>";
		return -1;
	}

	if(arguments.count() >= 2 && !arguments.at(1).contains(".pro")){
		qDebug() << "Invalid use. Expected either AMDeploy <project file> or AMDeploy <project file> <working directory>";
		return -2;
	}

	QString projectFile = arguments.at(1);
	QString workingDirectory;
	if(arguments.count() >= 3)
		workingDirectory = arguments.at(2);
	else
		workingDirectory = QDir::currentPath();

	AMDeploy *deployment = new AMDeploy(projectFile, workingDirectory);
	Q_UNUSED(deployment)

	return coreApp.exec();
}
