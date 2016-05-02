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


#include "IDEASAppController.h"

#include "beamline/CLS/CLSStorageRing.h"
#include "beamline/IDEAS/IDEASBeamline.h"


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

#include "dataman/IDEAS/IDEASDbUpgrade1Pt1.h"

#include "dataman/IDEAS/IDEASUserConfiguration.h"

#include "util/AMPeriodicTable.h"

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/AMGenericStepScanController.h"
#include "acquaman/IDEAS/IDEASScanConfiguration.h"
#include "acquaman/IDEAS/IDEASXASScanConfiguration.h"
#include "acquaman/IDEAS/IDEAS2DScanConfiguration.h"

#include "ui/util/AMChooseDataFolderDialog.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"

#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "ui/IDEAS/IDEASPersistentView.h"
#include "ui/IDEAS/IDEASXASScanConfigurationView.h"
#include "ui/IDEAS/IDEAS2DScanConfigurationView.h"

#include "beamline/IDEAS/IDEASKETEKDetailedDetectorView.h"
#include "beamline/IDEAS/IDEAS13ElementGeDetailedDetectorView.h"
#include "ui/IDEAS/IDEASXRFDetailedDetectorView.h"
#include "ui/IDEAS/IDEASSampleCameraPanel.h"


IDEASAppController::IDEASAppController(QObject *parent)
	: CLSAppController("IDEAS", parent)
{
	userConfiguration_ = new IDEASUserConfiguration(this);

	setDefaultUseLocalStorage(true);

	appendDatabaseUpgrade(new IDEASDbUpgrade1Pt1("user", this));
	appendDatabaseUpgrade(new IDEASDbUpgrade1Pt1("actions", this));
}

bool IDEASAppController::startup()
{
	// Start up the main program.
	if(CLSAppController::startup()) {

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Github setup for adding VESPERS specific comment.
		additionalIssueTypesAndAssignees_.append("I think it's a IDEAS specific issue", "epengr");

		setupUserConfiguration();

		return true;
	}
	else
		return false;
}

void IDEASAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	CLSAppController::shutdown();
}

bool IDEASAppController::setupDataFolder()
{
	// Get a destination folder.
	if (!AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/ideas", "/home/ideas", "users"))
		return false;

	return true;
}

void IDEASAppController::initializeBeamline()
{
	// Initialize central beamline object
	IDEASBeamline::ideas();
}

void IDEASAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<IDEASScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<IDEASXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<IDEAS2DScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<IDEASUserConfiguration>();
}

void IDEASAppController::setupExporterOptions()
{
	AMExporterOptionGeneralAscii *XASexporterOption = IDEAS::buildStandardExporterOption("IDEASXASDefault", false, true, true);

	if(XASexporterOption->id() > 0)
	{
		AMAppControllerSupport::registerClass<IDEASXASScanConfiguration, AMExporterAthena, AMExporterOptionGeneralAscii>(XASexporterOption->id());
		AMAppControllerSupport::registerClass<IDEASXASScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(XASexporterOption->id());
	}

	XASexporterOption->deleteLater();

	AMExporterOptionSMAK *smakOption = IDEAS::buildSMAKExporterOption("IDEAS2DDefault", false, true);

	if (smakOption->id() > 0)
		AMAppControllerSupport::registerClass<IDEAS2DScanConfiguration, AMSMAKExporter, AMExporterOptionSMAK>(smakOption->id());

	smakOption->deleteLater();
}

