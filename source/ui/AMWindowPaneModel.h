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


#ifndef AMWINDOWPANEMODEL_H
#define AMWINDOWPANEMODEL_H

#include "ui/AMDragDropItemModel.h"	/// \todo move this to UI
#include "acquaman.h"

/// The AMWindowPaneModel manages a collection of window panes used by the application's AMMainWindow. The panes can be docked and undocked from the main window, and the model can also be used to configure properties like the window titles and icons.

/*! Any QStandardItems (or subclasses of QStandardItems) can be used as items, as long as they don't have conflicting uses for the following data roles.

The following roles are used to access/represent/modify the state of the window panes:

- Qt::DisplayRole: (QString) Window title (corresponding to QWidget::windowTitle()).  This will change dynamically when the widget's title is changed using the normal QWidget API.  Calling setData() with this role has the same effect as calling QWidget::setWindowTitle().

- Qt::DecorationRole: (QIcon) Window icon (corresponding to QWidget::windowIcon()). This will change dynamically when the widget's icon is set using the normal QWidget API.  Calling setData() with this role has the same effect as calling QWidget::setWindowIcon(). The window icon is also used when displaying a list of window panes in the sidebar.

- AMWindowPaneModel::DockStateRole: (bool) true if the widget is docked in the main window; false if it has been undocked and is currently floating loose.

- AM::CanCloseRole: (bool) true if the widget can be permanently closed (ie: removed completely from the user interface) by the user.  Closeable windows will display a small 'x' in the sidebar view, which can be activated by the user.  Attempting to "close" a non-closeable window simply docks it back into the main window.

- AM::WidgetRole: (QWidget*) Access a pointer to the actual window pane widget

- AMWindowPaneModel::IsHeadingRole: (bool) Some entries in this model are just "Headings": titles for groups of window panes. This role returns true for all these items. \note This role is read-only; it simply indicates that the item has no QWidget* set for its AM::WidgetRole, nor is it an alias item.

- AMWindowPaneModel::IsAliasRole: (bool) "Aliases" are items that don't have their own unique window pane widgets, but instead link to another existing widget. When activated, they can deliver a message (in the form of a key-value pair) for that widget.  (For example, "Run" and "Experiment" items are shown in the Acquaman and Dataman sidebars, but all of these items are just aliases for a single AMDataView widget window pane.)  Whether an item is, in effect, an alias, is stored in this data role.

- AMWindowPaneModel::AliasTargetRole: (QStandardItem*) For alias items, this is the model item representing the "real" widget. \note It's important to never delete the "real" item while alias items for it still exist.  For now, this responsibility is up to the user. (For example, do not remove the AMDataView widget/window pane item, as long as runs and experiments are listed in the model.)

- AMWindowPaneModel::AliasKeyRole: (QString) For alias items, this is the "key" that can be used to figure out what the "real" widget should display when the alias item is activated. For example, AMDataView assumes it's either QString("run") or QString("experiment"), corresponding to the AMWindowPaneModel::AliasValueRole.

- AMWindowPaneModel::AliasValueRole: (QVariant) For alias items, this the "value" that can be used to figure out what the "real" window pane should display when the alias item is activated.  For example, AMDataView assumes it's the integer \c runId or \c experimentId of a run or experiment item.

<b>Inserting Window Pane Widgets</b>

You can insert a window pane widget using the addPane() convenience function.  This will create a new item in the model with the AM::WidgetRole set for the given widget.  Alternatively, you can create any subclass of QStandardItem, set its AM::WidgetRole (or alias roles) to point to your window pane widget manually, and then add it to the model.  The model is designed so that consistency is maintained across

- access through the model (ex: setData(), addRow(), etc.),
- or through the convenience functions (ex: addPane()),
- or through other APIs (ex: calling setWindowTitle() on a widget).

This allows a lot of flexibility in how the model is accessed, used, and extended.

<b>Drag and Drop</b>
The model can support item-specific decisions on "Drag and Drop" drop events. To accept drops, use a custom subclass of AMDragDropItem for your item class.
/// \todo add example
*/

/// \todo catch QEvents QEvent::WindowTitleChange and QEvent::WindowIconChange for all of our widgets, so we can issue dataChanged().

