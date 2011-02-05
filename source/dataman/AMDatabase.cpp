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


#include "AMDatabase.h"

#include <QStringList>

#include "util/AMErrorMonitor.h"

/// Internal instance pointers
AMDatabase* AMDatabase::userInstance_ = 0;
AMDatabase* AMDatabase::publicInstance_ = 0;

// This constructor is protected; only access is through AMDatabase::db()
AMDatabase::AMDatabase(const QString& connectionName, const QString& dbAccessString) :
		QObject(),
		connectionName_(connectionName) {

	// Initialize the database:
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName_);
	db.setDatabaseName(dbAccessString);
	bool ok = db.open();

	if(ok) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Connection established to database " + connectionName_));
	}
	else {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("error connecting to database (access %1). The SQL reply was: %2").arg(dbAccessString).arg(db.lastError().text())));
	}

}

/// Access (or create, if it doesn't exist) the user database [static]
AMDatabase* AMDatabase::userdb() {

	if(userInstance_ == 0) {

		// create a new AMDatabase object for the user database:
		QString filename = AMUserSettings::userDataFolder + AMUserSettings::userDatabaseFilename;
		userInstance_ = new AMDatabase("user", filename);

	}
	return userInstance_;
}

/// Access (or create, if it doesn't exist) the public database [static]
AMDatabase* AMDatabase::publicdb() {

	if(publicInstance_ == 0) {

		// create a new AMDatabase object for the public database:
		QString filename = AMSettings::publicDataFolder + AMSettings::publicDatabaseFilename;
		publicInstance_ = new AMDatabase("public", filename);

	}
	return publicInstance_;
}

AMDatabase* AMDatabase::dbByName(const QString& connectionName) {
	if(connectionName == userdb()->connectionName())
		return userdb();
	if(publicInstance_ && connectionName == publicdb()->connectionName())
		return publicdb();
	return 0;
}

/// Shut down database connections and free database resources [static]
void AMDatabase::releaseUserDb() {
	if(userInstance_) {
		userInstance_->qdb().close();
		delete userInstance_;
		userInstance_ = 0;
	}
}
void AMDatabase::releasePublicDb() {
	if(publicInstance_) {
		publicInstance_->qdb().close();

		delete publicInstance_;
		publicInstance_ = 0;
	}
}


/// Inserting or updating objects in the database.
/*! id is the object's row in the database (for updates), or 0 (for inserts).
	table is the database table name
	colNames is a list of the column names that the values will be inserted under
	values is a list of constant pointers to QVariants that provide the values to insert.
	(Note that the const and const& arguments are designed to prevent memory copies, so this should be fast.)
	Return value: (IMPORTANT) returns the id of the row that was inserted into or updated, or 0 on failure.
	When inserting new objects, make sure to set their id to the return value afterwards, otherwise they will be duplicated on next insert.
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
	if(!query.exec()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, QString("database save failed. Could not execute query (%1). The SQL reply was: %2").arg(query.executedQuery()).arg(query.lastError().text())));
		return 0;
	}
	// Query succeeded.

	// If we don't have one, set the unique id for this object (now that the database has established it)
	if(id < 1) {
		QVariant lastId = query.lastInsertId();
		if(lastId.isValid()) {
			emit created(table, lastId.toInt());
			return lastId.toInt();
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -4, "Database save completed, but could not get the last id after insert. This should never happen."));
			emit updated(table, -1);
			return 0;
		}
	}
	// else (we already had an id, which was used for successful insert:
	else {
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
	if(!query.exec()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, QString("database save failed. Could not execute query (%1). The SQL reply was: %2").arg(query.executedQuery()).arg(query.lastError().text())));
		return false;
	}
	// Query succeeded.
	emit updated(table, id);
	return true;

}


/// Changing single values in the database (where the id isn't known).  Will update all rows based on the condition specified; \c whereClause is a string suitable for appending after an SQL "WHERE" term.  Will set the value in \c dataColumn to \c dataValue.
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
	if(!query.exec()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, QString("Failed to update the database. Could not execute the query (%1). The SQL reply was: %2").arg(query.executedQuery()).arg(query.lastError().text())));
		return false;
	}
	// Query succeeded.
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
	if(!query.exec()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, QString("Failed to delete the database object. Could not execute the query (%1). The SQL reply was: %2").arg(query.executedQuery()).arg(query.lastError().text())));
		return false;
	}
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
	if(!query.exec()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, QString("Failed to delete the database object(s). Could not execute the query (%1). The SQL reply was: %2").arg(query.executedQuery()).arg(query.lastError().text())));
		return 0;
	}
	emit removed(tableName, -1);

	// Query succeeded.
	return query.numRowsAffected();
}



/// retrieve object information from the database.
/*! id is the object's row in the database.
	table is the database table name
	colNames is a list of the column names that the values will be retrieved from
	values is a list of pointers to QVariants that will be modified with the retrived values.
	(Note that the const and & arguments are designed to prevent memory copies, so this should be fast.)
	Return value: returns true on success.
*/
QVariantList AMDatabase::retrieve(int id, const QString& table, const QStringList& colNames) const {

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
	if(!q.exec()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, QString("database retrieve failed. Could not execute query (%1). The SQL reply was: %2").arg(q.executedQuery()).arg(q.lastError().text())));
		return values;
	}
	// If we found a record at this id:
	if(q.first()) {
		// copy columns to return values:
		for(int i=0; i<colNames.count(); i++)
			values << q.value(i);

		q.finish();
	}
	// otherwise: didn't find this id.  That's normal if it's not there; just return empty list
	return values;
}


