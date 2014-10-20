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


#include "AMActionHistoryTreeView.h"

#pragma GCC diagnostic ignored "-Wunused-private-field"
#include <QMouseEvent>
#include <QKeyEvent>
#pragma GCC diagnostic warning "-Wunused-private-field"

#include "AMActionHistoryModel.h"

 AMActionHistoryTreeView3::~AMActionHistoryTreeView3(){}
AMActionHistoryTreeView3::AMActionHistoryTreeView3(QWidget *parent) :
	QTreeView(parent)
{
	shiftKeyDown_ = false;
	controlKeyDown_ = false;
	hasOverriddenShiftCursor_ = false;
	hasOverriddenControlCursor_ = false;
	forbiddenCursor_ = new QCursor();
	forbiddenCursor_->setShape(Qt::ForbiddenCursor);
	regularCursor_ = new QCursor();
	lastClickWasDeselect_ = false;
	setCursor(*regularCursor_);

	// Set mouse tracking to true so we can see entered signal. Also connect that to internal handler slot.
	setMouseTracking(true);
	connect(this, SIGNAL(entered(QModelIndex)), this, SLOT(onEnteredIndex(QModelIndex)));
}

void AMActionHistoryTreeView3::setActuallySelectedByClickingCount(int actuallySelectedByClickingCount){
	actuallySelectedByClickingCount_ = actuallySelectedByClickingCount;
}

void AMActionHistoryTreeView3::onEnteredIndex(const QModelIndex &index){
	AMActionHistoryModel3 *historyModel = qobject_cast<AMActionHistoryModel3*>(model());
	if(!historyModel)
		return;
	if(shiftKeyDown_){
		// If shift key is down and we don't already have the forbidden cursor and we're hovering over an item that is not a sibling then we can't shift click ... go forbidden
		if(!hasOverriddenShiftCursor_ && (index.parent() != lastClickedIndex_.parent()) ){
			hasOverriddenShiftCursor_ = true;
			setCursor(*forbiddenCursor_);
		}
		// Or if the shift key is down and we already have the forbidden cursor and we're hovering over an item that is a sibling then we can shift click ... go default
		// Also check lastClickWasDeselect ... we can never shift click if that sucker is on
		else if(hasOverriddenShiftCursor_ && (index.parent() == lastClickedIndex_.parent() && !lastClickWasDeselect_) ){
			hasOverriddenShiftCursor_ = false;
			setCursor(*regularCursor_);
		}
	}
	if(controlKeyDown_){
		ParentSelectMap selectMap = model()->data(index, AMActionHistoryModel3::ParentSelectRole).value<ParentSelectMap>();
		// If control key is down and we have overridden the cursor ... go default
		if(hasOverriddenControlCursor_){
			hasOverriddenControlCursor_ = false;
			setCursor(*regularCursor_);
		}
		// If control key is down and we don't already have the forbidden cursor and this viewer is in the selection map and this index isn't parentSelected
		//  and we have a parent somewhere selected and our direct parent isn't selected one way or the other ... go forbidden
		// Just trying to make sure that you if you have three levels (A parents B, B parents C) if A or above is selected but B is not then you can't control-click in C or below ... what would that mean anyway
		if(!hasOverriddenControlCursor_ && selectMap.contains(this) && !selectMap.value(this) && hasSelectedParent(index) && !(selectionModel()->isSelected(index.parent()) || model()->data(index.parent(), AMActionHistoryModel3::ParentSelectRole).value<ParentSelectMap>().value(this)) ){
			hasOverriddenControlCursor_ = true;
			setCursor(*forbiddenCursor_);
		}
	}
}

