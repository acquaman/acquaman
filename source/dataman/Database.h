#ifndef DATABASE_H
#define DATABASE_H

#include "Settings.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "dataman/DbStorable.h"

/*! This class encapsulates all access to the user's metadata.  It is a singleton class like Beamline, except that it can provide
	access to either the user's private database object or the public database object.

	Access the database object via public static functions:
	Database::userdb()
	or
	Database::publicdb()

	Database Table Structure
	========================
	The main table stores only basic meta-data for each scan (as defined in Scan class.)
	Subclasses that provide more information are stored in separate tables (one for each layer of subclassing).
	The detailed tables are linked to the main table using the Scan's unique id().
	In all tables, this id() corresponds to id (the row index).

	Main Table Columns:
	-----------
	id: intrinsic row ID		[INTEGER PRIMARY KEY]
	name: user-given name		[TEXT]
	number: user-given number	[INTEGER]
	sampleName:					[TEXT]
	comments:					[TEXT]
	startTime:					[TEXT]
	channels: list of channel names (comma-separated) [STRING]

	(Note that we don't really need to know this information explicitly... All objects which can be
	stored in the database provide dbColumnNames() and dbColumnTypes() to give us this information dynamically.)










*/


class Database : QObject {
    Q_OBJECT

public:

	/// Get access to the user's database. (The constructor is private... This is the only way to access it.)
	static Database* userdb();
	/// Get access to the public database. (The constructor is private... this is the only way in.)
	static Database* publicdb();

	/// Free the database resources if no longer needed
	static void releaseUserDb();
	static void releasePublicDb();

	/// Inserting or updating objects in the database:
	bool insertOrUpdate(DbStorable& object);
	/// retrieve an object from the database. (Create the object first; it will be modified to reflect its state in the database)
	bool retrieve(DbStorable& object, int id);


	/// Return a list of all the Scans (by id) that match 'value' in a certain column {name, number, sample name, comment field, start time (rounded to second), or set of channels}
	/// ex: Database::db()->objectsMatching(Database::Name, "Carbon60"), or Database::db()->scansMatching(Database::StartTime, QDateTime::currentDateTime())
	QList<int> scansMatching(const QString& colName, const QVariant& value);

	/// Return a list of all the Scans (by id) that contain 'value' in a certain column {name, number, sample name, comment field, start time (rounded to second), or set of channels}
	/// ex: Database::db()->scansContaining(Database::Name, "Carbon60") could return Scans with names Carbon60_alpha and bCarbon60_gamma
	QList<int> scansContaining(const QString& colName, const QVariant& value);

	/// Database admin / temporary testing only:
	bool ensureTable(const QString& tableName, const QStringList& columnNames, const QStringList& columnTypes);


protected:
	/// Access the QSqlDatabase object for this connection.
	QSqlDatabase qdb() { return QSqlDatabase::database(connectionName_); }


private:
	/// Constructor is private.  ConnectionName is a unique connection name (program-wide) for this database. dbAccessString provides the database-engine specific connection details.
		/// (For the current SQLITE database, dbAccessString is just the path to the database file.)
	Database(const QString& connectionName, const QString& dbAccessString);
	QString connectionName_;

	/// Internal instance pointers
	static Database* userInstance_;
	static Database* publicInstance_;


};

#endif // DATABASE_H
