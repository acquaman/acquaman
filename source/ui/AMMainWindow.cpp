/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMMainWindow.h"

#include "ui/AMCloseItemDelegate.h"
#include "util/AMFontSizes.h"

// Default constructor
AMMainWindow::AMMainWindow(QWidget *parent) : QWidget(parent) {

	model_ = new AMWindowPaneModel(this);

	stackWidget_ = new QStackedWidget();

	sidebar_ = new QTreeView();
	sidebar_->setModel(model_);
	sidebar_->setHeaderHidden(true);
	sidebar_->setRootIsDecorated(true);
	sidebar_->setMouseTracking(true);
	sidebar_->setAttribute(Qt::WA_MacShowFocusRect, false);
	sidebar_->setWordWrap(false);
	sidebar_->setIndentation(10);
	sidebar_->setAnimated(true);
	sidebar_->setEditTriggers(QAbstractItemView::SelectedClicked);
	sidebar_->setSelectionMode(QAbstractItemView::SingleSelection);
	// accept drops onto items (specfically, experiment items, which are supposed to accept dropped scans)
	sidebar_->setDragDropMode(QAbstractItemView::DropOnly);
	sidebar_->setAcceptDrops(true);
	sidebar_->setDropIndicatorShown(true);
	sidebar_->setAutoExpandDelay(300);
	sidebar_->setMinimumWidth(200);
	sidebar_->setMaximumWidth(200);
	sidebar_->setStyleSheet("QTreeView { font: 500 " AM_FONT_SMALL_ "pt \"Lucida Grande\"; border-width: 1px;   border-style: solid;   border-color: rgb(221, 227, 234);  border-right-color: rgb(64, 64, 64); background-color: rgb(221, 227, 234); show-decoration-selected: 1; selection-background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); }"
							" QTreeView::item { height: 30; } "
							" QTreeView::item::selected { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); } ");
	AMCloseItemDelegate* del = new AMCloseItemDelegate();
	del->setCloseButtonsEnabled(true);
	sidebar_->setItemDelegate(del);


	model_->invisibleRootItem()->setFlags( Qt::NoItemFlags );


	// setup layouts
	vlayout_ = new QVBoxLayout(this);
	vlayout_->setContentsMargins(0, 0, 0, 0);
	vlayout_->setSpacing(0);
	hlayout_ = new QHBoxLayout();
	hlayout_->setContentsMargins(0, 0, 0, 0);
	hlayout_->setSpacing(0);
	vlayout_->addLayout(hlayout_);

	hlayout_->addWidget(sidebar_);
	hlayout_->addWidget(stackWidget_);

	// connect signals from the model:
	connect(model_, SIGNAL(dockStateChanged(QWidget*,bool,bool)), this, SLOT(onDockStateChanged(QWidget*,bool,bool)));
	connect(model_, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onModelRowsInserted(QModelIndex,int,int)));
	connect(model_, SIGNAL(rowsAboutToBeAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(onModelRowsAboutToBeRemoved(QModelIndex,int,int)));

	// connect click and double-click signals from the sidebar:
	connect(sidebar_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSidebarItemSelectionChanged()));
	connect(sidebar_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onSidebarItemDoubleClicked(QModelIndex)));


	// connect 'close' signal from AMCloseItemDelegate
	connect(del, SIGNAL(closeButtonClicked(QModelIndex)), this, SLOT(onItemCloseButtonClicked(QModelIndex)));
}

AMMainWindow::~AMMainWindow() {
	// determine all children widgets:
	QList<QWidget*> panes = model_->allPanes();

	// get rid of the sidebar, so that it's not emitting signals and causing changes while we delete the widgets themselves.
	delete sidebar_;

	// delete the model.
	delete model_;

	foreach(QWidget* pane, panes) {
		delete pane;
	}
}



QStandardItem* AMMainWindow::addPane(QWidget* pane, const QString& categoryName, const QString& title, const QString& iconFileName, bool resizeOnUndock) {

	return model_->addPane(pane, categoryName, title, QIcon(iconFileName), resizeOnUndock);
}




// Removes and deletes a pane widget (whether docked or undocked)
void AMMainWindow::deletePane(QWidget* pane) {

	removePane(pane);

	// delete actual widget:
	delete pane;
}



// Removes a pane widget but does not delete it.  Ownership is now the responsibility of the caller. The pane becomes a top-level window.
void AMMainWindow::removePane(QWidget* pane) {

	QModelIndex i = model_->indexForPane(pane);

	if(i.isValid())
		model_->removeRow(i.row(), i.parent());
}


// Insert a new heading item at a given index.  This can be used in situations where you want a pane added using addPane() to appear at a given \c position (from top to bottom).  Call insertHeading() first, with the top-level \c position and \c title of the heading, and then call addPane() with the same heading title.
/* If \c position is less than 0 or > than the number of existing headings, the heading will be inserted at the bottom.  Returns the newly-created heading item.  */
QStandardItem* AMMainWindow::insertHeading(const QString& title, int position) {
	return model_->headingItem(title, QModelIndex(), position);
}