QItemSelectionModel::SelectionFlags AMActionHistoryTreeView3::selectionCommand(const QModelIndex &index, const QEvent *selectionEvent) const{
	// Initialize by calling the parent version
	QItemSelectionModel::SelectionFlags retFlags = QTreeView::selectionCommand(index, selectionEvent);
	/* bool hasShiftModifier, hasControlModifier; Removed to prevent compiler warnings, see Issue734 */
	bool hasControlModifier;

	// If we have a valid index we have a lot of work to do (also make sure this thing is actually an AMActionHistoryModel viewing tree)
	if(index.isValid()){
		AMActionHistoryModel3 *historyModel = qobject_cast<AMActionHistoryModel3*>(model());


		// We only care about the mouse button press events otherwise return the parent class call
		if( !historyModel || (selectionEvent->type() != QEvent::MouseButtonPress) )
			return QTreeView::selectionCommand(index, selectionEvent);
		QMouseEvent *mouseEvent = (QMouseEvent*)selectionEvent;

		// Ignore clicks on non-selectable indices
		// We can ignore: not enabled indexes, and not selectable indexes
		if(!index.flags().testFlag(Qt::ItemIsEnabled) || !index.flags().testFlag(Qt::ItemIsSelectable))
			return QTreeView::selectionCommand(index, selectionEvent);

		// Grab keyboard shift and control modifiers
		Qt::KeyboardModifiers currentModifiers = mouseEvent->modifiers();
		hasControlModifier = currentModifiers&Qt::ControlModifier;
		/* hasShiftModifier = currentModifiers&Qt::ShiftModifier; Removed to prevent compile warning, see Issue734 */

		ParentSelectMap selectMap = model()->data(index, AMActionHistoryModel3::ParentSelectRole).value<ParentSelectMap>();
		// This is pretty bad, you can tell too much is in here that doesn't belong when I need to do a const cast
		AMActionHistoryTreeView3 *checkViewer = const_cast<AMActionHistoryTreeView3*>(this);
		bool parentIsSelected = false;
		// Figure out if the parent is selected via the mapping or the selection model
		if(index.parent().isValid()){
			ParentSelectMap parentsSelectMap = model()->data(index.parent(), AMActionHistoryModel3::ParentSelectRole).value<ParentSelectMap>();
			if(parentsSelectMap.contains(checkViewer))
				parentIsSelected |= parentsSelectMap.value(checkViewer);
			parentIsSelected |= selectionModel()->isSelected(index.parent());
		}

		// On control clicks where the parent is selected and this index is as well, do the group deselect and return noUpdate in the selection flags
		if(hasControlModifier && index.parent().isValid() && parentIsSelected && selectMap.contains(checkViewer) && selectMap.value(checkViewer)){
			historyModel->markIndexGroupAsDeselected(index, checkViewer);
			return QItemSelectionModel::NoUpdate;
		}
		// On control clicks where the parent is selected and this index is not, do the group select and return noUpdate in the selection flags
		else if(hasControlModifier && index.parent().isValid() && parentIsSelected && selectMap.contains(checkViewer) && !selectMap.value(checkViewer)){
			historyModel->markIndexGroupAsSelected(index, checkViewer);
			return QItemSelectionModel::NoUpdate;
		}

		// Ignore regular clicks on the an index that is already actually selected if it's the only one selected
		if( !hasControlModifier && (actuallySelectedByClickingCount_ == 1) && (selectionModel()->selectedIndexes().contains(index)) ){
			return QItemSelectionModel::NoUpdate;
		}
	}

	// Log some flags ... might be an easier way now that I know how to use QFlag::testFlag()
	bool clearFlag = (retFlags&QItemSelectionModel::Clear);
	bool selectFlag = (retFlags&QItemSelectionModel::Select);
	bool deselectFlag = (retFlags&QItemSelectionModel::Deselect);
	bool toggleFlag = (retFlags&QItemSelectionModel::Toggle);
	bool indexAlreadyOn = selectionModel()->selectedIndexes().contains(index);
	bool noUpdateFlag = (retFlags&QItemSelectionModel::NoUpdate);

	// An actual selection! Selected OR (Toggled AND NOT Currently On) OR (noUpdate AND Currently On)
	bool selectedTrue = selectFlag || (toggleFlag && !indexAlreadyOn) || (noUpdateFlag && indexAlreadyOn);
	// An actual deselection! Deselected OR (Toggled AND Currently On) OR (noUpdate AND NOT Currently On) OR (Actually selected an invalid index)
	bool selectedFalse = deselectFlag || (toggleFlag && indexAlreadyOn) || (noUpdateFlag && !indexAlreadyOn) || (selectedTrue && !index.isValid());

	// If we actually selected and the column is 0 (we only care about that one) log the state and emit actuallySelectedByClicking
	if(selectedTrue && (index.column() == 0) ){
		lastClickWasDeselect_ = false;
		lastClickedIndex_ = index;
		emit actuallySelectedByClicking(index, clearFlag);
	}
	// If we actually deselected and the column is 0 (we only care about that one) log the state (particularily lastClickWasDeselect) and emit actuallyDeselectedByClicking
	else if(selectedFalse && (index.column() == 0) ){
		QModelIndexList currentSelections = selectionModel()->selection().indexes();
		if(currentSelections.count() == 2){
			currentSelections.removeOne(index);
			lastClickedIndex_ = currentSelections.at(0);
			lastClickWasDeselect_ = false;
		}
		else
			lastClickWasDeselect_ = true;
		emit actuallyDeselectedByClicking(index, clearFlag);
	}
	// If neither selected nor deselected but clear flag is on then emit clearedByClicking
	else if(clearFlag){
		lastClickWasDeselect_ = false;
		emit clearedByClicking();
	}

	// Return the flags we have
	return retFlags;
}

