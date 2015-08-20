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

	// Context menu.

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );
}

AMStripTool::~AMStripTool()
{

}

bool AMStripTool::contains(AMControl *control)
{
	return controlPlotItemMap_.contains(control);
}

bool AMStripTool::contains(const QString &pvName)
{
	bool controlFound = false;
	QList<AMControl*> controls = controlPlotItemMap_.keys();

	for (int i = 0, count = controls.size(); i < count && !controlFound; i++) {
		AMControl *control = controls.at(i);
		if (control->name() == pvName)
			controlFound = true;
	}

	return controlFound;
}

void AMStripTool::addPV(const QString &pvName)
{
	AMReadOnlyPVControl *control = new AMReadOnlyPVControl(pvName, pvName, this);
	addControl(control);
}

void AMStripTool::removePV(const QString &pvName)
{
	QList<AMControl*> controls = controlPlotItemMap_.keys();

	for (int i = 0, count = controls.size(); i < count; i++) {
		AMControl *control = controls.at(i);

		if (control->name() == pvName) {
			removeControl(control);
		}
	}
}

void AMStripTool::addControl(AMControl *control)
{
	if (control) {
		MPlotItem *item = createPlotItem(control);

		if (item) {
			plot_->addItem(item);
			controlPlotItemMap_.insert(control, item);

			emit controlAdded(control);
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

			emit controlRemoved(control);

			delete item;
		}
	}
}

void AMStripTool::showAddPVDialog()
{
	bool ok = false;
	QString pvName = QInputDialog::getText(this, "Add PV", "PV name:", QLineEdit::Normal, "", &ok);

	if (ok && !pvName.isEmpty())
		addPV(pvName);
}

void AMStripTool::onCustomContextMenuRequested(QPoint point)
{
	QMenu menu(this);

	// Add menu options.

	QAction *addPVAction = menu.addAction("Add PV");

	// Execute selected option.

	QAction *selection = menu.exec(mapToGlobal(point));

	if (selection) {
		if (selection->text() == addPVAction->text())
			showAddPVDialog();
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
