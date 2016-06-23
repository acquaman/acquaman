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


#include "AMWindowPaneModel.h"
#include <QWidget>


 AMWindowPaneModel::~AMWindowPaneModel(){}
AMWindowPaneModel::AMWindowPaneModel(QObject* parent)
	: AMDragDropItemModel(parent) {

	connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
	connect(this, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(onRowsAboutToBeRemoved(QModelIndex,int,int)));
}

// Returns a heading item with the given text. Will create and add a new heading under \c parentIndex at \c position if no heading with that text exists yet.  (Use -1 for \c position to append at the bottom.) If \c text is empty, will return the top-level (invisible root) item.
QStandardItem* AMWindowPaneModel::headingItem(const QString& text, QModelIndex parentIndex, int position) {
	if(text.isEmpty())
		return invisibleRootItem();

	QList<QStandardItem*> searchItems = this->findItems(text);
	foreach(QStandardItem* i, searchItems) {
		if(isHeading(i->index()))
			return i;
	}

	// Didn't find it... time to make it:
	QStandardItem* newHeading = new QStandardItem(text);
	newHeading->setFlags(Qt::ItemIsEnabled);	// enabled, but should not be selectable
	// graphics defaults:
	QFont font = QFont("Lucida Grande", 10, QFont::Bold);
	font.setCapitalization(QFont::AllUppercase);
	newHeading->setFont(font);
	newHeading->setData(QBrush(QColor::fromRgb(100, 109, 125)), Qt::ForegroundRole);

	QStandardItem* parent = itemFromIndex(parentIndex);
	if(parent) {
		if(position < 0 || position > parent->rowCount())
			position = parent->rowCount();
		parent->insertRow(position, newHeading);
	}
	else {
		if(position < 0 || position > rowCount())
			position = rowCount();
		insertRow(position, newHeading);
	}

	return newHeading;
}

QList<QStandardItem*> AMWindowPaneModel::headingItems() const
{
	QList<QStandardItem*> headings;

	for (int i = 0, count = invisibleRootItem()->rowCount(); i < count; i++) {
		QStandardItem *item = invisibleRootItem()->child(i, 0);

		if (item)
			headings << item;
	}

	return headings;
}

bool AMWindowPaneModel::removeHeadingItem(const QString &text)
{
	if(text.isEmpty())
		return false;

	QStandardItem* headingItem = 0;
	QList<QStandardItem*> searchItems = findItems(text);
	foreach(QStandardItem* i, searchItems) {
		if(isHeading(i->index())) {
			headingItem = i;
			break;
		}
	}

	if (headingItem)
		return removeRow(headingItem->row());
	else
		return false;
}

/// data() is re-implemented from QStandardItemModel to return the widget window title and icon for the Qt::DecorationRole and Qt::DisplayRole/Qt::EditRole
QVariant AMWindowPaneModel::data(const QModelIndex &index, int role) const {

	switch(role) {

	case Qt::DisplayRole:
	case Qt::EditRole:
		// alias items can have separate descriptions, but for normal items, the DisplayRole and EditRole should be the window title
		if(!isAlias(index) && internalPane(index))
			return internalPane(index)->windowTitle();
		else
			return AMDragDropItemModel::data(index, role);
		break;


	case Qt::DecorationRole:
		if(!isAlias(index) && internalPane(index))
			return internalPane(index)->windowIcon();
		else
			return AMDragDropItemModel::data(index, role);
		break;

	// read-only property, defined as being (a) NOT an alias, AND (b) having no valid window pane widget set
	case AMWindowPaneModel::IsHeadingRole:
		return (index.isValid() && !isAlias(index) && internalPane(index) == 0);
		break;

		// Dock state: alias items should return the dock state of their target. Other items simply return their dock state, as stored in the AMWindowPaneModel::DockStateRole.
	case AMWindowPaneModel::DockStateRole:
		if(isAlias(index)) {
			QStandardItem* target = aliasTarget(index);
			if(target)
				return data(target->index(), role);
			else
				return true;
		}
		else
			return AMDragDropItemModel::data(index, role);
		break;

	case AMWindowPaneModel::UndockResizeRole:
		return AMDragDropItemModel::data(index, role);
		break;

	case AMWindowPaneModel::IsVisibleRole:
		if (AMDragDropItemModel::data(index, role).isValid())
			return AMDragDropItemModel::data(index, role);
		else
			return true;
	    break;

	default:
		return AMDragDropItemModel::data(index, role);
	}
}

