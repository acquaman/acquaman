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


#ifndef VESPERSDATABASEDUPLICATEENTRYPATCH_H
#define VESPERSDATABASEDUPLICATEENTRYPATCH_H

#include <QObject>

/// Class that takes a database, finds all duplicate entries and removes them.  Fixes all associated tables and references as well.
class VESPERSDatabaseDuplicateEntryPatch : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit VESPERSDatabaseDuplicateEntryPatch(QObject *parent = 0);

signals:

public slots:
	/// Starts the patch process.
	bool start();

};

#endif // VESPERSDATABASEDUPLICATEENTRYPATCH_H
