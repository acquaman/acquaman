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


#include "AMDatabase.h"

#include <QStringList>
#include <QStringBuilder>
#include <QThread>
#include <QSet>
#include <QMutexLocker>
#include <QApplication>
#include <QSqlDriver>
#include <QTime>
#include <QFileInfo>

#include "util/AMErrorMonitor.h"

// Internal instance records:
QHash<QString, AMDatabase*> AMDatabase::connectionName2Instance_;
QMutex AMDatabase::databaseLookupMutex_(QMutex::Recursive);

// This constructor is protected. Only access is through AMDatabase::createDatabase().
 AMDatabase::~AMDatabase(){}
AMDatabase::AMDatabase(const QString& connectionName, const QString& dbAccessString, bool openAsReadOnly)
	: QObject(),
	connectionName_(connectionName),
	dbAccessString_(dbAccessString),
	qdbMutex_(QMutex::Recursive)
{

	if (openAsReadOnly){

		isReadOnly_ = true;
	}

	else {

		QFileInfo accessInfo(dbAccessString_);
		if(accessInfo.exists())
			isReadOnly_ = !accessInfo.isWritable();
		else
			isReadOnly_ = false; // Probably not correct. If this the call that creates a new database, when does the .db file actually get created?
	}

	// Make sure the database is initialized in the creating thread:
	qdb();

}

AMDatabase* AMDatabase::createDatabase(const QString &connectionName, const QString &dbAccessString, bool openAsReadOnly)
{
	QMutexLocker ml(&databaseLookupMutex_);

	if(connectionName2Instance_.contains(connectionName)) {
		ml.unlock();
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -1, QString("AMDatabase: Could not create new database instance: A database with this connection name '%1' already exists").arg(connectionName)));
		return 0;
	}

	AMDatabase* db = new AMDatabase(connectionName, dbAccessString, openAsReadOnly);
	// keep this object in the main thread, if not there already:
	if(QThread::currentThread() != QApplication::instance()->thread())
		db->moveToThread(QApplication::instance()->thread());

	connectionName2Instance_.insert(connectionName, db);
	return db;
}

AMDatabase* AMDatabase::database(const QString& connectionName) {
	QMutexLocker ml(&databaseLookupMutex_);

	QHash<QString, AMDatabase*>::const_iterator i = connectionName2Instance_.find(connectionName);
	if(i == connectionName2Instance_.end())
		return 0;
	else
		return i.value();
}

/// Shut down database connections and free database resources [static]
void AMDatabase::deleteDatabase(const QString& connectionName) {
	QMutexLocker ml(&databaseLookupMutex_);

	if(connectionName2Instance_.contains(connectionName)) {
		AMDatabase* db = connectionName2Instance_[connectionName];
		connectionName2Instance_.remove(connectionName);
		ml.unlock();

		db->qdb().close();
		db->deleteLater();
	}
}

QStringList AMDatabase::registeredDatabases() {
	QMutexLocker ml(&databaseLookupMutex_);

	QStringList retVal;

	QHash<QString, AMDatabase*>::const_iterator i = connectionName2Instance_.begin();
	while(i != connectionName2Instance_.end()){
		retVal << i.key();
		i++;
	}

	return retVal;
}


