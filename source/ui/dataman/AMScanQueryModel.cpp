/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMScanQueryModel.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMDateTimeUtils.h"

#include "dataman/AMScan.h"
#include "dataman/AMSample.h"
#include "dataman/AMRun.h"

#include <QStringBuilder>

#include <QDebug>


AMScanQueryModel::AMScanQueryModel(AMDatabase* db,
								   QObject *parent,
								   const QString& tableName,
								   const QString& whereClause,
								   const QList<AMScanQueryModelColumnInfo> columnsToShow) :
	QSqlQueryModel(parent)
{

	db_ = db;
	whereClause_ = whereClause;

	if(tableName.isEmpty())
		tableName_ = AMDbObjectSupport::s()->tableNameForClass<AMScan>();
	else
		tableName_ = tableName;

	if(columnsToShow.isEmpty()) {
		columns_ = QList<AMScanQueryModelColumnInfo>()
				<< AMScanQueryModelColumnInfo("Row", "id")
				<< AMScanQueryModelColumnInfo("Name", "name")
				<< AMScanQueryModelColumnInfo("#", "number")
				<< AMScanQueryModelColumnInfo("When", "dateTime")
				   // << AMScanQueryModelColumnInfo("About", "scanInfo")
				<< AMScanQueryModelColumnInfo("Sample", "sampleId", true, AMDbObjectSupport::s()->tableNameForClass<AMSample>(), "name")
				<< AMScanQueryModelColumnInfo("Technique", "AMDbObjectType", true, "AMDbObjectTypes_table", "description", "AMDbObjectType")
				   // << AMScanQueryModelColumnInfo("Where", "facilityId", true, AMDbObjectSupport::s()->tableNameForClass<AMFacility>(), "description")
				<< AMScanQueryModelColumnInfo("Where", "runId", true, AMDbObjectSupport::s()->tableNameForClass<AMRun>(), "name")
				<< AMScanQueryModelColumnInfo("Notes", "notes");

		orderClause_ = "dateTime ASC";
		idColumnNumber_ = 0;
	}

	else {
		idColumnNumber_ = -1;
		columns_ = columnsToShow;
		for(int i=columns_.count()-1; i>=0; i--) {
			columns_[i].foreignKeyCache->clear();
			if(columns_.at(i).name == "id")
				idColumnNumber_ = i;
		}
	}


	setSupportedDragActions(Qt::CopyAction);

}



QString AMScanQueryModel::columnNames() const
{
	QStringList rv;
	foreach(const AMScanQueryModelColumnInfo& col, columns_)
		rv << col.name;

	return rv.join(", ");
}

bool AMScanQueryModel::refreshQuery()
{
	for(int i=columns_.count()-1; i>=0; i--) {
		if(columns_.at(i).isForeignKey) {
			columns_[i].foreignKeyCache->clear();
		}
	}

	// check for empty column set? Bad database?

	// set the query
	QString whereString = whereClause_.isEmpty() ? QString() : " WHERE " % whereClause_;
	QString orderByString = orderClause_.isEmpty() ? QString() : " ORDER BY " % orderClause_;
	QString queryString = "SELECT " % columnNames() % " FROM " % tableName_ % whereString % orderByString % ";";

	QSqlQuery q = db_->query();
	q.prepare(queryString);
	if(q.exec()) {
		setQuery(q);
		return true;
	}
	else {
		AMErrorMon::report(AMErrorReport(this,
										 AMErrorReport::Debug,
										 -1,
										 "Could not load the results out of the database for this query: '" % queryString % "'.  The error was: " % q.lastError().text()));
		return false;
	}
}

QVariant AMScanQueryModel::data(const QModelIndex &item, int role) const
{
	if(!(role == Qt::EditRole || role == Qt::DisplayRole))
		return QSqlQueryModel::data(item, role);

	QVariant rv;

	if(!item.parent().isValid() && item.column() < columns_.count()) {
		if(columns_.at(item.column()).isForeignKey)
			rv = resolveForeignKey(item);
		else
			rv = QSqlQueryModel::data(item, role);
	}
	else
		rv = QSqlQueryModel::data(item, role);

	if(item.column() < columns_.count() && columns_.at(item.column()).name == "dateTime") {
		QDateTime dt = rv.toDateTime();
		if(dt.isValid())
			return AMDateTimeUtils::prettyDateTime(dt);
	}

	return rv;
}

QVariant AMScanQueryModel::resolveForeignKey(const QModelIndex &item) const {

	// optimization: check if item.data() is an integer index, and is less than 1.  If so, we know it's an invalid foreign key (at least, that's our convention) and we can save time by not even doing a database search
	QVariant foreignKey = QSqlQueryModel::data(item);
	QString foreignKeyString = foreignKey.toString();

	bool conversionOk;
	int foreignKeyId = foreignKey.toInt(&conversionOk);
	if(conversionOk && foreignKeyId < 1)
		return QVariant();

	const AMScanQueryModelColumnInfo& col = columns_.at(item.column());

	// Do we have the value cached already?
	QHash<QString, QVariant>::const_iterator iForeignKeyCache = col.foreignKeyCache->find(foreignKeyString);
	if(iForeignKeyCache != col.foreignKeyCache->end())
		return iForeignKeyCache.value();

	QSqlQuery q = db_->query();
	q.prepare("SELECT " % col.foreignKeyReplacementColumn % ", " % col.foreignKeyMatchColumn % " FROM " % col.foreignKeyTable % " WHERE " % col.foreignKeyMatchColumn % " = ?;");
	q.bindValue(0, foreignKey);
	QVariant value;

	if(q.exec() && q.first()) {
		value = q.value(0);
	}

	(*(col.foreignKeyCache))[foreignKeyString] = value;
	return value;
}

QVariant AMScanQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Horizontal && section < columns_.count() && role == Qt::DisplayRole)
		return columns_.at(section).heading;

	return QSqlQueryModel::headerData(section, orientation, role);
}

void AMScanQueryModel::sort(int column, Qt::SortOrder order)
{
	QString orderClause;

	if(column < columns_.count()) {
		orderClause = columns_.at(column).name % ((order == Qt::AscendingOrder) ? " ASC" : " DESC");
	}


	if(orderClause_ != orderClause) {
		orderClause_ = orderClause;
		refreshQuery();
	}
}

#include <QMimeData>
#include <QUrl>

QMimeData * AMScanQueryModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData* mimeData = new QMimeData();
	QList<QUrl> urls;

	if(idColumnNumber_ == -1)
		return mimeData;	// if we don't have a column which records the id... We can't support drag and drop.

	QSet<int> rowsCompleted;	// indexes will be all the selected indexes...
	foreach (const QModelIndex &i, indexes) {
		if (i.isValid() && !rowsCompleted.contains(i.row())) {
			rowsCompleted << i.row();

			bool ok;
			int id = data(index(i.row(),0,i.parent()), Qt::DisplayRole).toInt(&ok);

			if(ok)
				urls << QUrl("amd://" % db_->connectionName() % "/" % tableName_ % "/" % QString::number(id));
		}
	}

	mimeData->setUrls(urls);
	return mimeData;
}


