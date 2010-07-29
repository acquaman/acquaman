#include "AMRunExperimentTree.h"
#include "dataman/AMDatabase.h"
#include "AMErrorMonitor.h"
#include <QDateTime>

#include <QDebug>

AMRunExperimentModel::AMRunExperimentModel(AMDatabase* db, QObject *parent) :
	QStandardItemModel(parent)
{

	db_ = db;
	refresh();
}

void AMRunExperimentModel::refresh() {

	this->clear();

	experimentItem = new QStandardItem(QIcon(":/applications-science.png"), "Experiments");
	runItem = new QStandardItem(QIcon(":/22x22/view_calendar_upcoming_days.png"), "Runs");

	invisibleRootItem()->appendRow(runItem);
	invisibleRootItem()->appendRow(experimentItem);

	QSqlQuery q = db_->query();
	q.setForwardOnly(true);
	q.prepare("SELECT Runs.name, Runs.dateTime, Facilities.description, Thumbnails.thumbnail, Thumbnails.type "
			  "FROM Runs,Facilities,Thumbnails "
			  "WHERE Facilities.id = Runs.facilityId AND Thumbnails.id = Facilities.thumbnailFirstId "
			  "ORDER BY Runs.dateTime ASC");
	if(!q.exec())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Could not retrieve a list of run information from the database."));

	while(q.next()) {

		QStandardItem* item = new QStandardItem();

		/// "editRole" is just the name, because you can't change the date:
		item->setData(q.value(0).toString(), Qt::EditRole);

		/// "displayRole" looks like "[name] [date]" or "SGM Aug 4 (2010)"
		item->setData(QString(q.value(0).toString() + q.value(1).toDateTime().toString(" MMM d (yyyy)")), Qt::DisplayRole);

		/// "decorationRole" is the icon for the facility:
		if(q.value(4).toString() == "PNG") {
			QPixmap p;
			if(p.loadFromData(q.value(3).toByteArray(), "PNG"))
				item->setData(p.scaledToHeight(22, Qt::SmoothTransformation), Qt::DecorationRole);
		}

		/// "toolTipRole" is the long description of the facility
		item->setData(q.value(2).toString(), Qt::ToolTipRole);

		qDebug() << "adding run " << q.value(0).toString() << "to model, dateTime is " << q.value(1).toDateTime().toString();
		runItem->appendRow(item);
	}
}











AMRunExperimentTree::AMRunExperimentTree(AMDatabase* db, QWidget* parent)
	: QTreeView(parent)
{


	model_ = new AMRunExperimentModel(db, this);
	setModel(model_);

	setHeaderHidden(true);
	setRootIsDecorated(true);
	setAttribute(Qt::WA_MacShowFocusRect, false);
	setWordWrap(false);
	setIndentation(15);

	setStyleSheet("AMRunExperimentTree { font: 500 10pt \"Lucida Grande\"; border: 0px none transparent; background-color: transparent; show-decoration-selected: 1; selection-background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(170, 176, 197, 255), stop:1 rgba(115, 122, 153, 255)); } QTreeView::item { height: 26; } \n QTreeView::item::hover { /*border-width: 1px; border-style: solid;	border-color: rgb(22, 84, 170); border-top-color: rgb(69, 128, 200);*/ background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); }  QTreeView::item::selected { /*border: 1px solid rgb(115, 122, 153); border-top-color: rgb(131, 137, 167); */ background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(170, 176, 197, 255), stop:1 rgba(115, 122, 153, 255)); }");

}
