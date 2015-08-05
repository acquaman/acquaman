/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AM_DATABASE_H
#define AM_DATABASE_H

#include "util/AMSettings.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSet>
#include <QHash>
#include <QMutex>
#include <QStringList>

#define AMDATABASE_ERROR_COLUMN_VALUE_COUNT_MISMATCH -3102
#define AMDATABASE_COMMIT_CONTENTION_SUCCEEDED -3103
#define AMDATABASE_COMMIT_CONTENTION_FAILED -3104
#define AMDATABASE_LOCK_FOR_EXECQUERY_CONTENTION_SUCCEEDED -3105
#define AMDATABASE_LOCK_FOR_EXECQUERY_CONTENTION_FAILED -3106
#define AMDATABASE_MISSING_TABLE_NAME_IN_RETRIEVE -3107
#define AMDATABASE_RETRIEVE_QUERY_FAILED -3108
#define AMDATABASE_IS_READ_ONLY -3109

/// This class provides thread-safe, general access to an SQL database.
/*! Instances of this class are used to query or modify a database; all of the functions are thread-safe and will operate using a per-thread connection to the same underlying database.

	Access to instances is organized using a "multiton" approach: to create a new instance, use the static AMDatabase::createDatabase() function and include a connection name.  Later, access to the instance can be retrieved using the same connection name with the static AMDatabase::database() function. This ensures unique connection names across the entire program.

	Once you have an instance, you can query, insert rows, create tables, etc. using query(), insertOrUpdate(), update(), retrieve(), ensureTable(), deleteRow(), etc. All of these are thread-safe and can be called from any thread.

	\note Regarding related classes AMDbObject and AMDbObjectSupport: this class only provides tools for accessing and working with an arbitrary SQL database; no specific schema or meaning is implied.  The actual schema -- or table structure -- for an Acquaman user database is defined dynamically by the AMDbObject classes and AMDbObjectSupport.  The objective of this class is just to cleanly encapsulate the useful SQL queries, and protect the database integrity by reducing the amount of times anyone requires direct access to run arbitrary queries.
*/
class AMDatabase : public QObject {
	Q_OBJECT

public:
	virtual ~AMDatabase();

	// Multiton accessor functions: getting access to an AMDatabase instance.
	/////////////////////////////////////
	/// Create a new database instance with the given \c connectionName and \c dbAccessString.
	/*! The AMDatabase constructor is private so that we can enforce unique connection names across the program. This is the only way to access it.
		The \c connectionName will be used to retrieve this instance later using AMDatabase::database(). It needs to be unique (program-wide); this function will return 0 if a connection with that name already exists.
		The parameters by which to access the database are given in \c dbAccessString. (For the current SQLITE database, dbAccessString is just the path to the database file.)
		\param openAsReadOnly is a defaulted parameter that defaults to false (you typically want the ability to have changes to your database stick).  If set to true, execQuery() will not take place for statements that modify the database.
*/
	static AMDatabase* createDatabase(const QString& connectionName, const QString& dbAccessString, bool openAsReadOnly = false);
	/// Get access to a database based on the \c connectionName. If the connection doesn't exist, returns 0.
	static AMDatabase* database(const QString& connectionName);
	/// Delete a database instance that is not longer required.
	static void deleteDatabase(const QString& connectionName);
	/// Returns a list of all registered databases as given by connectionName
	static QStringList registeredDatabases();


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


	/// Contrary to what the docs say, the QSQLiteDriver never re-tries a query that fails when the database was busy/locked. You can use this function instead of QSqlQuery::exec(). Whenever a query fails due to a busy error, it will block up to a maximum timeout of \c timeoutMs ms while re-trying automatically every 5ms.
	bool execQuery(QSqlQuery& query, int timeoutMs = 5000);


	// Instance Functions
	///////////////////////////

	/// This is the connection name of this database instance.
	QString connectionName() const { return connectionName_; }

	/// This is the full path to the database
	QString dbAccessString() const { return dbAccessString_; }

	/// This returns whether or not this instance can be written to (all valid connections can be read from)
	bool isReadOnly() const { return isReadOnly_; }

	/// This returns whether or not the database has any tables (if no tables, then it's empty)
	bool isEmpty() const { return (qdb().tables().count() == 0); }


	/// Inserting or updating a row in the database.
	/*! \c id is the object's row in the database (for updates), or 0 (for inserts).
		\c table is the database table name
		\c colNames is a list of the column names that the values will be inserted under
		\c values is a list of QVariants that provide the values to insert.
		(Note that QList uses copy-on-write / implicit sharing, so you can pass a QList<QVariant> around very fast, as long as it isn't modified.)
		Return value: (IMPORTANT) returns the id of the row that was inserted into or updated, or 0 on failure.
		When inserting new AMDbObjects, make sure to set their id to the return value afterwards, otherwise they will be duplicated on next insert.
	*/
	int insertOrUpdate(int id, const QString& table, const QStringList& colNames, const QVariantList& values);

