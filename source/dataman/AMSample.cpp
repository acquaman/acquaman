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


#include "AMSample.h"


AMSample::AMSample(QObject *parent) :
	AMDbObject(parent)
{

}


// This constructor initializes a sample with a given name.
AMSample::AMSample(const QString& sampleName, QObject* parent)
	: AMDbObject(parent)
{
	dateTime_ = QDateTime::currentDateTime();
	setName(sampleName);
}

// This constructor immediately loads a stored sample from the database.
AMSample::AMSample(int databaseId, AMDatabase* database, QObject* parent)
	: AMDbObject(parent)
{

	loadFromDb(database, databaseId);
}

#include "dataman/database/AMDbObjectSupport.h"
void AMSample::destroySample(AMDatabase* db, int id) {
	if(db == 0)
		return;
	db->deleteRow(id, AMDbObjectSupport::s()->tableNameForClass<AMSample>());
}

#include "util/AMPeriodicTable.h"

#include <QDebug>

// Format the elements into a string: ex: "B, N, Cl"
QString AMSample::elementString() const {

	QStringList sl;

	for(int i=0; i<elements_.count(); i++) {
		sl.append(AMPeriodicTable::table()->elementByAtomicNumber(elements_.at(i))->symbol());
	}

	return sl.join(", ");
}


///