void IDEASAppController::setupUserConfiguration()
{
	// It is sufficient to only connect the user configuration to the single element because the single element and four element are synchronized together.
	connect(userConfiguration_, SIGNAL(loadedFromDb()), this, SLOT(onUserConfigurationLoadedFromDb()));

	if (!userConfiguration_->loadFromDb(AMDatabase::database("user"), 1)){

		userConfiguration_->storeToDb(AMDatabase::database("user"));
		// This is connected here because our standard way for these signal connections is to load from db first, which clearly won't happen on the first time.
		connect(IDEASBeamline::ideas()->ketek(), SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
		connect(IDEASBeamline::ideas()->ketek(), SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
		connect(IDEASBeamline::ideas()->ketek(), SIGNAL(regionOfInterestBoundingRangeChanged(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest*)));
	}
}

void IDEASAppController::setupUserInterface()
{
	// Create panes in the main window:
	////////////////////////////////////

	mw_->insertHeading("General", 0);

	mw_->insertHeading("XRF Detectors", 1);

	ideasKETEKDetailedDetectorView_ = new IDEASKETEKDetailedDetectorView(IDEASBeamline::ideas()->ketek());
	ideasKETEKDetailedDetectorView_->buildDetectorView();
	ideasKETEKDetailedDetectorView_->setEnergyRange(1000, 20480);
	ideasKETEKDetailedDetectorView_->addEmissionLineNameFilter(QRegExp("1"));
	ideasKETEKDetailedDetectorView_->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	ideasKETEKDetailedDetectorView_->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
	mw_->addPane(ideasKETEKDetailedDetectorView_, "XRF Detectors", "KETEK", ":/system-search.png");

	onGe13Connected(false);

	mw_->insertHeading("Scans", 2);

	IDEASPersistentView *persistentPanel = new IDEASPersistentView;
	mw_->addRightWidget(persistentPanel);

	xasScanConfiguration_ = new IDEASXASScanConfiguration(this);
	xasScanConfigurationView_ = 0; //NULL
	xasScanConfigurationHolder3_ = new AMScanConfigurationViewHolder3("IDEAS XAS Scan", true, true);

	mw_->addPane(xasScanConfigurationHolder3_, "Scans", "IDEAS XAS Scan", ":/utilities-system-monitor.png");

	mapScanConfiguration_ = new IDEAS2DScanConfiguration(this);
	mapScanConfigurationView_ = new IDEAS2DScanConfigurationView(mapScanConfiguration_);
	mapScanConfigurationHolder3_ = new AMScanConfigurationViewHolder3("IDEAS 2D Map Scan", false, true, mapScanConfigurationView_);

	mw_->addPane(mapScanConfigurationHolder3_, "Scans", "IDEAS 2D Scan", ":/utilities-system-monitor.png");

	genericConfiguration_ = new AMGenericStepScanConfiguration;
	genericConfiguration_->addDetector(AMBeamline::bl()->exposedDetectorByName("I_0")->toInfo());
	genericConfigurationView_ = new AMGenericStepScanConfigurationView(genericConfiguration_, AMBeamline::bl()->exposedControls(), AMBeamline::bl()->exposedDetectors());
	genericConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Generic Scan", false, true, genericConfigurationView_);
	mw_->addPane(genericConfigurationViewHolder_, "Scans", "Generic Scan", ":/utilities-system-monitor.png");

	sampleCameraPanel_ = new IDEASSampleCameraPanel();
	mw_->addPane(sampleCameraPanel_, "Experiment Tools", "Sample Alignment",":/22x22/gnome-display-properties.png");

	connect(IDEASBeamline::ideas()->monoEnergyControl(), SIGNAL(connected(bool)), this, SLOT(onEnergyConnected(bool)));
	connect(IDEASBeamline::ideas()->ge13Element(), SIGNAL(connected(bool)), this, SLOT(onGe13Connected(bool)));
	onEnergyConnected(false);
}

void IDEASAppController::makeConnections()
{
	connect(this, SIGNAL(scanEditorCreated(AMGenericScanEditor*)), this, SLOT(onScanEditorCreated(AMGenericScanEditor*)));
}

void IDEASAppController::onGe13Connected(bool connected)
{
	Q_UNUSED(connected)
	if(IDEASBeamline::ideas()->ge13Element()->isConnected())
	{
		ideas13ElementGeDetailedDetectorView_ = new IDEAS13ElementGeDetailedDetectorView(IDEASBeamline::ideas()->ge13Element());
		ideas13ElementGeDetailedDetectorView_->buildDetectorView();
		ideas13ElementGeDetailedDetectorView_->setEnergyRange(1000, 20480);
		ideas13ElementGeDetailedDetectorView_->addEmissionLineNameFilter(QRegExp("1"));
		ideas13ElementGeDetailedDetectorView_->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
		ideas13ElementGeDetailedDetectorView_->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
		mw_->addPane(ideas13ElementGeDetailedDetectorView_, "XRF Detectors", "13-el Ge", ":/system-search.png");
	}
}

void IDEASAppController::onEnergyConnected(bool connected){
	Q_UNUSED(connected)
	if(IDEASBeamline::ideas()->monoEnergyControl() && IDEASBeamline::ideas()->monoEnergyControl()->isConnected() && !xasScanConfigurationView_){
		// Do New XAS

		xasScanConfigurationView_ = new IDEASXASScanConfigurationView(xasScanConfiguration_);
		xasScanConfigurationView_->setupDefaultXANESScanRegions();
		xasScanConfigurationHolder3_->setView(xasScanConfigurationView_, "Configure XAS scan", true, true);

		connect(xasScanConfiguration_, SIGNAL(totalTimeChanged(double)), xasScanConfigurationHolder3_, SLOT(updateOverallScanTime(double)));
		xasScanConfigurationHolder3_->updateOverallScanTime(xasScanConfiguration_->totalTime());
	}
}

void IDEASAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
	connect(CLSStorageRing::sr1(), SIGNAL(beamAvaliability(bool)), this, SLOT(onBeamAvailabilityChanged(bool)));
	userConfiguration_->storeToDb(AMDatabase::database("user"));
}

void IDEASAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
	disconnect(CLSStorageRing::sr1(), SIGNAL(beamAvaliability(bool)), this, SLOT(onBeamAvailabilityChanged(bool)));

	// Save the current configuration to the database.
	// Being explicit due to the nature of how many casts were necessary.  I could probably explicitly check to ensure each cast is successful, but I'll risk it for now.
	const AMScanActionInfo *actionInfo = qobject_cast<const AMScanActionInfo *>(action->info());
	const IDEASScanConfiguration *ideasConfiguration = dynamic_cast<const IDEASScanConfiguration *>(actionInfo->configuration());

	if (ideasConfiguration){

		IDEASScanConfigurationDbObject *configuration = qobject_cast<IDEASScanConfigurationDbObject *>(ideasConfiguration->dbObject());

		if (configuration){

			userConfiguration_->setFluorescenceDetector(configuration->fluorescenceDetector());
			userConfiguration_->storeToDb(AMDatabase::database("user"));
		}
	}
}