void AMActionHistoryTreeView3::keyPressEvent(QKeyEvent *event){
	if(event->key() == Qt::Key_Shift){
		// Set shift key as down and set mouse cursor as forbidden if the last click was a deselect
		shiftKeyDown_ = true;
		if(lastClickWasDeselect_){
			hasOverriddenShiftCursor_ = true;
			setCursor(*forbiddenCursor_);
		}
		// Otherwise check our current position and let the onEnteredIndex function handle it
		else
			onEnteredIndex(indexAt(viewport()->mapFromGlobal(QCursor::pos())));
	}
	else if(event->key() == Qt::Key_Control){
		// Set control key as down and let the onEnteredIndex function handle any cursor issues
		controlKeyDown_ = true;
		onEnteredIndex(indexAt(viewport()->mapFromGlobal(QCursor::pos())));
	}
}

void AMActionHistoryTreeView3::keyReleaseEvent(QKeyEvent *event){
	if(event->key() == Qt::Key_Shift){
		// Set shift key as not down and reset cursor if necessary
		shiftKeyDown_ = false;
		if(hasOverriddenShiftCursor_){
			hasOverriddenShiftCursor_ = false;
			setCursor(*regularCursor_);
		}
	}
	else if(event->key() == Qt::Key_Control){
		// Set control key as not down and reset cursor if necessary
		controlKeyDown_ = false;
		if(hasOverriddenControlCursor_){
			hasOverriddenControlCursor_ = false;
			setCursor(*regularCursor_);
		}
	}
}

