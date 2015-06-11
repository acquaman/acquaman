#ifndef BIOXASSSRLMONOCHROMATORENERGYCONTROLCALIBRATIONCONTROLLER_H
#define BIOXASSSRLMONOCHROMATORENERGYCONTROLCALIBRATIONCONTROLLER_H

#include <QObject>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotTools.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "dataman/AMScan.h"
#include "dataman/AMScanSetModel.h"

class BioXASSSRLMonochromatorEnergyControlCalibrationController : public QObject
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorEnergyControlCalibrationController(BioXASSSRLMonochromatorEnergyControl *energyControl, AMScan *scan, const QStringList &sourceNames, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEnergyControlCalibrationController();

	/// Returns the energy control being calibrated.
	BioXASSSRLMonochromatorEnergyControl* energyControl() const { return energyControl_; }
	/// Returns the scan containing the calibration data.
	AMScan* scan() const { return scan_; }
	/// Returns the names of the scan sources of interest.
	QStringList sourceNames() const { return sourceNames_; }
	/// Returns the scan set model.
	AMScanSetModel* scanModel() const { return scanModel_; }
	/// Returns the plot.
	MPlot* plot() const { return plot_; }
	/// Returns the calibration bragg position.
	double braggPosition() const { return braggPosition_; }
	/// Returns the calibration energy.
	double energy() const { return energy_; }

signals:
	/// Notifier that the energy control being calibrated has changed.
	void energyControlChanged(BioXASSSRLMonochromatorEnergyControl *newControl);
	/// Notifier that the scan containing the calibration data has changed.
	void scanChanged(AMScan *newScan);
	/// Notifier that the named plot sources have changed.
	void sourceNamesChanged(QStringList newSources);
	/// Notifier that the calibration bragg position has changed.
	void braggPositionChanged(double newPosition);
	/// Notifier that the calibration energy has changed.
	void energyChanged(double newEnergy);

public slots:
	/// Sets the energy control being calibrated.
	void setEnergyControl(BioXASSSRLMonochromatorEnergyControl *newControl);
	/// Sets the scan containing the calibration data.
	void setScan(AMScan *newScan);
	/// Sets the names of the data sources to appear in the plot.
	void setSourceNames(QStringList newSources);
	/// Sets the desired calibration bragg position.
	void setBraggPosition(double newPosition);
	/// Sets the desired calibration energy.
	void setEnergy(double newEnergy);

	/// Applies desired calibration changes to the energy control.
	void applyCalibrationSettings();

protected slots:
	/// Adds the named data source in the given scan to the given plot.
	void addDataSource(const QString &sourceName, AMScan *scan, MPlot *plot);
	/// Adds each named data source in the list in the given scan to the given plot.
	void addDataSources(const QStringList &sourceNames, AMScan *scan, MPlot *plot);
	/// Removes all data sources from the given plot.
	void removeAllDataSources(MPlot *plot);

protected:
	/// The energy control being calibrated.
	BioXASSSRLMonochromatorEnergyControl *energyControl_;
	/// The scan containing the calibration data.
	AMScan *scan_;
	/// The list of data source names to include in the plot.
	QStringList sourceNames_;

	/// The scan model.
	AMScanSetModel *scanModel_;

	/// The calibration bragg position.
	double braggPosition_;
	/// The calibration energy.
	double energy_;
};

#endif // BIOXASSSRLMONOCHROMATORENERGYCONTROLCALIBRATIONCONTROLLER_H
