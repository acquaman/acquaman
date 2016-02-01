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


#include "AMDbObjectGeneralView.h"

#include <QFormLayout>
#include <QLabel>
#include <QStringBuilder>

#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"

 AMDbObjectGeneralView::~AMDbObjectGeneralView(){}
AMDbObjectGeneralView::AMDbObjectGeneralView(QWidget *parent) :
	QWidget(parent)
{
}

void AMDbObjectGeneralView::createView(const AMDbObject *dbObject)
{
	if(dbObject)
		createView(dbObject->database()->connectionName(), dbObject->dbTableName(), dbObject->id());
}

void AMDbObjectGeneralView::createView(const QString &databaseName, const QString &tableName, int dbId)
{
	AMDatabase *database = AMDatabase::database(databaseName);
	if(database){
		QSqlQuery q = database->query();
		q.prepare("PRAGMA table_info("%tableName%")");
		if(!database->execQuery(q)) {
			q.finish();
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -275001, QString("General Object View: There was an error while trying to read meta data on table %1.").arg(tableName)));
		}
		else{
			QStringList columnNames;
			while(q.next()){
				columnNames << q.value(1).toString();
			}
			columnNames.removeFirst();
			q.finish();

			QVariantList columnValues = database->retrieve(dbId, tableName, columnNames);

			QFormLayout *fl = new QFormLayout();
			QLabel *tempLabel;
			for(int x = 0; x < columnNames.count(); x++){
				QString labelValue = columnValues.at(x).toString();
				if(labelValue.isEmpty())
					labelValue = "<EMPTY>";
				tempLabel = new QLabel(labelValue);
				fl->addRow(columnNames.at(x), tempLabel);
			}

			fl->setLabelAlignment(Qt::AlignLeft);
			setLayout(fl);
		}
	}
}

bool AMDbObjectGeneralView::canView(const QString &databaseName, const QString &tableName, int dbId){
	Q_UNUSED(databaseName)
	Q_UNUSED(tableName)
	Q_UNUSED(dbId)
	return true;
}
