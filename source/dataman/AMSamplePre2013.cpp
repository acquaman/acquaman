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


#include "AMSamplePre2013.h"


AMSamplePre2013::AMSamplePre2013(QObject *parent) :
	AMDbObject(parent)
{

}


// This constructor initializes a sample with a given name.
AMSamplePre2013::AMSamplePre2013(const QString& sampleName, QObject* parent)
	: AMDbObject(parent)
{
	dateTime_ = QDateTime::currentDateTime();
	setName(sampleName);
}

// This constructor immediately loads a stored sample from the database.
AMSamplePre2013::AMSamplePre2013(int databaseId, AMDatabase* database, QObject* parent)
	: AMDbObject(parent)
{

	loadFromDb(database, databaseId);
}

#include "dataman/database/AMDbObjectSupport.h"
void AMSamplePre2013::destroySample(AMDatabase* db, int id) {
	if(db == 0)
		return;
	db->deleteRow(id, AMDbObjectSupport::s()->tableNameForClass<AMSamplePre2013>());
}

#include "util/AMPeriodicTable.h"

// Format the elements into a string: ex: "B, N, Cl"
QString AMSamplePre2013::elementString() const {

	QStringList sl;

	for(int i=0; i<elements_.count(); i++) {
		sl.append(AMPeriodicTable::table()->elementByAtomicNumber(elements_.at(i))->symbol());
	}

	return sl.join(", ");
}

QString AMSamplePre2013::sampleNameForId(AMDatabase *db, int sampleId)
{
	QVariant rv = db->retrieve(sampleId, AMDbObjectSupport::s()->tableNameForClass<AMSamplePre2013>(), "name");
	if(!rv.isValid())
		return "[Invalid Sample]";
	else
		return rv.toString();
}


///
