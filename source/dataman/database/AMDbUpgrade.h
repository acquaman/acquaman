#ifndef AMDBUPGRADE_H
#define AMDBUPGRADE_H

#include <QObject>
#include <QStringList>

#include "AMDatabase.h"

class AMDbUpgrade : public QObject
{
Q_OBJECT
public:
	AMDbUpgrade(QList<AMDatabase*> databasesToUpgrade, QObject *parent = 0);

	virtual QStringList upgradeFromTags() = 0;
	virtual bool upgradeNecessary() = 0;
	virtual bool upgradeImplementation() = 0;
	virtual QString upgradeToTag() = 0;
	virtual QString description() = 0;

	bool upgrade();
	bool updateUpgradeTable(bool isNecessary, bool duringCreation);
	bool upgradeRequired();

protected:
	QList<AMDatabase*> databasesToUpgrade_;
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
	bool dbObjectClassBecomes(const QString &originalClassName, const QString &newClassName, QMap<QString, QString> parentTablesToColumnNames, QMap<QString, int> indexTablesToIndexSide);

	/// Merges two AMDbObject classes by taking the \c mergeFromClassName, converting them into the \c mergeToClassName and removing all of the database entities related to \c mergeFromClassName. Use this function carefully, incorrect or incomplete parameters can lead to corrupted databases. Not fully tested for the parentTables portion.
	/*! This function should be used if you have two IDENTICAL classes (all the same AMDbObjectProperties/database columns) and you want to remove the "from" class and keep the "to" class by converting the former to the latter.
	  The function is only for classes of medium complexity. Other AMDbObjects can store this class directly or lists of this class (for example, a class that inherits AMDetectorInfo is okay, but this can't handle something like AMDetectorInfoSet).
	  This function will ensure that the AMDbObjectTypes_table and its related tables are properply updated.
	  The \c parentTablesToColumnNames will map a table name to a column name and look in that table under that column to change instances of the "from" table name to the "to" table name.
	  The \c indexTablesToIndexSide will map an index table to a side (either the id1/table1 side or the id2/table2 side BUT THE id1/table1 SIDE IS NOT YET TESTED AND MAY NOT WORK) and update to new ids and change instances of the "from" table name to the "to" table name.
	  */
	bool dbObjectClassMerge(const QString &mergeToClassName, const QString &mergeFromClassName, QMap<QString, QString> parentTablesToColumnNames, QMap<QString, int> indexTablesToIndexSide);

}

#endif // AMDBUPGRADE_H
