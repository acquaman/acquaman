#ifndef BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include <QUrl>
#include <QToolButton>

class AMScan;
class AMScanSetModel;
class AMScanView;
class AMChooseScanDialog;
class BioXASSSRLMonochromator;

class BioXASSSRLMonochromatorEnergyCalibrationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorEnergyCalibrationView(BioXASSSRLMonochromator *mono, AMScan *scan = 0, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEnergyCalibrationView();

signals:
	/// Notifier that the monochromator being calibrated has changed.
	void monoChanged(BioXASSSRLMonochromator *newMono);
	/// Notifier that the current scan has changed.
	void currentScanChanged(AMScan *newScan);

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
	/// Applies the given mono energy: moves the mono to the given energy, sets the plot cursor coordinates, and sets the energy spinbox value.
	void applyMonoEnergy(double newEnergy);

	/// Handles loading previously collected scan data.
	void onLoadDataButtonClicked();
	/// Handles updating the view with previously collected data.
	void onScanChosen();
	/// Handles updating the view when the scan view cursor position changes.
	void onScanViewDataPositionChanged(const QPointF &newPosition);
	/// Handles calibrating the mono when the calibrate button is clicked.
	void onCalibrateButtonClicked();

	/// Opens a set of scans from the database. Returns true if the list contains at least one valid scan that was added.
	bool dropScanURLs(const QList<QUrl>& urls);

protected:
	/// The current mono energy.
	double monoEnergy_;

	/// The monochromator being calibrated.
	BioXASSSRLMonochromator *mono_;
	/// The current scan.
	AMScan *currentScan_;
	/// The scan view model.
	AMScanSetModel *scanViewModel_;

	/// The load data button.
	QPushButton *loadDataButton_;
	/// The scan view.
	AMScanView *scanView_;
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