/// setData() is re-implemented from QStandardItemModel to set the widget window title and icon for the Qt::DecorationRole and Qt::DisplayRole/Qt::EditRole
bool AMWindowPaneModel::setData(const QModelIndex &index, const QVariant &value, int role) {

	if(!index.isValid())
		return false;

	switch(role) {

	case Qt::DisplayRole:
	case Qt::EditRole:
		// alias items and heading items can have separate descriptions, but for normal items, the DisplayRole and EditRole should be the window title
		if(isAlias(index) || isHeading(index))
			return AMDragDropItemModel::setData(index, value, role);

		if(internalPane(index)) {
			internalPane(index)->setWindowTitle(value.toString());
			return true;
		}
		else
			return false;

		break;


	case Qt::DecorationRole:
		// alias items and heading items can have separate icons, but for normal items, the Decoration role should be the window icon
		if(isAlias(index) || isHeading(index))
			return AMDragDropItemModel::setData(index, value, role);

		if(internalPane(index)) {
			internalPane(index)->setWindowIcon(value.value<QIcon>());
			return true;
		}
		else
			return false;
		break;

	case AMWindowPaneModel::DockStateRole:
	{

		// docking an alias? dock the target instead.
		if(isAlias(index)) {
			QStandardItem* target = aliasTarget(index);
			if(target)
				return setData(target->index(), value, role);
			else
				return false;
		}

		bool nowDocked = value.toBool();
		bool wasDocked = isDocked(index);

		// set dock state like normal, but emit special signal dockStateChanged() if it's changing.
		if(AMDragDropItemModel::setData(index, value, role)) {

			QWidget* w = internalPane(index);

			if(wasDocked && !nowDocked)
				emit dockStateChanged(w, false, index.data(AMWindowPaneModel::UndockResizeRole).toBool());
			else if(!wasDocked && nowDocked)
				emit dockStateChanged(w, true, index.data(AMWindowPaneModel::UndockResizeRole).toBool());

			return true;
		}
		else
			return false;
		break;
	}
	case AMWindowPaneModel::UndockResizeRole:
		return AMDragDropItemModel::setData(index, value, role);
		break;

	case AMWindowPaneModel::IsVisibleRole:
	    if (AMDragDropItemModel::setData(index, value, role)) {
			QWidget *pane = internalPane(index);
			emit visibilityChanged(pane, value.toBool());
			return true;
	    } else {
			return false;
	    }

	    break;

	default:
		return AMDragDropItemModel::setData(index, value, role);
	}
}

QStandardItem* AMWindowPaneModel::addPane(QWidget* pane, const QString& headingText, bool resizeOnUndock, bool visible) {

	QStandardItem* newItem = new QStandardItem();
	newItem->setData(qVariantFromValue(pane), AM::WidgetRole);
	newItem->setData(true, AMWindowPaneModel::DockStateRole);
	newItem->setData(resizeOnUndock, AMWindowPaneModel::UndockResizeRole);
	newItem->setData(visible, AMWindowPaneModel::IsVisibleRole);

	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	headingItem(headingText)->appendRow(newItem);

	return newItem;
}

QStandardItem* AMWindowPaneModel::addPane(QWidget *pane, const QString &headingText, const QString& windowTitle, const QIcon& windowIcon, bool resizeOnUndock, bool visible) {

	pane->setWindowTitle(windowTitle);
	pane->setWindowIcon(windowIcon);

	return addPane(pane, headingText, resizeOnUndock, visible);
}


bool AMWindowPaneModel::initAliasItem(QStandardItem* newAliasItem, QWidget* targetWindowPane, const QString& aliasKey, const QVariant& aliasValue) {

	if(!widget2item_.contains(targetWindowPane))
		return false;
	else {
		initAliasItem(newAliasItem, widget2item_.value(targetWindowPane), aliasKey, aliasValue);
		return true;
	}

}

void AMWindowPaneModel::initAliasItem(QStandardItem *newAliasItem, QStandardItem *targetItem, const QString &aliasKey, const QVariant &aliasValue) {

	newAliasItem->setData(true, AMWindowPaneModel::IsAliasRole);
	newAliasItem->setData(qVariantFromValue(targetItem), AMWindowPaneModel::AliasTargetRole);
	newAliasItem->setData(aliasKey, AMWindowPaneModel::AliasKeyRole);
	newAliasItem->setData(aliasValue, AMWindowPaneModel::AliasValueRole);
	newAliasItem->setData(QVariant(), AM::WidgetRole);
	newAliasItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

}