/// Inserting or updating objects in the database.
/*! id is the object's row in the database (for updates), or 0 (for inserts).
 table is the database table name
 colNames is a list of the column names that the values will be inserted under
 values is a list of QVariants that provide the values to insert.
 (Note that the const& arguments are designed to prevent memory copies, so this should be fast.)
 Return value: (IMPORTANT) returns the id of the row that was inserted into or updated, or 0 on failure.
 When inserting new objects, make sure to set their id to the return value afterwards, otherwise they will be duplicated on next insert.

 Note: This is equivalent to an SQL "INSERT OR REPLACE" (or "REPLACE", in MySQL) command. If a row with \c id exists, it will be replaced completely, so you need to specify ALL column values. To change just some column values, use update().
*/
int AMDatabase::insertOrUpdate(int id, const QString& table, const QStringList& colNames, const QVariantList& values) {

	QSqlDatabase db = qdb();

	if(!db.isOpen()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not save to database. (Database is not open.)"));
		return false;
	}


	// Create the list of columns:
	QString cols = colNames.join(", ");	// this will become something like "name, number, sampleName, comments, startTime"
	QString colPlaceholders;
	for(int i=0; i<colNames.count()+1; i++)
		colPlaceholders.append("?, ");
	colPlaceholders.chop(2);	// remove trailing ", " from "?, ?, ?, ?, ...?, "
	// placeholders will become something like "?, ?, ?, ?, ?, ?", with enough ? for each column name + the id

	// Prepare the query. Todo: sanitize column names and table name. (Can't use binding because it's not an expression here)
	QSqlQuery query(db);
	query.prepare(QString("INSERT OR REPLACE INTO %1 (id, %2) VALUES (%3)").arg(table).arg(cols).arg(colPlaceholders));

	// If we have a unique id already, use that (This will update ourself in the DB)
	if(id > 0)
		query.bindValue(0, id);
	// Otherwise, use NULL for the id. (This will create a new one.)
	else
		query.bindValue(0, QVariant(QVariant::Int));

	// Bind remaining values
	for(int i=0; i<colNames.count(); i++)
		query.bindValue(i+1, values.at(i));

	// Run query. Query failed?
	if(!execQuery(query)) {
		query.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, QString("database save failed. Could not execute query (%1). The SQL reply was: %2").arg(query.executedQuery()).arg(query.lastError().text())));
		return 0;
	}
	// Query succeeded.

	// If we don't have one, set the unique id for this object (now that the database has established it)
	if(id < 1) {
		QVariant lastId = query.lastInsertId();
		query.finish();	// make sure that sqlite lock is released before emitting signals
		if(lastId.isValid()) {
			emit created(table, lastId.toInt());
			return lastId.toInt();
		}
		else {
			query.finish();	// make sure that sqlite lock is released before emitting signals
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -4, "Database save completed, but could not get the last id after insert. This should never happen."));
			emit updated(table, -1);
			return 0;
		}
	}
	// else (we already had an id, which was used for successful insert:
	else {
		query.finish();	// make sure that sqlite lock is released before emitting signals
		emit updated(table, id);
		return id;
	}


}

/// changing single values in the database, at row \c id.
bool AMDatabase::update(int id, const QString& table, const QString& column, const QVariant& value) {

	QSqlDatabase db = qdb();

	if(!db.isOpen()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not save to database. (Database is not open.)"));
		return false;
	}

	// Prepare the query. Todo: sanitize column names and table name. (Can't use binding because it's not an expression here)
	QSqlQuery query(db);
	query.prepare(QString("UPDATE %1 SET %2 = ? WHERE id = ?").arg(table).arg(column));
	query.bindValue(0, value);
	query.bindValue(1, QVariant(id));

	// Run query. Query failed?
	if(!execQuery(query)) {
		query.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, QString("database update failed. Could not execute query (%1). The SQL reply was: %2").arg(query.executedQuery()).arg(query.lastError().text())));
		return false;
	}
	// Query succeeded.
	query.finish();	// make sure that sqlite lock is released before emitting signals
	emit updated(table, id);
	return true;

}

/// changing multiple column values in the database, at row \c id.
bool AMDatabase::update(int id, const QString& table, const QStringList& columns, const QVariantList& values) {

	QSqlDatabase db = qdb();

	if(columns.count() != values.count()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDATABASE_ERROR_COLUMN_VALUE_COUNT_MISMATCH, "Error trying to update the database: the number of columns provided doesn't match the number of values."));
		return false;
	}
	if(!db.isOpen()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not save to database. (Database is not open.)"));
		return false;
	}

	QStringList cols;
	foreach(QString column, columns) {
		cols << (column % " = ?");
	}
	QString colString = cols.join(", ");

	// Prepare the query. Todo: sanitize column names and table name.
	QSqlQuery query(db);
	query.prepare(QString("UPDATE %1 SET %2 WHERE id = ?").arg(table).arg(colString));
	int i=0;
	for(i=0; i<values.count(); i++)
		query.bindValue(i, values.at(i));
	query.bindValue(i, QVariant(id));

	// Run query. Query failed?
	if(!execQuery(query)) {
		query.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, QString("database update failed. Could not execute query (%1). The SQL reply was: %2").arg(query.executedQuery()).arg(query.lastError().text())));
		return false;
	}

	// Query succeeded.
	query.finish();	// make sure that sqlite lock is released before emitting signals
	emit updated(table, id);
	return true;

}


