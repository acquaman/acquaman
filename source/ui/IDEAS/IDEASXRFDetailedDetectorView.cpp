/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "IDEASXRFDetailedDetectorView.h"

#include <QDir>

#include "dataman/AMScan.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"

#include "ui/util/AMDialog.h"

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSSR570.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"

#include "beamline/IDEAS/IDEASBeamline.h"

IDEASXRFDetailedDetectorView::IDEASXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{
	chooseScanDialog_ = 0;
	setMaximumHeight(885);
}

IDEASXRFDetailedDetectorView::~IDEASXRFDetailedDetectorView()
{
}

void IDEASXRFDetailedDetectorView::buildDetectorView()
{
	// This is going to extend the base view, so we will simply call it's build method too.
	AMXRFDetailedDetectorView::buildDetectorView();

	buildScanSaveViews();

}

void IDEASXRFDetailedDetectorView::buildScanSaveViews()
{
	deadTimeCheckButton = new QPushButton("Check Dead Time");

	rightLayout_->addWidget(deadTimeCheckButton);

	rightLayout_->addStretch();

	saveScanButton_ = new QPushButton("Save Scan");

	rightLayout_->addWidget(saveScanButton_);


	connect(saveScanButton_, SIGNAL(clicked()), this, SLOT(onSaveScanButtonClicked()));
	connect(deadTimeCheckButton, SIGNAL(clicked()), this, SLOT(onDeadTimeCheckButtonClicked()));

}

void IDEASXRFDetailedDetectorView::onSaveScanButtonClicked()
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

void IDEASXRFDetailedDetectorView::exportScan()
{
	QList<AMScan *> scans;

	foreach (QUrl scanUrl, chooseScanDialog_->getSelectedScans()){
		AMScan *scan = AMScan::createFromDatabaseUrl(scanUrl, false);
		if (!scan) {
			QString errorMessage = QString("Failed to open AMScan for %1").arg(scanUrl.toString());
			AMErrorMon::alert(this, ERR_IDEAS_XRF_DETECTOR_SCAN_NOT_EXIST, errorMessage);
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
	exportController_->setOption(IDEAS::buildStandardExporterOption("IDEASXRFDefault", true, true, false));
	exportController_->option()->setFileName("$name_$number.dat");
	exportController_->start(true);
}

void IDEASXRFDetailedDetectorView::onExportControllerStateChanged(int state)
{
	if (state == AMExportController::Finished)
		exportController_->deleteLater();
}

void IDEASXRFDetailedDetectorView::startAcquisition()
{
	IDEASXRFScanConfiguration *configuration = new IDEASXRFScanConfiguration;
	configuration->setFluorescenceDetector(detector_->type());

	AMDetectorInfoSet detectorSet;
	detectorSet.addDetectorInfo(detector_->toInfo());
	configuration->setDetectorConfigurations(detectorSet);
	AMScanAction *scanAction = new AMScanAction(new AMScanActionInfo(configuration));

	connect(scanAction, SIGNAL(cancelled()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(failed()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(succeeded()), scanAction, SLOT(scheduleForDeletion()));

	scanAction->start();
}

void IDEASXRFDetailedDetectorView::onDeadTimeCheckButtonClicked()
{
	if(detector_->isAcquiring())
		return;

	double requestedTime = detector_->acquisitionTime();

	AMListAction3 *deadTimeCheckActions = new AMListAction3(new AMListActionInfo3("Quick Deadtime Check", "Quick Deadtime Check"));
	deadTimeCheckActions->addSubAction(detector_->createSetAcquisitionTimeAction(0.1));
	deadTimeCheckActions->addSubAction(detector_->createAcquisitionAction(AMDetectorDefinitions::SingleRead));
	deadTimeCheckActions->addSubAction(detector_->createSetAcquisitionTimeAction(requestedTime));

	deadTimeCheckActions->start();
}
