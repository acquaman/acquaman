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


#ifndef SGMPERIODICTABLE_H
#define SGMPERIODICTABLE_H

#include "util/AMPeriodicTable.h"
#include "util/SGM/SGMElementInfo.h"

class SGMPeriodicTable : public QObject
{
Q_OBJECT
public:
	/// Access to the application-wide SGM Periodic table object.
	static SGMPeriodicTable* sgmTable(){
		if(!instance_)
			instance_ = new SGMPeriodicTable();
		return instance_;
	}

	static QString SGMPeriodicTableAllDatabasesConnectionName() { return SGMPeriodicTableAllDatabasesConnectionName_; }

	SGMElementInfo* elementInfoByName(const QString &elementName, const QString &databaseConnectionName = "user") const;
	QStringList fastScanPresetsStrings(const QString &databaseConnectionName = "user") const;
	QList<SGMFastScanParameters*> fastScanPresets(const QString &databaseConnectionName = "user") const;


protected:
	/// Holds the additional SGM Information for elements
	AMOrderedSet<AMDatabase *, AMOrderedSet<AMElement*, SGMElementInfo*>* > sgmPeriodicTableInfo_;

	/// Singleton instance variable
	static SGMPeriodicTable *instance_;

	static QString SGMPeriodicTableAllDatabasesConnectionName_;

	/// Constructor: Builds the SGM Element info attached to the periodic table (protected: access via sgmTable())
 	virtual ~SGMPeriodicTable();
	explicit SGMPeriodicTable(QObject *parent = 0);
};

#endif // SGMPERIODICTABLE_H