/// Changing single values in the database (where the id isn't known).  Will update all rows based on the condition specified. \c whereClause is a string suitable for appending after an SQL "WHERE" term.  Will set the value in \c dataColumn to \c dataValue.
bool AMDatabase::update(const QString& tableName, const QString& whereClause, const QString& dataColumn, const QVariant& dataValue) {

	/// \todo sanitize more than this...
	if(whereClause.isEmpty() || dataColumn.isEmpty() || tableName.isEmpty()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, "Could not complete the database update; missing the table name, column, or WHERE clause."));
		return false;
	}

	QSqlDatabase db = qdb();

	if(!db.isOpen()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not save to the database. (Database is not open.)"));
		return false;
	}

	// Prepare the query. Todo: sanitize column names and table name. (Can't use binding because it's not an expression here)
	QSqlQuery query(db);
	query.prepare(QString("UPDATE %1 SET %2 = ? WHERE %3").arg(tableName).arg(dataColumn).arg(whereClause));
	query.bindValue(0, dataValue);

	// Run query. Query failed?
	if(!execQuery(query)) {
		query.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, QString("Failed to update the database. Could not execute the query (%1). The SQL reply was: %2").arg(query.executedQuery()).arg(query.lastError().text())));
		return false;
	}
	// Query succeeded.
	query.finish();	// make sure that sqlite lock is released before emitting signals
	emit updated(tableName, -1);
	return true;

}



/// delete the object/row in \c tableName at id \c id. Returns true on success.
bool AMDatabase::deleteRow(int id, const QString& tableName) {

	/// \todo sanitize more than this...
	if(tableName.isEmpty()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, "Could not update the database. (Missing the table name, column, or WHERE clause.)"));
		return false;
	}

	QSqlDatabase db = qdb();

	if(!db.isOpen()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not update the database. (Database is not open.)"));
		return false;
	}

	// Prepare the query (todo: sanitize table name)
	QSqlQuery query(db);
	query.prepare(QString("DELETE FROM %1 WHERE id = ?").arg(tableName));
	query.bindValue(0, id);

	// Run query. Query failed?
	if(!execQuery(query)) {
		query.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, QString("Failed to delete the database object. Could not execute the query (%1). The SQL reply was: %2").arg(query.executedQuery()).arg(query.lastError().text())));
		return false;
	}
	query.finish();	// make sure that sqlite lock is released before emitting signals
	emit removed(tableName, id);
	// Query succeeded.
	return true;

}

/// delete all objects/rows in \c tableName that meet a certain condition. \c whereClause is a string suitable for appending after an SQL "WHERE" term. Returns the number of rows deleted, or 0 if it fails to delete any.
int AMDatabase::deleteRows(const QString& tableName, const QString& whereClause) {

	/// \todo sanitize more than this...
	if(tableName.isEmpty() || whereClause.isEmpty()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, "Could not update the database. (Missing the table name, column, or WHERE clause.)"));
		return 0;
	}

	QSqlDatabase db = qdb();

	if(!db.isOpen()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not update the database. (Database is not open.)"));
		return 0;
	}

	// Prepare the query (todo: sanitize table name)
	QSqlQuery query(db);
	query.prepare(QString("DELETE FROM %1 WHERE %2").arg(tableName).arg(whereClause));

	// Run query. Query failed?
	if(!execQuery(query)) {
		query.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, QString("Failed to delete the database object(s). Could not execute the query (%1). The SQL reply was: %2").arg(query.executedQuery()).arg(query.lastError().text())));
		return 0;
	}

	int numRowsAffected = query.numRowsAffected();

	query.finish();	// make sure that sqlite lock is released before emitting signals
	emit removed(tableName, -1);

	// Query succeeded.
	return numRowsAffected;
}



