#include "AMStripTool.h"

#include "dataman/datasource/AMStripToolControlDataSource.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"

#include "MPlot/MPlotSeries.h"

AMStripTool::AMStripTool(QWidget *parent) :
	QWidget(parent)
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

	plotWidget_ = new MPlotWidget();
	plotWidget_->setPlot(plot_);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(plotWidget_);

	setLayout(layout);
}

AMStripTool::~AMStripTool()
{

}

void AMStripTool::addControl(AMControl *control)
{
	if (control) {
		MPlotItem *item = createPlotItem(control);

		if (item) {
			plot_->addItem(item);
			controlPlotItemMap_.insert(control, item);
		}
	}
}

void AMStripTool::removeControl(AMControl *control)
{
	if (control) {
		MPlotItem *item = controlPlotItemMap_.value(control, 0);

		if (item) {
			plot_->removeItem(item);
			controlPlotItemMap_.remove(control);

//			delete item;
		}
	}
}

MPlotItem* AMStripTool::createPlotItem(AMControl *control)
{
	MPlotSeriesBasic *result = 0;

	if (control) {
		AMDataSource *dataSource = new AMStripToolControlDataSource(control, control->name(), this);
		result = new MPlotSeriesBasic(new AMDataSourceSeriesData(dataSource));
	}

	return result;
}
