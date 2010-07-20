#ifndef ACQMAN_DATABASE_H
#define ACQMAN_DATABASE_H

#include "AMSettings.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QMap>


/*! This class encapsulates all access to the user's metadata database.  It is a singleton class like AMBeamline, except that it can provide access to either the user's private database or the public database.

	Access the database instances via public static functions:
		- AMDatabase::userdb(), or
		- AMDatabase::publicdb()




	This class provides tools for accessing and working with an arbitrary database.  (The actual schema -- or table structure -- for the user database is defined in AMDatabaseDefinition.)  The objective of this class is to cleanly encapsulate the useful SQL queries, and protect the database integrity by reducing the amount of times anyone requires direct access to run arbitrary queries.



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

	/// Converts from QVariant::Type to the recommended column type in the database
	static QString metaType2DbType(QVariant::Type type) {
		switch(type) {
		case QVariant::Invalid:
			return "NULL";
		case QVariant::Int:
			return "INTEGER";
		case QVariant::Double:
			return "REAL";
		case QVariant::ByteArray:
			return "BLOB";
		default:
			return "TEXT";
		}
	}

	/// Inserting or updating a row in the database.
	/*! id is the object's row in the database (for updates), or 0 (for inserts).
		table is the database table name
		colNames is a list of the column names that the values will be inserted under
		values is a list of constant references to QVariants that provide the values to insert.
		(Note that the const and const& arguments are designed to prevent memory copies, so this should be fast.)
		Return value: (IMPORTANT) returns the id of the row that was inserted into or updated, or 0 on failure.
		When inserting new objects, make sure to set their id to the return value afterwards, otherwise they will be duplicated on next insert.
	*/
	int insertOrUpdate(int id, const QString& table, const QStringList& colNames, const QList<const QVariant*>& values);

	/// changing single values in the database, at row \c id.
	bool update(int id, const QString& table, const QString& column, const QVariant& value);

	/// Changing single values in the database (where the id isn't known).  Will update all rows based on the condition specified; \c whereClause is a string suitable for appending after an SQL "WHERE" term.  Will set the value in \c dataColumn to \c dataValue.
	bool update(const QString& tableName, const QString& whereClause, const QString& dataColumn, const QVariant& dataValue);

	/// delete the object/row in \c tableName at id \c id. Returns true on success.
	bool deleteRow(int id, const QString& tableName);
	/// delete all objects/rows in \c tableName that meet a certain condition. \c whereClause is a string suitable for appending after an SQL "WHERE" term. Returns the number of rows deleted.
	int deleteRows(const QString& tableName, const QString& whereClause);




	/// retrieve the parameters of an object from the database.
	/*! id is the object's row in the database.
		table is the database table name
		colNames is a list of the column names that the values will be retrieved from
		values is a list of references to QVariants that will be modified with the retrived values.
		(Note that the const and & arguments are designed to prevent memory copies, so this should be fast.)
		Return value: returns true on success.
	*/
	bool retrieve(int id, const QString& table, const QStringList& colNames, const QList<QVariant*>& values);

	/// ensure that a table of the given name exists. If it doesn't, it will be created with the columns contained in \c columnNames. \c columnTypes is a list of SQLite column types ("TEXT", "INTEGER", etc.), which must have an entry for each \c columnName.
	bool ensureTable(const QString& tableName, const QStringList& columnNames, const QStringList& columnTypes);
	/// ensure that a given column (with \c columName and \c columnType) exists, in the table \c tableName.  \c columnType is an SQLite type ("TEXT" or "INTEGER" recommended).
	bool ensureColumn(const QString& tableName, const QString& columnName, const QString& columnType = "TEXT");

	/// Returns a QSqlQuery object for this database. The contents of the query have not been initialized. Beware: this can give you full-power access to the database. Don't break it!
	QSqlQuery query() { return QSqlQuery(qdb()); }


	/// Return a list of all the objects/rows (by id) that match 'value' in a certain column.
	/// ex: AMDatabase::db()->objectsMatching("name", "Carbon60"), or AMDatabase::db()->objectsMatching("dateTime", QDateTime::currentDateTime())
	QList<int> objectsMatching(const QString& tableName, const QString& colName, const QVariant& value);

	/// Return a list of all the objects/rows (by id) that contain 'value' in a certain column
	/// ex: AMDatabase::db()->scansContaining("name", "Carbon60") could return Scans with names Carbon60_alpha and bCarbon60_gamma
	QList<int> objectsContaining(const QString& tableName, const QString& colName, const QVariant& value);

	/// returns a list of all the objecst/rows (by id) that match a given condition. \c whereClause is a string suitable for appending after an SQL "WHERE" statement.
	QList<int> objectsWhere(const QString& tableName, const QString& whereClause);




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



};

#endif // DATABASE_H