/// retrieve object information from the database.
/*! id is the object's row in the database.
 table is the database table name
 colNames is a list of the column names that the values will be retrieved from
 values is a list of pointers to QVariants that will be modified with the retrived values.
 (Note that the const and & arguments are designed to prevent memory copies, so this should be fast.)
 Return value: returns true on success.
*/
QVariantList AMDatabase::retrieve(int id, const QString& table, const QStringList& colNames)
{
	QVariantList values;	// return value

	/// \todo sanitize more than this...
	if(table.isEmpty()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, "Could not search the database. (Missing the table name.)"));
		return values;
	}

	// create a query on our database connection:
	QSqlQuery q( qdb() );

	// Create the list of columns:
	QString cols = colNames.join(", ");	// this will become something like "name, number, sampleName, comments, startTime"
	// Prepare the query. Todo: sanitize column names and table name. (Can't use binding because it's not an expression here)
	q.prepare(QString("SELECT %1 FROM %2 WHERE id = ?").arg(cols).arg(table));
	q.bindValue(0,id);

	// run query. Did it succeed?
	if(!execQuery(q)) {
		q.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, QString("database retrieve failed. Could not execute query (%1). The SQL reply was: %2").arg(q.executedQuery()).arg(q.lastError().text())));
		return values;
	}
	// If we found a record at this id:
	if(q.first()) {
		// copy columns to return values:
		for(int i=0; i<colNames.count(); i++)
			values << q.value(i);
	}
	q.finish();	// make sure that sqlite lock is released before emitting signals
	// otherwise: didn't find this id.  That's normal if it's not there, just return empty list
	return values;
}

/// retrieve a column of information from the database.
/*! table is the database table name
 colName is the name of the column you wish to get all the values for
 values is a list of pointers to QVariants that will be modified with the retrived values.
 (Note that the const and & arguments are designed to prevent memory copies, so this should be fast.)
 Return value: returns the list of values
*/
QVariantList AMDatabase::retrieve(const QString& table, const QString& colName)
{
	QVariantList values;	// return value

	/// \todo sanitize more than this...
	if(table.isEmpty()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDATABASE_MISSING_TABLE_NAME_IN_RETRIEVE, "Could not search the database. (Missing the table name.)"));
		return values;
	}

	// create a query on our database connection:
	QSqlQuery q( qdb() );

	// Prepare the query.
	q.prepare(QString("SELECT %1 FROM %2").arg(colName).arg(table));

	// run query. Did it succeed?
	if(!execQuery(q)) {
		q.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMDATABASE_RETRIEVE_QUERY_FAILED, QString("database retrieve failed. Could not execute query (%1). The SQL reply was: %2").arg(q.executedQuery()).arg(q.lastError().text())));
		return values;
	}

	while(q.next())
		values << q.value(0);

	q.finish();	// make sure that sqlite lock is released before emitting signals
	// otherwise: didn't find this column.  That's normal if it's not there, just return empty list
	return values;
}


QVariant AMDatabase::retrieve(int id, const QString& table, const QString& colName)
{
	/// \todo sanitize more than this...
	if(table.isEmpty()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, "Could not search the database. (Missing the table name.)"));
		return false;
	}

	// create a query on our database connection:
	QSqlQuery q( qdb() );


	/// Prepare the query. \todo: sanitize column names and table name. (Can't use binding because it's not an expression here)
	q.prepare(QString("SELECT %1 FROM %2 WHERE id = ?").arg(colName).arg(table));
	q.bindValue(0,id);

	// run query. Did it succeed?
	if(!execQuery(q)) {
		q.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, QString("database retrieve failed. Could not execute query (%1). The SQL reply was: %2").arg(q.executedQuery()).arg(q.lastError().text())));
		return false;
	}
	// If we found a record at this id:
	if(q.first()) {
		return q.value(0);
	}
	// else: didn't find this id.  That's normal if it's not there, just return null QVariant.
	else {
		return QVariant();
	}

}

