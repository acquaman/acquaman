/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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
#include <QSet>
#include <QMutex>


/// This class provides thread-safe, general access to an SQL database.
/*! Instances of this class are used to query or modify a database; all of the functions are thread-safe and will operate using a per-thread connection to the same underlying database.

	Access to instances is organized using a "multiton" approach: to create a new instance, use the static AMDatabase::createDatabase() function and include a connection name.  Later, access to the instance can be retrieved using the same connection name with the static AMDatabase::database() function. This ensures unique connection names across the entire program.

	Once you have an instance, you can query, insert rows, create tables, etc. using query(), insertOrUpdate(), update(), retrieve(), ensureTable(), deleteRow(), etc. All of these are thread-safe and can be called from any thread.

	\note Regarding related classes AMDbObject and AMDbObjectSupport: this class only provides tools for accessing and working with an arbitrary SQL database; no specific schema or meaning is implied.  The actual schema -- or table structure -- for an Acquaman user database is defined dynamically by the AMDbObject classes and AMDbObjectSupport.  The objective of this class is just to cleanly encapsulate the useful SQL queries, and protect the database integrity by reducing the amount of times anyone requires direct access to run arbitrary queries.
*/
class AMDatabase : public QObject {
	Q_OBJECT

public:

	// Multiton accessor functions: getting access to an AMDatabase instance.
	/////////////////////////////////////
	/// Create a new database instance with the given \c connectionName and \c dbAccessString.
	/*! The AMDatabase constructor is private so that we can enforce unique connection names across the program. This is the only way to access it.
The \c connectionName will be used to retrieve this instance later using AMDatabase::database(). It needs to be unique (program-wide); this function will return 0 if a connection with that name already exists.

The parameters by which to access the database are given in \c dbAccessString. (For the current SQLITE database, dbAccessString is just the path to the database file.)
*/
	static AMDatabase* createDatabase(const QString& connectionName, const QString& dbAccessString);
	/// Get access to a database based on the \c connectionName. If the connection doesn't exist, returns 0.
	static AMDatabase* database(const QString& connectionName);
	/// Delete a database instance that is not longer required.
	static void deleteDatabase(const QString& connectionName);


	// Static functions
	/////////////////////////////////////

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


	// Instance Functions
	///////////////////////////

	/// This is the connection name of this database instance.
	QString connectionName() {
		return connectionName_;
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
	/// changing multiple column values in the database, at row \c id.
	bool update(int id, const QString& table, const QStringList& columns, const QVariantList& values);

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

	/// Returns a QSqlQuery prepared to run a SELECT on the given \c tableName.  \c columnNames is a comma-separated list of columns to include in the search.  \c whereClause is a string suitable for appending after an SQL "WHERE" statement, or empty (by default). The query has been prepared, but not executed... You still need to call exec() on it, and you can still bindValue()s first.
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
	/// Access the QSqlDatabase object for this connection. This function is thread-safe, and will return a connection that can be used safely from the calling thread.
	QSqlDatabase qdb() const;


private:
	/// Constructor is private.  ConnectionName is a unique connection name (program-wide) for this database. dbAccessString provides the database-engine specific connection details.
	/*! (For the current SQLITE database, dbAccessString is just the path to the database file.)*/
	AMDatabase(const QString& connectionName, const QString& dbAccessString);

	// Instance variables:
	///////////////////////////

	/// This is the connection name specified in the constructor: a unique (program-wide) way of referring to this database.
	QString connectionName_;
	/// This is the string used to specify how to connect to the database.  (For the current SQLITE database, dbAccessString is just the path to the database file.)
	QString dbAccessString_;

	/// For every AMDatabase instance, we can actually have multiple connections to the database: one for each thread. All connections are to the same underlying database. This is a set of the thread IDs for which we've already created connections. (The connection name for those is our connectionName_ with the threadId appended.)
	mutable QSet<Qt::HANDLE> threadIDsOfOpenConnections_;
	/// This mutex is used in the thread-safe function qdb() to protect access to threadIDsOfOpenConnections_.
	mutable QMutex qdbMutex_;


	// Multiton variables:
	///////////////////////////
	static QHash<QString, AMDatabase*> connectionName2Instance_;
	static QMutex databaseLookupMutex_;

};

#endif // DATABASE_H
