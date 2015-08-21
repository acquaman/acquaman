#include "AMStripTool.h"

AMStripTool::AMStripTool(QObject *parent) :
	QObject(parent)
{
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

bool AMStripTool::addItem(AMControl *control)
{
	bool itemAdded = false;

	if (control)
		itemAdded = addItem(new AMStripToolItem(control, this));

	return itemAdded;
}

bool AMStripTool::removeItem(AMControl *control)
{
	bool itemRemoved = false;

	if (control) {
		for (int itemIndex = 0, itemCount = items_.size(); itemIndex < itemCount && !itemRemoved; itemIndex++) {
			AMStripToolItem *item = items_.at(itemIndex);

			if (item->control() == control) {
				items_.removeAt(itemIndex);
				itemRemoved = true;

				item->deleteLater();
			}
		}
	}

	return itemRemoved;
}

bool AMStripTool::addItem(const QString &pvName)
{
	bool itemAdded = false;

	if (!pvName.isEmpty())
		itemAdded = addItem(new AMStripToolItem(pvName, this));

	return itemAdded;
}

bool AMStripTool::removeItem(const QString &pvName)
{
	bool itemRemoved = false;

	if (!pvName.isEmpty()) {
		for (int itemIndex = 0, itemCount = items_.size(); itemIndex < itemCount && !itemRemoved; itemIndex++) {
			AMStripToolItem *item = items_.at(itemIndex);

			if (item->control() && item->control()->name() == pvName) {
				items_.removeAt(itemIndex);
				itemRemoved = true;

				item->deleteLater();
			}
		}
	}

	return itemRemoved;
}

bool AMStripTool::addItem(AMStripToolItem *item)
{
	bool itemAdded = false;

	if (item && !items_.contains(item)) {
		items_.append(item);
		itemAdded = true;
	}

	return itemAdded;
}

bool AMStripTool::removeItem(AMStripToolItem *item)
{
	bool itemRemoved = false;

	if (item)
		itemRemoved = items_.removeOne(item);

	return itemRemoved;
}