QVariant AMDatabase::retrieveMax(const QString &table, const QString &colName, const QString &whereClause)
{
	// Sanitize the options
	if(table.isEmpty() || colName.isEmpty())
	{
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, "Could not search the database. (Missing table name or column name)"));
		return QVariant();
	}

	/*
	if(!tableExists(table))
	{
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, QString("Could not search the database. (Table %1 does not exist)").arg(table)));
		return QVariant();
	}

	if(!columnExists(table, colName))
	{
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, QString("Could not search the database. (Table %1 does not contain column %2").arg(table).arg(colName)));
		return QVariant();
	}
	*/

	QSqlQuery q( qdb() );

	QString queryString = QString("SELECT MAX(%1) FROM %2").arg(colName).arg(table);

	if(!whereClause.isEmpty())
	{
		queryString.append(" WHERE ").append(whereClause);
	}

	q.prepare(queryString);

	if(!execQuery(q)) {
		q.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, QString("database retrieve failed. Could not execute query (%1). The SQL reply was: %2").arg(q.executedQuery()).arg(q.lastError().text())));
		return QVariant();
	}
	// If we found a record at this id:
	if(q.first()) {
		return q.value(0);
	}
	// else: didn't find this id.  That's normal if it's not there, just return null QVariant.
	else {
		return QVariant();
	}

}

/// returns a list of all the objecst/rows (by id) that match a given condition. \c whereClause is a string suitable for appending after an SQL "WHERE" statement.
QList<int> AMDatabase::objectsWhere(const QString& tableName, const QString& whereClause)
{
	QList<int> rl;

	/// \todo sanitize more than this...
	if(tableName.isEmpty()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, "Could not search the database. (Missing the table name.)"));
		return rl;
	}

	QSqlQuery q( qdb() );

	QString query = "SELECT id FROM " % tableName;
	if(!whereClause.isEmpty())
		query.append(" WHERE ").append(whereClause);

	q.prepare(query);
	execQuery(q);

	while(q.next()) {
		rl << q.value(0).toInt();
	}
	q.finish();


	return rl;
}

/// Return a list of all the objects/rows (by id) that match 'value' in a certain column.
/// ex: AMDatabase::db()->objectsMatching("name", "Carbon60"), or AMDatabase::db()->objectsMatching("dateTime", QDateTime::currentDateTime())
QList<int> AMDatabase::objectsMatching(const QString& tableName, const QString& colName, const QVariant& value)
{
	// return value: list of id's that match
	QList<int> rl;

	/// \todo sanitize more than this...
	if(tableName.isEmpty() || colName.isEmpty()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, "Could not search the database. (Missing the table name or column name.)"));
		return rl;
	}

	QSqlQuery q( qdb() );

	// For date/times, we want a resolution of one minute to count as a match
	if(value.type() == QVariant::DateTime) {
		q.prepare(QString("SELECT id FROM %1 WHERE %2 BETWEEN datetime(?, '-1 minute') AND datetime(?, '+1 minute')").arg(tableName).arg(colName));
		q.bindValue(0, value);
		q.bindValue(1, value);
	}
	else {
		q.prepare(QString("SELECT id FROM %1 WHERE %2 = ?").arg(tableName).arg(colName));
		q.bindValue(0, value);
	}
	execQuery(q);

	while(q.next()) {
		rl << q.value(0).toInt();
	}
	q.finish();

	return rl;
}


/// Return a list of all the objects/rows (by id) that contain 'value' in a certain column
/// ex: AMDatabase::db()->scansContaining("name", "Carbon60") could return Scans with names Carbon60_alpha and bCarbon60_gamma
QList<int> AMDatabase::objectsContaining(const QString& tableName, const QString& colName, const QVariant& value)
{

	QList<int> rl;

	/// \todo sanitize more than this...
	if(tableName.isEmpty() || colName.isEmpty()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, "Could not search the database. (Missing the table name or column name.)"));
		return rl;
	}


	QSqlQuery q( qdb() );

	q.prepare(QString("SELECT id FROM %1 WHERE %2 LIKE ('%' || :val || '%')").arg(tableName).arg(colName));

	q.bindValue(":val", value);
	execQuery(q);

	while(q.next()) {
		rl << q.value(0).toInt();
	}
	q.finish();


	return rl;
}

bool AMDatabase::tableExists(const QString &tableName)
{
	QSqlQuery q = query();
	q.prepare(QString("SELECT tbl_name FROM sqlite_master WHERE tbl_name='%1'").arg(tableName));
	execQuery(q);

	bool returnVal = false;

	if (q.first()){

		do {
			if (q.value(0).toString() == tableName)
				returnVal = true;
		}while(q.next());
	}

	q.finish();

	return returnVal;
}

