#ifndef AMSTRIPTOOLITEMS_H
#define AMSTRIPTOOLITEMS_H

#include <QObject>

#include "ui/AMStripToolItem.h"

class AMStripToolItems : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMStripToolItems(QObject *parent = 0);
	/// Destructor.
	virtual ~AMStripToolItems();

	/// Returns the list of items.
	QList<AMStripToolItem*> itemsList() const { return itemsList_; }

signals:
	/// Notifier that an item has been added.
	void itemAdded(AMStripToolItem *itemAdded);
	/// Notifier that an item has been removed.
	void itemRemoved();

public slots:
	/// Adds an item by control. Returns true if successful, false otherwise.
	bool addItem(AMControl *control);
	/// Removes an item by control. Returns true if successful, false otherwise.
	bool removeItem(AMControl *control);

	/// Adds an item by pv name. Returns true if successful, false otherwise.
	bool addItem(const QString &pvName);
	/// Removes an item by pv name. Returns true if successful, false otherwise.
	bool removeItem(const QString &pvName);

	/// Adds an item to the list of items.
	bool addItem(AMStripToolItem *newItem);
	/// Removes an item from the list of items.
	bool removeItem(AMStripToolItem *item);

protected:
	/// The list of striptool items.
	QList<AMStripToolItem*> itemsList_;
};

#endif // AMSTRIPTOOLITEMS_H
