#include "BioXASSSRLMonochromatorEnergyCalibrationView.h"

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "dataman/AMScan.h"
#include "ui/dataman/AMScanView.h"
#include "ui/dataman/AMChooseScanDialog.h"

BioXASSSRLMonochromatorEnergyCalibrationView::BioXASSSRLMonochromatorEnergyCalibrationView(BioXASSSRLMonochromator *mono, AMScan *scan, QWidget *parent) :
    QWidget(parent)
{
	// Initialize local variables.

	mono_ = 0;
	currentScan_ = 0;

	// Create UI elements.

	loadDataButton_ = new QPushButton("Load data");

	scanView_ = new AMScanView();
	scanViewModel_ = scanView_->model();

	QLabel *peakEnergyPrompt = new QLabel("Peak energy: ");

	peakEnergySpinBox_ = new QDoubleSpinBox(this);
	peakEnergySpinBox_->setSuffix("eV");

	QLabel *desiredEnergyPrompt = new QLabel("Desired energy: ");

	desiredEnergySpinBox_ = new QDoubleSpinBox(this);
	desiredEnergySpinBox_->setSuffix("eV");

	calibrateButton_ = new QPushButton("Calibrate");

	// Create and set layouts.

	QHBoxLayout *dataOptionsLayout = new QHBoxLayout();
	dataOptionsLayout->addWidget(loadDataButton_);
	dataOptionsLayout->addStretch();

	QGridLayout *energyLayout = new QGridLayout();
	energyLayout->addWidget(peakEnergyPrompt, 0, 0, 1, 1, Qt::AlignRight);
	energyLayout->addWidget(peakEnergySpinBox_, 0, 1);
	energyLayout->addWidget(desiredEnergyPrompt, 1, 0, 1, 1, Qt::AlignRight);
	energyLayout->addWidget(desiredEnergySpinBox_, 1, 1);
	energyLayout->addWidget(calibrateButton_, 1, 2);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(dataOptionsLayout);
	layout->addWidget(scanView_);
	layout->addLayout(energyLayout);

	setLayout(layout);

	// Initial settings.

	peakEnergySpinBox_->setEnabled(false);
	desiredEnergySpinBox_->setEnabled(false);
	calibrateButton_->setEnabled(false);

	// Make connections.

	connect( loadDataButton_, SIGNAL(clicked()), this, SLOT(onLoadDataButtonClicked()) );
	connect( peakEnergySpinBox_, SIGNAL(valueChanged(double)), this, SLOT(onPeakEnergySpinBoxValueChanged()) );
	connect( desiredEnergySpinBox_, SIGNAL(valueChanged(double)), this, SLOT(onDesiredEnergySpinBoxValueChanged()) );
	connect( calibrateButton_, SIGNAL(clicked()), this, SLOT(onCalibrateButtonClicked()) );

	// Current settings.

	setMono(mono);
	setCurrentScan(scan);
}

BioXASSSRLMonochromatorEnergyCalibrationView::~BioXASSSRLMonochromatorEnergyCalibrationView()
{

}

