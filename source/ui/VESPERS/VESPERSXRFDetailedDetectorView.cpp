#include "VESPERSXRFDetailedDetectorView.h"

#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"
#include "actions3/actions/AMScanAction.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"
#include "dataman/AMScan.h"
#include "application/VESPERS/VESPERS.h"
#include "ui/util/AMDialog.h"

#include <QDir>

VESPERSXRFDetailedDetectorView::VESPERSXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{
	chooseScanDialog_ = 0;
}

//void VESPERSXRFDetailedDetectorView::buildDetectorView()
//{
//	AMXRFDetailedDetectorView::buildDetectorView();

//	QPushButton *saveButton = new QPushButton(QIcon(), "Save");
//	connect(saveButton, SIGNAL(clicked()), this, SLOT(onSaveButtonClicked()));
//	energyRangeLayout_->addWidget(saveButton);
//}

//void VESPERSXRFDetailedDetectorView::onSaveButtonClicked()
//{
//	if(!chooseScanDialog_) {

//		chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::database("user"), "Choose XRF Spectrum...", "Choose the XRF Spectrum you want to export.", this);
//		chooseScanDialog_->setAttribute(Qt::WA_DeleteOnClose, false);
//		connect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(exportScan()));
//	}

//	chooseScanDialog_->setFilterKeyColumn(4);
//	chooseScanDialog_->setFilterRegExp("X(|-)Ray Fluorescence Scan");
//	chooseScanDialog_->show();
//}

//void VESPERSXRFDetailedDetectorView::exportScan()
//{
//	QList<AMScan *> scans;

//	foreach (QUrl scanUrl, chooseScanDialog_->getSelectedScans()){

//		AMScan *scan = AMScan::createFromDatabaseUrl(scanUrl, false);

//		QString scanName = AMLineEditDialog::retrieveAnswer("Choose a scan name...",
//															"Please choose the name you wish to name the scan.",
//															scan->name());

//		if (!scanName.isEmpty()){

//			scan->setName(scanName);
//			scan->storeToDb(AMDatabase::database("user"));
//			scans << scan;
//		}
//	}

//	// exporting is cancelled
//	if (scans.size() == 0)
//		return;

//	exportController_ = new AMExportController(scans);
//	connect(exportController_, SIGNAL(stateChanged(int)), this, SLOT(onExportControllerStateChanged(int)));

//	QDir exportDir;

//	if(!AMUserSettings::remoteDataFolder.isEmpty())
//		exportDir.setCurrent(AMUserSettings::remoteDataFolder);

//	else
//		exportDir.setCurrent(AMUserSettings::userDataFolder);

//	exportDir.cdUp();

//	if(!exportDir.entryList(QDir::AllDirs).contains("exportData")){

//		if(!exportDir.mkdir("exportData"))
//			return;
//	}

//	exportDir.cd("exportData");
//	exportController_->setDestinationFolderPath(exportDir.absolutePath());
//	exportController_->chooseExporter("AMExporterGeneralAscii");
//	exportController_->setOption(VESPERS::buildStandardExporterOption("VESPERSDefault", true, false, false, true));
//	exportController_->option()->setFileName("$name_$number.dat");
//	exportController_->start(true);
//}

//void VESPERSXRFDetailedDetectorView::onExportControllerStateChanged(int state)
//{
//	if (state == AMExportController::Finished)
//		exportController_->deleteLater();
//}

//void VESPERSXRFDetailedDetectorView::startAcquisition()
//{
//	VESPERSXRFScanConfiguration *configuration = new VESPERSXRFScanConfiguration;
//	AMDetectorInfoSet detectorSet;
//	detectorSet.addDetectorInfo(detector_->toInfo());
//	configuration->setDetectorConfigurations(detectorSet);
//	AMScanAction *scanAction = new AMScanAction(new AMScanActionInfo(configuration));

//	connect(scanAction, SIGNAL(cancelled()), scanAction, SLOT(scheduleForDeletion()));
//	connect(scanAction, SIGNAL(failed()), scanAction, SLOT(scheduleForDeletion()));
//	connect(scanAction, SIGNAL(succeeded()), scanAction, SLOT(scheduleForDeletion()));

//	scanAction->start();
//}
