#include "AMRunExperimentTree.h"
#include "dataman/AMDatabase.h"
#include "AMErrorMonitor.h"
#include <QDateTime>
#include <QTimer>



QVariant AMRunModelItem::data(int role) const {
	if(role == Qt::DisplayRole)
		return QStandardItem::data(Qt::EditRole).toString() + QStandardItem::data(AM::DateTimeRole).toDateTime().toString(", MMM d (yyyy)");
	else
		return QStandardItem::data(role);
}

/// Re-implemented to save the edited run name back to the database:
void AMRunModelItem::setData(const QVariant &value, int role) {
	if(role == Qt::EditRole) {
		db_->update( data(AM::IdRole).toInt(), "Runs", "name", value);
	}
	QStandardItem::setData(value, role);
}

/// Re-implemented to save the edited experiment name back to the database:
void AMExperimentModelItem::setData(const QVariant &value, int role) {
	if(role == Qt::EditRole) {
		db_->update( data(AM::IdRole).toInt(), "Experiments", "name", value);
	}
	QStandardItem::setData(value, role);
}

AMRunExperimentInsert::AMRunExperimentInsert(AMDatabase* db, QStandardItem* runParent, QStandardItem* experimentParent, QObject *parent) :
	QObject(parent)
{

	db_ = db;

	experimentItem_ = experimentParent;
			// new QStandardItem(QIcon(":/applications-science.png"), "Experiments");
	experimentItem_->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	runItem_ =  runParent;
			//new QStandardItem(QIcon(":/22x22/view_calendar_upcoming_days.png"), "Runs");
	runItem_->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);


	connect(db_, SIGNAL(updated(QString,int)), this, SLOT(onDatabaseUpdated(QString,int)));
	connect(db_, SIGNAL(removed(QString,int)), this, SLOT(onDatabaseUpdated(QString,int)));

	refreshRuns();
	refreshExperiments();
}

/// This slot receives updated() signals from the database, and (if a refresh hasn't been scheduled yet), schedules a refresh() for once control returns to the Qt event loop.  This provides a performance boost by potentially only doing one refresh() for multiple sequential database updates.
void AMRunExperimentInsert::onDatabaseUpdated(const QString& table, int id) {

	Q_UNUSED(id)

	if(table == "Runs" && !runRefreshScheduled_) {
		runRefreshScheduled_ = true;
		QTimer::singleShot(0, this, SLOT(refreshRuns()));
	}

	if(table == "Experiments" && !expRefreshScheduled_) {
		expRefreshScheduled_ = true;
		QTimer::singleShot(0, this, SLOT(refreshExperiments()));
	}
}

void AMRunExperimentInsert::refreshRuns() {

	runItem_->removeRows(0, runItem_->rowCount());

	QSqlQuery q = db_->query();
	q.setForwardOnly(true);
	q.prepare("SELECT Runs.name, Runs.dateTime, Facilities.description, Thumbnails.thumbnail, Thumbnails.type, Runs.id "
			  "FROM Runs,Facilities,Thumbnails "
			  "WHERE Facilities.id = Runs.facilityId AND Thumbnails.id = Facilities.thumbnailFirstId "
			  "ORDER BY Runs.dateTime ASC");
	if(!q.exec())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Could not retrieve a list of run information from the database."));

	while(q.next()) {

		/// "editRole" is just the name, because you can't change the date:
		AMRunModelItem* item = new AMRunModelItem(db_, q.value(5).toInt(), q.value(0).toString());

		/// "displayRole" looks like "[name], [date]" or "SGM, Aug 4 (2010)". We get this by storing the date/time in AM::DateTimeRole
		item->setData(q.value(1).toDateTime(), AM::DateTimeRole);

		/// "decorationRole" is the icon for the facility:
		if(q.value(4).toString() == "PNG") {
			QPixmap p;
			if(p.loadFromData(q.value(3).toByteArray(), "PNG"))
				item->setData(p.scaledToHeight(22, Qt::SmoothTransformation), Qt::DecorationRole);
		}

		/// "toolTipRole" is the long description of the facility
		item->setData(q.value(2).toString(), Qt::ToolTipRole);

		/// Copy the Link role from runItem_ so that we open the same widget in the main window
		item->setData(runItem_->data(AM::LinkRole), AM::LinkRole);

		runItem_->appendRow(item);
	}

	// no more refresh scheduled, since we just completed it.
	runRefreshScheduled_ = false;
}


void AMRunExperimentInsert::refreshExperiments() {

	experimentItem_->removeRows(0, experimentItem_->rowCount());

	QSqlQuery q = db_->query();
	q.setForwardOnly(true);
	q.prepare("SELECT name,id "
			  "FROM Experiments "
			  "ORDER BY id ASC");
	if(!q.exec())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Could not retrieve a list of experiment information from the database."));

	while(q.next()) {

		/// "editRole" and "displayRole" are just the name:
		AMExperimentModelItem* item = new AMExperimentModelItem(db_, q.value(1).toInt(), q.value(0).toString());

		/// "decorationRole" is a standard folder icon
		item->setData(QPixmap(":/22x22/folder.png"), Qt::DecorationRole);

		/// "toolTipRole" is the name, again. (\todo eventually, this could contain the number of scans in the experiment)
		item->setData(q.value(0).toString(), Qt::ToolTipRole);

		/// Copy the Link role from runItem_ so that we open the same widget in the main window
		item->setData(experimentItem_->data(AM::LinkRole), AM::LinkRole);

		experimentItem_->appendRow(item);
	}

	// no more refresh scheduled, since we just completed it.
	expRefreshScheduled_ = false;
}

/// Connect this slot to the view's clicked(const QModelIndex& index) signal. It will emit runSelected and experimentSelected as required.
void AMRunExperimentInsert::onItemClicked(const QModelIndex& index) {
	/// Run heading clicked?
	if(index == runItem_->index()) {
		emit runSelected(-1);
	}
	else if(index == experimentItem_->index()) {
		emit experimentSelected(-1);
	}
	else if(index.parent() == runItem_->index()) {
		emit runSelected(runItem_->model()->itemFromIndex(index)->data(AM::IdRole).toInt());
	}

	else if(index.parent() == experimentItem_->index()) {
		emit experimentSelected(experimentItem_->model()->itemFromIndex(index)->data(AM::IdRole).toInt());
	}
}







/*
	setStyleSheet("AMRunExperimentTree { font: 500 10pt \"Lucida Grande\"; border: 0px none transparent; background-color: transparent; show-decoration-selected: 1; selection-background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(170, 176, 197, 255), stop:1 rgba(115, 122, 153, 255)); } QTreeView::item { height: 26; } \n QTreeView::item::hover { border-width: 1px; border-style: solid;	border-color: rgb(22, 84, 170); border-top-color: rgb(69, 128, 200); background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); }  QTreeView::item::selected { border: 1px solid rgb(115, 122, 153); border-top-color: rgb(131, 137, 167);  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(170, 176, 197, 255), stop:1 rgba(115, 122, 153, 255)); }");
*/