	/// changing single values in the database, at row \c id.
	bool update(int id, const QString& table, const QString& column, const QVariant& value);
	/// changing multiple column values in the database, at row \c id.
	bool update(int id, const QString& table, const QStringList& columns, const QVariantList& values);

	/// Changing single values in the database (where the id isn't known).  Will update all rows based on the condition specified. \c whereClause is a string suitable for appending after an SQL "WHERE" term.  Will set the value in \c dataColumn to \c dataValue.
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
	QVariantList retrieve(int id, const QString& table, const QStringList& colNames);
	/// retrieve a column from the database
	/*! \c table is the database table name
		\c colName is the name of the column you wish to get all the values for
		Returns a list of QVariants that will hold the results. Note that QList uses implicit sharing / copy-on-write, so returning the result is fast -- as long you don't modify the resulting list (ie: call non-const methods on it).

		No id parameter is used, because a value for each id is returned
		Returns an empty list on failure.
	*/
	QVariantList retrieve(const QString& table, const QString& colName);

	/// Retrieve a single parameter/value for an object.  This is simpler than retrieve() when all you need is a single value.
	QVariant retrieve(int id, const QString& table, const QString& colName);

	/// Retrieves the maximum value stored in table.colName, with the option to match prior to detemining the max with a where clause.
	QVariant retrieveMax(const QString& table, const QString& colName, const QString& whereClause = QString());

	/// Checks whether \param tableName exists within the database.
	bool tableExists(const QString &tableName);
	/// Checks whether \param columnName exists inside of \param tableName.  Assumes that table name exists.
	bool columnExists(const QString &tableName, const QString &columnName);
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
	QList<int> objectsMatching(const QString& tableName, const QString& colName, const QVariant& value);

	/// Return a list of all the objects/rows (by id) that contain 'value' in a certain column
	/// ex: AMDatabase::db()->scansContaining("name", "Carbon60") could return Scans with names Carbon60_alpha and bCarbon60_gamma
	QList<int> objectsContaining(const QString& tableName, const QString& colName, const QVariant& value);

	/// returns a list of all the objecst/rows (by id) that match a given condition. \c whereClause is a string suitable for appending after an SQL "WHERE" statement. If you want the id of all objects, you can omit the where clause.
	QList<int> objectsWhere(const QString& tableName, const QString& whereClause = QString());


	/// Starts an SQL transaction if the implementation supports them. Returns true on success.
	bool startTransaction();
	/// Tries to commit a transaction. Since SQLite commits may fail with SQLITE_BUSY errors, this will keep retrying up to \c timeoutMs ms for the commit to succeed. Returns true on success.
	bool commitTransaction(int timeoutMs = 5000);
	/// Tries (just once) to roll back (revert) a transaction. Normally this always succeeds, as long as a transaction is open. Returns true on success.
	bool rollbackTransaction();

	/// Returns whether this connection supports transactions (ie: the database implementation supports transactions)
	bool supportsTransactions() const;
	/// Returns whether a transaction is in progress for this connection
	bool transactionInProgress() const;




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
	/// \param openAsReadOnly is a defaulted parameter that defaults to false (you typically want the ability to have changes to your database stick).  If set to true, execQuery() will not take place for statements that modify the database.
	AMDatabase(const QString& connectionName, const QString& dbAccessString, bool openAsReadOnly = false);

	// Instance variables:
	///////////////////////////

	/// This is the connection name specified in the constructor: a unique (program-wide) way of referring to this database.
	QString connectionName_;
	/// This is the string used to specify how to connect to the database.  (For the current SQLITE database, dbAccessString is just the path to the database file.)
	QString dbAccessString_;
	/// This holds whether or not the database can to written to as well as read from (all valid connections can be read from)
	bool isReadOnly_;

	/// For every AMDatabase instance, we can actually have multiple connections to the database: one for each thread. All connections are to the same underlying database. This is a set of the thread IDs for which we've already created connections. (The connection name for those is our connectionName_ with the threadId appended.)
	mutable QSet<Qt::HANDLE> threadIDsOfOpenConnections_;
	/// Transactions are opened on a per-thread/per-connection basis. They cannot be nested, so this is used to track if an explicit transaction is in progress for a connection.
	QSet<Qt::HANDLE> transactionOpenOnThread_;
	/// This mutex is used in the thread-safe function qdb() to protect access to threadIDsOfOpenConnections_ and transactionOpenOnThread_.
	mutable QMutex qdbMutex_;


	// Multiton variables:
	///////////////////////////
	static QHash<QString, AMDatabase*> connectionName2Instance_;
	static QMutex databaseLookupMutex_;

};

#endif // DATABASE_H
