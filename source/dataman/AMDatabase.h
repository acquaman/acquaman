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


#ifndef ACQMAN_DATABASE_H
#define ACQMAN_DATABASE_H

#include "util/AMSettings.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QMap>


/// This class encapsulates all access to the user's metadata database.  It is a singleton class like AMBeamline, except that it can provide access to either the user's private database or the public database.
/*!
	Access the database instances via public static functions:
		- AMDatabase::userdb(), or
		- AMDatabase::publicdb()


	This class provides tools for accessing and working with an arbitrary database.  (The actual schema -- or table structure -- for the user database is defined in AMDatabaseDefinition.)  The objective of this class is to cleanly encapsulate the useful SQL queries, and protect the database integrity by reducing the amount of times anyone requires direct access to run arbitrary queries.
*/
class AMDatabase : public QObject {
	Q_OBJECT

public:

	/// Get access to the user's database. (The constructor is private... This is the only way to access it.)
	static AMDatabase* userdb();
	/// Get access to the public database. (The constructor is private... this is the only way in.)
	static AMDatabase* publicdb();
	/// Get access to a database based on the connection name. If the connection doesn't exist, returns 0
	static AMDatabase* dbByName(const QString& connectionName);

	/// Free the database resources if no longer needed
	static void releaseUserDb();
	static void releasePublicDb();

	/// This is the connection name of your database object:
	QString connectionName() {
		return qdb().connectionName();
	}

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
		// Everything else should be prepared to write itself out as a string.
		default:
			return "TEXT";
		}
	}

	/// Inserting or updating a row in the database.
	/*! \c id is the object's row in the database (for updates), or 0 (for inserts).
		\c table is the database table name
		\c colNames is a list of the column names that the values will be inserted under
		\c values is a list of QVariants that provide the values to insert.
		(Note that QList uses copy-on-write / implicit sharing, so you can pass a QList<QVariant> around very fast, as long as it isn't modified.)
		Return value: (IMPORTANT) returns the id of the row that was inserted into or updated, or 0 on failure.
		When inserting new objects, make sure to set their id to the return value afterwards, otherwise they will be duplicated on next insert.
	*/
	int insertOrUpdate(int id, const QString& table, const QStringList& colNames, const QVariantList& values);

	/// changing single values in the database, at row \c id.
	bool update(int id, const QString& table, const QString& column, const QVariant& value);

	/// Changing single values in the database (where the id isn't known).  Will update all rows based on the condition specified; \c whereClause is a string suitable for appending after an SQL "WHERE" term.  Will set the value in \c dataColumn to \c dataValue.
	bool update(const QString& tableName, const QString& whereClause, const QString& dataColumn, const QVariant& dataValue);

	/// delete the object/row in \c tableName at id \c id. Returns true on success.
	bool deleteRow(int id, const QString& tableName);
	/// delete all objects/rows in \c tableName that meet a certain condition. \c whereClause is a string suitable for appending after an SQL "WHERE" term. Returns the number of rows deleted.
	int deleteRows(const QString& tableName, const QString& whereClause);




	/// retrieve the parameters of an object from the database.
	/*! \c id is the object's row in the database.
		\c table is the database table name
		\c colNames is a list of the column names that the values will be retrieved from
		Returns a list of QVariants that will hold the results. Note that QList uses implicit sharing / copy-on-write, so returning the result is fast -- as long you don't modify the resulting list (ie: call non-const methods on it).

		Returns an empty list on failure.
	*/
	QVariantList retrieve(int id, const QString& table, const QStringList& colNames) const;
	/// Retrieve a single parameter/value for an object.  This is simpler than retrieve() when all you need is a single value.
	QVariant retrieve(int id, const QString& table, const QString& colName) const;

	/// ensure that a table of the given name exists. If it doesn't, it will be created with the columns contained in \c columnNames. \c columnTypes is a list of SQLite column types ("TEXT", "INTEGER", etc.), which must have an entry for each \c columnName.
	bool ensureTable(const QString& tableName, const QStringList& columnNames, const QStringList& columnTypes, bool reuseDeletedIds = true);
	/// ensure that a given column (with \c columName and \c columnType) exists, in the table \c tableName.  \c columnType is an SQLite type ("TEXT" or "INTEGER" recommended).
	bool ensureColumn(const QString& tableName, const QString& columnName, const QString& columnType = "TEXT");
	/// create an index on a column or columns \c columnNames in the table \c tableName. For multiple columns, separate the columnNames with commas.
	bool createIndex(const QString& tableName, const QString& columnNames);

	/// Returns a QSqlQuery object for this database. The contents of the query have not been initialized. Beware: this can give you full-power access to the database. Don't break it!
	QSqlQuery query() { return QSqlQuery(qdb()); }

	/// Returns a QSqlQuery initialized to run a SELECT on the given \c tableName.  \c columnNames is a comma-separated list of columns to include in the search.  \c whereClause is a string suitable for appending after an SQL "WHERE" statement, or empty (by default).
	QSqlQuery select(const QString& tableName, const QString& columnNames, const QString& whereClause = QString());


	/// Return a list of all the objects/rows (by id) that match 'value' in a certain column.
	/// ex: AMDatabase::db()->objectsMatching("name", "Carbon60"), or AMDatabase::db()->objectsMatching("dateTime", QDateTime::currentDateTime())
	QList<int> objectsMatching(const QString& tableName, const QString& colName, const QVariant& value) const;

	/// Return a list of all the objects/rows (by id) that contain 'value' in a certain column
	/// ex: AMDatabase::db()->scansContaining("name", "Carbon60") could return Scans with names Carbon60_alpha and bCarbon60_gamma
	QList<int> objectsContaining(const QString& tableName, const QString& colName, const QVariant& value) const;

	/// returns a list of all the objecst/rows (by id) that match a given condition. \c whereClause is a string suitable for appending after an SQL "WHERE" statement.
	QList<int> objectsWhere(const QString& tableName, const QString& whereClause) const;


	/// For people who really know what they're doing. You shouldn't normally use this.
	void startTransaction() {
		qdb().transaction();
	}
	void commitTransaction() {
		qdb().commit();
	}
	void rollbackTransaction() {
		qdb().rollback();
	}




signals:
	/// Emitted when an object is newly created and stored in the db. Contains the id of the newly-inserted object.  \note This is only emitted when using the AMDatabase API to insert rows. If using a raw SQL query, this signal will not be emitted.
	void created(const QString& tableName, int id);
	/// Emitted when an object is modified. Contains the id of the modified object, or -1 if a whole refresh is recommended.  \note This is only emitted when using the AMDatabase API to modify rows. If using a raw SQL query, this signal will not be emitted.
	void updated(const QString& tableName, int id);
	/// Emitted after an object is removed. Contains the old id of the removed object.  \note This is only emitted when using the AMDatabase API to remove rows. If using a raw SQL query, this signal will not be emitted.
	void removed(const QString& tableName, int oldId);

protected:
	/// Access the QSqlAMDatabase object for this connection.
	QSqlDatabase qdb() const { return QSqlDatabase::database(connectionName_); }


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
