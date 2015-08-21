#include "AMStripToolItems.h"

AMStripToolItems::AMStripToolItems(QObject *parent) :
	QObject(parent)
{

}

AMStripToolItems::~AMStripToolItems()
{

}

bool AMStripToolItems::addItem(AMControl *control)
{
	bool itemAdded = false;

	if (control)
		itemAdded = addItem(new AMStripToolItem(control, this));

	return itemAdded;
}

bool AMStripToolItems::removeItem(AMControl *control)
{
	bool itemRemoved = false;

	if (control) {

		for (int itemIndex = 0, itemCount = itemsList_.size(); itemIndex < itemCount && !itemRemoved; itemIndex++) {
			AMStripToolItem *item = itemsList_.at(itemIndex);

			if (item->control() == control) {
				removeItem(item);
			}
		}
	}

	return itemRemoved;
}

bool AMStripToolItems::addItem(const QString &pvName)
{
	bool itemAdded = false;

	if (!pvName.isEmpty())
		itemAdded = addItem(new AMStripToolItem(pvName, this));

	return itemAdded;
}

bool AMStripToolItems::removeItem(const QString &pvName)
{
	bool itemRemoved = false;

	if (!pvName.isEmpty()) {
		for (int itemIndex = 0, itemCount = itemsList_.size(); itemIndex < itemCount && !itemRemoved; itemIndex++) {
			AMStripToolItem *item = itemsList_.at(itemIndex);

			if (item && item->control() && item->control()->name() == pvName)
				itemRemoved = removeItem(item);
		}
	}

	return itemRemoved;
}

bool AMStripToolItems::addItem(AMStripToolItem *newItem)
{
	bool itemAdded = false;

	if (newItem && !itemsList_.contains(newItem)) {
		itemsList_.append(newItem);
		itemAdded = true;
	}

	return itemAdded;
}

bool AMStripToolItems::removeItem(AMStripToolItem *item)
{
	bool itemRemoved = false;

	if (item) {
		itemRemoved = itemsList_.removeOne(item);

		if (itemRemoved && item->parent() == this)
			item->deleteLater();
	}

	return itemRemoved;
}
