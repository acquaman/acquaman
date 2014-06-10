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


#ifndef AMACTIONHISTORYTREEVIEW_H
#define AMACTIONHISTORYTREEVIEW_H

#include <QStyledItemDelegate>
#include <QTreeView>

class AMActionLogItemDelegate3;

/// This class was created to help with the custom selection of log items using click, control-click, and shift-click
/*! This was a first learning run on how to use the selection model, selection flags, and some custom delegate stuff ... so it's not pretty.
  Given this experience, starting from scratch would look very different and get rid of a lot of coupling and incorrect placement.
  Of particular concern is the selectionCommand() function, which does some non-const stuff in it and has a lot of logic that would be better elsewhere.
  Also of concern is the coupling across classes. To get the selected-like look, I think there needs to be some coupling ... but likely not this much.
  */
class AMActionHistoryTreeView3 : public QTreeView
{
Q_OBJECT
public:
	/// Standard constructor simply sets the internal variables to their initial states and makes connections.
 	virtual ~AMActionHistoryTreeView3();
	AMActionHistoryTreeView3(QWidget *parent = 0);

public slots:
	/// Set the number of indices currently actually selected by clicks.
	void setActuallySelectedByClickingCount(int actuallySelectedByClickingCount);

signals:
	/// Signal emitted when we know an index was selected (ON) by a click. This is done in selectionCommand ... which seems like a bad place to do it in hindsight.
	void actuallySelectedByClicking(const QModelIndex &index, bool othersCleared) const;
	/// Signal emitted when we know an index was deselected (OFF) by a click. This is done in selectionCommand ... which seems like a bad place to do it in hindsight.
	void actuallyDeselectedByClicking(const QModelIndex &index, bool othersCleared) const;
	/// Signal emitted nothing was selected or deselected but a general clear took place. This is done in selectionCommand ... which seems like a bad place to do it in hindsight.
	void clearedByClicking() const;

protected slots:
	/// Handles checking if the mouse cursor should go to forbidden for certain shift- and control-key down situations
	void onEnteredIndex(const QModelIndex &index);

protected:
	/// Overrides the selectionCommand and does way too much logic about other stuff.
	virtual QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *selectionEvent) const;
	/// Overrides the key press event to help with tracking shift- and control-key situations
	virtual void keyPressEvent(QKeyEvent *event);
	/// Overrides the key release event to help with tracking shift- and control-key situations
	virtual void keyReleaseEvent(QKeyEvent *event);

	/// Overrides the mouse press event principally to handle shift clicks but also to ignore clicks when the mouse cursor is forbidden
	virtual void mousePressEvent(QMouseEvent *event);

	/// Recursive call to see if an index's parent, grandparent, great-grandparent, etc, is actually selected in the selection model.
	bool hasSelectedParent(const QModelIndex &index) const;

protected:
	/// Coupled information coming back from AMActionHistoryView (it has a master list about how many items are selected by actual clicks). This might be able to be entirely internal if reworked.
	int actuallySelectedByClickingCount_;
	/// Internal tracking bools for the shift and control keys
	bool shiftKeyDown_, controlKeyDown_;
	/// Internal tracking bools for if we've overridden the cursor for a shift- or control-key situation
	bool hasOverriddenShiftCursor_, hasOverriddenControlCursor_;
	/// The two cursors we're using (forbidden and default)
	QCursor *forbiddenCursor_, *regularCursor_;
	/// Tracks the last clicked index (mutable because it's set in selectionCommand ... not a great place, I know)
	mutable QModelIndex lastClickedIndex_;
	/// Tracks whether or not the last click was a deselection to help with shift-clicking situations (mutable because it's set in selectionCommand ... not a great place, I know)
	mutable bool lastClickWasDeselect_;
};

/// This delegate is used by the AMActionHistoryTreeView in AMActionHistoryView to show custom selected-like coloring when necessary. You should never need to use this class directly.
class AMActionLogItemDelegate3 : public QStyledItemDelegate {
	Q_OBJECT
public:
	/// Constructor takes a pointer to the AMActionHistoryTreeView viewing it (not great coupling). This is necessary to check with the log item to see if its parentSelected flag is on for a particular view.
	/*! This could likely be changed to a more generic QAbstractItemView pointer, but the coupling is still needed in this design. */
 	virtual ~AMActionLogItemDelegate3();
	explicit AMActionLogItemDelegate3(AMActionHistoryTreeView3 *viewer, QObject* parent = 0);

protected:
	/// Overriding the initStyleOption call to override the backgroundBrush color with an alpha-blended version of the highlight brush color when the parentSelected flag is on
	virtual void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;

protected:
	/// Pointer kept to compare the log item's parentSelected flag mapping
	AMActionHistoryTreeView3 *viewer_;
};



#endif // AMACTIONHISTORYTREEVIEW_H
