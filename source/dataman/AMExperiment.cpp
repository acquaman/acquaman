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


#include "AMExperiment.h"
#include "database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"

 AMExperiment::~AMExperiment(){}
AMExperiment::AMExperiment(QObject *parent) :
		AMDbObject(parent)
{

}
/// initializing experiment with a given name.
AMExperiment::AMExperiment(const QString& experimentName, QObject* parent)
	: AMDbObject(parent)
{

	this->setName(experimentName);
}

/// This constructor immediately loads a stored experiment from the database.
AMExperiment::AMExperiment(int databaseId, AMDatabase* database, QObject* parent)
		: AMDbObject(parent)
{
	loadFromDb(database, databaseId);
}

bool AMExperiment::deleteExperiment(int id, AMDatabase *database)
{
	database->deleteRows(AMDbObjectSupport::experimentEntriesTableName(),
						 QString("experimentId = %1").arg(id));
	return database->deleteRow(id, AMDbObjectSupport::s()->tableNameForClass(&staticMetaObject));
}

QList<int> AMExperiment::experimentsContainingScan(int scanId, AMDatabase *database)
{
	QList<int> rv;
	QSqlQuery q = database->select(AMDbObjectSupport::experimentEntriesTableName(),
								   "experimentId",
								   QString("objectId = %1").arg(scanId));
	if(!q.exec()) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -4, "Error accessing the experiment registry in the database. Please report this problem to the Acquaman developers."));
	}
	while(q.next()) {
		rv << q.value(0).toInt();
	}
	return rv;
}

bool AMExperiment::addScanToExperiment(int scanId, int experimentId, AMDatabase *database)
{
	// object already exists in this experiment. Don't add again.
	if( database->objectsWhere(AMDbObjectSupport::experimentEntriesTableName(),
						  QString("objectId = '%1' AND experimentId = '%2'")
						  .arg(scanId).arg(experimentId))
		.count() > 0)
		return false;	// already in there.

	return database->insertOrUpdate(0,
							   AMDbObjectSupport::experimentEntriesTableName(),
							   QStringList() << "objectId" << "experimentId",
							   QVariantList() << scanId << experimentId);
}

bool AMExperiment::removeScanFromExperiment(int scanId, int experimentId, AMDatabase *database)
{
	return (database->deleteRows(AMDbObjectSupport::experimentEntriesTableName(),
						 QString("experimentId = %1 AND objectId = %2").arg(experimentId).arg(scanId)) > 0);
}




///
