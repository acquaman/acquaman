#ifndef AMSTRIPTOOL_H
#define AMSTRIPTOOL_H

#include <QWidget>
#include <QLayout>
#include <QMenu>
#include <QInputDialog>

#include "beamline/AMControl.h"
#include "ui/AMStripToolItem.h"
#include "MPlot/MPlot.h"

class AMStripTool : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMStripTool(QObject *parent = 0);
	/// Destructor.
	virtual ~AMStripTool();

	/// Returns the plot.
	MPlot* plot() const { return plot_; }
	/// Returns the list of striptool items.
	QList<AMStripToolItem*> items() const { return items_; }

signals:
	/// Notifier that an item has been added.
	void itemAdded(AMStripToolItem *newItem);
	/// Notifier that an item has been removed.
	void itemRemoved(AMStripToolItem *item);

public slots:
	/// Adds an item by control. Returns true if successful, false otherwise.
	bool addItem(AMControl *control);
	/// Removes an item by control. Returns true if successful, false otherwise.
	bool removeItem(AMControl *control);

	/// Adds an item by pv name. Returns true if successful, false otherwise.
	bool addItem(const QString &pvName);
	/// Removes an item by pv name. Returns true if successful, false otherwise.
	bool removeItem(const QString &pvName);

	/// Adds an item. Returns true if successful, false otherwise.
	bool addItem(AMStripToolItem *item);
	/// Removes an item. Returns true if successful, false otherwise.
	bool removeItem(AMStripToolItem *item);

protected:
	/// The plot.
	MPlot *plot_;
	/// The list of striptool items.
	QList<AMStripToolItem*> items_;
};

#endif // AMSTRIPTOOL_H
