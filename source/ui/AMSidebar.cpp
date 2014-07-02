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


#include "AMSidebar.h"

AMSidebar::AMSidebar(QWidget* parent)
	: QTreeView(parent) {

	model_ = new AMDragDropItemModel(this);
	setModel(model_);

	setHeaderHidden(true);
	setRootIsDecorated(true);
	setAttribute(Qt::WA_MacShowFocusRect, false);
	setWordWrap(false);
	setIndentation(10);
	setAnimated(true);
	setEditTriggers(QAbstractItemView::SelectedClicked);


	setSelectionMode(QAbstractItemView::SingleSelection);

	// accept drops onto items (specfically, experiment items, which are supposed to accept dropped scans)
	setDragDropMode(QAbstractItemView::DropOnly);
	setAcceptDrops(true);
	setDropIndicatorShown(true);
	setAutoExpandDelay(300);

	model_->invisibleRootItem()->setFlags( Qt::NoItemFlags );



	connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onItemDoubleClicked(QModelIndex)));


	setMinimumWidth(200);
	setMaximumWidth(200);

	setStyleSheet("AMSidebar { font: 500 10pt \"Lucida Grande\"; border-width: 1px;   border-style: solid;   border-color: rgb(221, 227, 234);  border-right-color: rgb(64, 64, 64); background-color: rgb(221, 227, 234); show-decoration-selected: 1; selection-background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); }"
				  " QTreeView::item { height: 30; } "
				  " QTreeView::item::selected { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); } ");
}

/// add a category header with a certain \c weight
void AMSidebar::addHeading(const QString& categoryName, double weight) {
	if(headings_.contains(categoryName))
		return;
	AMSidebarHeading* h = new AMSidebarHeading(categoryName, weight);
	headings_.insert(categoryName, h);
	model_->invisibleRootItem()->appendRow(h);
	model_->invisibleRootItem()->sortChildren(0, Qt::AscendingOrder);
}

/// add a new link under category \c categoryName, with a payload \c linkContent.
/*! If no category with this name exists, a new category is created.
  */
QStandardItem* AMSidebar::addLink(const QString& categoryName, const QVariant& linkContent, const QString& text, const QIcon& icon, double weight) {

	AMSidebarHeading* h = heading(categoryName);
	AMSidebarItem* item = new AMSidebarItem(text, linkContent,icon, weight);
	h->appendRow(item);
	h->sortChildren(0, Qt::AscendingOrder);
	expand(h->index());

	return item;
}

/// add a new link (or group of links) under category \c categoryName, with a payload \c linkContent. The QStandardItem \c item is used to represent the top-level link.
/*!  To create nested links, you can give \c item a set of sub-items with QStandardItem::addRow() before calling this function.  Be sure to manually set each sub-item's AM::LinkRole to the desired destination first. */
void AMSidebar::addLink(const QString& categoryName, const QVariant& linkContent, QStandardItem* item, double weight) {

	AMSidebarHeading* h = heading(categoryName);
	item->setData(linkContent, AM::LinkRole);
	item->setData(weight, AM::WeightRole);
	h->appendRow(item);
	h->sortChildren(0, Qt::AscendingOrder);
}





/// Access the heading item for a given category
AMSidebarHeading* AMSidebar::heading(const QString& headingTitle) {
	if(!headings_.contains(headingTitle))
		addHeading(headingTitle);

	return headings_.value(headingTitle);
}

void AMSidebar::onItemDoubleClicked(const QModelIndex & index) {
	QVariant link = model_->itemFromIndex(index)->data(AM::LinkRole);
	if(!link.isNull())
		emit linkDoubleClicked(link);
}

void AMSidebar::currentChanged ( const QModelIndex & current, const QModelIndex & previous ) {
	QTreeView::currentChanged(current, previous);

	QVariant link = model_->itemFromIndex(current)->data(AM::LinkRole);
	if(!link.isNull())
		emit linkSelected(link);
}

AMSidebarItem::~AMSidebarItem(){}

AMSidebarHeading::~AMSidebarHeading(){}
