#ifndef BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include <QUrl>
#include <QToolButton>
#include <QSignalMapper>
#include <QGroupBox>

class AMScan;
class AMScanSetModel;
class AMScanView;
class AMDataSourcesEditor;
class AMChooseScanDialog;
class BioXASSSRLMonochromator;
class BioXASXASScanConfiguration;

class BioXASSSRLMonochromatorEnergyCalibrationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorEnergyCalibrationView(BioXASSSRLMonochromator *mono, AMScan *scan = 0, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEnergyCalibrationView();

	/// Returns the current mono energy.
	double monoEnergy() const { return monoEnergy_; }
	/// Returns the current desired energy.
	double desiredEnergy() const { return desiredEnergy_; }
	/// Returns the current monochromator.
	BioXASSSRLMonochromator* mono() const { return mono_; }
	/// Returns the current scan.
	AMScan* currentScan() const { return currentScan_; }
	/// Returns the scan view model.
	AMScanSetModel* scanViewModel() const { return scanViewModel_; }

signals:
	/// Notifier that the monochromator being calibrated has changed.
	void monoChanged(BioXASSSRLMonochromator *newMono);
	/// Notifier that the current scan has changed.
	void currentScanChanged(AMScan *newScan);
	/// Notifier that a monochromator energy calibration scan has been requested.
	void energyCalibrationScanRequested();
	/// Notifier that the mono energy has changed.
	void monoEnergyChanged(double newEnergy);
	/// Notifier that the desired energy has changed.
	void desiredEnergyChanged(double newEnergy);

public slots:
	/// Sets the mono being calibrated.
	void setMono(BioXASSSRLMonochromator *newMono);
	/// Sets the current scan.
	void setCurrentScan(AMScan *newScan);

	/// Clears the view.
	virtual void clear();
	/// Updates the view to reflect most current settings.
	virtual void update();
	/// Clears the view and then updates it.
	virtual void refresh();

protected slots:
	/// Add a scan.
	void addScan(AMScan *newScan);
	/// Remove a scan.
	void removeScan(AMScan *toRemove);

	/// Sets the current mono energy, and then applies it.
	void setMonoEnergy(double newEnergy);
	/// Sets the desired energy.
	void setDesiredEnergy(double newEnergy);

	/// Applies the settings from the given scan.
	void applyScanSettings(AMScan *scan);
	/// Applies the settings from the given configuration.
	void applyConfigurationSettings(BioXASXASScanConfiguration *configuration);

	/// Handles loading previously collected scan data.
	void onLoadDataButtonClicked();
	/// Handles updating the view with previously collected data.
	void onScanChosen();
	/// Handles updating the view when the scan view cursor position changes.
	void onScanViewDataPositionChanged(const QPointF &newPosition);
	/// Handles calibrating the mono when the calibrate button is clicked.
	void onCalibrateButtonClicked();

	/// Handles updating the 'load data' button.
	void updateLoadDataButton();
	/// Handles updating the scan view.
	void updateScanView();
	/// Handles updating the data sources editor.
	void updateScanSourcesEditor();
	/// Handles updating the mono energy display.
	void updateMonoEnergySpinbox();
	/// Handles updating the desired energy display.
	void updateDesiredEnergySpinbox();
	/// Handles updating the calibrate button.
	void updateCalibrateButton();

	/// Opens a set of scans from the database. Returns true if the list contains at least one valid scan that was added.
	bool dropScanURLs(const QList<QUrl>& urls);

	/// Handles notifying the user when the calibration has started.
	void onCalibrationStarted();
	/// Handles notifying the user when the calibration has failed.
	void onCalibrationFailed();
	/// Handles notifying the user when the calibration has succeeded.
	void onCalibrationSucceeded();

protected:
	/// The current mono energy.
	double monoEnergy_;
	/// The current desired energy.
	double desiredEnergy_;

	/// The monochromator being calibrated.
	BioXASSSRLMonochromator *mono_;
	/// The current scan.
	AMScan *currentScan_;
	/// The scan view model.
	AMScanSetModel *scanViewModel_;

	/// The load data button.
	QPushButton *loadDataButton_;
	/// The new data button.
	QPushButton *newDataButton_;

	/// The scan view.
	AMScanView *scanView_;
	/// The scan data sources editor.
	AMDataSourcesEditor *scanSourcesEditor_;
	/// The mono energy spinbox.
	QDoubleSpinBox *monoEnergySpinBox_;
	/// The desired energy spinbox.
	QDoubleSpinBox *desiredEnergySpinBox_;
	/// The calibrate button.
	QPushButton *calibrateButton_;
	/// The choose scan dialog.
	AMChooseScanDialog *chooseScanDialog_;
};

#endif // BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONVIEW_H
