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


#include "VESPERSDatabaseDuplicateEntryPatch.h"

#include <QDebug>
#include <QStringBuilder>

#include "dataman/database/AMDatabase.h"
#include "util/AMErrorMonitor.h"
#include "dataman/VESPERS/VESPERSDatabaseDuplicateEntryObject.h"

VESPERSDatabaseDuplicateEntryPatch::VESPERSDatabaseDuplicateEntryPatch(QObject *parent) :
	QObject(parent)
{
}

bool VESPERSDatabaseDuplicateEntryPatch::start()
{
	AMDatabase *database = AMDatabase::createDatabase("Patch", "/nas/vespers/users/17-5255/userData/userdata.db");

	QSqlQuery query = database->query();
	query.prepare("SELECT id, measurementId, name FROM AMRawDataSource_table;");

	if (!database->execQuery(query)){

		query.finish();
		AMErrorMon::alert(this, 0, "Could not read from AMRawDataSource_table");
		return false;
	}

	QList<int> rawDataSourceIds;
	QList<int> rawDataSourceMeasurementIds;
	QStringList rawDataSourceNames;

	while (query.next()){

		rawDataSourceIds << query.value(0).toInt();
		rawDataSourceMeasurementIds << query.value(1).toInt();
		rawDataSourceNames << query.value(2).toString();
	}

	query.finish();

	QList<int> duplicateRawDataSourceIds;

	// Only go to N-1 because the last entry was already checked.
	for (int i = 0, size = rawDataSourceIds.size(); i < size-1; i++)
		if (rawDataSourceNames.at(i) == rawDataSourceNames.at(i+1) && rawDataSourceMeasurementIds.at(i+1) != 0)
			duplicateRawDataSourceIds << rawDataSourceIds.at(i+1);

	// Nothing wrong with the database.
	if (duplicateRawDataSourceIds.isEmpty())
		return true;

	query = database->query();
	QString duplicateWhereClause = QString("id2=%1").arg(duplicateRawDataSourceIds.first());

	for (int i = 1, size = duplicateRawDataSourceIds.size(); i < size; i++)
		duplicateWhereClause += QString(" OR id2=%1").arg(duplicateRawDataSourceIds.at(i));

	query.prepare("SELECT id1 FROM AMScan_table_rawDataSources WHERE " % duplicateWhereClause);

	if (!database->execQuery(query)){

		query.finish();
		return false;
	}

	QList<int> scanIds;

	while(query.next()){

		int id = query.value(0).toInt();

		if (!scanIds.contains(id))
			scanIds << id;
	}

	QList<VESPERSDatabaseDuplicateEntryObject *> duplicateEntryObjects;

	foreach(int id, scanIds)
		duplicateEntryObjects.append(new VESPERSDatabaseDuplicateEntryObject(database, id));

	foreach(VESPERSDatabaseDuplicateEntryObject *patchObject, duplicateEntryObjects)
		patchObject->fix();

	qDebug() << "Done!";

	return true;
}
