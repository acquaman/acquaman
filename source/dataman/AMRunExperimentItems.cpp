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


#include "AMRunExperimentItems.h"
#include "dataman/database/AMDatabase.h"
#include "util/AMDateTimeUtils.h"

#include <QMimeData>
#include <QUrl>
#include <QList>
#include <QStringList>

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMScan.h"

QVariant AMRunModelItem::data(int role) const {
	if(role == Qt::DisplayRole)
		return QStandardItem::data(Qt::EditRole).toString() + ", " + AMDateTimeUtils::prettyDate(QStandardItem::data(AM::DateTimeRole).toDateTime());
	else
		return QStandardItem::data(role);
}

#include "dataman/AMRun.h"
void AMRunModelItem::setData(const QVariant &value, int role) {
	if(role == Qt::EditRole) {
		db_->update( data(AM::IdRole).toInt(), AMDbObjectSupport::s()->tableNameForClass<AMRun>(), "name", value);
	}
	QStandardItem::setData(value, role);
}

#include "dataman/AMExperiment.h"
void AMExperimentModelItem::setData(const QVariant &value, int role) {
	if(role == Qt::EditRole) {
		db_->update( data(AM::IdRole).toInt(), AMDbObjectSupport::s()->tableNameForClass<AMExperiment>(), "name", value);
	}
	QStandardItem::setData(value, role);
}




bool AMExperimentModelItem::dropMimeData(const QMimeData *data, Qt::DropAction action) {

	bool accepted = false;

	/// For drops with a Qt::CopyAction and URLs containing "amd://databaseConnectionName/tableName/id", add these objects to this experiment.
	if( (action & Qt::CopyAction) && data->hasUrls() ) {

		QList<QUrl> urls = data->urls();

		foreach(QUrl url, urls) {

			if(url.scheme() != "amd")
				break;

			// Since this experiment item is from a particular database, make sure that the object comes from the same database
			if(db_->connectionName() != url.host())
				break;

			QStringList path = url.path().split('/');
			if(path.count() != 3)
				break;

			QString tableName = path.at(1);
			bool idOkay;
			int scanId = path.at(2).toInt(&idOkay);
			if(!idOkay || scanId < 1)
				break;

			/// Only store things that belong in the main scans table for now. We have no way of tracking which table the objects came from, so the assumption is they are scans.
			if(tableName != AMDbObjectSupport::s()->tableNameForClass<AMScan>())
				break;

			AMExperiment::addScanToExperiment(scanId, id(), db_);
			accepted = true;
		}
	}

	return accepted;
}

 AMRunModelItem::~AMRunModelItem(){}
 AMExperimentModelItem::~AMExperimentModelItem(){}
