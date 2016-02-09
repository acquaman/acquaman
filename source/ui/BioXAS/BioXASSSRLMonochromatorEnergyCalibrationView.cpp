#include "BioXASSSRLMonochromatorEnergyCalibrationView.h"

#include "acquaman/AMScanConfiguration.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "dataman/AMScan.h"
#include "ui/dataman/AMScanView.h"
#include "ui/dataman/AMDataSourcesEditor.h"
#include "ui/dataman/AMChooseScanDialog.h"
#include "util/AMPeriodicTable.h"

BioXASSSRLMonochromatorEnergyCalibrationView::BioXASSSRLMonochromatorEnergyCalibrationView(BioXASSSRLMonochromator *mono, AMScan *scan, QWidget *parent) :
    QWidget(parent)
{
	// Initialize local variables.

	monoEnergy_ = 0;
	desiredEnergy_ = 0;

	mono_ = 0;
	currentScan_ = 0;

	scanViewModel_ = 0;

	// Create UI elements.

	loadDataButton_ = new QPushButton("Load data");
	loadDataButton_->setToolTip("Use previously acquired data for calibration.");

	newDataButton_ = new QPushButton("New data");
	newDataButton_->setToolTip("Configure a new scan for calibration.");

	scanView_ = new AMScanView();
	scanViewModel_ = scanView_->model();

	scanSourcesEditor_ = new AMDataSourcesEditor(scanViewModel_);

	QLabel *monoEnergyPrompt = new QLabel("Mono energy: ");

	monoEnergySpinBox_ = new QDoubleSpinBox(this);
	monoEnergySpinBox_->setValue(0);

	QLabel *desiredEnergyPrompt = new QLabel("Desired energy: ");

	desiredEnergySpinBox_ = new QDoubleSpinBox(this);
	desiredEnergySpinBox_->setValue(0);

	calibrateButton_ = new QPushButton("Calibrate");

	chooseScanDialog_ = 0;

	// Create and set layouts.

	QVBoxLayout *dataButtonsLayout = new QVBoxLayout();
	dataButtonsLayout->addWidget(loadDataButton_);
	dataButtonsLayout->addWidget(newDataButton_);

	QGridLayout *energyLayout = new QGridLayout();
	energyLayout->addWidget(monoEnergyPrompt, 0, 0, 1, 1, Qt::AlignRight);
	energyLayout->addWidget(monoEnergySpinBox_, 0, 1);
	energyLayout->addWidget(desiredEnergyPrompt, 1, 0, 1, 1, Qt::AlignRight);
	energyLayout->addWidget(desiredEnergySpinBox_, 1, 1);
	energyLayout->addWidget(calibrateButton_, 1, 2);

	QHBoxLayout *calibrationViewLayout = new QHBoxLayout();
	calibrationViewLayout->addLayout(dataButtonsLayout);
	calibrationViewLayout->addLayout(energyLayout);

	QGroupBox *calibrationView = new QGroupBox();
	calibrationView->setLayout(calibrationViewLayout);

	QVBoxLayout *leftLayout = new QVBoxLayout();
	leftLayout->addWidget(scanView_);
	leftLayout->addWidget(calibrationView);

	QVBoxLayout *rightLayout = new QVBoxLayout();
	rightLayout->addWidget(scanSourcesEditor_);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addLayout(leftLayout);
	layout->addLayout(rightLayout);

	setLayout(layout);

	// Make connections.

	connect( loadDataButton_, SIGNAL(clicked()), this, SLOT(onLoadDataButtonClicked()) );
	connect( newDataButton_, SIGNAL(clicked()), this, SIGNAL(energyCalibrationScanRequested()) );
	connect( scanView_, SIGNAL(dataPositionChanged(QPointF)), this, SLOT(onScanViewDataPositionChanged(QPointF)) );
	connect( monoEnergySpinBox_, SIGNAL(valueChanged(double)), this, SLOT(setMonoEnergy(double)) );
	connect( desiredEnergySpinBox_, SIGNAL(valueChanged(double)), this, SLOT(setDesiredEnergy(double)) );
	connect( calibrateButton_, SIGNAL(clicked()), this, SLOT(onCalibrateButtonClicked()) );

	// Initial settings.

	update();

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

		if (mono_ && mono_->energy()) {
			disconnect( mono_->energy(), 0, this, 0 );
		}

		mono_ = newMono;

		if (mono_ && mono_->energy()) {
			connect( mono_->energy(), SIGNAL(movingChanged(bool)), this, SLOT(updateCalibrateButton()) );
			connect( mono_->energy(), SIGNAL(calibrationStarted()), this, SLOT(onCalibrationStarted()) );
			connect( mono_->energy(), SIGNAL(calibrationFailed(int)), this, SLOT(onCalibrationFailed()) );
			connect( mono_->energy(), SIGNAL(calibrationSucceeded()), this, SLOT(onCalibrationSucceeded()) );
		}

		update();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::setCurrentScan(AMScan *newScan)
{
	if (currentScan_ != newScan) {

		if (currentScan_) {
			removeScan(currentScan_);
		}

		currentScan_ = newScan;

		if (currentScan_) {
			addScan(currentScan_);
		}

		refresh();

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
	updateLoadDataButton();
	updateScanView();
	updateScanSourcesEditor();
	updateMonoEnergySpinbox();
	updateDesiredEnergySpinbox();
	updateCalibrateButton();
}

void BioXASSSRLMonochromatorEnergyCalibrationView::refresh()
{
	// Clear the view.

	clear();

	// Apply current scan selection.

	applyScanSettings(currentScan_);

	// Update the view.

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
	if (monoEnergy_ != newEnergy) {

		monoEnergy_ = newEnergy;

		updateMonoEnergySpinbox();
		updateScanView();

		emit monoEnergyChanged(monoEnergy_);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::setDesiredEnergy(double newEnergy)
{
	if (desiredEnergy_ != newEnergy) {

		desiredEnergy_ = newEnergy;

		updateDesiredEnergySpinbox();

		emit desiredEnergyChanged(desiredEnergy_);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::applyScanSettings(AMScan *scan)
{
	if (scan) {

		// Set the mono energy to be the point midway through the scan data.
		// This would be a handy place for a peak finding algorithm upgrade later.

		AMDataSource *energySource = scan->dataSourceAt(0);

		if (energySource) {

			int valueIndex = energySource->size(0)/2;

			if (valueIndex > -1) {
				double value = energySource->axisValue(0, valueIndex);
				double valueMax = energySource->axisValue(0, energySource->size(0) - 1);
				double valueMin = energySource->axisValue(0, 0);

				// Update min/max energy values.

				monoEnergySpinBox_->setMinimum(valueMin);
				monoEnergySpinBox_->setMaximum(valueMax);

				desiredEnergySpinBox_->setMinimum(valueMin);
				desiredEnergySpinBox_->setMaximum(valueMax);

				// Update mono energy.

				setMonoEnergy(value);
			}
		}

		// The desired energy should initially be the edge scanned, value from the Periodic Table.

		applyConfigurationSettings(qobject_cast<BioXASXASScanConfiguration*>(scan->scanConfiguration()));

		// If available, set the dark current corrected absorbance data source as the source shown.

		AMDataSource *displayedSource = 0;
		int absorbanceIndex = scan->indexOfDataSource("Absorbance_DarkCorrect");
		if (absorbanceIndex > -1)
			displayedSource = scan->dataSourceAt(absorbanceIndex);

		// If the absorbance isn't available, set the first data source as the source shown.

		if (!displayedSource)
			displayedSource = scan->dataSourceAt(0);

		// Set the displayed source.

		if (displayedSource)
			scanViewModel_->setExclusiveDataSourceByName(displayedSource->name());
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::applyConfigurationSettings(BioXASXASScanConfiguration *configuration)
{
	if (configuration) {

		// The desired energy should initially be the edge scanned, energy from the Periodic Table.

		QString edgeText = configuration->edge();
		QStringList edgeTextParts = edgeText.split(" ");

		if (edgeTextParts.count() > 0) {
			QString elementSymbol = edgeTextParts.first();
			QString elementEdge = edgeTextParts.last();

			AMElement *element = AMPeriodicTable::table()->elementBySymbol(elementSymbol);

			double edgeEnergy = 0;

			if (elementEdge == "K")
				edgeEnergy = element->KEdge().energy();
			else if (elementEdge == "L1")
				edgeEnergy = element->L1Edge().energy();
			else if (elementEdge == "L2")
				edgeEnergy = element->L2Edge().energy();
			else if (elementEdge == "L3")
				edgeEnergy = element->L3Edge().energy();

			// If the edge energy is still zero, set desired energy to be the same as the mono energy.
			// Otherwise, set desired energy to the edge energy.

			if (edgeEnergy == 0)
				setDesiredEnergy(monoEnergy_);
			else
				setDesiredEnergy(edgeEnergy);

		} else {
			setDesiredEnergy(monoEnergy_);
		}

	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onLoadDataButtonClicked()
{
	if (!chooseScanDialog_) {
		chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::database("user"), "Open an existing scan", "Choose existing XAS scan to open.", this);
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
	setMonoEnergy(newPosition.x());
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onCalibrateButtonClicked()
{
	// Start calibration.

	if (mono_ && mono_->energy()) {
		mono_->energy()->calibrate(monoEnergy_, desiredEnergy_);
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::updateLoadDataButton()
{
	if (!currentScan_)
		loadDataButton_->setDefault(true);
	else
		loadDataButton_->setDefault(false);
}

void BioXASSSRLMonochromatorEnergyCalibrationView::updateScanView()
{
	bool cursorVisible = false;

	if (currentScan_ && mono_ && mono_->energy()) {
		cursorVisible = true;
	}

	scanView_->setPlotCursorVisibility(cursorVisible);
	scanView_->setPlotCursorCoordinates(monoEnergy_);
}

void BioXASSSRLMonochromatorEnergyCalibrationView::updateScanSourcesEditor()
{
	// Update the scan sources editor with the current scan.

	scanSourcesEditor_->setCurrentScan(currentScan_);
}

void BioXASSSRLMonochromatorEnergyCalibrationView::updateMonoEnergySpinbox()
{
	bool isEnabled = false;

	if (currentScan_ && mono_ && mono_->energy()) {
		isEnabled = true;
	}

	if (monoEnergy_ != monoEnergySpinBox_->value())
		monoEnergySpinBox_->setValue(monoEnergy_);

	if (isEnabled != monoEnergySpinBox_->isEnabled())
		monoEnergySpinBox_->setEnabled(isEnabled);
}

void BioXASSSRLMonochromatorEnergyCalibrationView::updateDesiredEnergySpinbox()
{
	bool isEnabled = false;

	if (currentScan_ && mono_ && mono_->energy()) {
		isEnabled = true;
	}

	if (desiredEnergy_ != desiredEnergySpinBox_->value())
		desiredEnergySpinBox_->setValue(desiredEnergy_);

	if (isEnabled != desiredEnergySpinBox_->isEnabled())
		desiredEnergySpinBox_->setEnabled(isEnabled);
}

void BioXASSSRLMonochromatorEnergyCalibrationView::updateCalibrateButton()
{
	bool isEnabled = false;

	if (currentScan_ && mono_ && mono_->energy() && !mono_->energy()->isMoving())
		isEnabled = true;

	if (isEnabled != calibrateButton_->isEnabled())
		calibrateButton_->setEnabled(isEnabled);
}

bool BioXASSSRLMonochromatorEnergyCalibrationView::dropScanURLs(const QList<QUrl> &urls)
{
	bool result = false;

	if (urls.count() > 0) {

		bool isScanning;
		QString scanName;

		AMScan *scan = AMScan::createFromDatabaseUrl(urls.last(), false, &isScanning, &scanName);

		// Check that the scan at the given URL is valid.

		if (scan) {

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
			QMessageBox scanValidNotification;
			scanValidNotification.setWindowTitle("Invalid scan selected.");
			scanValidNotification.setText(QString("The scan selected is not a valid scan. Must choose a valid XAS scan to proceed with calibration."));
			scanValidNotification.setIcon(QMessageBox::Information);
			scanValidNotification.addButton(QMessageBox::Ok);
			scanValidNotification.exec();
		}

		// If the scan is valid, an XAS scan, and not scanning, then set it as the current scan. Do nothing otherwise--the above QMessageBoxes should be notification enough.

		if (scan && !isScanning) {

			qDebug() << "\nValid scan selected. Proceeding with loading data...";
			setCurrentScan(scan);
			result = true;
		}
	}

	return result;
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onCalibrationStarted()
{
	QMessageBox calibrationStarted;
	calibrationStarted.setWindowTitle("Calibration Started");
	calibrationStarted.setText("The calibration procedure has started.");
	calibrationStarted.setIcon(QMessageBox::Information);
	calibrationStarted.addButton(QMessageBox::Ok);
	calibrationStarted.exec();
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onCalibrationFailed()
{
	QMessageBox calibrationFailed;
	calibrationFailed.setWindowTitle("Calibration Failed");
	calibrationFailed.setText("The calibration procedure failed.");
	calibrationFailed.setIcon(QMessageBox::Information);
	calibrationFailed.addButton(QMessageBox::Ok);
	calibrationFailed.exec();
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onCalibrationSucceeded()
{
	QMessageBox calibrationSucceeded;
	calibrationSucceeded.setWindowTitle("Calibration Successful");
	calibrationSucceeded.setText("The calibration procedure completed successfully!");
	calibrationSucceeded.setIcon(QMessageBox::Information);
	calibrationSucceeded.addButton(QMessageBox::Ok);
	calibrationSucceeded.exec();
}
