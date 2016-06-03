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


#include "PGMAppController.h"

#include "beamline/PGM/PGMBeamline.h"

#include "acquaman/PGM/PGMXASScanConfiguration.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "application/AMAppControllerSupport.h"

#include "dataman/AMRun.h"
#include "dataman/AMScan.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/AMSMAKExporter.h"
#include "dataman/export/AMExporterOptionSMAK.h"

#include "ui/util/AMChooseDataFolderDialog.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/CLS/CLSSynchronizedDwellTimeView.h"
#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include "ui/PGM/PGMSlitControlView.h"
#include "ui/PGM/PGMPersistentView.h"
#include "ui/PGM/PGMBladeCurrentView.h"
#include "ui/PGM/PGMVAMView.h"
#include "ui/PGM/PGMXASScanConfigurationView.h"
#include "ui/PGM/PGMGratingView.h"
#include "ui/PGM/PGMUndulatorView.h"

PGMAppController::PGMAppController(QObject *parent)
	: CLSAppController("PGM", parent)
{
	setDefaultUseLocalStorage(true);

	detectorPaneCategoryName_ = "XRF Detectors";
}

bool PGMAppController::setupDataFolder()
{
	// Get a destination folder.
	return AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/pgm",  //local directory
							   "/home/pgm",               //remote directory
						       "users",                   //data directory
						       QStringList());            //extra data directory
}

void PGMAppController::initializeBeamline()
{
	// Initialize central beamline object
	PGMBeamline::pgm();
}

void PGMAppController::registerDBClasses()
{
	AMDbObjectSupport::s()->registerClass<PGMXASScanConfiguration>();
}

void PGMAppController::registerExporterOptions()
{

}

void PGMAppController::setupScanConfigurations()
{
	xasScanConfiguration_ = new PGMXASScanConfiguration;
	xasScanConfiguration_->setControl(0, AMBeamline::bl()->exposedControlByName("Energy")->toInfo());
//	xasScanConfiguration_->addDetector(AMBeamline::bl()->exposedDetectorByName("teyBladeCurrentDetector")->toInfo());
//	xasScanConfiguration_->addDetector(AMBeamline::bl()->exposedDetectorByName("flyBladeCurrentDetector")->toInfo());
	xasScanConfiguration_->addDetector(AMBeamline::bl()->exposedDetectorByName("i0EndstationBladeCurrentDetector")->toInfo());
	xasScanConfiguration_->addDetector(AMBeamline::bl()->exposedDetectorByName("i0BeamlineBladeCurrentDetector")->toInfo());
//	xasScanConfiguration_->addDetector(AMBeamline::bl()->exposedDetectorByName("photodiodeBladeCurrentDetector")->toInfo());

	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(91);
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.1);
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(92);
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1);
}

void PGMAppController::setupUserConfiguration()
{
}

void PGMAppController::onScanEditorCreatedImplementation(AMGenericScanEditor *editor)
{
	Q_UNUSED(editor)
}

void PGMAppController::createPersistentView()
{
	PGMPersistentView *persistentPanel = new PGMPersistentView;
	mw_->addRightWidget(persistentPanel);
}

void PGMAppController::createGeneralPanes()
{
	CLSSynchronizedDwellTime *synchronizedDwellTime = qobject_cast<CLSSynchronizedDwellTime *>(AMBeamline::bl()->synchronizedDwellTime());
	CLSSynchronizedDwellTimeView *synchronizedDwellTimeView = new CLSSynchronizedDwellTimeView(synchronizedDwellTime);
	synchronizedDwellTimeView->setAdvancedViewVisible(true);
	mw_->addPane(mw_->buildMainWindowPane("Synchronized Dwell", generalPaneIcon_, synchronizedDwellTimeView), generalPaneCategeryName_, "Synchronized Dwell", generalPaneIcon_);

	mw_->addPane(mw_->buildMainWindowPane("Blade Currents", generalPaneIcon_, new PGMBladeCurrentView), generalPaneCategeryName_, "Blade Currents", generalPaneIcon_);
	mw_->addPane(mw_->buildMainWindowPane("Slits", generalPaneIcon_, new PGMSlitControlView), generalPaneCategeryName_, "Slits", generalPaneIcon_);
	mw_->addPane(mw_->buildMainWindowPane("Mono Grating", generalPaneIcon_, new PGMGratingView), generalPaneCategeryName_, "Mono Grating", generalPaneIcon_);
	mw_->addPane(mw_->buildMainWindowPane("Undulator", generalPaneIcon_, new PGMUndulatorView), generalPaneCategeryName_, "Undulator", generalPaneIcon_);
	mw_->addPane(mw_->buildMainWindowPane("VAM", generalPaneIcon_, new PGMVAMView(PGMBeamline::pgm()->vam())), generalPaneCategeryName_, "VAM", generalPaneIcon_);
}

void PGMAppController::createDetectorPanes()
{
	AMXRFDetailedDetectorView *oceanOpticsDetectorView = new AMXRFDetailedDetectorView(PGMBeamline::pgm()->oceanOpticsDetector());
	oceanOpticsDetectorView->buildDetectorView();
	mw_->addPane(oceanOpticsDetectorView, detectorPaneCategoryName_, "Ocean Optics", detectorPaneIcon_);
}

void PGMAppController::createScanConfigurationPanes()
{
	xasScanConfigurationView_ = new PGMXASScanConfigurationView(xasScanConfiguration_);
	xasScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(xasScanConfigurationView_, true);
	mw_->addPane(xasScanConfigurationViewHolder3_, scanPaneCategoryName_, "XAS", scanPaneIcon_);
}
