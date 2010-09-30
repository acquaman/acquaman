#include "AMWindowPaneModel.h"
#include <QWidget>


AMWindowPaneModel::AMWindowPaneModel(QObject* parent)
	: AMDragDropItemModel(parent) {

	connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
	connect(this, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(onRowsAboutToBeRemoved(QModelIndex,int,int)));
}

/// Returns a heading item with the given text. Will create and add a new heading under parentIndex if no heading with that text exists yet.  If \c text is empty, will return the top-level (invisible root) item.
QStandardItem* AMWindowPaneModel::headingItem(const QString& text, QModelIndex parentIndex) {
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
	if(parent)
		parent->appendRow(newHeading);
	else
		appendRow(newHeading);

	return newHeading;
}

/// data() is re-implemented from QStandardItemModel to return the widget window title and icon for the Qt::DecorationRole and Qt::DisplayRole/Qt::EditRole
QVariant AMWindowPaneModel::data(const QModelIndex &index, int role) const {

	switch(role) {

	case Qt::DisplayRole:
	case Qt::EditRole:
		// alias items can have separate descriptions, but for normal items, the DisplayRole and EditRole should be the window title
		if(!isAlias(index) && pane_(index))
			return pane_(index)->windowTitle();
		else
			return AMDragDropItemModel::data(index, role);
		break;


	case Qt::DecorationRole:
		if(!isAlias(index) && pane_(index))
			return pane_(index)->windowIcon();
		else
			return AMDragDropItemModel::data(index, role);
		break;

	// read-only property, defined as being (a) NOT an alias, AND (b) having no valid window pane widget set
	case AMWindowPaneModel::IsHeadingRole:
		return (index.isValid() && !isAlias(index) && pane_(index) == 0);
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

		if(pane_(index)) {
			pane_(index)->setWindowTitle(value.toString());
			return true;
		}
		else
			return false;

		break;


	case Qt::DecorationRole:
		// alias items and heading items can have separate icons, but for normal items, the Decoration role should be the window icon
		if(isAlias(index) || isHeading(index))
			return AMDragDropItemModel::setData(index, value, role);

		if(pane_(index)) {
			pane_(index)->setWindowIcon(value.value<QIcon>());
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

			QWidget* w = pane_(index);

			if(wasDocked && !nowDocked)
				emit dockStateChanged(w, false);
			else if(!wasDocked && nowDocked)
				emit dockStateChanged(w, true);

			return true;
		}
		else
			return false;
		break;
	}

	default:
		return AMDragDropItemModel::setData(index, value, role);
	}
}

/// This convenience function can be used to insert a new window pane widget into the model, under the heading \c headingText.  Alternatively, you can set the AM::WidgetRole on any QStandardItem and add it using the conventional QStandardItemModel::addRow() API.
/*! \note \c pane must be a valid widget until this item is removed from the model */
void AMWindowPaneModel::addPane(QWidget* pane, const QString& headingText) {

	QStandardItem* newItem = new QStandardItem(pane->windowIcon(), pane->windowTitle());


	headingItem(headingText)->appendRow(newItem);
}



bool AMWindowPaneModel::initAliasItem(QStandardItem* newAliasItem, QWidget* targetWindowPane, const QString& aliasKey, const QVariant& aliasValue) {

	if(!widget2item_.contains(targetWindowPane))
		return false;
	else
		return initAliasItem(newAliasItem, widget2item_.value(targetWindowPane), aliasKey, aliasValue);

}

bool AMWindowPaneModel::initAliasItem(QStandardItem *newAliasItem, QStandardItem *targetItem, const QString &aliasKey, const QVariant &aliasValue) {

	newAliasItem->setData(true, AMWindowPaneModel::IsAliasRole);
	newAliasItem->setData(qVariantFromValue(targetItem), AMWindowPaneModel::AliasTargetRole);
	newAliasItem->setData(aliasKey, AMWindowPaneModel::AliasKeyRole);
	newAliasItem->setData(aliasValue, AMWindowPaneModel::AliasValueRole);
	newAliasItem->setData(QVariant(), AM::WidgetRole);

	return true;
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
	return (index.isValid() && !isAlias(index) && pane_(index) == 0);
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

/// Convenience function to return a pointer to an item's QWidget window pane. For alias items, will return the window pane of the target. \warning Might be 0 if an invalid item has been added to the model, or if this index is a heading item.
QWidget* AMWindowPaneModel::pane(const QModelIndex& index) const {
	if(!index.isValid())
		return 0;

	if(isAlias(index)) {
		QStandardItem* target = aliasTarget(index);
		if(target)
			return pane_(target->index());
		else
			return 0;
	}

	else {
		return pane_(index);
	}
}

QWidget* AMWindowPaneModel::pane_(const QModelIndex &index) const {
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
			QWidget* widget = pane_(i);
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
			QWidget* w = pane_(i);
			if(w) {
				widget2item_.remove(w);
				w->removeEventFilter(this);
			}
		}
	}
}