QVariant AMDatabase::retrieve(int id, const QString& table, const QString& colName) const {

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
	if(!q.exec()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, QString("database retrieve failed. Could not execute query (%1). The SQL reply was: %2").arg(q.executedQuery()).arg(q.lastError().text())));
		return false;
	}
	// If we found a record at this id:
	if(q.first()) {
		return q.value(0);
	}
	// else: didn't find this id.  That's normal if it's not there; just return null QVariant.
	else {
		return QVariant();
	}

}



/// returns a list of all the objecst/rows (by id) that match a given condition. \c whereClause is a string suitable for appending after an SQL "WHERE" statement.
QList<int> AMDatabase::objectsWhere(const QString& tableName, const QString& whereClause) const {

	QList<int> rl;

	/// \todo sanitize more than this...
	if(tableName.isEmpty() || whereClause.isEmpty()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, "Could not search the database. (Missing the table name or column name.)"));
		return rl;
	}

	QSqlQuery q( qdb() );

	q.prepare(QString("SELECT id FROM %1 WHERE %2").arg(tableName).arg(whereClause));
	q.exec();

	while(q.next()) {
		rl << q.value(0).toInt();
	}
	q.finish();


	return rl;
}

/// Return a list of all the objects/rows (by id) that match 'value' in a certain column.
/// ex: AMDatabase::db()->objectsMatching("name", "Carbon60"), or AMDatabase::db()->objectsMatching("dateTime", QDateTime::currentDateTime())
QList<int> AMDatabase::objectsMatching(const QString& tableName, const QString& colName, const QVariant& value) const {

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
	q.exec();

	while(q.next()) {
		rl << q.value(0).toInt();
	}
	q.finish();

	return rl;
}


/// Return a list of all the objects/rows (by id) that contain 'value' in a certain column
/// ex: AMDatabase::db()->scansContaining("name", "Carbon60") could return Scans with names Carbon60_alpha and bCarbon60_gamma
QList<int> AMDatabase::objectsContaining(const QString& tableName, const QString& colName, const QVariant& value) const {

	QList<int> rl;

	/// \todo sanitize more than this...
	if(tableName.isEmpty() || colName.isEmpty()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -10, "Could not search the database. (Missing the table name or column name.)"));
		return rl;
	}


	QSqlQuery q( qdb() );

	q.prepare(QString("SELECT id FROM %1 WHERE %2 LIKE ('%' || :val || '%')").arg(tableName).arg(colName));

	q.bindValue(":val", value);
	q.exec();

	while(q.next()) {
		rl << q.value(0).toInt();
	}
	q.finish();


	return rl;
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

	if(q.exec(qs)) {
		return true;
	}
	else {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -6, QString("database table create failed. Could not execute query (%1). The SQL reply was: %2").arg(q.executedQuery()).arg(q.lastError().text())));
		return false;
	}
}


/// ensure that a given column (with \c columName and \c columnType) exists, in the table \c tableName.  \c columnType is an SQLite type ("TEXT" or "INTEGER" recommended).
bool AMDatabase::ensureColumn(const QString& tableName, const QString& columnName, const QString& columnType) {

	QSqlQuery q( qdb() );

	q.prepare(QString("ALTER TABLE %1 ADD COLUMN %2 %3;").arg(tableName).arg(columnName).arg(columnType));

	if(q.exec()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, QString("Adding database column %1 to table %2.").arg(columnName).arg(tableName)));
		return true;
	}
	else {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, QString("Error adding database column %1 to table %2. Maybe it's already there? Sql reply says: %3").arg(columnName).arg(tableName).arg(q.lastError().text())));
		return false;
	}
}

bool AMDatabase::createIndex(const QString& tableName, const QString& columnNames) {
	QSqlQuery q( qdb() );
	QString indexName = QString("idx_%1_%2").arg(tableName, columnNames);
	indexName.remove(QRegExp("[\\s\\,\\;]"));// remove whitespace, commas, and semicolons from index name...
	q.prepare(QString("CREATE INDEX %1 ON %2(%3);").arg(indexName, tableName, columnNames));
	if(q.exec()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, QString("Added index on columns (%1) to table '%2'.").arg(columnNames).arg(tableName)));
		return true;
	}
	else {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, QString("Error adding index on columns (%1) to table '%2'. Maybe it's already there? Sql reply says: %3").arg(columnNames).arg(tableName).arg(q.lastError().text())));
		return false;
	}
}
