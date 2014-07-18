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


#include "CLSNetworkDirectorySynchronizer.h"

#include <QCoreApplication>
#include <QDebug>

CLSNetworkDirectorySynchronizer::CLSNetworkDirectorySynchronizer(const QString &localPath, const QString &networkPath, QObject *parent) :
	QObject(parent)
{
	localPath_ = localPath;
	networkPath_ = networkPath;

	closeOnCompletion_ = true;

	directorySynchronizer_ = new AMDirectorySynchronizer(localPath_, networkPath_, this);
	directorySynchronizer_->setAllowSide1Creation(true);

	directorySynchronizer_->setSide1DirectorName("Local");
	directorySynchronizer_->setSide2DirectorName("Network");

	connect(directorySynchronizer_, SIGNAL(progressChanged(int,int,int)), this, SLOT(onProgressChanged(int,int,int)));
}

CLSNetworkDirectorySynchronizer::~CLSNetworkDirectorySynchronizer()
{
}

AMRecursiveDirectoryCompare::DirectoryCompareResult CLSNetworkDirectorySynchronizer::compareDirectories(){
	return directorySynchronizer_->prepare();
}

bool CLSNetworkDirectorySynchronizer::startSynchronization(){
	if(closeOnCompletion_){
		connect(directorySynchronizer_, SIGNAL(copyCompleted()), QCoreApplication::instance(), SLOT(quit()));
		connect(directorySynchronizer_, SIGNAL(copyFailed()), QCoreApplication::instance(), SLOT(quit()));
	}

	return directorySynchronizer_->start();
}

QString CLSNetworkDirectorySynchronizer::localPath() const{
	return localPath_;
}

QString CLSNetworkDirectorySynchronizer::networkPath() const{
	return networkPath_;
}

QStringList CLSNetworkDirectorySynchronizer::errorMessages() const{
	return directorySynchronizer_->errorMessages();
}

void CLSNetworkDirectorySynchronizer::setCloseOnCompletion(bool closeOnCompletion){
	closeOnCompletion_ = closeOnCompletion;
}

void CLSNetworkDirectorySynchronizer::onProgressChanged(int percentCompleteFile, int remainingFilesToCopy, int totalFilesToCopy){
	qDebug() << QString("%1\% (%2 of %3)").arg(percentCompleteFile).arg(totalFilesToCopy-remainingFilesToCopy).arg(totalFilesToCopy);
}
