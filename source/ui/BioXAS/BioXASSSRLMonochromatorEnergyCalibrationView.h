#ifndef BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QMessageBox>

#include "ui/dataman/AMScanView.h"
#include "ui/dataman/AMChooseScanDialog.h"

class BioXASSSRLMonochromatorEnergyCalibrationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorEnergyCalibrationView(AMScan *scan = 0, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEnergyCalibrationView();

signals:
	/// Notifier that the current scan has changed.
	void currentScanChanged(AMScan *newScan);

public slots:
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

	/// Handles loading previously collected scan data.
	void onLoadDataButtonClicked();
	/// Handles updating the view with previously collected data.
	void onScanChosen();
	/// Handles updating the scan cursor position to correspond to the peak energy.
	void onPeakEnergySpinBoxValueChanged();

	/// Opens a set of scans from the database. Returns true if the list contains at least one valid scan that was added.
	bool dropScanURLs(const QList<QUrl>& urls);

protected:
	/// The current scan.
	AMScan *currentScan_;
	/// The scan view model.
	AMScanSetModel *scanViewModel_;

	/// The load data button.
	QPushButton *loadDataButton_;
	/// The scan view.
	AMScanView *scanView_;
	/// The peak energy spinbox.
	QDoubleSpinBox *peakEnergySpinBox_;
	/// The choose scan dialog.
	AMChooseScanDialog *chooseScanDialog_;
};

#endif // BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONVIEW_H
