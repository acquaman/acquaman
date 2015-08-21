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

	if (control && items_)
		itemAdded = items_->addItem(control);

	return itemAdded;
}

bool AMStripTool::removeItem(AMControl *control)
{
	bool result = false;

	if (control && items_)
		result = items_->removeItem(control);

	return result;
}

bool AMStripTool::addItem(const QString &pvName)
{
	bool itemAdded = false;

	if (!pvName.isEmpty() && items_)
		itemAdded = items_->addItem(pvName);

	return itemAdded;
}

bool AMStripTool::removeItem(const QString &pvName)
{
	bool itemRemoved = false;

	if (!pvName.isEmpty() && items_)
		itemRemoved = items_->removeItem(pvName);

	return itemRemoved;
}

bool AMStripTool::addItem(AMStripToolItem *item)
{
	bool itemAdded = false;

	if (item && items_)
		itemAdded = items_->addItem(item);

	return itemAdded;
}

bool AMStripTool::removeItem(AMStripToolItem *item)
{
	bool itemRemoved = false;

	if (item && items_)
		itemRemoved = items_->removeItem(item);

	return itemRemoved;
}
