#include "AMRunExperimentTree.h"

AMRunExperimentModel::AMRunExperimentModel(QObject *parent) :
    QStandardItemModel(parent)
{

	refresh();
}

void AMRunExperimentModel::refresh() {

	this->clear();

	experimentItem = new QStandardItem(QIcon(":/applications-science.png"), "Experiments");
	runItem = new QStandardItem(QIcon(":/22x22/view_calendar_upcoming_days.png"), "Runs");

	invisibleRootItem()->appendRow(runItem);
	invisibleRootItem()->appendRow(experimentItem);

	// todo: insert runs and experiments

}











AMRunExperimentTree::AMRunExperimentTree(QWidget* parent)
	: QTreeView(parent)
{

	model_ = new AMRunExperimentModel(this);
	setModel(model_);

	setHeaderHidden(true);
	setRootIsDecorated(false);
	setAttribute(Qt::WA_MacShowFocusRect, false);

	setStyleSheet("AMRunExperimentTree { border: 0px none transparent; background-color: transparent;  } QTreeView::item { height: 26; } \n QTreeView::item::hover { border-width: 1px; border-style: solid;	border-color: rgb(22, 84, 170); border-top-color: rgb(69, 128, 200); background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); }  QTreeView::item::selected { border: 1px solid rgb(115, 122, 153); border-top-color: rgb(131, 137, 167); background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(170, 176, 197, 255), stop:1 rgba(115, 122, 153, 255)); }");

}
