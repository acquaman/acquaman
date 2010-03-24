#include "Database.h"

#include <QDebug>
#include <QDir>
#include <QStringList>

Database* Database::instance_ = 0;

// This constructor is protected; only access is through Database::db()
Database::Database() : QObject() {

	// QSqlDatabase is a strange class... You can create a new instance to create or access an existing connection,
	// and the connection will remain over all current and future instances of QSqlDatabase.
	// It has a "default" connection (which we will use as the user's database), as well as
	// named connections.  (We use the "public" connection for the public data database.)

	// Attempt to create user's data folder, only if it doesn't exist:
	QDir userDataDir(UserSettings::userDataFolder);
	if(!userDataDir.exists()) {
		qDebug() << "Database: alert: creating new user data folder " << UserSettings::userDataFolder;
		if(!userDataDir.mkpath(UserSettings::userDataFolder))
			qDebug() << "Database: error: could not create user data folder " << UserSettings::userDataFolder;
	}


	// Initialize user database:
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QString filename = UserSettings::userDataFolder + UserSettings::userDatabaseFilename;
	db.setDatabaseName(filename);
	bool okUser = db.open();

	if(okUser) {
		qDebug() << "Database: connection established to user database";
	}
	else {
		// todo: error handling
		qDebug() << "Database: error connecting to user database (file " << filename << "): " << db.lastError();
	}

	// Check to make sure tables exist in user database
	// TODO: structure this more cleanly?
	if(okUser) {
		QSqlQuery q(db);
		q.prepare("CREATE TABLE IF NOT EXISTS scanTable(id INTEGER PRIMARY KEY ASC, name TEXT, number INTEGER, sampleName TEXT, comments TEXT, startTime TEXT, channels TEXT)");
		if(!q.exec()) {
			qDebug() << "Database: failed to create scanTable.";
		}
	}



	// Initialize public database:
	db = QSqlDatabase::addDatabase("QSQLITE", "public");
	filename = Settings::publicDataFolder + Settings::publicDatabaseFilename;
	db.setDatabaseName(filename);
	bool okPublic = db.open();
	if(okPublic) {
		qDebug() << "Database: connection established to public database";
	}
	else {
		// todo: error handling
		qDebug() << "Database: error connecting to public database (file " << filename << "): " << db.lastError();

	}

	// TODO: create tables in public database...
	// TODO: instead of "public" and "user" databases, should we move to either (a) arbitrary number of concurrent databases, or (b) a single database?

}

Database* Database::db() {
	if(instance_ == 0)
		instance_ = new Database();
	return instance_;
}

void Database::releaseDb() {
	if(instance_) {
		instance_->userDb().close();
		instance_->publicDb().close();

		delete instance_;
		instance_ = 0;
	}
}

/// Return a list of all the Scans (by id) that match 'value' in a certain column {name, number, sample name, comment field, start time (rounded to second), or set of channels}
/// ex: Database::db()->scansMatching(Database::Name, "Carbon60"), or Database::db()->scansMatching(Database::StartTime, QDateTime::currentDateTime())
QList<int> Database::scansMatching(Database::Heading parameter, QVariant value, bool fromPublic) {

	QList<int> rl;

	QSqlDatabase qdb = fromPublic ? publicDb() : userDb();
	QSqlQuery q(qdb);

	switch(parameter) {
		case Database::Name:
			q.prepare("SELECT id FROM scanTable WHERE name=:val"); break;
		case Database::Number:
			q.prepare("SELECT id FROM scanTable where number=:val"); break;
		case Database::SampleName:
			q.prepare("SELECT id FROM scanTable WHERE sampleName=:val"); break;
		case Database::Comments:
			q.prepare("SELECT id FROM scanTable WHERE comments=:val"); break;
		case Database::Channels:
			q.prepare("SELECT id FROM scanTable WHERE channels=:val"); break;
		case Database::StartTime:
			q.prepare("SELECT id FROM scanTable WHERE startTime BETWEEN datetime(:val, '-1 minute') AND datetime(:val, '+1 minute')"); break;
	}
	q.bindValue(":val", value);
	q.exec();

	while(q.next()) {
		rl << q.value(0).toInt();
	}
	q.finish();

	return rl;
}

/// Return a list of all the Scans (by id) that contain part or all of 'value' in a certain column {name, number, sample name, comment field, start time (rounded to second), or set of channels}
/// ex: Database::db()->scansMatching(Database::Name, "Carbon60") could return Scans with names Carbon60_alpha and bCarbon60_gamma
QList<int> Database::scansContaining(Database::Heading parameter, QVariant value, bool fromPublic) {

	QList<int> rl;

	QSqlDatabase qdb = fromPublic ? publicDb() : userDb();
	QSqlQuery q(qdb);

	switch(parameter) {
		case Database::Name:
			q.prepare("SELECT id FROM scanTable WHERE name LIKE ('%' || :val || '%')"); break;
		case Database::Number:
			q.prepare("SELECT id FROM scanTable where number LIKE ('%' || :val || '%')"); break;
		case Database::SampleName:
			q.prepare("SELECT id FROM scanTable WHERE sampleName LIKE ('%' || :val || '%')"); break;
		case Database::Comments:
			q.prepare("SELECT id FROM scanTable WHERE comments LIKE ('%' || :val || '%')"); break;
		case Database::Channels:
			q.prepare("SELECT id FROM scanTable WHERE channels LIKE ('%' || :val || '%')"); break;
		case Database::StartTime:
			q.prepare("SELECT id FROM scanTable WHERE startTime LIKE ('%' || :val || '%')"); break;
	}
	q.bindValue(":val", value);
	q.exec();

	while(q.next()) {
		rl << q.value(0).toInt();
	}
	q.finish();

	return rl;
}


