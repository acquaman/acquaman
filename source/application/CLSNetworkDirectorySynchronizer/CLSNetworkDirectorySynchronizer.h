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


#include <QObject>

#include <QStringList>

#include "util/AMDirectorySynchronizer.h"

class CLSNetworkDirectorySynchronizer : public QObject
{
Q_OBJECT
public:
	CLSNetworkDirectorySynchronizer(const QString &localPath, const QString &networkPath, QObject *parent = 0);
	virtual ~CLSNetworkDirectorySynchronizer();

	AMRecursiveDirectoryCompare::DirectoryCompareResult compareDirectories();
	bool startSynchronization();

	QString localPath() const;
	QString networkPath() const;

	QStringList errorMessages() const;

public slots:
	void setCloseOnCompletion(bool closeOnCompletion);

	void onProgressChanged(int percentCompleteFile, int remainingFilesToCopy, int totalFilesToCopy);

protected:
	QString localPath_;
	QString networkPath_;

	AMDirectorySynchronizer *directorySynchronizer_;

	bool closeOnCompletion_;
};
