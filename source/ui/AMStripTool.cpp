#include "AMStripTool.h"

AMStripTool::AMStripTool(QObject *parent) :
	QObject(parent)
{
	// Create items.

	items_ = new AMStripToolItems(this);

	// Create plot.

	plot_ = new MPlot();

	plot_->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	plot_->axisRight()->setTicks(0);
	plot_->axisBottom()->setTicks(4);
	plot_->axisLeft()->showGrid(false);
	plot_->axisBottom()->showAxisName(false);
	plot_->axisLeft()->showAxisName(false);
	plot_->axisScaleBottom()->setPadding(0);
	plot_->axisScaleLeft()->setPadding(0);
	plot_->legend()->enableDefaultLegend(false);
}

AMStripTool::~AMStripTool()
{

}

void AMStripTool::setItems(AMStripToolItems *newItems)
{
	if (items_ != newItems) {

		if (items_) {

			if (items_->parent() == this)
				items_->deleteLater();
		}

		items_ = newItems;

		if (items_) {

		}

		emit itemsChanged(items_);
	}
}

bool AMStripTool::addItem(AMControl *control)
{
	bool itemAdded = false;

	if (control && items_) {
		itemAdded = addItem(new AMStripToolItem(control, items_));
	}

	return itemAdded;
}

bool AMStripTool::removeItem(AMControl *control)
{
	bool itemRemoved = false;

	if (control && items_) {
		itemRemoved = removeItem( items_->item(control) );
	}

	return itemRemoved;
}

bool AMStripTool::addItem(const QString &pvName)
{
	bool itemAdded = false;

	if (!pvName.isEmpty() && items_) {
		itemAdded = addItem( new AMStripToolItem(pvName, items_));
	}

	return itemAdded;
}

bool AMStripTool::removeItem(const QString &pvName)
{
	bool itemRemoved = false;

	if (!pvName.isEmpty() && items_) {
		itemRemoved = removeItem( items_->item(pvName) );
	}

	return itemRemoved;
}

bool AMStripTool::addItem(AMStripToolItem *item)
{
	bool itemAdded = false;

	if (item && items_) {
		itemAdded = items_->addItem(item);
		addItemToPlot( item );
	}

	return itemAdded;
}

bool AMStripTool::removeItem(AMStripToolItem *item)
{
	bool itemRemoved = false;

	if (item && items_) {
		removeItemFromPlot( item );
		itemRemoved = items_->removeItem(item);
	}

	return itemRemoved;
}

void AMStripTool::addItemToPlot(AMStripToolItem *item)
{
	if (item && item->plotItem())
		plot_->addItem(item->plotItem());
}

void AMStripTool::removeItemFromPlot(AMStripToolItem *item)
{
	if (item && item->plotItem())
		plot_->removeItem(item->plotItem());
}