void AMMainWindow::onModelRowsInserted(const QModelIndex &parent, int start, int end) {

	for(int row=start; row<=end; row++) {
		QModelIndex i = model_->index(row, 0, parent);

		// qdebug() << "Inserting new item. isAlias() = " << model_->isAlias(i) << "isHeading = " << model_->isHeading(i);

		// for "real" item entries. (Nothing to do for aliases or headings)
		if(!model_->isAlias(i) && !model_->isHeading(i)) {

			QWidget* pane = model_->pane(i);
			// qdebug() << "  Still inserting new item. pane is:" << pane;

			if(model_->isDocked(i) && pane) {
				stackWidget_->addWidget(pane);
			}
			else if(pane) {
				pane->setParent(0);
				pane->show();
			}
		}
	}

	sidebar_->expand(parent);
}

void AMMainWindow::onModelRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) {


	for(int row=start; row<=end; row++) {
		QModelIndex i = model_->index(row, 0, parent);

		removeFromPreviousSelectionsQueue(i);

		// for "real" items only. Don't care about removing headings or aliases.
		if(!model_->isAlias(i) && !model_->isHeading(i)) {

			if(model_->isDocked(i)) {
				QWidget* pane = model_->pane(i);
				if(pane) {

					// If this was the currently-selected item, select something different in the main window. We don't want to have that sidebar item selected if its corresponding widget is removed.
					if(stackWidget_->currentWidget() == pane)
						sidebar_->setCurrentIndex(getPreviousSelection(i));
					else {
						// qdebug() << "Not current widget";
					}

					QSize oldSize = pane->size();
					QPoint oldPos = pane->mapToGlobal(pane->geometry().topLeft());
					stackWidget_->removeWidget(pane);
					pane->setParent(0);
					pane->setGeometry(QRect(oldPos + QPoint(20, 20), oldSize));
					pane->show();
				}
			}
		}
	}
}

void AMMainWindow::onItemCloseButtonClicked(const QModelIndex &index) {
	emit itemCloseButtonClicked(index);
}

void AMMainWindow::onDockStateChanged(QWidget* pane, bool isDocked, bool shouldResize) {
	// dock it
	if(isDocked) {
		stackWidget_->addWidget(pane);
	}

	// undock it
	else {
		QSize oldSize = pane->size();
		if(shouldResize)
			oldSize = pane->sizeHint();
		QPoint oldPos = pane->mapToGlobal(pane->geometry().topLeft());

		// If this was the currently-selected item, select something different in the main window. (Can't have a non-existent pane selected in the sidebar)
		if(stackWidget_->currentWidget() == pane)
			sidebar_->setCurrentIndex(getPreviousSelection(model_->indexForPane(pane)));

		stackWidget_->removeWidget(pane);
		pane->setParent(0);
		pane->setGeometry(QRect(oldPos + QPoint(20, 20), oldSize));
		pane->show();
	}
}


void AMMainWindow::setCurrentPane(QWidget* pane){

	// widget doesn't exist
	QModelIndex i = model_->indexForPane(pane);
	if(!i.isValid())
		return;

	setCurrentIndex(i);

}

void AMMainWindow::setCurrentIndex(const QModelIndex &i) {

	// if its a docked widget, set as current widget
	if(model_->isDocked(i)) {
		sidebar_->setCurrentIndex(i);	// will trigger onSidebarItemSelectionChanged()
	}

	// if it's undocked, bring it to the front
	else {
		model_->pane(i)->activateWindow();
	}

}


void AMMainWindow::onSidebarItemSelectionChanged() {

	QModelIndex index;
	QModelIndexList selectedItems(sidebar_->selectionModel()->selectedIndexes());
	if(!selectedItems.isEmpty()) {
		index = selectedItems.at(0);
	}

	// qdebug() << "Sidebar selection changed with index " << index;

	if(!index.isValid()) {
		// do nothing?
		return;
	}

	QWidget* pane = model_->pane(index);

	if(pane == 0)
		return;

	// if this item is an "alias" item for another widget, pane() will correctly access the real widget. However, we should emit the 'aliasActivated' signal to notify
	if(model_->isAlias(index)) {
		emit aliasItemActivated(pane, model_->aliasKey(index), model_->aliasValue(index));
	}

	// If it's floating free, need to grab it back
	if(!model_->isDocked(index)) {
		model_->dock(index);
	}

	stackWidget_->setCurrentWidget(pane);
	addToPreviousSelectionsQueue(index);
	emit currentPaneChanged(pane);
}

void AMMainWindow::onSidebarItemDoubleClicked(const QModelIndex& index) {
	model_->undock(index);
}

void AMMainWindow::removeFromPreviousSelectionsQueue(const QModelIndex &removed)
{
	previousSelections_.removeAll(QPersistentModelIndex(removed));
}

void AMMainWindow::addToPreviousSelectionsQueue(const QModelIndex &current)
{
	previousSelections_.enqueue(QPersistentModelIndex(current));
	if(previousSelections_.count() > 10)
		previousSelections_.dequeue();
}

QModelIndex AMMainWindow::getPreviousSelection(const QModelIndex &current)
{
	// qdebug() << "Current index:" << current;
	// qdebug()<< "Previous selections list:" << previousSelections_;

	QPersistentModelIndex potential;
	bool searchSuccess = false;
	while(!searchSuccess && !previousSelections_.isEmpty() ) {
		potential = previousSelections_.takeLast();
		if(potential.isValid() && model_->isDocked(potential) && model_->pane(potential) != model_->pane(current))
			searchSuccess = true;
	}

	QModelIndex rv = searchSuccess ? QModelIndex(potential) : QModelIndex();

	// qdebug() << "Recommending go to" << rv;
	return rv;
}

QWidget * AMMainWindow::currentPane() const
{
	return stackWidget_->currentWidget();
}



