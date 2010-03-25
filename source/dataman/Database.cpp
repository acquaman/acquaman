#include "Database.h"

#include <QDebug>
#include <QDir>
#include <QStringList>

/// Internal instance pointers
Database* Database::userInstance_ = 0;
Database* Database::publicInstance_ = 0;

// This constructor is protected; only access is through Database::db()
Database::Database(const QString& connectionName, const QString& dbAccessString) :
		QObject(),
		connectionName_(connectionName) {

	// Initialize the database:
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName_);
	db.setDatabaseName(dbAccessString);
	bool ok = db.open();

	if(ok) {
		qDebug() << "Database: connection established to database:" << connectionName_;
	}
	else {
		// todo: error handling
		qDebug() << "Database: error connecting to database (access " << dbAccessString << "): " << db.lastError();
	}

}

/// Access (or create, if it doesn't exist) the user database [static]
Database* Database::userdb() {

	if(userInstance_ == 0) {

		// Attempt to create user's data folder, only if it doesn't exist:
		QDir userDataDir(UserSettings::userDataFolder);
		if(!userDataDir.exists()) {
			qDebug() << "Database: alert: creating new user data folder " << UserSettings::userDataFolder;
			if(!userDataDir.mkpath(UserSettings::userDataFolder))
				qDebug() << "Database: error: could not create user data folder " << UserSettings::userDataFolder;
		}

		// create a new Database object for the user database:
		QString filename = UserSettings::userDataFolder + UserSettings::userDatabaseFilename;
		userInstance_ = new Database("user", filename);

	}
	return userInstance_;
}

/// Access (or create, if it doesn't exist) the public database [static]
Database* Database::publicdb() {

	if(publicInstance_ == 0) {

		// create a new Database object for the public database:
		QString filename = Settings::publicDataFolder + Settings::publicDatabaseFilename;
		publicInstance_ = new Database("public", filename);

	}
	return publicInstance_;
}

