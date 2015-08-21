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


#include "AM2DScanConfigurationGeneralView.h"

#include <QFormLayout>
#include <QLabel>
#include <QStringBuilder>

#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"

 AM2DScanConfigurationGeneralView::~AM2DScanConfigurationGeneralView(){}
AM2DScanConfigurationGeneralView::AM2DScanConfigurationGeneralView(QWidget *parent) :
	AMDbObjectGeneralView(parent)
{
}

void AM2DScanConfigurationGeneralView::createView(const QString &databaseName, const QString &tableName, int dbId)
{
	AMDatabase *database = AMDatabase::database(databaseName);
	if(database){
		QSqlQuery q = database->query();
		q.prepare("PRAGMA table_info("%tableName%")");
		if(!database->execQuery(q)) {
			q.finish();
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -275002, QString("2D Scan Configuration Generl View: There was an error while trying to read meta data on table %1.").arg(tableName)));
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
			QString labelValue;

			labelValue = columnValues.at(columnNames.indexOf("xStart")).toString();
			tempLabel = new QLabel(labelValue);
			fl->addRow("X Start", tempLabel);

			labelValue = columnValues.at(columnNames.indexOf("xStep")).toString();
			tempLabel = new QLabel(labelValue);
			fl->addRow("X Step", tempLabel);

			labelValue = columnValues.at(columnNames.indexOf("xEnd")).toString();
			tempLabel = new QLabel(labelValue);
			fl->addRow("X End", tempLabel);

			labelValue = columnValues.at(columnNames.indexOf("yStart")).toString();
			tempLabel = new QLabel(labelValue);
			fl->addRow("Y Start", tempLabel);

			labelValue = columnValues.at(columnNames.indexOf("yStep")).toString();
			tempLabel = new QLabel(labelValue);
			fl->addRow("Y Step", tempLabel);

			labelValue = columnValues.at(columnNames.indexOf("yEnd")).toString();
			tempLabel = new QLabel(labelValue);
			fl->addRow("Y End", tempLabel);

			labelValue = columnValues.at(columnNames.indexOf("timeStep")).toString();
			tempLabel = new QLabel(labelValue);
			fl->addRow("Dwell Time", tempLabel);

			labelValue = columnValues.at(columnNames.indexOf("fastAxis")).toString();
			tempLabel = new QLabel(labelValue);
			fl->addRow("fastAxis", tempLabel);

			labelValue = columnValues.at(columnNames.indexOf("slowAxis")).toString();
			tempLabel = new QLabel(labelValue);
			fl->addRow("Slow Axis", tempLabel);

			fl->setLabelAlignment(Qt::AlignLeft);
			setLayout(fl);
		}
	}
}

bool AM2DScanConfigurationGeneralView::canView(const QString &databaseName, const QString &tableName, int dbId){
	Q_UNUSED(dbId)
	AMDatabase *database = AMDatabase::database(databaseName);
	if(database){
		QSqlQuery q = database->query();
		q.prepare("PRAGMA table_info("%tableName%")");
		if(!database->execQuery(q)) {
			q.finish();
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -275003, QString("2D Scan Configuration Generl View: There was an error while trying to read meta data on table %1.").arg(tableName)));
		}
		else{
			QStringList columnNames;
			while(q.next()){
				columnNames << q.value(1).toString();
			}
			columnNames.removeFirst();
			q.finish();

			if(columnNames.contains("xStart") && columnNames.contains("xStep") && columnNames.contains("xEnd") &&
					columnNames.contains("yStart") && columnNames.contains("yStep") && columnNames.contains("yEnd") &&
					columnNames.contains("timeStep") && columnNames.contains("fastAxis") && columnNames.contains("slowAxis")){
				return true;
			}
		}
	}
	return false;
}