bool AMDatabase::columnExists(const QString &tableName, const QString &columnName)
{
	QSqlQuery q = query();
	q.prepare(QString("PRAGMA table_info(%1);").arg(tableName));
	execQuery(q);

	QStringList columnNames;

	if (q.first()){

		do {

			columnNames << q.value(1).toString();
		}while(q.next());
	}

	q.finish();

	return columnNames.contains(columnName);
}

bool AMDatabase::ensureTable(const QString& tableName, const QStringList& columnNames, const QStringList& columnTypes, bool reuseDeletedIds) {

	if(columnNames.count() != columnTypes.count()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -5, QString("could not create table: invalid structure. Different number of column names and types. Names: %1. Types:%2").arg(columnNames.join(",")).arg(columnTypes.join(","))));
		return false;
	}


	// todo: sanitize all inputs...
	QSqlQuery q(qdb());

	QString qs = "CREATE TABLE IF NOT EXISTS ";
	qs.append(tableName);
	if(reuseDeletedIds)
		qs.append("(id INTEGER PRIMARY KEY ASC, ");
	else
		qs.append("(id INTEGER PRIMARY KEY AUTOINCREMENT, ");
	for(int i=0; i<columnNames.count(); i++) {
		qs.append(columnNames.at(i));
		qs.append(" ");
		qs.append(columnTypes.at(i));
		qs.append(", ");
	}
	qs.chop(2);
	qs.append(")");

	q.prepare(qs);
	if(execQuery(q)) {
		return true;
	}
	else {
		q.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -6, QString("database table create failed on database %3 (%4). Could not execute query (%1). The SQL reply was: %2").arg(q.executedQuery()).arg(q.lastError().text()).arg(connectionName()).arg(dbAccessString())));
		return false;
	}
}


/// ensure that a given column (with \c columName and \c columnType) exists, in the table \c tableName.  \c columnType is an SQLite type ("TEXT" or "INTEGER" recommended).
bool AMDatabase::ensureColumn(const QString& tableName, const QString& columnName, const QString& columnType) {

	QSqlQuery q( qdb() );

	q.prepare(QString("ALTER TABLE %1 ADD COLUMN %2 %3;").arg(tableName).arg(columnName).arg(columnType));

	if(execQuery(q)) {
		q.finish();
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, QString("Adding database column %1 to table %2.").arg(columnName).arg(tableName)));
		return true;
	}
	else {
		q.finish();	// make sure that sqlite lock is released before emitting signals
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, QString("Error adding database column %1 to table %2. Maybe it's already there? Sql reply says: %3").arg(columnName).arg(tableName).arg(q.lastError().text())));
		return false;
	}
}

bool AMDatabase::createIndex(const QString& tableName, const QString& columnNames) {
	QSqlQuery q( qdb() );
	QString indexName = QString("idx_%1_%2").arg(tableName, columnNames);
	indexName.remove(QRegExp("[\\s\\,\\;]"));// remove whitespace, commas, and semicolons from index name...
	q.prepare(QString("CREATE INDEX %1 ON %2(%3);").arg(indexName, tableName, columnNames));
	if(execQuery(q)) {
		q.finish();
		return true;
	}
	else {
		q.finish();	// make sure that sqlite lock is released before emitting signals
		return false;
	}
}

QSqlQuery AMDatabase::select(const QString &tableName, const QString &columnNames, const QString &whereClause)
{
	QSqlQuery q(qdb());

	QString whereString = whereClause.isEmpty() ? QString() : " WHERE " % whereClause;

	QString query = "SELECT " % columnNames % " FROM " % tableName % whereString % ";";
	q.prepare(query);

	return q;
}


