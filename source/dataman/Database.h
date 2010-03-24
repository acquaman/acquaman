#ifndef DATABASE_H
#define DATABASE_H

#include "Settings.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

/*! This class encapsulates all access to the user's metadata.  It is a singleton class like Beamline.  All access is through Database::db().

	Database Table Structure
	========================
	The main table stores only basic meta-data for each scan (as defined in Scan class.)
	Subclasses that provide more information are stored in separate tables (one for each layer of subclassing).
	The detailed tables are linked to the main table using the Scan's unique id().
	In the main table, this id() corresponds to id (the row index).  In detail tables, it corresponds to scanId.

	Main Table Columns:
	-----------
	id: intrinsic row ID		[INTEGER PRIMARY KEY]
	name: user-given name		[TEXT]
	number: user-given number	[INTEGER]
	sampleName:					[TEXT]
	comments:					[TEXT]
	startTime:					[TEXT]
	channels: list of channel names (comma-separated) [STRING]










*/


class Database : QObject {
    Q_OBJECT

public:

	enum Heading { Name = 1, Number, SampleName, Comments, StartTime, Channels };

	/// Get access to the database class. (The constructor is private... This is the only way to access it.)
    static Database* db();
	/// Free the database resources if no longer needed
    static void releaseDb();


	/// Return a list of all the Scans (by id) that match 'value' in a certain column {name, number, sample name, comment field, start time (rounded to second), or set of channels}
	/// ex: Database::db()->scansMatching(Database::Name, "Carbon60"), or Database::db()->scansMatching(Database::StartTime, QDateTime::currentDateTime())
	QList<int> scansMatching(Database::Heading parameter, QVariant value, bool fromPublic = false);

	/// Return a list of all the Scans (by id) that contain 'value' in a certain column {name, number, sample name, comment field, start time (rounded to second), or set of channels}
	/// ex: Database::db()->scansContaining(Database::Name, "Carbon60") could return Scans with names Carbon60_alpha and bCarbon60_gamma
	QList<int> scansContaining(Database::Heading parameter, QVariant value, bool fromPublic = false);


/// Consider these "protected".  You're only allowed to use them from Scan::loadFromDb or Scan::saveToDb, and then only if you know what you're doing!
	/// Retrieve a QSqlDatabase object for the user database.
	QSqlDatabase userDb() { return QSqlDatabase::database(); }
	/// Retrieve a QSqlDatabase object for the public db.
	QSqlDatabase publicDb() { return QSqlDatabase::database("public"); }
	// TODO: these (above) might be too dangerous. Could be safer to encapsulate all query processing in Database and not let Scan-subclass writers touch the SQL.
		// How?

private:
	/// Constructor is private.
    Database();
	/// Internal instance pointer
    static Database* instance_;
};

#endif // DATABASE_H