/// \todo make sure to cache all QWidget*, and detect+remove when rows removed...


class AMWindowPaneModel : public AMDragDropItemModel
{
	Q_OBJECT

public:

	enum DataRoles { DockStateRole = AM::UserRole + 1, IsHeadingRole, IsAliasRole, AliasTargetRole, AliasKeyRole, AliasValueRole, UndockResizeRole, IsVisibleRole };

 	virtual ~AMWindowPaneModel();
	AMWindowPaneModel(QObject* parent = 0);

	/// Returns a heading item with the given text. Will create and add a new heading under \c parentIndex at \c position if no heading with that text exists yet.  (Use -1 for \c position to append at the bottom.) If \c text is empty, will return the top-level (invisible root) item.
	QStandardItem* headingItem(const QString& text, QModelIndex parentIndex = QModelIndex(), int position = -1);

	/// Returns a list of all heading items.
	QList<QStandardItem*> headingItems() const;

	/// try to remove a heading item with given text
	bool removeHeadingItem(const QString &text);

	/// data() is re-implemented from QStandardItemModel to return the widget window title (QWidget::windowTitle()) and icon (QWidget::windowIcon()) for the Qt::DisplayRole/Qt::EditRole, and Qt::DecorationRole respectively.
	/*! It also ensure that for 'alias' items, the dock state of the target item is returned instead.*/
	QVariant data(const QModelIndex &index, int role) const;

	/// setData() is re-implemented from QStandardItemModel to set the widget window title and icon for the Qt::DecorationRole and Qt::DisplayRole/Qt::EditRole.
	/*! It also ensures that when docking 'alias' items, the target items are docked instead.*/
	bool setData(const QModelIndex &index, const QVariant &value, int role);

	/// Insert a new window pane widget \c pane into the model, under the heading \c headingText. The \c resizeOnUndock is used to tell the manager to resize the pane to its sizeHint when it undocks. It returns the newly-created model item representing that pane.
	/*! This is a convenience function. Alternatively, you can set the AM::WidgetRole pointer on any QStandardItem and add it using the conventional QStandardItemModel::addRow() interface.
	\note \c pane must be a valid widget until this item is removed from the model */
	QStandardItem* addPane(QWidget* pane, const QString& headingText = QString(), bool resizeOnUndock = false, bool visible = true);
	/// This is an overloaded function to allow setting the window title and icon while calling addPane().
	QStandardItem* addPane(QWidget *pane, const QString &headingText, const QString& windowTitle, const QIcon& windowIcon, bool resizeOnUndock = false, bool visible = true);

	/// This convenience function can be used to initialize any QStandardItem as a proper "alias" item, pointing to the existing window pane widget \c targetWindowPane.  Returns true on success, and false if the \c targetWindowPane was not found in the model.  It modifies \c newAliasItem to set the IsAliasRole, AliasTargetRole, AliasKeyRole, and AliasValueRole, but does not add the item to the model. (You should follow up by inserting it anywhere you want.)
	bool initAliasItem(QStandardItem* newAliasItem, QWidget* targetWindowPane, const QString& aliasKey = QString(), const QVariant& aliasValue = QVariant());

	/// This convenience function can be used to initialize any QStandardItem as a proper "alias" item, pointing to the existing window pane widget described by \c targetItem.  It modifies \c newAliasItem to set the IsAliasRole, AliasTargetRole, AliasKeyRole, and AliasValueRole, but does not add the item to the model. (You should follow up by inserting it anywhere you want.)
	static void initAliasItem(QStandardItem* newAliasItem, QStandardItem* targetItem, const QString& aliasKey = QString(), const QVariant& aliasValue = QVariant());


	/// Returns a list of all the window pane widgets in the model. Does not include headers and aliases. For panes with multipler aliases, only one instance of the pane is included.
	QList<QWidget*> allPanes() const { return widget2item_.keys(); }

	// Convenience access functions:
	/////////////////////////////////
	/// Convenience function to lookup the index for a given window pane widget. Returns an invalid index if the pane could not be found. \note This will always return the index of the "real" item for this window pane, never the index for an alias item.
	QModelIndex indexForPane(QWidget* pane) const;

