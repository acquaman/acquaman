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


#include "AMRunExperimentInsert.h"
#include "dataman/AMRunExperimentItems.h"
#include "dataman/database/AMDatabase.h"
#include "util/AMErrorMonitor.h"
#include <QDateTime>
#include <QTimer>
#include <QAbstractItemView>
#include <QTreeView>

#include "ui/AMWindowPaneModel.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMRun.h"
#include "dataman/AMExperiment.h"

#include <QStringBuilder>


AMRunExperimentInsert::AMRunExperimentInsert(AMDatabase* db, QStandardItem* runParent, QStandardItem* experimentParent, QObject *parent) :
	QObject(parent)
{
	experimentItem_ = 0;
	runItem_ = 0;

	db_ = db;

	connect(db_, SIGNAL(created(QString,int)), this, SLOT(onDatabaseObjectCreated(QString,int)));
	connect(db_, SIGNAL(updated(QString,int)), this, SLOT(onDatabaseUpdated(QString,int)));
	connect(db_, SIGNAL(removed(QString,int)), this, SLOT(onDatabaseUpdated(QString,int)));

	if (experimentParent){

		newExperimentId_ = -1;

		experimentItem_ = experimentParent;
		experimentItem_->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		refreshExperiments();
	}

	if (runParent){

		runItem_ =  runParent;
		runItem_->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		refreshRuns();
	}

}

/// This slot receives updated() signals from the database, and (if a refresh hasn't been scheduled yet), schedules a refresh() for once control returns to the Qt event loop.  This provides a performance boost by potentially only doing one refresh() for multiple sequential database updates.
void AMRunExperimentInsert::onDatabaseUpdated(const QString& table, int id) {

	Q_UNUSED(id)

	if (runItem_ && table == AMDbObjectSupport::s()->tableNameForClass<AMRun>() && !runRefreshScheduled_) {
		runRefreshScheduled_ = true;
		QTimer::singleShot(0, this, SLOT(refreshRuns()));
	}

	if(experimentItem_ && table == AMDbObjectSupport::s()->tableNameForClass<AMExperiment>() && !expRefreshScheduled_) {
		expRefreshScheduled_ = true;
		QTimer::singleShot(0, this, SLOT(refreshExperiments()));
	}
}

void AMRunExperimentInsert::onDatabaseObjectCreated(const QString& table, int id) {
	onDatabaseUpdated(table, id);

	if(table == AMDbObjectSupport::s()->tableNameForClass<AMExperiment>())
		newExperimentId_ = id;
}

void AMRunExperimentInsert::refreshRuns() {

	runItem_->removeRows(0, runItem_->rowCount());

	QSqlQuery q = db_->query();
	q.setForwardOnly(true);

	// get the Thumbnails
	q.prepare(QString("SELECT %1.id,%1.name,%1.description,AMDbObjectThumbnails_table.type,AMDbObjectThumbnails_table.thumbnail "
					"FROM %1,AMDbObjectThumbnails_table "
					"WHERE %1.name=AMDbObjectThumbnails_table.title"
				).arg(AMDbObjectSupport::s()->tableNameForClass<AMFacility>()));

	// Load the facility information and the facility icons
	bool facilityIconLoaded = false;
	QString facilityDescrition = "";
	QPixmap faciliytIcon;
	if (q.exec() && q.next()) {
		facilityDescrition = q.value(2).toString();
		if(q.value(3).toString() == "PNG")
			facilityIconLoaded = faciliytIcon.loadFromData(q.value(4).toByteArray(), "PNG");
	}

	// load the AMRuns
	q.clear();
	q.prepare(QString("SELECT %1.id,%1.name,%1.dateTime "
						"FROM %1 "
						"ORDER BY %1.dateTime DESC").arg(AMDbObjectSupport::s()->tableNameForClass<AMRun>()));
	if(!q.exec()) {
		q.finish();
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Could not retrieve a list of run information from the database."));
	}
	else {
		while(q.next()) {

			/// "editRole" is just the name, because you can't change the date:
			AMRunModelItem* item = new AMRunModelItem(db_, q.value(0).toInt(), q.value(1).toString());

			/// "displayRole" looks like "[name], [startDate]" or "SGM, Aug 4 (2010)". We get this by storing the date/time in AM::DateTimeRole
			item->setData(q.value(2).toDateTime(), AM::DateTimeRole);

			/// "decorationRole" is the icon for the facility:
			if(facilityIconLoaded) {
				item->setData(faciliytIcon.scaledToHeight(22, Qt::SmoothTransformation), Qt::DecorationRole);
			}

			/// "toolTipRole" is the full name, followed by the long description of the facility
			item->setData(QString(item->data(Qt::DisplayRole).toString() % ": " % facilityDescrition), Qt::ToolTipRole);

			/// Fill the alias information for this to be a valid 'Alias' item
			AMWindowPaneModel::initAliasItem(item, runItem_->data(AMWindowPaneModel::AliasTargetRole).value<QStandardItem*>(), "Runs", q.value(0).toInt());

			runItem_->appendRow(item);
		}
	}

	// no more refresh scheduled, since we just completed it.
	runRefreshScheduled_ = false;
}


void AMRunExperimentInsert::refreshExperiments() {

	experimentItem_->removeRows(0, experimentItem_->rowCount());

	QSqlQuery q = db_->query();
	q.setForwardOnly(true);
	q.prepare(QString("SELECT name,id "
					  "FROM %1 "
					  "ORDER BY id ASC").arg(AMDbObjectSupport::s()->tableNameForClass<AMExperiment>()));
	if(!q.exec()) {
		q.finish();
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Could not retrieve a list of experiment information from the database."));
	}

	else {
		while(q.next()) {

			int id = q.value(1).toInt();

			/// "editRole" and "displayRole" are just the name:
			AMExperimentModelItem* item = new AMExperimentModelItem(db_, id, q.value(0).toString());

			/// "decorationRole" is a standard folder icon
			item->setData(QPixmap(":/22x22/folder.png"), Qt::DecorationRole);

			/// "toolTipRole" is the name, again. (\todo eventually, this could contain the number of scans in the experiment)
			item->setData(q.value(0).toString(), Qt::ToolTipRole);

			/// Fill the alias information for this to be a valid 'Alias' item
			AMWindowPaneModel::initAliasItem(item, experimentItem_->data(AMWindowPaneModel::AliasTargetRole).value<QStandardItem*>(), "Experiments", id);
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable);

			experimentItem_->appendRow(item);

			// Last of all... if this one is the newly-added experiment, let's select and start editing it's name:
			if(id == newExperimentId_) {
				emit newExperimentAdded(item->index());
				newExperimentId_ = -1;
			}
		}
	}

	// no more refresh scheduled, since we just completed it.
	expRefreshScheduled_ = false;
}






/*
  UNUSED:
 setStyleSheet("AMRunExperimentTree { font: 500 10pt \"Lucida Grande\"; border: 0px none transparent; background-color: transparent; show-decoration-selected: 1; selection-background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(170, 176, 197, 255), stop:1 rgba(115, 122, 153, 255)); } QTreeView::item { height: 26; } \n QTreeView::item::hover { border-width: 1px; border-style: solid;	border-color: rgb(22, 84, 170); border-top-color: rgb(69, 128, 200); background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); }  QTreeView::item::selected { border: 1px solid rgb(115, 122, 153); border-top-color: rgb(131, 137, 167);  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(170, 176, 197, 255), stop:1 rgba(115, 122, 153, 255)); }");
*/
