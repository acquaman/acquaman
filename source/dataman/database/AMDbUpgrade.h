/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMDBUPGRADE_H
#define AMDBUPGRADE_H

#include <QObject>
#include <QStringList>

#include "AMDatabase.h"

class AMDbUpgrade : public QObject
{
Q_OBJECT
public:
	/// The general constructor. The \c databaseNameToUpgrade is the name of the AMDatabases that you wish to apply this upgrade to.
	AMDbUpgrade(QString databaseNameToUpgrade, QObject *parent = 0);

	/// The tags that this upgrade depends on. If a particular upgrade cannot be attempted if certain upgrades haven't already happened, the list of tags should be returned by this function. This function can return an empty list (it has no dependencies).
	virtual QStringList upgradeFromTags() const = 0;

	/// Returns true if the upgrade is necessary based on the contents of the database examined. For example, an upgrade can be required (that tag is not found in the upgrade table) but not be necessary because those classes are not in this database.
	/*! In this case, the database upgrade will be tagged as completed but no changes are made to the database (there is a column for this in the upgrade table).
	    Each subclass must provide this, likely by interogating the database's AMDbObjectTypes_table.
	  */
	virtual bool upgradeNecessary() const = 0;

	/// This function is called by upgrade to complete the work of your specific database upgrade.
	/*! You can call custom code here or use functions in the AMDbUpgradeSupport namespace.
	  */
	virtual bool upgradeImplementation() = 0;

	/// Creates new copy of this upgrade (caller is responsible for memory). You must reimplement this in your subclass to ensure the correct type is new'd.
	virtual AMDbUpgrade* createCopy() const = 0;

	/// Returns the tag that will be inserted into the upgrades table upon completion. This tag will be searched for to determine whether or not this upgrade is required.
	virtual QString upgradeToTag() const = 0;

	/// Returns a string describing the upgrade to be saved in the upgrade table
	virtual QString description() const = 0;

	/// This function calls upgradeImplementation() to do the work of the actual upgrade. Additionally, this call makes sure that the dependencies for the upgrade listed by upgradeFromTags exist in the database requested. If not, this function will fail before calling upgradeImplementation().
	bool upgrade();

	/// This function is called to update the upgrade table once the upgrade() call is successfully completed. It ensures that the new upgrade data and tags are inserted into the database.
	bool updateUpgradeTable(bool isNecessary, bool duringCreation);

	/// Returns true if the upgrade has not happened yet. That is, false is returned if the tag specified by this upgrade is already in the upgrade table.
	/*! This should not be confused with upgradeNecessary().
	    It will also return true if the upgrade table does not exist yet (and it will ensure that the table does exist before the value true is returned).
	  */
	bool upgradeRequired() const;

	/// Loads up the database from the name supplied. Returns false if the database cannot be loaded.
	bool loadDatabaseFromName();

	/// Returns whether or not this upgrade instance is responsible for doing the upgrade (or just checking that it happened). The default value is true.
	bool isResponsibleForUpgrade() const;

	/// Returns the database name requested for this upgrade
	QString databaseNameToUpgrade() const;

	/// Returns database requested for this upgrade (this will be empty until loadDatabaseFromName() is called).
	AMDatabase* databaseToUpgrade() const;

public slots:
	/// Sets the name of the database to be upgraded. You must call loadDatabaseFromName() to actually load up the database at the correct time.
	void setDatabaseNameToUpgrade(const QString &databaseNameToUpgrade);

	/// Convenience call to load after setting. Returns true if the database was successfully loaded.
	bool setDatabaseNameToUpgradeAndLoad(const QString &databaseNameToUpgrade);

	/// Sets whether or not this instance is responsible for doing the upgrade (for shared databases) or just checking that it's been done
	void setIsResponsibleForUpgrade(bool isResponsibleForUpgrade);

protected:
	/// The database name that has been requested for this upgrade
	QString databaseNameToUpgrade_;
	/// The database that has been requested for this upgrade.
	AMDatabase* databaseToUpgrade_;

	/// Holds whether or not this upgrade is the responsibility of this app controller, the default value is true. An upgrade might be required, but the database in question might be a shared database, in which case the user application should not be responsible. At the same time, an upgrade might have already occured on a shared database and a first time user need not do anything.
	bool isResponsibleForUpgrade_;
};

namespace AMDbUpgradeSupport{
	/// Upgrades an AMDbObject class originally called \c originalClassName to \c newClassName. Use this function carefully, incorrect or incomplete parameters can lead to corrupted databases.
	/*! This takes care of the problem where a class used in the database needs to be renamed. There are several caveats on this one.
	  The function is only for classes of medium complexity. Other AMDbObjects can store this class directly or lists of this class (for example, a class that inherits AMDetectorInfo is okay, but this can't handle something like AMDetectorInfoSet).
	  This function will ensure that the AMDbObjectTypes_table is properply updated, as well as changing the name of the class table and its AMDbObjectType column.
	  The \c parentTablesToColumnNames will map a table name to a column name and look in that table under that column to change instances of the "from" table name to the "to" table name.
	  The \c indexTablesToIndexSide will map an index table to a side (either the id1/table1 side or the id2/table2 side BUT THE id1/table1 SIDE IS NOT YET TESTED AND MAY NOT WORK) and change instances of the "from" table name to the "to" table name.
	  For example, if you have an index table you can speficy the index table name and the column to check in. This will run through all of the row of that index table and update instances of the old class name to the new class name.
	  */
	bool dbObjectClassBecomes(AMDatabase *databaseToEdit, const QString &originalClassName, const QString &newClassName, QMap<QString, QString> parentTablesToColumnNames, QMap<QString, int> indexTablesToIndexSide);

	/// Merges two AMDbObject classes by taking the \c mergeFromClassName, converting them into the \c mergeToClassName and removing all of the database entities related to \c mergeFromClassName. Use this function carefully, incorrect or incomplete parameters can lead to corrupted databases. Not fully tested for the parentTables portion.
	/*! This function should be used if you have two IDENTICAL classes (all the same AMDbObjectProperties/database columns) and you want to remove the "from" class and keep the "to" class by converting the former to the latter.
	  The function is only for classes of medium complexity. Other AMDbObjects can store this class directly or lists of this class (for example, a class that inherits AMDetectorInfo is okay, but this can't handle something like AMDetectorInfoSet).
	  This function will ensure that the AMDbObjectTypes_table and its related tables are properply updated.
	  The \c parentTablesToColumnNames will map a table name to a column name and look in that table under that column to change instances of the "from" table name to the "to" table name.
	  The \c indexTablesToIndexSide will map an index table to a side (either the id1/table1 side or the id2/table2 side BUT THE id1/table1 SIDE IS NOT YET TESTED AND MAY NOT WORK) and update to new ids and change instances of the "from" table name to the "to" table name.
	  */
	bool dbObjectClassMerge(AMDatabase *databaseToEdit, const QString &mergeToClassName, const QString &mergeFromClassName, QMap<QString, QString> parentTablesToColumnNames, QMap<QString, int> indexTablesToIndexSide);

}

#endif // AMDBUPGRADE_H
