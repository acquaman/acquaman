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


#include "AMExperiment.h"
#include "database/AMDbObjectSupport.h"

AMExperiment::AMExperiment(QObject *parent) :
		AMDbObject(parent)
{

}
/// initializing experiment with a given name.
AMExperiment::AMExperiment(const QString& experimentName, QObject* parent)
	: AMDbObject(parent)
{

	this->setName(experimentName);
}

/// This constructor immediately loads a stored experiment from the database.
AMExperiment::AMExperiment(int databaseId, AMDatabase* database, QObject* parent)
		: AMDbObject(parent)
{
	loadFromDb(database, databaseId);
}

bool AMExperiment::deleteExperiment(int id, AMDatabase *database)
{
	database->deleteRow(id, AMDbObjectSupport::s()->tableNameForClass(&staticMetaObject));
	database->deleteRows(AMDbObjectSupport::experimentEntriesTableName(),
						 QString("experimentId = %1").arg(id));
}




///