	/// Convenience function to check if this item is a header item. This is defined as being true when (a) the item is NOT an alias, AND (b) that the AM::Widget role is 0/unset.
	bool isHeading(const QModelIndex& index) const;

	/// Convenience function to check if this item is actually an "alias" item.
	bool isAlias(const QModelIndex& index) const;

	/// Convenience function to retrieve an alias item's target item. Returns 0 if the item at this \c index is not an alias.
	QStandardItem* aliasTarget(const QModelIndex& index) const;

	/// Convenience function to retrieve an alias item's "key" message.
	QString aliasKey(const QModelIndex& index) const { return data(index, AMWindowPaneModel::AliasKeyRole).toString(); }

	/// Convenience function to retrive an alias item's "value" message.
	QVariant aliasValue(const QModelIndex& index) const { return data(index, AMWindowPaneModel::AliasValueRole); }

	/// Convenience function to check if this window pane is currently docked.
	bool isDocked(const QModelIndex& index) const;

	/// Returns true if the window pane is currently visible.
	bool isVisible(const QModelIndex &index) const;

	/// Convenience function to return a pointer to an item's QWidget window pane. For alias items, will return the window pane of the target. \warning Might be 0 if an invalid item has been added to the model, or if this index is a heading item.
	QWidget* pane(const QModelIndex& index) const;

	// Convenience setter functions:
	///////////////////////////////////

	/// Convenience function to dock or undock a window pane widget
	void setDocked(const QModelIndex& index, bool isDocked) { setData(index, isDocked, DockStateRole); }
	void dock(const QModelIndex& index) { setDocked(index, true); }
	void undock(const QModelIndex& index) { setDocked(index, false); }

	/// Sets a pane's visibility.
	void setVisibility(const QModelIndex &index, bool isVisible) { setData(index, isVisible, IsVisibleRole); }
	/// Shows a pane.
	void show(const QModelIndex &index) { setVisibility(index, true); }
	/// Hides a pane.
	void hide(const QModelIndex &index) { setVisibility(index, false); }

signals:
	/// Emitted when the dock state changes for an item (ie: true emitted when docked, false emitted when undocked) and whether or not a resize is desired
	void dockStateChanged(QWidget* pane, bool isDocked, bool shouldResize);
	/// Emitted when the visibility changes for an item.
	void visibilityChanged(QWidget *pane, bool isVisible);
	/// The default rowsAboutToBeRemoved() signal gets delivered to Qt views before any programmer-created connections are called. This can be a problem because the attached view will change its selection in response to the removal BEFORE the programmer can respond.  If this unacceptable to you (ie: you want to change the selection yourself, and just once to avoid flicker) then you can watch for this signal instead.  Both this and the normal signal will be delivered.
	void rowsAboutToBeAboutToBeRemoved(const QModelIndex& parent, int first, int last);

protected slots:
	/// Catch when items are added to the model. If new window panes are added, add them to the widget2item_ cache, and setup event filters as required.
	void onRowsInserted(const QModelIndex &parent, int first, int last);
	/// Catch before items are removed from the model. If window panes are being removed, remove them from the cache.
	void onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);

protected:

	/// Remove rows (re-implemented to provide early-warning signal rowsAboutToBeAboutToBeRemoved() BEFORE the views get notified, otherwise they will change the current selection before we get a chance to respond.)
	virtual bool removeRows(int row, int count, const QModelIndex &parent) {
		emit rowsAboutToBeAboutToBeRemoved(parent, row, row+count-1);
		return AMDragDropItemModel::removeRows(row, count, parent);
	}

	/// optimized cache: map from QWidget* to QStandardItem*.  Does not include separate entries for aliases
	QHash<QWidget*, QStandardItem*> widget2item_;

	/// event filter: catch title change and icon change events for all our window pane widgets, so we can emit dataChanged():
	bool eventFilter(QObject *source, QEvent *event);

	/// Internal version of pane() that can be used when you know that index is not an alias item.
	QWidget* internalPane(const QModelIndex &index) const;


};

Q_DECLARE_METATYPE(QStandardItem*)

#endif // AMWINDOWPANEMODEL_H
