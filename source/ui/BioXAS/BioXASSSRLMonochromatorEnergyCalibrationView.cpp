#include "BioXASSSRLMonochromatorEnergyCalibrationView.h"
#include "dataman/AMScan.h"

BioXASSSRLMonochromatorEnergyCalibrationView::BioXASSSRLMonochromatorEnergyCalibrationView(AMScan *scan, QWidget *parent) :
    QWidget(parent)
{
	currentScan_ = 0;

	// Create UI elements.

	loadDataButton_ = new QPushButton();

	scanView_ = new AMScanView();
	scanViewModel_ = scanView_->model();

	QLabel *peakEnergyPrompt = new QLabel("Peak energy: ");

	peakEnergySpinBox_ = new QDoubleSpinBox(this);
	peakEnergySpinBox_->setSuffix("eV");

	// Create and set layouts.

	QHBoxLayout *dataOptionsLayout = new QHBoxLayout();
	dataOptionsLayout->addWidget(loadDataButton_);

	QHBoxLayout *peakEnergyLayout = new QHBoxLayout();
	peakEnergyLayout->addWidget(peakEnergyPrompt);
	peakEnergyLayout->addWidget(peakEnergySpinBox);

	QVBoxLayout *calibrationOptionsLayout = new QHBoxLayout();
	calibrationOptionsLayout->addLayout(peakEnergyLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(dataOptionsLayout);
	layout->addWidget(scanView);
	layout->addLayout(calibrationOptionsLayout);

	setLayout(scanView);

	// Initial settings.

	peakEnergySpinBox_->setEnabled(false);

	// Make connections.

	connect( loadDataButton_, SIGNAL(clicked()), this, SLOT(onLoadDataButtonClicked()) );
//	connect( scanView_->)
	connect( peakEnergySpinBox_, SIGNAL(valueChanged(double)), this, SLOT(onEnergySpinBoxValueChanged()) );

	// Current settings.

	setCurrentScan(scan);
}

BioXASSSRLMonochromatorEnergyCalibrationView::~BioXASSSRLMonochromatorEnergyCalibrationView()
{

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

	// Clear the peak energy.

	peakEnergySpinBox_->clear();
}

void BioXASSSRLMonochromatorEnergyCalibrationView::update()
{
	if (currentScan_) {

		AMDataSource *energySource = currentScan_->dataSourceAt(0);

		if (energySource) {
			double valueIndex = energySource->size(0)/2;
			double value = energySource->axisValue(0, valueIndex);
			double valueMax = energySource->axisValue(0, energySource->size(0));
			double valueMin = energySource->axisValue(0, 0);

			// Update the scan view cursor coordinates with the given value.

			scanView_->setPlotCursorCoordinates(value);
			scanView_->setPlotCursorVisibility(true);

			// Update the peak energy to display the value.

			peakEnergySpinBox_->setEnabled(true);
			peakEnergySpinBox_->setMinimum(valueMin);
			peakEnergySpinBox_->setMaximum(valueMax);
			peakEnergySpinBox_->setValue(value);
		}

	} else {

		scanView_->setPlotCursorVisibility(false);
		peakEnergySpinBox_->setEnabled(false);
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
	if (peakEnergySpinBox_->isEnabled()) {

		// Set the plot cursor coordinates to match the peak energy.

		double peakEnergy = peakEnergySpinBox_->value();
		scanView_->setPlotCursorCoordinates(peakEnergy);
	}
}

bool BioXASSSRLMonochromatorEnergyCalibrationView::dropScanURLs(const QList<QUrl> &urls)
{
	if(	!urls.count() )
		return false;

	bool accepted = false;

	foreach(QUrl url, urls) {

		bool isScanning;
		QString scanName;
		AMScan* scan = AMScan::createFromDatabaseUrl(url, false, &isScanning, &scanName);

		// See if this scan is acquiring, and refuse to create a new instance if so.
		/// \todo With the new AMScan memory management model, we could actually open the existing AMScan* instance in multiple editors if desired... But propagation of changes under editing might be a problem, all editors currently assuming they are the only ones modifying the scan.
		if(isScanning) {
			QMessageBox stillScanningEnquiry;
			stillScanningEnquiry.setWindowTitle("This scan is still acquiring.");
			stillScanningEnquiry.setText(QString("The scan '%1' is currently still acquiring data, so you can't open multiple copies of it yet.")
										 .arg(scanName));
			stillScanningEnquiry.setIcon(QMessageBox::Information);
			stillScanningEnquiry.addButton(QMessageBox::Ok);
			stillScanningEnquiry.exec();
			continue;
		}

		if(!scan)
			continue;
		if(scan->type().compare("AMXASScan"))  //.compare() returns 0 if same
			continue;

		// success!
		addScan(scan);
		accepted = true;
	}

	return accepted;
}
