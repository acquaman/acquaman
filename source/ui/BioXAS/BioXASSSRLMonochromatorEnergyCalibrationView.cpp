#include "BioXASSSRLMonochromatorEnergyCalibrationView.h"

#include "beamline/BioXAS/BioXASBeamline.h"
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

	QLabel *peakEnergyPrompt = new QLabel("Mono energy: ");

	monoEnergySpinBox_ = new QDoubleSpinBox(this);

	QLabel *desiredEnergyPrompt = new QLabel("Desired energy: ");

	desiredEnergySpinBox_ = new QDoubleSpinBox(this);

	calibrateButton_ = new QPushButton("Calibrate");

	chooseScanDialog_ = 0;

	// Create and set layouts.

	QHBoxLayout *dataOptionsLayout = new QHBoxLayout();
	dataOptionsLayout->addWidget(loadDataButton_);
	dataOptionsLayout->addStretch();

	QGridLayout *energyLayout = new QGridLayout();
	energyLayout->addWidget(peakEnergyPrompt, 0, 0, 1, 1, Qt::AlignRight);
	energyLayout->addWidget(monoEnergySpinBox_, 0, 1);
	energyLayout->addWidget(desiredEnergyPrompt, 1, 0, 1, 1, Qt::AlignRight);
	energyLayout->addWidget(desiredEnergySpinBox_, 1, 1);
	energyLayout->addWidget(calibrateButton_, 1, 2);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(dataOptionsLayout);
	layout->addWidget(scanView_);
	layout->addLayout(energyLayout);

	setLayout(layout);

	// Initial settings.

	monoEnergySpinBox_->setEnabled(false);
	desiredEnergySpinBox_->setEnabled(false);
	calibrateButton_->setEnabled(false);

	// Make connections.

	connect( loadDataButton_, SIGNAL(clicked()), this, SLOT(onLoadDataButtonClicked()) );
	connect( scanView_, SIGNAL(dataPositionChanged(QPointF)), this, SLOT(onScanViewDataPositionChanged(QPointF)) );
	connect( monoEnergySpinBox_, SIGNAL(valueChanged(double)), this, SLOT(setMonoEnergy(double)) );
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

		if (mono_ && mono_->energyControl()) {
			mono_->disconnect(this);
			mono_->energyControl()->disconnect(this);
		}

		mono_ = newMono;

		if (mono_) {
			connect( mono_->energyControl(), SIGNAL(valueChanged(double)), this, SLOT(update()) );
			connect( mono_->energyControl(), SIGNAL(movingChanged(bool)), this, SLOT(update()) );
		}

		update();

		emit monoChanged(mono_);
	}
}

#include <QDebug>

void BioXASSSRLMonochromatorEnergyCalibrationView::setCurrentScan(AMScan *newScan)
{
	qDebug() << "Preparing to set calibration view current scan...";

	if (currentScan_ != newScan) {

		qDebug() << "Setting current scan...";

		if (currentScan_) {
			qDebug() << "Removing old scan...";
			removeScan(currentScan_);
			qDebug() << "Old scan removed.";
		}

		currentScan_ = newScan;

		if (currentScan_) {
			qDebug() << "Adding new scan...";
			addScan(currentScan_);
			qDebug() << "New scan added.";
		}

		qDebug() << "Refreshing view for new scan...";

		refresh();

		qDebug() << "View refreshed. Setting current scan complete.";

		emit currentScanChanged(currentScan_);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::clear()
{
	// Clear the scan view.

	scanView_->setPlotCursorVisibility(false);

	// Clear the energy boxes.

	monoEnergySpinBox_->clear();
	desiredEnergySpinBox_->clear();
}

void BioXASSSRLMonochromatorEnergyCalibrationView::update()
{
	if (currentScan_) {

		// Update the energy spinboxes and the calibrate button.

		if (mono_ && mono_->energyControl()) {
			monoEnergySpinBox_->setEnabled(true);
			desiredEnergySpinBox_->setEnabled(true);

			if (mono_->energyControl()->isMoving())
				calibrateButton_->setEnabled(false);
			else
				calibrateButton_->setEnabled(true);

		} else {
			monoEnergySpinBox_->setEnabled(false);
			desiredEnergySpinBox_->setEnabled(false);
			calibrateButton_->setEnabled(false);
		}

		// Update the mono energy.

		setMonoEnergy(monoEnergy_);

	} else {

		// If there is no scan, all widgets should be disabled/empty.

		scanView_->setPlotCursorVisibility(false);
		monoEnergySpinBox_->setEnabled(false);
		desiredEnergySpinBox_->setEnabled(false);
		calibrateButton_->setEnabled(false);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::refresh()
{
	// Clear the view.

	clear();

	// Apply current scan selection.

	if (currentScan_) {

		// Update the scan view.

		scanView_->setPlotCursorVisibility(true);

		// Set the mono energy to be the point midway through the scan data.
		// This would be a handy place for a peak finding algorithm upgrade later.

		AMDataSource *energySource = currentScan_->dataSourceAt(0);

		if (energySource) {

			int valueIndex = energySource->size(0)/2;

			if (valueIndex > -1) {
				double value = energySource->axisValue(0, valueIndex);
				double valueMax = energySource->axisValue(0, energySource->size(0) - 1);
				double valueMin = energySource->axisValue(0, 0);

				// Update the scan view cursor coordinates with the given value.

				setMonoEnergy(value);

				// Update the mono energy spinbox settings.

				monoEnergySpinBox_->setSuffix(" eV");
				monoEnergySpinBox_->setMinimum(valueMin);
				monoEnergySpinBox_->setMaximum(valueMax);

				// Update the desired energy spinbox settings.

				desiredEnergySpinBox_->setSuffix(" eV");
				desiredEnergySpinBox_->setMinimum(valueMin);
				desiredEnergySpinBox_->setMaximum(valueMax);
				desiredEnergySpinBox_->setValue(value);
			}
		}

		// Set the first data source as the source shown.

		AMDataSource *dataSource = currentScan_->dataSourceAt(0);

		if (dataSource) {
			scanViewModel_->setExclusiveDataSourceByName(dataSource->name());
		}
	}

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

void BioXASSSRLMonochromatorEnergyCalibrationView::setMonoEnergy(double newEnergy)
{
	qDebug() << "\nPreparing to set mono energy...";

	if (monoEnergy_ != newEnergy && mono_ && mono_->energyControl() && mono_->energyControl()->canMove()) {

		qDebug() << "Setting mono energy:" << newEnergy;

		monoEnergy_ = newEnergy;

		applyMonoEnergy(monoEnergy_);

		qDebug() << "Mono energy update complete.";
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::applyMonoEnergy(double newEnergy)
{
	if (mono_ && mono_->energyControl()) {
		// Move the mono to the given energy.

		mono_->energyControl()->move(newEnergy);

		// Update the mono energy spinbox.

		monoEnergySpinBox_->setValue(newEnergy);

		// Update the scan view cursor position.

		scanView_->setPlotCursorCoordinates(newEnergy);
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

void BioXASSSRLMonochromatorEnergyCalibrationView::onScanViewDataPositionChanged(const QPointF &newPosition)
{
	double newEnergy = newPosition.x();
	setMonoEnergy(newEnergy);
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
