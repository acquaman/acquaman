#include "AMScanQueryModel.h"
#include "dataman/AMDbObjectSupport.h"
#include "ui/AMDateTimeUtils.h"

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
		tableName_ = AMDbObjectSupport::tableNameForClass<AMScan>();
	else
		tableName_ = tableName;

	if(columnsToShow.isEmpty()) {
		columns_ = QList<AMScanQueryModelColumnInfo>()
				<< AMScanQueryModelColumnInfo("Row", "id")
				<< AMScanQueryModelColumnInfo("Name", "name")
				<< AMScanQueryModelColumnInfo("#", "number")
				<< AMScanQueryModelColumnInfo("When", "dateTime")
				   // << AMScanQueryModelColumnInfo("About", "scanInfo")
				<< AMScanQueryModelColumnInfo("Sample", "sampleId", true, AMDbObjectSupport::tableNameForClass<AMSample>(), "name")
				<< AMScanQueryModelColumnInfo("Technique", "AMDbObjectType", true, "AMDbObjectTypes_table", "description", "AMDbObjectType")
				// << AMScanQueryModelColumnInfo("Where", "facilityId", true, AMDbObjectSupport::tableNameForClass<AMFacility>(), "description")
				<< AMScanQueryModelColumnInfo("Where", "runId", true, AMDbObjectSupport::tableNameForClass<AMRun>(), "name")
				<< AMScanQueryModelColumnInfo("Notes", "notes");

		orderClause_ = "dateTime ASC";
	}

	else {
		columns_ = columnsToShow;
		for(int i=columns_.count()-1; i>=0; i--)
			columns_[i].foreignKeyCache->clear();
	}



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