/// Shut down database connections and free database resources [static]
void Database::releaseUserDb() {
	if(userInstance_) {
		userInstance_->qdb().close();
		delete userInstance_;
		userInstance_ = 0;
	}
}
void Database::releasePublicDb() {
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
int Database::insertOrUpdate(int id, const QString& table, const QStringList& colNames, const QList<const QVariant*>& values) {

	QSqlDatabase db = qdb();

	if(!db.isOpen()) {
		qDebug() << "Database: saving failed; database is not open.";
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
	query.prepare(QString("INSERT OR REPLACE INTO '%1' (id, %2) VALUES (%3)").arg(table).arg(cols).arg(colPlaceholders));

	// If we have a unique id already, use that (This will update ourself in the DB)
	if(id > 0)
		query.bindValue(0, id);
	// Otherwise, use NULL for the id. (This will create a new one.)
	else
		query.bindValue(0, QVariant(QVariant::Int));

	// Bind remaining values
	for(int i=0; i<colNames.count(); i++)
		query.bindValue(i+1, *(values.at(i)));

	// Run query. Query failed?
	if(!query.exec()) {
		qDebug() << "Database: saving failed; could not execute query: " << query.executedQuery();
		return 0;
	}
	// Query succeeded.

	// If we don't have one, set the unique id for this object (now that the database has established it)
	if(id < 1) {
		QVariant lastId = query.lastInsertId();
		if(lastId.isValid())
			return lastId.toInt();
		else {
			qDebug() << "Database: insert succeeded, but could not get lastId after insert. This should never happen...";
			return 0;
		}
	}
	// else (we already had an id, which was used for successful insert:
	else {
		return id;
	}


}

/// retrieve object information from the database.
/*! id is the object's row in the database.
	table is the database table name
	colNames is a list of the column names that the values will be retrieved from
	values is a list of pointers to QVariants that will be modified with the retrived values.
	(Note that the const and & arguments are designed to prevent memory copies, so this should be fast.)
	Return value: returns true on success.
*/
bool Database::retrieve(int id, const QString& table, const QStringList& colNames, const QList<QVariant*>& values) {

	// create a query on our database connection:
	QSqlQuery q( qdb() );

	// Create the list of columns:
	QString cols = colNames.join(", ");	// this will become something like "name, number, sampleName, comments, startTime"
	// Prepare the query. Todo: sanitize column names and table name. (Can't use binding because it's not an expression here)
	q.prepare(QString("SELECT (%1) FROM '%2' WHERE id = ?").arg(cols).arg(table));
	q.bindValue(0, id);

	// run query and return true if succeeded at finding id:
	if(q.exec() && q.first()) {

		// copy columns to return values:
		for(int i=0; i<values.count(); i++)
			*(values.at(i)) = q.value(i);

		q.finish();
		return true;
	}

	else {
		return false;
	}
}






/// Return a list of all the objects (by id) that match 'value' in a certain column {name, number, sample name, comment field, start time (rounded to second), or set of channels}
/// ex: Database::db()->objectsMatching(Database::Name, "Carbon60"), or Database::db()->scansMatching(Database::StartTime, QDateTime::currentDateTime())
QList<int> Database::scansMatching(const QString& colName, const QVariant& value) {

	// return value: list of id's that match
	QList<int> rl;

	// We've been given a column name. Figure out what table this could be in:
	QStringList tables = columns2tables.values(colName);

	// Note: we're searching ALL the tables that have a column with this name...
	foreach(QString table, tables) {

		QSqlQuery q( qdb() );
		// Todo: we're just searching the scanTable here... how to search all tables?

		// For date/times, we want a resolution of one minute to count as a match
		if(value.type() == QVariant::DateTime)
			q.prepare(QString("SELECT id FROM %1 WHERE %2 BETWEEN datetime(:val, '-1 minute') AND datetime(:val, '+1 minute')").arg(table).arg(colName));
		else
			q.prepare(QString("SELECT id FROM %1 WHERE %2 = :val").arg(table).arg(colName));

		q.bindValue(":val", value);
		q.exec();

		while(q.next()) {
			rl << q.value(0).toInt();
		}
		q.finish();
	}

	return rl;
}


/// Return a list of all the Scans (by id) that contain 'value' in a certain column
/// ex: Database::db()->scansContaining(Database::Name, "Carbon60") could return Scans with names Carbon60_alpha and bCarbon60_gamma
QList<int> Database::scansContaining(const QString& colName, const QVariant& value) {

	QList<int> rl;

	// We've been given a column name. Figure out what table this could be in:
	QStringList tables = columns2tables.values(colName);

	// Note: we're searching ALL the tables that have a column with this name.
	foreach(QString table, tables) {

		QSqlQuery q( qdb() );
		// Todo: we're just searching the scanTable here... how to search all tables?

		q.prepare(QString("SELECT id FROM %1 WHERE %2 LIKE ('%' || :val || '%')").arg(table).arg(colName));

		q.bindValue(":val", value);
		q.exec();

		while(q.next()) {
			rl << q.value(0).toInt();
		}
		q.finish();
	}

	return rl;
}

/// Database admin / temporary testing only:
bool Database::ensureTable(const QString& tableName, const QStringList& columnNames, const QStringList& columnTypes) {

	if(columnNames.count() != columnTypes.count()) {
		qDebug() << "Database: could not create table: invalid structure. Different number of column names and types.\n  Names:" << columnNames << "\n  Types:" << columnTypes;
		return false;
	}

	// Maintain a mapping from column names to tables...
	// Given a column names, this will let us search in the right table for it. (Used by scansMatching, scansContaining...)
	foreach(QString colName, columnNames) {
		columns2tables.insertMulti(colName, tableName);
	}

	// todo: sanitize all inputs...
	QSqlQuery q(qdb());

	QString qs = "CREATE TABLE IF NOT EXISTS ";
	qs.append(tableName);
	qs.append("(id INTEGER PRIMARY KEY ASC, ");
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
		qDebug() << "Database: error creating table using query: " << qs;
		return false;
	}
}

/// Return the type of an object stored at 'id'. (Returns empty string if not found.)
QString Database::scanType(int id) {
	// create a query on our database connection:
	QSqlQuery q( qdb() );

	// Prepare the query. Todo: sanitize name?
	q.prepare(QString("SELECT type FROM %1 WHERE id = ?").arg(Settings::dbObjectTableName));
	q.bindValue(0, id);

	// run query and return true if succeeded at finding id:
	if(q.exec() && q.first()) {

		return q.value(0).toString();
	}

	else {
		return "";
	}
}