// Convenience access functions:
/////////////////////////////////
/// Convenience function to lookup the index for a given window pane widget. Returns an invalid index if the pane could not be found.
QModelIndex AMWindowPaneModel::indexForPane(QWidget* pane) const {
	if(pane && widget2item_.contains(pane))
		return widget2item_.value(pane)->index();
	return QModelIndex();
}

/// Convenience function to check if this item is actually an "alias" item.
bool AMWindowPaneModel::isAlias(const QModelIndex& index) const {
	return (index.isValid() && data(index, AMWindowPaneModel::IsAliasRole).toBool());
}

/// Convenience function to check if this item is a header item. This is defined as being true when (a) the item is NOT an alias, AND (b) that the AM::Widget role is 0/unset.
bool AMWindowPaneModel::isHeading(const QModelIndex &index) const {
	return (index.isValid() && !isAlias(index) && internalPane(index) == 0);
}

/// Convenience function to retrieve an alias item's target item. Returns 0 if the item at this \c index is not an alias.
QStandardItem* AMWindowPaneModel::aliasTarget(const QModelIndex& index) const {
	if(!index.isValid() || !isAlias(index))
		return 0;

	QVariant itemPointer = data(index, AMWindowPaneModel::AliasTargetRole);
	if(itemPointer.isValid())
		return itemPointer.value<QStandardItem*>();
	else
		return 0;
}

/// Convenience function to check if this window pane is currently docked.
bool AMWindowPaneModel::isDocked(const QModelIndex& index) const {
	return data(index, AMWindowPaneModel::DockStateRole).toBool();
}

bool AMWindowPaneModel::isVisible(const QModelIndex &index) const
{
    return data(index, AMWindowPaneModel::IsVisibleRole).toBool();
}

/// Convenience function to return a pointer to an item's QWidget window pane. For alias items, will return the window pane of the target. \warning Might be 0 if an invalid item has been added to the model, or if this index is a heading item.
QWidget* AMWindowPaneModel::pane(const QModelIndex& index) const {
	if(!index.isValid())
		return 0;

	if(isAlias(index)) {
		QStandardItem* target = aliasTarget(index);
		if(target)
			return internalPane(target->index());
		else
			return 0;
	}

	else {
		return internalPane(index);
	}
}

QWidget* AMWindowPaneModel::internalPane(const QModelIndex &index) const {
	QVariant widgetPointer = data(index, AM::WidgetRole);
	if(widgetPointer.isValid())
		return widgetPointer.value<QWidget*>();
	else
		return 0;
}


bool AMWindowPaneModel::eventFilter(QObject *source, QEvent *event) {
	QWidget* pane = qobject_cast<QWidget*>(source);

	// not one of our widgets... never mind.
	if(!widget2item_.contains(pane))
		return AMDragDropItemModel::eventFilter(source, event);

	QModelIndex index = widget2item_.value(pane)->index();

	// if an undocked widget is being closed, we need to filter out that close event and dock it instead
	if(!isDocked(index) && event->type() == QEvent::Close) {
		dock(index);
		return true;
	}

	// if the window title or window icon is changing, we will need to emit dataChanged(), since the Qt::DisplayRole and Qt::DecorationRole data() will now be different.
	if(event->type() == QEvent::WindowTitleChange || event->type() == QEvent::WindowIconChange) {
		emit dataChanged(index, index);	/// \todo Have the new title/icon already been set, when these events go off? Might need to defer this.
	}

	return AMDragDropItemModel::eventFilter(source, event);
}

void AMWindowPaneModel::onRowsInserted(const QModelIndex &parent, int first, int last) {
	// for each item inserted: install event filter on the widget, and add to the widget lookup hash.
	for(int row=first; row<=last; row++) {
		QModelIndex i = index(row, 0, parent);

		if(!isAlias(i)) {
			QWidget* widget = internalPane(i);
			if(widget) {
				widget2item_.insert(widget, itemFromIndex(i));
				widget->installEventFilter(this);
			}
		}
	}
}

void AMWindowPaneModel::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last) {

	for(int row=first; row<=last; row++) {
		QModelIndex i = index(row, 0, parent);

		if(!isAlias(i)) {
			QWidget* w = internalPane(i);
			if(w) {
				widget2item_.remove(w);
				w->removeEventFilter(this);
			}
		}
	}
}


