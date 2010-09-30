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

- AMWindowPaneModel::IsAliasRole: (bool) Some entries in this model are just "aliases". Rather than referring to their own unique window pane widgets, alias items are simply links to to an existing widget, found under another model index.  For example, "Run" and "Experiment" items are shown in the Acquaman and Dataman sidebars, but all of these items are just aliases for a single AMDataView widget window pane.

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

	enum DataRoles { DockStateRole = AM::UserRole + 1, IsHeadingRole, IsAliasRole, AliasTargetRole, AliasKeyRole, AliasValueRole };

	AMWindowPaneModel(QObject* parent = 0);

	/// Returns a heading item with the given text. Will create and add a new heading under parentIndex if no heading with that text exists yet.  If \c text is empty, will return the top-level (invisible root) item.
	QStandardItem* headingItem(const QString& text, QModelIndex parentIndex = QModelIndex());

	/// data() is re-implemented from QStandardItemModel to return the widget window title (QWidget::windowTitle()) and icon (QWidget::windowIcon()) for the Qt::DisplayRole/Qt::EditRole, and Qt::DecorationRole respectively.
	/*! It also ensure that for 'alias' items, the dock state of the target item is returned instead.*/
	QVariant data(const QModelIndex &index, int role) const;

	/// setData() is re-implemented from QStandardItemModel to set the widget window title and icon for the Qt::DecorationRole and Qt::DisplayRole/Qt::EditRole.
	/*! It also ensures that when docking 'alias' items, the target items are docked instead.*/
	bool setData(const QModelIndex &index, const QVariant &value, int role);

	/// This convenience function can be used to insert a new window pane widget into the model, under the heading \c headingText.  Alternatively, you can set the AM::WidgetRole on any QStandardItem and add it using the conventional QStandardItemModel::addRow() API.
	void addPane(QWidget* pane, const QString& headingText = QString());

	/// This convenience function can be used to initialize any QStandardItem as a proper "alias" item, pointing to the existing window pane widget \c targetWindowPane.  Returns true on success, and false if the \c targetWindowPane was not found in the model.  It modifies \c newAliasItem to set the IsAliasRole, AliasTargetRole, AliasKeyRole, and AliasValueRole, but does not add the item to the model. (You should follow up by inserting it anywhere you want.)
	bool initAliasItem(QStandardItem* newAliasItem, QWidget* targetWindowPane, const QString& aliasKey = QString(), const QVariant& aliasValue = QVariant());

	/// This convenience function can be used to initialize any QStandardItem as a proper "alias" item, pointing to the existing window pane widget described by \c targetItem.  Returns true on success, and false if the \c targetItem was not found in the model.  It modifies \c newAliasItem to set the IsAliasRole, AliasTargetRole, AliasKeyRole, and AliasValueRole, but does not add the item to the model. (You should follow up by inserting it anywhere you want.)
	bool initAliasItem(QStandardItem* newAliasItem, QStandardItem* targetItem, const QString& aliasKey = QString(), const QVariant& aliasValue = QVariant());



	// Convenience access functions:
	/////////////////////////////////
	/// Convenience function to lookup the index for a given window pane widget. Returns an invalid index if the pane could not be found. \note This will always return the index of the "real" item for this window pane; never the index for an alias item.
	QModelIndex indexForPane(QWidget* pane) const;

	/// Convenience function to check if this item is a header item. This is defined as being true when (a) the item is NOT an alias, AND (b) that the AM::Widget role is 0/unset.
	bool isHeading(const QModelIndex& index) const;

	/// Convenience function to check if this item is actually an "alias" item.
	bool isAlias(const QModelIndex& index) const;

	/// Convenience function to retrieve an alias item's target item. Returns 0 if the item at this \c index is not an alias.
	QStandardItem* aliasTarget(const QModelIndex& index) const;

	/// Convenience function to check if this window pane is currently docked.
	bool isDocked(const QModelIndex& index) const;

	/// Convenience function to return a pointer to an item's QWidget window pane. For alias items, will return the window pane of the target. \warning Might be 0 if an invalid item has been added to the model, or if this index is a heading item.
	QWidget* pane(const QModelIndex& index) const;

	// Convenience setter functions:
	///////////////////////////////////

	/// Convenience function to dock or undock a window pane widget
	void setDocked(const QModelIndex& index, bool isDocked) { setData(index, isDocked, DockStateRole); }
	void dock(const QModelIndex& index) { setDocked(index, true); }
	void unDock(const QModelIndex& index) { setDocked(index, false); }

signals:
	/// Emitted when the dock state changes for an item (ie: true emitted when docked, false emitted when undocked)
	void dockStateChanged(QWidget* pane, bool isDocked);

protected slots:
	/// Catch when items are added to the model. If new window panes are added, add them to the widget2item_ cache, and setup event filters as required.
	void onRowsInserted(const QModelIndex &parent, int first, int last);
	/// Catch before items are removed from the model. If window panes are being removed, remove them from the cache.
	void onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);

protected:
	/// optimized cache: map from QWidget* to QStandardItem*.  Does not include separate entries for aliases
	QHash<QWidget*, QStandardItem*> widget2item_;

	/// event filter: catch title change and icon change events for all our window pane widgets, so we can emit dataChanged():
	bool eventFilter(QObject *source, QEvent *event);

	/// Internal version of pane() that can be used when you know that index is not an alias item.
	QWidget* pane_(const QModelIndex &index) const;


};

Q_DECLARE_METATYPE(QStandardItem*)

#endif // AMWINDOWPANEMODEL_H
