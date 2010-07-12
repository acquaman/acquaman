#include "AMXASRegionsView.h"

/// Creates buttons for add and delete region.
/// Defines a new model and views using table view.
/// Adds all items to form layout.
/// \todo Connect add and delete to something.
AMXASRegionsView::AMXASRegionsView(AMXASRegionsList *regions, QWidget *parent) :
	QWidget(parent)
{
//	this->setMaximumWidth(400);
	addButton_ = new QPushButton("Add Region", this);
	deleteButton_ = new QPushButton("Delete Region", this);
	QHBoxLayout *hl_ = new QHBoxLayout();
	hl_->addWidget(addButton_);
	hl_->addWidget(deleteButton_);
	regions_ = regions;
	tv_ = new QTableView(this);
	tv_->setModel(regions_->model());
	tv_->hideColumn(0);
	tv_->hideColumn(4);
	tv_->hideColumn(5);
	tv_->setMaximumWidth(350);
	tv_->setMinimumHeight(150);
	tv_->setMaximumHeight(150);
	tv_->resize(tv_->sizeHint());
	tv_->horizontalHeader()->setStretchLastSection(true);
//	int totalWidth = tv_->columnWidth(1) + tv_->columnWidth(2) + tv_->columnWidth(3);
//	tv_->setColumnWidth(1, 2*totalWidth/5);
//	tv_->setColumnWidth(3, 2*totalWidth/5);
//	tv_->setColumnWidth(2, totalWidth-tv_->columnWidth(1)-tv_->columnWidth(3));

	QFormLayout *fl_ = new QFormLayout(this);
	fl_->addRow(tv_);
	fl_->addRow(hl_);
	setLayout(fl_);
	this->setMaximumSize(350, 300);
	this->setMinimumSize(350, 300);
	addRegionMenu_= NULL;
	deleteRegionMenu_ = NULL;
	connect(addButton_, SIGNAL(clicked()), this, SLOT(addRegion()));
	connect(deleteButton_, SIGNAL(clicked()), this, SLOT(deleteRegion()));
}

AMXASRegionsView::~AMXASRegionsView(){
	disconnect(addButton_, SIGNAL(clicked()), this, SIGNAL(addRegionClicked()));
	disconnect(deleteButton_, SIGNAL(clicked()), this, SLOT(deleteRegion()));
	regions_ = NULL;
}

bool AMXASRegionsView::addRegion(){
	if(addRegionMenu_)
		delete addRegionMenu_;
	addRegionMenu_ = new QMenu(this);
	QAction *tmpAction;
	tmpAction = addRegionMenu_->addAction("Front");
	tmpAction->setData(0);
	connect(tmpAction, SIGNAL(hovered()), this, SLOT(setInsertIndex()));
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(insertRegion()));
	QString tmpStr;
	for(int x = 0; x < regions_->count(); x++){
		tmpStr.clear();
		tmpStr.setNum(x+1);
		tmpAction = addRegionMenu_->addAction(tmpStr.prepend("After Region "));
		tmpAction->setData(x+1);
		connect(tmpAction, SIGNAL(hovered()), this, SLOT(setInsertIndex()));
		connect(tmpAction, SIGNAL(triggered()), this, SLOT(insertRegion()));
	}
	addRegionMenu_->popup(QCursor::pos());
	addRegionMenu_->show();
	return true;
}

bool AMXASRegionsView::deleteRegion(){
	if(deleteRegionMenu_)
		delete deleteRegionMenu_;
	deleteRegionMenu_ = new QMenu(this);
	QAction *tmpAction;
	QString tmpStr;
	for(int x = 0; x < regions_->count(); x++){
		tmpStr.clear();
		tmpStr.setNum(x);
		tmpAction = deleteRegionMenu_->addAction(tmpStr.prepend("Region "));
		tmpAction->setData(x);
		connect(tmpAction, SIGNAL(hovered()), this, SLOT(setRemoveIndex()));
		connect(tmpAction, SIGNAL(triggered()), this, SLOT(removeRegion()));
	}
	deleteRegionMenu_->popup(QCursor::pos());
	deleteRegionMenu_->show();
	return true;
}

void AMXASRegionsView::setInsertIndex(){
	QAction *tmpAction;
	tmpAction = addRegionMenu_->activeAction();
	insertIndex_ = tmpAction->data().toInt();
}

void AMXASRegionsView::setRemoveIndex(){
	QAction *tmpAction;
	tmpAction = deleteRegionMenu_->activeAction();
	removeIndex_ = tmpAction->data().toInt();
}
