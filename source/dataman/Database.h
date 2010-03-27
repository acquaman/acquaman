#ifndef ACQMAN_DATABASE_H
#define ACQMAN_DATABASE_H

#include "Settings.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QMap>


/*! This class encapsulates all access to the user's metadata.  It is a singleton class like Beamline, except that it can provide
	access to either the user's private database object or the public database object.

	Access the database object via public static functions:
        AMDatabase::userdb()
	or
        AMDatabase::publicdb()

        AMDatabase Table Structure
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


class AMDatabase : QObject {
    Q_OBJECT

public:

	/// Get access to the user's database. (The constructor is private... This is the only way to access it.)
        static AMDatabase* userdb();
	/// Get access to the public database. (The constructor is private... this is the only way in.)
        static AMDatabase* publicdb();

	/// Free the database resources if no longer needed
	static void releaseUserDb();
	static void releasePublicDb();

	/// Inserting or updating objects in the database.
	/*! id is the object's row in the database (for updates), or 0 (for inserts).
		table is the database table name
		colNames is a list of the column names that the values will be inserted under
		values is a list of constant references to QVariants that provide the values to insert.
		(Note that the const and const& arguments are designed to prevent memory copies, so this should be fast.)
		Return value: (IMPORTANT) returns the id of the row that was inserted into or updated, or 0 on failure.
		When inserting new objects, make sure to set their id to the return value afterwards, otherwise they will be duplicated on next insert.
	*/
	int insertOrUpdate(int id, const QString& table, const QStringList& colNames, const QList<const QVariant*>& values);

	/// retrieve an object from the database. (Create the object first; it will be modified to reflect its state in the database)
	/*! id is the object's row in the database.
		table is the database table name
		colNames is a list of the column names that the values will be retrieved from
		values is a list of references to QVariants that will be modified with the retrived values.
		(Note that the const and & arguments are designed to prevent memory copies, so this should be fast.)
		Return value: returns true on success.
	*/
	bool retrieve(int id, const QString& table, const QStringList& colNames, const QList<QVariant*>& values);


	/// Return a list of all the Scans (by id) that match 'value' in a certain column {name, number, sample name, comment field, start time (rounded to second), or set of channels}
        /// ex: AMDatabase::db()->objectsMatching(AMDatabase::Name, "Carbon60"), or AMDatabase::db()->scansMatching(AMDatabase::StartTime, QDateTime::currentDateTime())
	QList<int> scansMatching(const QString& colName, const QVariant& value);

	/// Return a list of all the Scans (by id) that contain 'value' in a certain column {name, number, sample name, comment field, start time (rounded to second), or set of channels}
        /// ex: AMDatabase::db()->scansContaining(AMDatabase::Name, "Carbon60") could return Scans with names Carbon60_alpha and bCarbon60_gamma
	QList<int> scansContaining(const QString& colName, const QVariant& value);

	/// Return the type of an object stored at 'id'. (Returns empty string if not found.)
	QString scanType(int id);

        /// AMDatabase admin / temporary testing only:
	bool ensureTable(const QString& tableName, const QStringList& columnNames, const QStringList& columnTypes);


signals:
	/// Emitted when an object is inserted or modified. Contains the id of the inserted or modified object.
	void updated(int id);

protected:
        /// Access the QSqlAMDatabase object for this connection.
        QSqlDatabase qdb() { return QSqlDatabase::database(connectionName_); }


private:
	/// Constructor is private.  ConnectionName is a unique connection name (program-wide) for this database. dbAccessString provides the database-engine specific connection details.
		/// (For the current SQLITE database, dbAccessString is just the path to the database file.)
        AMDatabase(const QString& connectionName, const QString& dbAccessString);
	QString connectionName_;

	/// Internal instance pointers
        static AMDatabase* userInstance_;
        static AMDatabase* publicInstance_;

	// given a column name key, allows lookup of the table containing that column:
	QMap<QString,QString> columns2tables;


};

#endif // DATABASE_H