QSqlDatabase AMDatabase::qdb() const
{
	// threadIDsOfOpenConnections_ can be written and read from any thread by this function. Need to mutex it.
	QMutexLocker ml(&qdbMutex_);

	// Do we have an open connection for this thread already?
	Qt::HANDLE threadId = QThread::currentThreadId();

	if(threadIDsOfOpenConnections_.contains(threadId)) {
		return QSqlDatabase::database(QString("%1%2").arg(connectionName_).arg((qulonglong)threadId));
	}

	else {
		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString("%1%2").arg(connectionName_).arg((qulonglong)threadId));
		db.setDatabaseName(dbAccessString_);
		db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=10");

		bool ok = db.open();
		if(!ok) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("error connecting to database (access %1). The SQL reply was: %2").arg(dbAccessString_).arg(db.lastError().text())));
		}

		threadIDsOfOpenConnections_ << threadId;
		return db;
	}
}


bool AMDatabase::startTransaction()
{
	bool success = qdb().transaction();
	if(success) {
		qdbMutex_.lock();
		transactionOpenOnThread_.insert(QThread::currentThreadId());
		qdbMutex_.unlock();
	}

	else {

		QSqlError error = qdb().lastError();
		AMErrorMon::error(this, 0, QString("Unable to start transaction.\nDatabase says: %1\nDriver says: %2").arg(error.databaseText()).arg(error.driverText()));
	}

	return success;
}

#include <unistd.h>
bool AMDatabase::commitTransaction(int timeoutMs)
{
	QTime startTime;
	startTime.start();

	bool success;
	int attempt = 0;
	do {
		success = qdb().commit();
		attempt++;
		if(!success)
			usleep(5000);
	} while (!success && startTime.elapsed() < timeoutMs);

	qdbMutex_.lock();
	transactionOpenOnThread_.remove(QThread::currentThreadId());
	qdbMutex_.unlock();

	if(attempt > 1) {
		if(success) {
			AMErrorMon::debug(this, AMDATABASE_COMMIT_CONTENTION_SUCCEEDED, QString("AMDatabase detected contention for database access in commitTransaction(). It took %1 tries for the commit to succeed.").arg(attempt) );
		}
		else {
			AMErrorMon::debug(this, AMDATABASE_COMMIT_CONTENTION_FAILED, QString("AMDatabase detected contention for database access in commitTransaction(). After %1 attempts, the commit still did not succeed.").arg(attempt) );
		}
	}

	return success;
}

bool AMDatabase::rollbackTransaction()
{
	bool success = qdb().rollback();

	qdbMutex_.lock();
	transactionOpenOnThread_.remove(QThread::currentThreadId());
	qdbMutex_.unlock();

	return success;
}

bool AMDatabase::transactionInProgress() const
{
	QMutexLocker ml(&qdbMutex_);
	return transactionOpenOnThread_.contains(QThread::currentThreadId());
}


bool AMDatabase::supportsTransactions() const
{
	return qdb().driver()->hasFeature(QSqlDriver::Transactions);
}

bool AMDatabase::execQuery(QSqlQuery &query, int timeoutMs)
{
	if (isReadOnly() && !(query.executedQuery().startsWith("SELECT", Qt::CaseInsensitive) || query.executedQuery().startsWith("PRAGMA", Qt::CaseInsensitive))){

		AMErrorMon::debug(this, AMDATABASE_IS_READ_ONLY, QString("This database is read-only and the desired command would modify the contents of the database. Query: %1").arg(query.executedQuery()));
		return false;
	}

	QTime startTime;
	startTime.start();

	bool success;
	int lastErrorNumber;
	QString lastErrorMessage;
	int attempt = 0;
	do {
		success = query.exec();
		lastErrorNumber = query.lastError().number();
		lastErrorMessage = query.lastError().text();
		attempt++;
		if(lastErrorNumber == 5)
			usleep(5000);
	} while(success != true && startTime.elapsed() < timeoutMs && (lastErrorNumber == 5));

	if(attempt > 1) {
		if(success) {
			AMErrorMon::debug(0, AMDATABASE_LOCK_FOR_EXECQUERY_CONTENTION_SUCCEEDED, QString("AMDatabase detected contention for database locking in execQuery(). It took %1 tries for the query to succeed.").arg(attempt) );
		}
		else {
			AMErrorMon::debug(0, AMDATABASE_LOCK_FOR_EXECQUERY_CONTENTION_FAILED, QString("AMDatabase detected contention for database locking in execQuery(). After %1 attempts, the query still did not succeed. The last error is %2").arg(attempt).arg(lastErrorMessage) );
		}
	}

	return success;
}
