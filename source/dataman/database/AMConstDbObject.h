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


#ifndef AMCONSTDBOBJECT_H
#define AMCONSTDBOBJECT_H

#include "dataman/database/AMDbObject.h"

class AMConstDbObject : public QObject
{
Q_OBJECT
public:
	/// Constructor, takes an underlying AMDbObject*
	AMConstDbObject(const AMDbObject *object, QObject *parent = 0);

	/// Constructor, takes a database, a table name, and an id for lazy loading when needed
	AMConstDbObject(AMDatabase *db, const QString &tableName, int id, QObject *parent = 0);

	/// Retrieves the object, you will need to qobject_cast it to the type you desire
	const AMDbObject* object() const;

	/// Retrieves the database this object is associated with
	AMDatabase *database() const;

	/// Retrieves the table this object is associated with
	QString tableName() const;

	/// Retrieves the id of this object
	int id() const;

	/// Returns true when the object is fully loaded into memory. Returns false if there is no object or if it's lazy loaded at the moment
	bool isFullyLoaded() const;

public slots:
	/// Sets the object
	void setObject(const AMDbObject *object);

	/// Sets the lazy loading information directly and also clears the object at the same time, which is used by AMDbObject::loadFromDb to restore AMConstDbObjectLists
	void setLazyLoadingInfo(AMDatabase *db, const QString &tableName, int id);

protected slots:
	/// Handles updating the database, tableName, and id (if necessary) when the object is stored to the database
	void onObjectStoredToDb();

protected:
	/// Initializes the internal variables for both constructors
	void initialize();

protected:
	/// The object being stored
	mutable const AMDbObject *object_;

	/// The database in case we need to lazy load or lazy reload
	AMDatabase *db_;
	/// The table name in case we need to lazy load or lazy reload
	QString tableName_;
	/// The database id in case we need to lazy load or lazy reload
	int id_;
};

Q_DECLARE_METATYPE(AMConstDbObject*)

#endif // AMCONSTDBOBJECT_H
