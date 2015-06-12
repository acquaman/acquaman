#include "BioXASSSRLMonochromatorEnergyControlCalibrationController.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotItem.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotTools.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"

BioXASSSRLMonochromatorEnergyControlCalibrationController::BioXASSSRLMonochromatorEnergyControlCalibrationController(BioXASSSRLMonochromatorEnergyControl *energyControl, AMScan *scan, const QStringList &sourceNames, QObject *parent) :
    QObject(parent)
{
	// Initialize member variables.

	energyControl_ = 0;
	scan_ = 0;

	plot_ = new MPlot();
	plot_->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	plot_->axisRight()->setTicks(0);

	cursor_ = new MPlotCursorTool();
	plot_->addTool(cursor_);

	cursor_->addCursor(plot_->axisScaleLeft(), plot_->axisScaleBottom());
	cursor_->cursor(0)->marker()->setPen(QPen(QColor(Qt::blue)));
	cursor_->cursor(0)->setMarker(MPlotMarkerShape::CrossCircle);

	braggPosition_ = 0;
	energy_ = 0;

	// Current settings.

	setEnergyControl(energyControl);
	setScan(scan);
	setSourceNames(sourceNames);
}

BioXASSSRLMonochromatorEnergyControlCalibrationController::~BioXASSSRLMonochromatorEnergyControlCalibrationController()
{

}

void BioXASSSRLMonochromatorEnergyControlCalibrationController::setEnergyControl(BioXASSSRLMonochromatorEnergyControl *newControl)
{
	if (energyControl_ != newControl) {
		energyControl_ = newControl;
		emit energyControlChanged(energyControl_);
	}
}

void BioXASSSRLMonochromatorEnergyControlCalibrationController::setScan(AMScan *newScan)
{
	if (scan_ != newScan) {

		if (scan_) {
			// Remove all items from the plot.
			removeAllDataSources(plot_);
		}

		scan_ = newScan;

		if (scan_) {
			// Add named sources to plot.
			addDataSources(sourceNames_, scan_, plot_);
		}

		emit scanChanged(scan_);
	}
}

void BioXASSSRLMonochromatorEnergyControlCalibrationController::setSourceNames(QStringList newSources)
{
	if (sourceNames_ != newSources) {

		// Remove all items from the plot.
		removeAllDataSources(plot_);


		sourceNames_ = newSources;

		// Add named sources to plot.
		addDataSources(sourceNames_, scan_, plot_);

		emit sourceNamesChanged(sourceNames_);
	}
}

void BioXASSSRLMonochromatorEnergyControlCalibrationController::setBraggPosition(double newPosition)
{
	if (braggPosition_ != newPosition) {
		braggPosition_ = newPosition;
		emit braggPositionChanged(braggPosition_);
	}
}

void BioXASSSRLMonochromatorEnergyControlCalibrationController::setEnergy(double newEnergy)
{
	if (energy_ != newEnergy) {
		energy_ = newEnergy;
		emit energyChanged(energy_);
	}
}

void BioXASSSRLMonochromatorEnergyControlCalibrationController::applyCalibrationSettings()
{
	if (energyControl_ && braggPosition_ != 0 && energy_ != 0) {
		energyControl_->setEnergy(braggPosition_, energy_);
	}
}

void BioXASSSRLMonochromatorEnergyControlCalibrationController::addDataSource(const QString &sourceName, AMScan *scan, MPlot *plot)
{
	if (scan && plot) {
		AMDataSource *toAdd = scan->dataSourceAt(scan->indexOfDataSource(sourceName));

		if (toAdd)
			plot->addItem(new MPlotSeriesBasic(new AMDataSourceSeriesData(toAdd)));
	}
}

void BioXASSSRLMonochromatorEnergyControlCalibrationController::addDataSources(const QStringList &sourceNames, AMScan *scan, MPlot *plot)
{
	if (scan && plot) {
		foreach (QString name, sourceNames)
			addDataSource(name, scan, plot);
	}
}

void BioXASSSRLMonochromatorEnergyControlCalibrationController::removeAllDataSources(MPlot *plot)
{
	if (plot) {
		plot->plotItems().clear();
	}
}
