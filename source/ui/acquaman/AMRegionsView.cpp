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


#include "AMRegionsView.h"

#include <QTableView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMenu>
#include <QCursor>

/// Creates buttons for add and delete region.
/// Defines a new model and views using table view.
/// Adds all items to form layout.
/// \todo Connect add and delete to something.
AMRegionsView::AMRegionsView(AMRegionsList *regions, QWidget *parent) :
		QWidget(parent)
{
	addButton_ = new QPushButton("Add Region", this);
	deleteButton_ = new QPushButton("Delete Region", this);
	QHBoxLayout *hl_ = new QHBoxLayout();
	hl_->addWidget(addButton_);
	hl_->addWidget(deleteButton_);
	hl_->setContentsMargins(0, 5, 5, 0);
	regions_ = regions;
	tv_ = new QTableView(this);
	tv_->setModel(regions_->model());
	for (int i = 0; i < regions_->model()->columnCount(QModelIndex()); i++)
		if (!(i == 1 || i == 2 || i == 3 || i == 7))
			tv_->hideColumn(i);
	tv_->verticalHeader()->setVisible(false);
	tv_->resize(tv_->sizeHint());

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(tv_, 10);
	vl->addLayout(hl_);
	vl->setContentsMargins(5, 0, 5, 0);
	setLayout(vl);
	addRegionMenu_= 0;//NULL
	deleteRegionMenu_ = 0;//NULL
	connect(addButton_, SIGNAL(clicked()), this, SLOT(addRegion()));
	connect(deleteButton_, SIGNAL(clicked()), this, SLOT(deleteRegion()));
}

AMRegionsView::~AMRegionsView(){
	disconnect(addButton_, SIGNAL(clicked()), this, SIGNAL(addRegionClicked()));
	disconnect(deleteButton_, SIGNAL(clicked()), this, SLOT(deleteRegion()));
	regions_ = NULL;
}

bool AMRegionsView::addRegion(){
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

bool AMRegionsView::deleteRegion(){
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

void AMRegionsView::setInsertIndex(){
	QAction *tmpAction;
	tmpAction = addRegionMenu_->activeAction();
	insertIndex_ = tmpAction->data().toInt();
}

void AMRegionsView::setRemoveIndex(){
	QAction *tmpAction;
	tmpAction = deleteRegionMenu_->activeAction();
	removeIndex_ = tmpAction->data().toInt();
}

void AMRegionsView::setDisabled(bool disabled){
	tv_->setDisabled(disabled);
	addButton_->setDisabled(disabled);
	deleteButton_->setDisabled(disabled);
}

void AMRegionsView::resizeEvent(QResizeEvent *event){
	Q_UNUSED(event)
	int totalWidth = tv_->size().width();
	tv_->setColumnWidth(1, totalWidth/3-1);
	tv_->setColumnWidth(2, totalWidth/6-1);
	tv_->setColumnWidth(3, totalWidth/3-1);
	tv_->setColumnWidth(7, totalWidth/6-1);
}