void AMActionHistoryTreeView3::mousePressEvent(QMouseEvent *event){
	// Ignore mouse clicks if the cursor shows forbidden
	if(shiftKeyDown_ && hasOverriddenShiftCursor_){
		event->ignore();
	}
	// Actually handling valid shift-clicks
	else if(shiftKeyDown_){
		AMActionHistoryModel3 *historyModel = qobject_cast<AMActionHistoryModel3*>(model());
		QModelIndex index = indexAt(event->pos());

		// We can ignore: bad models, invalid indexes, not enabled indexes, and not selectable indexes
		if(!historyModel || !index.isValid() || !index.flags().testFlag(Qt::ItemIsEnabled) || !index.flags().testFlag(Qt::ItemIsSelectable))
			event->ignore();

		// Grab current selection from model and the indices between the shift click
		QItemSelection currentSelection = selectionModel()->selection();
		QItemSelection newSelection = historyModel->indicesBetween(index, lastClickedIndex_);

		// Call the selectionCommand function and merge the current and new selections
		QItemSelectionModel::SelectionFlags command = selectionCommand(index, event);
		currentSelection.merge(newSelection, command);

		// Clear the selection and select the combined list
		selectionModel()->clearSelection();
		selectionModel()->select(currentSelection, command);

		// Grab the list indices between the shift click so we can apply the right parentSelected states to them
		QModelIndexList interiorIndices = newSelection.indexes();
		// We only care about lists that have interior elements, the indicesBetween call returns the ends as well
		if(interiorIndices.count() >= 2){
			// Grab the clear flag for the signal emitted below
			bool clearFlag = command.testFlag(QItemSelectionModel::Clear);

			// Not sure if the indices are ordered correctly, so figure out the min and max to remove them (they should be the ends we don't care about)
			QModelIndex minRowIndex = interiorIndices.at(0);
			int minRowValue = minRowIndex.row();
			QModelIndex maxRowIndex = interiorIndices.at(1);
			int maxRowValue = maxRowIndex.row();
			for(int x = 0; x < interiorIndices.count(); x++){
				if(interiorIndices.at(x).row() < minRowValue){
					minRowIndex = interiorIndices.at(x);
					minRowValue = minRowIndex.row();
				}
				if(interiorIndices.at(x).row() > maxRowValue){
					maxRowIndex = interiorIndices.at(x);
					maxRowValue = maxRowIndex.row();
				}
			}

			// Remove both ends
			interiorIndices.removeOne(maxRowIndex);
			interiorIndices.removeOne(minRowIndex);

			// For all interior indices emulate a click by emitting the actuallySelectedByClicking signal ... then the regular chain of events can happen to parentSelect the children
			for(int x = 0; x < interiorIndices.count(); x++)
				emit actuallySelectedByClicking(interiorIndices.at(x), clearFlag);
		}

		// Accept the event
		event->accept();
	}
	// Ignore mouse clicks if the cursor shows forbidden
	else if(controlKeyDown_ && hasOverriddenControlCursor_){
		event->ignore();
	}
	// Falls through, call the parent class
	else
		QTreeView::mousePressEvent(event);
}

bool AMActionHistoryTreeView3::hasSelectedParent(const QModelIndex &index) const{
	// Base case false is no valid parent
	if(!index.parent().isValid())
		return false;
	// Base case true is parent is actually selected in the selection model
	if(selectionModel()->isSelected(index.parent()))
		return true;
	// Call recursively if neither of those
	return hasSelectedParent(index.parent());
}

// AMActionLogItemDelegate
////////////////////////////

 AMActionLogItemDelegate3::~AMActionLogItemDelegate3(){}
AMActionLogItemDelegate3::AMActionLogItemDelegate3(AMActionHistoryTreeView3 *viewer, QObject *parent) :
	QStyledItemDelegate(parent)
{
	viewer_ = viewer;
}

void AMActionLogItemDelegate3::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const{
	// Call the parent class version
	QStyledItemDelegate::initStyleOption(option, index);
	QStyleOptionViewItemV4 *optionV4 = qstyleoption_cast<QStyleOptionViewItemV4 *>(option);

	// We only care about column 0 right now
	if(!optionV4 || !index.model() || (index.column() != 0) )
		return;

	// If we can we override the backgroundBrush when the parentSelected flag is on for this viewer
	const AMActionHistoryModel3 *historyModel = qobject_cast<const AMActionHistoryModel3*>(index.model());
	if(historyModel){
		AMActionLogItem3 *logItem = historyModel->logItem(index);
		// Coupled call right here ... we need this viewer to pull specific information out of the general mapping
		if(logItem->parentSelected(viewer_)){
			QBrush highlightBrush = optionV4->palette.highlight();
			QColor highlightColor = highlightBrush.color();
			highlightColor.setAlpha(170);
			optionV4->backgroundBrush.setColor(highlightColor);
		}
	}
}
