/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMRunExperimentItems.h"
#include "dataman/AMDatabase.h"
#include "ui/AMDateTimeUtils.h"

#include <QMimeData>
#include <QUrl>
#include <QList>
#include <QStringList>

#include "dataman/AMDbObjectSupport.h"
#include "dataman/AMScan.h"

QVariant AMRunModelItem::data(int role) const {
	if(role == Qt::DisplayRole)
		return QStandardItem::data(Qt::EditRole).toString() + ", " + AMDateTimeUtils::prettyDateRange(QStandardItem::data(AM::DateTimeRole).toDateTime(), QStandardItem::data(AM::EndDateTimeRole).toDateTime());
	else
		return QStandardItem::data(role);
}

#include "dataman/AMRun.h"
void AMRunModelItem::setData(const QVariant &value, int role) {
	if(role == Qt::EditRole) {
		db_->update( data(AM::IdRole).toInt(), AMDbObjectSupport::tableNameForClass<AMRun>(), "name", value);
	}
	QStandardItem::setData(value, role);
}

#include "dataman/AMExperiment.h"
void AMExperimentModelItem::setData(const QVariant &value, int role) {
	if(role == Qt::EditRole) {
		db_->update( data(AM::IdRole).toInt(), AMDbObjectSupport::tableNameForClass<AMExperiment>(), "name", value);
	}
	QStandardItem::setData(value, role);
}




bool AMExperimentModelItem::dropMimeData(const QMimeData *data, Qt::DropAction action) {

	bool accepted = false;

	/// For drops with a Qt::CopyAction and URLs containing "amd://databaseConnectionName/tableName/id", add these objects to this experiment.
	if( (action & Qt::CopyAction) && data->hasUrls() ) {


		QList<QUrl> urls = data->urls();

		foreach(QUrl url, urls) {

			if(url.scheme() != "amd")
				break;

			// Since this experiment item is from a particular database, make sure that the object comes from the same database
			if(db_->connectionName() != url.host())
				break;

			QStringList path = url.path().split('/');
			if(path.count() != 3)
				break;

			QString tableName = path.at(1);
			bool idOkay;
			int id = path.at(2).toInt(&idOkay);
			if(!idOkay || id < 1)
				break;


			/// \todo Determine if this is still necessary: Only store things that belong in the main scans table for now.
			if(tableName != AMDbObjectSupport::tableNameForClass<AMScan>())
				break;

			addObjectToExperiment(id);
			accepted = true;
		}
	}

	return accepted;
}

bool AMExperimentModelItem::addObjectToExperiment(int objectId) {
	QSqlQuery q = db_->query();

	// object already exists in this experiment. Don't add again.
	if( db_->objectsWhere("ObjectExperimentEntries",
						  QString("objectId = '%1' AND experimentId = '%2'")
						  .arg(objectId).arg(id()))
		.count() > 0)
		return false;

	QStringList colNames;
	colNames << "objectId" << "experimentId";
	QVariantList colValues;
	colValues << objectId << id();

	return db_->insertOrUpdate(0, "ObjectExperimentEntries", colNames, colValues);
}
