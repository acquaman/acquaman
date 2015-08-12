#include "SXRMBXRFDetailedDetectorView.h"

#include "actions3/actions/AMScanAction.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"
#include "dataman/AMScan.h"
#include "ui/util/AMDialog.h"

#include "acquaman/SXRMB/SXRMBXRFScanConfiguration.h"
#include "application/SXRMB/SXRMB.h"

SXRMBXRFDetailedDetectorView::SXRMBXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent)
	:AMXRFDetailedDetectorView(detector, parent)
{
	chooseScanDialog_ = 0;
	exportController_ = 0;
}

SXRMBXRFDetailedDetectorView::~SXRMBXRFDetailedDetectorView()
{

}

void SXRMBXRFDetailedDetectorView::buildDetectorView()
{
	AMXRFDetailedDetectorView::buildDetectorView();

	QPushButton *saveButton = new QPushButton(QIcon(), "Save");
	connect(saveButton, SIGNAL(clicked()), this, SLOT(onSaveButtonClicked()));
	energyRangeLayout_->insertWidget(0, saveButton);
}

void SXRMBXRFDetailedDetectorView::onSaveButtonClicked()
{

	if(!chooseScanDialog_) {
		chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::database("user"), "Choose XRF Spectrum...", "Choose the XRF Spectrum you want to export.", this);
		chooseScanDialog_->setAttribute(Qt::WA_DeleteOnClose, false);
		connect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(exportScan()));
	}

	chooseScanDialog_->setFilterKeyColumn(4);
	chooseScanDialog_->setFilterRegExp("XRay Fluorescence Scan");
	chooseScanDialog_->show();
}

void SXRMBXRFDetailedDetectorView::exportScan()
{
	QList<AMScan *> scans;

	foreach (QUrl scanUrl, chooseScanDialog_->getSelectedScans()){
		AMScan *scan = AMScan::createFromDatabaseUrl(scanUrl, false);
		if (!scan) {
			QString errorMessage = QString("Failed to open AMScan for %1").arg(scanUrl.toString());
			AMErrorMon::alert(this, ERR_SXRMB_XRF_DETECTOR_SCAN_NOT_EXIST, errorMessage);
			return;
		}

		QString scanName = AMLineEditDialog::retrieveAnswer("Choose a scan name...",
															"Please choose the name you wish to name the scan.",
															scan->name());

		if (!scanName.isEmpty()){

			scan->setName(scanName);
			scan->storeToDb(AMDatabase::database("user"));
			scans << scan;
		}
	}

	// exporting is cancelled
	if (scans.size() == 0)
		return;

	exportController_ = new AMExportController(scans);
	connect(exportController_, SIGNAL(stateChanged(int)), this, SLOT(onExportControllerStateChanged(int)));

	QDir exportDir;

	if(!AMUserSettings::remoteDataFolder.isEmpty())
		exportDir.setCurrent(AMUserSettings::remoteDataFolder);
	else
		exportDir.setCurrent(AMUserSettings::userDataFolder);

	exportDir.cdUp();

	if(!exportDir.entryList(QDir::AllDirs).contains("exportData")){

		if(!exportDir.mkdir("exportData"))
			return;
	}

	exportDir.cd("exportData");
	exportController_->setDestinationFolderPath(exportDir.absolutePath());
	exportController_->chooseExporter("AMExporterGeneralAscii");
	exportController_->setOption(SXRMB::buildStandardExporterOption("SXRMBDefault", true, false, false, true));
	exportController_->option()->setFileName("$name_$number.dat");
	exportController_->start(true);
}

void SXRMBXRFDetailedDetectorView::onExportControllerStateChanged(int state)
{
	if (state == AMExportController::Finished)
		exportController_->deleteLater();
}

void SXRMBXRFDetailedDetectorView::startAcquisition()
{
	SXRMBXRFScanConfiguration *configuration = new SXRMBXRFScanConfiguration;
	AMDetectorInfoSet detectorSet;
	detectorSet.addDetectorInfo(detector_->toInfo());
	configuration->setDetectorConfigurations(detectorSet);
	AMScanAction *scanAction = new AMScanAction(new AMScanActionInfo(configuration));

	connect(scanAction, SIGNAL(cancelled()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(failed()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(succeeded()), scanAction, SLOT(scheduleForDeletion()));

	scanAction->start();
}