void IDEASAppController::onBeamAvailabilityChanged(bool beamAvailable)
{
	if (!beamAvailable && !AMActionRunner3::workflow()->pauseCurrentAction())
		AMActionRunner3::workflow()->setQueuePaused(true);

	else if (beamAvailable && AMActionRunner3::workflow()->queuedActionCount() > 0)
		AMActionRunner3::workflow()->setQueuePaused(false);
}

void IDEASAppController::onScanEditorCreated(AMGenericScanEditor *editor)
{
	connect(editor, SIGNAL(scanAdded(AMGenericScanEditor*,AMScan*)), this, SLOT(onScanAddedToEditor(AMGenericScanEditor*,AMScan*)));
	editor->setEnergyRange(AMPeriodicTable::table()->elementBySymbol("K")->Kalpha().energy(), 20480);
}

void IDEASAppController::onScanAddedToEditor(AMGenericScanEditor *editor, AMScan *scan)
{
	QString exclusiveName = QString();

	for (int i = 0, count = scan->analyzedDataSourceCount(); i < count && exclusiveName.isNull(); i++){

		AMDataSource *source = scan->analyzedDataSources()->at(i);

		if (source->name().contains("norm") && source->name().contains("Ka") && !source->hiddenFromUsers())
			exclusiveName = source->name();
	}
	if (exclusiveName.isNull())
		for (int i = 0, count = scan->analyzedDataSourceCount(); i < count && exclusiveName.isNull(); i++){

			AMDataSource *source = scan->analyzedDataSources()->at(i);

			if (source->name().contains("normSample"))
				exclusiveName = source->name();
		}


	if (!exclusiveName.isNull())
		editor->setExclusiveDataSourceByName(exclusiveName);

	else if (editor->scanAt(0)->analyzedDataSourceCount())
		editor->setExclusiveDataSourceByName(editor->scanAt(0)->analyzedDataSources()->at(editor->scanAt(0)->analyzedDataSourceCount()-1)->name());

	configureSingleSpectrumView(editor, scan);
}

void IDEASAppController::configureSingleSpectrumView(AMGenericScanEditor *editor, AMScan *scan)
{
	int scanRank = scan->scanRank();
	QStringList spectraNames;

	for (int i = 0, size = scan->dataSourceCount(); i < size; i++)
		if (scan->dataSourceAt(i)->rank()-scanRank == 1)
			spectraNames << scan->dataSourceAt(i)->name();

	if(!spectraNames.isEmpty())
			editor->setSingleSpectrumViewDataSourceName(spectraNames.first());

	editor->setEnergyRange(AMPeriodicTable::table()->elementBySymbol("Al")->Kalpha().energy(), 20480);
	editor->addSingleSpectrumEmissionLineNameFilter(QRegExp("1"));
	editor->addSingleSpectrumPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	editor->addSingleSpectrumCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
}

void IDEASAppController::onUserConfigurationLoadedFromDb()
{
	QList<IDEASScanConfiguration *> configurations = QList<IDEASScanConfiguration *>()
			<< xasScanConfiguration_
			<< mapScanConfiguration_;

	foreach (IDEASScanConfiguration *configuration, configurations)
		configuration->setFluorescenceDetector(userConfiguration_->fluorescenceDetector());

	AMXRFDetector *detector = IDEASBeamline::ideas()->ketek();

	foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){

		AMRegionOfInterest *newRegion = region->createCopy();
		detector->addRegionOfInterest(newRegion);
		mapScanConfiguration_->addRegionOfInterest(region);
		xasScanConfiguration_->addRegionOfInterest(region);
		genericConfiguration_->addRegionOfInterest(region);
	}

	// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
	connect(IDEASBeamline::ideas()->ketek(), SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
	connect(IDEASBeamline::ideas()->ketek(), SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
	connect(IDEASBeamline::ideas()->ketek(), SIGNAL(regionOfInterestBoundingRangeChanged(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest*)));
}

void IDEASAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	userConfiguration_->addRegionOfInterest(region);
	mapScanConfiguration_->addRegionOfInterest(region);
	xasScanConfiguration_->addRegionOfInterest(region);
	genericConfiguration_->addRegionOfInterest(region);
}

void IDEASAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	userConfiguration_->removeRegionOfInterest(region);
	mapScanConfiguration_->removeRegionOfInterest(region);
	xasScanConfiguration_->removeRegionOfInterest(region);
	genericConfiguration_->removeRegionOfInterest(region);
}

void IDEASAppController::onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region)
{
	userConfiguration_->setRegionOfInterestBoundingRange(region);
	mapScanConfiguration_->setRegionOfInterestBoundingRange(region);
	xasScanConfiguration_->setRegionOfInterestBoundingRange(region);
	genericConfiguration_->setRegionOfInterestBoundingRange(region);
}
