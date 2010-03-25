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

/// Inserting or updating objects in the database:
bool Database::insertOrUpdate(DbStorable& object) {

	QSqlDatabase db = qdb();

	if(!db.isOpen()) {
		qDebug() << "Database: saving failed; database is not open.";
		return false;
	}

	// start a transaction
	db.transaction();

	// Create the list of columns:
	QStringList colList = object.dbColumnNames();

	QString cols = colList.join(", ");	// this will become something like "name, number, sampleName, comments, startTime"
	QString colPlaceholders;
	for(int i=0; i<colList.count()+2; i++)
		colPlaceholders.append("?, ");
	colPlaceholders.chop(2);	// remove trailing ", " from "?, ?, ?, ?, ...?, "
	// placeholders will become something like "?, ?, ?, ?, ?, ?", with enough ? for each column name + id + className (which we'll be adding to the front)

	QString table = object.dbTableName();

	// Prepare the query. Todo: sanitize column names and table name. (Can't use binding because it's not an expression here)
	QSqlQuery query(db);
	query.prepare(QString("INSERT OR REPLACE INTO '%1' (id, className, %2) VALUES (%3)").arg(table).arg(cols).arg(colPlaceholders));

	// If we have a unique id already, use that (This will update ourself in the DB)
	if(object.dbId() > 0)
		query.bindValue(0, object.dbId());
	// Otherwise, use NULL for the id. (This will create a new one.)
	else
		query.bindValue(0, QVariant(QVariant::Int));

	// Bind remaining values
	query.bindValue(1, object.dbClassName());
	for(int i=2; i<colList.count()+2; i++)
		query.bindValue(i, object.dbValue(i));

	// Query failed:
	if(!query.exec()) {
		qDebug() << "Database: saving failed; could not execute query: " << query.executedQuery();
		return false;
	}
	// Query succeeded.
	else {
		// If we don't have one, set the unique id for this object (now that the database has established it)
		if(object.dbId() < 1) {
			QVariant lastId = query.lastInsertId();
			if(lastId.isValid())
				object.dbSetId(lastId.toInt());
			else {
				qDebug() << "Database: insert succeeded, but could not get lastId after insert. This should never happen...";
			}
		}
	}
	// end transaction
	db.commit();

	return true;
}

/// retrieve an object from the database. (Create the object first; it will be modified to reflect its state in the database)
bool Database::retrieve(DbStorable& object, int id) {

	// create a query on our database connection:
	QSqlQuery q( qdb() );

	// Create the list of columns:
	QStringList colList = object.dbColumnNames();
	QString cols = colList.join(", ");	// this will become something like "name, number, sampleName, comments, startTime"
	QString table = object.dbTableName();
	// Prepare the query. Todo: sanitize column names and table name. (Can't use binding because it's not an expression here)
	q.prepare(QString("SELECT (%1) FROM '%2' WHERE id = ?").arg(cols).arg(table));
	q.bindValue(0, id);

	// run query and return true if succeeded at finding id:
	if(q.exec() && q.first()) {

		object.dbSetId(id);
		for(int i=0; i<colList.count(); i++)
			object.dbSetValue(i, q.value(i));

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

	QList<int> rl;

	QSqlQuery q( qdb() );
	// Todo: we're just searching the scanTable here... how to search all tables?

	// For date/times, we want a resolution of one minute to count as a match
	if(value.type() == QVariant::DateTime)
		q.prepare(QString("SELECT id FROM scanTable WHERE '%1' BETWEEN datetime(:val, '-1 minute') AND datetime(:val, '+1 minute')").arg(colName));
	else
		q.prepare(QString("SELECT id FROM scanTable WHERE '%1' = '?'").arg(colName));

	q.bindValue(0, value);
	q.exec();

	while(q.next()) {
		rl << q.value(0).toInt();
	}
	q.finish();

	return rl;
}


/// Return a list of all the Scans (by id) that contain 'value' in a certain column {name, number, sample name, comment field, start time (rounded to second), or set of channels}
/// ex: Database::db()->scansContaining(Database::Name, "Carbon60") could return Scans with names Carbon60_alpha and bCarbon60_gamma
QList<int> Database::scansContaining(const QString& colName, const QVariant& value) {

	QList<int> rl;

	QSqlQuery q( qdb() );
	// Todo: we're just searching the scanTable here... how to search all tables?

	q.prepare(QString("SELECT id FROM scanTable WHERE '%1' LIKE ('%' || ? || '%')").arg(colName));

	q.bindValue(0, value);
	q.exec();

	while(q.next()) {
		rl << q.value(0).toInt();
	}
	q.finish();

	return rl;
}

/// Database admin / temporary testing only:
bool Database::ensureTable(const QString& tableName, const QStringList& columnNames, const QStringList& columnTypes) {
	// todo: sanitize all inputs...
	QSqlQuery q(qdb());

	QString qs = "CREATE TABLE IF NOT EXISTS ";
	qs.append(tableName);
	qs.append("(id INTEGER PRIMARY KEY ASC, className TEXT, ");
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