void BioXASSSRLMonochromatorEnergyCalibrationView::setMono(BioXASSSRLMonochromator *newMono)
{
	if (mono_ != newMono) {

		mono_ = newMono;

		update();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::setCurrentScan(AMScan *newScan)
{
	if (currentScan_ != newScan) {

		if (currentScan_)
			removeScan(currentScan_);

		currentScan_ = newScan;

		if (currentScan_)
			addScan(currentScan_);

		update();

		emit currentScanChanged(currentScan_);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::clear()
{
	// Clear the scan view.

	int scanCount = scanViewModel_->scanCount();

	for (int scanIndex = 0; scanIndex < scanCount; scanIndex++) {
		AMScan *scan = scanViewModel_->scanAt(scanIndex);

		if (scan)
			scanViewModel_->removeScan(scan);
	}

	// Clear the energy boxes.

	peakEnergySpinBox_->clear();
	desiredEnergySpinBox_->clear();
}

void BioXASSSRLMonochromatorEnergyCalibrationView::update()
{
	if (currentScan_) {

		AMDataSource *energySource = currentScan_->dataSourceAt(0);

		if (energySource) {

			int valueIndex = energySource->size(0)/2;
			double value = energySource->axisValue(0, valueIndex);
			double valueMax = energySource->axisValue(0, energySource->size(0));
			double valueMin = energySource->axisValue(0, 0);

			// Update the scan view cursor coordinates with the given value.

//			scanView_->setPlotCursorCoordinates(value);
//			scanView_->setPlotCursorVisibility(true);

			// Update the peak energy to display the value.

			peakEnergySpinBox_->setEnabled(true);
			peakEnergySpinBox_->setSuffix("eV");
			peakEnergySpinBox_->setMinimum(valueMin);
			peakEnergySpinBox_->setMaximum(valueMax);
			peakEnergySpinBox_->setValue(value);

			if (mono_) {

				// Update the desired energy to display the value.

				desiredEnergySpinBox_->setEnabled(true);
				desiredEnergySpinBox_->setSuffix("eV");
				desiredEnergySpinBox_->setMinimum(valueMin);
				desiredEnergySpinBox_->setMaximum(valueMax);
				desiredEnergySpinBox_->setValue(value);

				// Update the calibrate button.

				calibrateButton_->setEnabled(true);
			}
		}

	} else {

		// If there is no current scan, all calibration view elements should be disabled.

//		scanView_->setPlotCursorVisibility(false);
		peakEnergySpinBox_->setEnabled(false);
		desiredEnergySpinBox_->setEnabled(false);
		calibrateButton_->setEnabled(false);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::refresh()
{
	clear();
	update();
}

void BioXASSSRLMonochromatorEnergyCalibrationView::addScan(AMScan *newScan)
{
	if (newScan) {
		scanViewModel_->addScan(newScan);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::removeScan(AMScan *toRemove)
{
	if (toRemove) {
		scanViewModel_->removeScan(toRemove);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onLoadDataButtonClicked()
{
	if (!chooseScanDialog_) {
		chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::database("user"), "Open an existing scan", "Choose existing XAS scan to open.", false, this);
		connect( chooseScanDialog_, SIGNAL(accepted()), this, SLOT(onScanChosen()) );
	}

	chooseScanDialog_->show();
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onScanChosen()
{
	if (chooseScanDialog_)
		dropScanURLs(chooseScanDialog_->getSelectedScans());
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onPeakEnergySpinBoxValueChanged()
{
	// Set the plot cursor coordinates to match the peak energy.

//	if (peakEnergySpinBox_->isEnabled()) {
//		double peakEnergy = peakEnergySpinBox_->value();
//		scanView_->setPlotCursorCoordinates(peakEnergy);
//	}

	// The mono moves to the energy that is specified by the peak energy box.

	if (mono_ && mono_->energyControl() && peakEnergySpinBox_->isEnabled()) {
		double peakEnergy = peakEnergySpinBox_->value();
		mono_->energyControl()->move(peakEnergy);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onDesiredEnergySpinBoxValueChanged()
{

}

void BioXASSSRLMonochromatorEnergyCalibrationView::onCalibrateButtonClicked()
{
	// Set the monos current position to the desired energy.

	if (mono_ && desiredEnergySpinBox_->isEnabled()) {
		double desiredEnergy = desiredEnergySpinBox_->value();
		mono_->energyControl()->setEnergy(desiredEnergy);
	}
}

bool BioXASSSRLMonochromatorEnergyCalibrationView::dropScanURLs(const QList<QUrl> &urls)
{
	bool result = false;

	if (urls.count() > 0) {

		bool isScanning;
		QString scanName;
		bool validType = false;

		AMScan *scan = AMScan::createFromDatabaseUrl(urls.last(), false, &isScanning, &scanName);

		// Check that the scan at the given URL is valid.

		if (scan) {

			// Check that the scan is of an appropriate type.

			QString scanType = scan->type();

			if (scanType == "AMXASScan") {
				validType = true;

				// Check that this scan is NOT still acquiring before accepting it.
				// It's possible to view an acquiring scan here (ie in multiple editors) but if one is edited, changes may not make it to the other.

				if (isScanning) {
					QMessageBox scanningNotification;
					scanningNotification.setWindowTitle("Scan still acquiring");
					scanningNotification.setText(QString("The scan '%1' is currently still acquiring data. It's safest not to open multiple copies of a scan while it is still acquiring.").arg(scanName));
					scanningNotification.setIcon(QMessageBox::Information);
					scanningNotification.addButton(QMessageBox::Ok);
					scanningNotification.exec();
				}

			} else {
				QMessageBox scanTypeNotification;
				scanTypeNotification.setWindowTitle("Not an XAS scan");
				scanTypeNotification.setText(QString("The scan selected is not an XAS scan. Must choose an XAS scan to proceed with calibration."));
				scanTypeNotification.setIcon(QMessageBox::Information);
				scanTypeNotification.addButton(QMessageBox::Ok);
				scanTypeNotification.exec();
			}

		} else {
			QMessageBox scanValidNotification;
			scanValidNotification.setWindowTitle("Invalid scan selected.");
			scanValidNotification.setText(QString("The scan selected is not a valid scan. Must choose a valid XAS scan to proceed with calibration."));
			scanValidNotification.setIcon(QMessageBox::Information);
			scanValidNotification.addButton(QMessageBox::Ok);
			scanValidNotification.exec();
		}

		// If the scan is valid, an XAS scan, and not scanning, then set it as the current scan. Do nothing otherwise--the above QMessageBoxes should be notification enough.

		if (scan && validType && !isScanning) {
			setCurrentScan(scan);
			result = true;
		}
	}

	return result;
}
