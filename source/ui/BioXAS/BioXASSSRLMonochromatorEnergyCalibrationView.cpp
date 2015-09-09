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

#include <QDebug>

BioXASSSRLMonochromatorEnergyCalibrationView::BioXASSSRLMonochromatorEnergyCalibrationView(BioXASSSRLMonochromator *mono, AMScan *scan, QWidget *parent) :
    QWidget(parent)
{
	// Initialize local variables.

	monoEnergy_ = 0;
	desiredEnergy_ = 0;

	mono_ = 0;
	currentScan_ = 0;

	scanViewModel_ = 0;

	cancelledMapper_ = new QSignalMapper(this);
	connect( cancelledMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onCalibrationCancelled(QObject*)) );

	failedMapper_ = new QSignalMapper(this);
	connect( failedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onCalibrationFailed(QObject*)) );

	succeededMapper_ = new QSignalMapper(this);
	connect( succeededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onCalibrationSucceeded(QObject*)) );

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
	connect( desiredEnergySpinBox_, SIGNAL(valueChanged(double)), this, SLOT(onDesiredEnergyValueChanged()) );
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

		if (mono_ && mono_->energyControl()) {
			disconnect( mono_->energyControl(), 0, this, 0 );
		}

		mono_ = newMono;

		if (mono_) {
			connect( mono_->energyControl(), SIGNAL(movingChanged(bool)), this, SLOT(updateCalibrateButton()) );
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

		QString energyUnits;

		if (scan->rawData() && scan->rawData()->scanAxesCount() >= 1)
			energyUnits = scan->rawData()->scanAxisAt(0).units;

		qDebug() << "Scan raw data units:" << energyUnits;

		// Set the mono energy to be the point midway through the scan data.
		// This would be a handy place for a peak finding algorithm upgrade later.

		AMDataSource *energySource = scan->dataSourceAt(0);

		if (energySource) {

			int valueIndex = energySource->size(0)/2;

			if (valueIndex > -1) {
				double value = energySource->axisValue(0, valueIndex);
				double valueMax = energySource->axisValue(0, energySource->size(0) - 1);
				double valueMin = energySource->axisValue(0, 0);
				QString units = energySource->units();

				qDebug() << "Energy source units:" << units;

				// Update min/max energy values.

				monoEnergySpinBox_->setMinimum(valueMin);
				monoEnergySpinBox_->setMaximum(valueMax);

				desiredEnergySpinBox_->setMinimum(valueMin);
				desiredEnergySpinBox_->setMaximum(valueMax);

				// Update units.

//				if (!units.isEmpty()) {
//					monoEnergySpinBox_->setSuffix(" " + units);
//					desiredEnergySpinBox_->setSuffix(" " + units);
//				}

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
	qDebug() << "Applying configuration settings.";

	if (configuration) {

		qDebug() << "Configuration is valid.";

		// The desired energy should initially be the edge scanned, energy from the Periodic Table.

		qDebug() << "\nScan configuration edge:" << configuration->edge();

		QString elementSymbol = configuration->edge().split(" ").first();
		QString elementEdge = configuration->edge().split(" ").last();

		qDebug() << "Element scanned:" << elementSymbol;
		qDebug() << "Edge scanned:" << elementEdge;

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

		qDebug() << "Edge energy: " << edgeEnergy;

		// If the edge energy is still zero, set it to be the same as the mono energy.
		// Otherwise, set it to the edge energy.

		if (edgeEnergy == 0)
			setDesiredEnergy(monoEnergy_);
		else
			setDesiredEnergy(edgeEnergy);

	} else {
		qDebug() << "Invalid scan configuration.";
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
	if (mono_) {

		// Create and execute a mono calibration action.

		AMAction3 *calibrationAction = mono_->createEnergyCalibrationAction(monoEnergy_, desiredEnergy_);

		if (calibrationAction) {

			cancelledMapper_->setMapping(calibrationAction, calibrationAction);
			failedMapper_->setMapping(calibrationAction, calibrationAction);
			succeededMapper_->setMapping(calibrationAction, calibrationAction);

			calibrationAction->start();

		} else {
			onCalibrationFailed();
		}
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::updateScanView()
{
	bool cursorVisible = false;

	if (currentScan_ && mono_ && mono_->energyControl()) {
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

	if (currentScan_ && mono_ && mono_->energyControl()) {
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

	if (currentScan_ && mono_ && mono_->energyControl()) {
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

	if (currentScan_ && mono_ && mono_->energyControl() && !mono_->energyControl()->isMoving())
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

			qDebug() << "Valid scan selected. Proceeding with loading data...";
			setCurrentScan(scan);
			result = true;
		}
	}

	return result;
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onCalibrationCancelled(QObject *action)
{
	onCalibrationCancelled();
	actionCleanup(action);
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onCalibrationCancelled()
{
	QMessageBox calibrationCancelled;
	calibrationCancelled.setWindowTitle("Calibration Cancelled");
	calibrationCancelled.setText(QString("The calibration procedure was cancelled."));
	calibrationCancelled.setIcon(QMessageBox::Information);
	calibrationCancelled.addButton(QMessageBox::Ok);
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onCalibrationFailed(QObject *action)
{
	onCalibrationFailed();
	actionCleanup(action);
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onCalibrationFailed()
{
	QMessageBox calibrationFailed;
	calibrationFailed.setWindowTitle("Calibration Failed");
	calibrationFailed.setText("The calibration procedure failed.");
	calibrationFailed.setIcon(QMessageBox::Information);
	calibrationFailed.addButton(QMessageBox::Ok);
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onCalibrationSucceeded(QObject *action)
{
	onCalibrationSucceeded();
	actionCleanup(action);
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onCalibrationSucceeded()
{
	QMessageBox calibrationSucceeded;
	calibrationSucceeded.setWindowTitle("Calibration Successful");
	calibrationSucceeded.setText("The calibration procedure completed successfully!");
	calibrationSucceeded.setIcon(QMessageBox::Information);
	calibrationSucceeded.addButton(QMessageBox::Ok);
}

void BioXASSSRLMonochromatorEnergyCalibrationView::actionCleanup(QObject *action)
{
	if (action) {
		action->disconnect();

		cancelledMapper_->removeMappings(action);
		failedMapper_->removeMappings(action);
		succeededMapper_->removeMappings(action);

		action->deleteLater();
	}
}

void BioXASSSRLMonochromatorEnergyCalibrationView::onDesiredEnergyValueChanged()
{
	double newEnergy = desiredEnergySpinBox_->value();
	setDesiredEnergy(newEnergy);
}
