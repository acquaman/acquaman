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

#include "beamline/CLS/CLSFacilityID.h"
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

#include "util/AMPeriodicTable.h"

#include "acquaman/IDEAS/IDEASXASScanConfiguration.h"
#include "acquaman/IDEAS/IDEASXRFScanConfiguration.h"

//#include "ui/CLS/CLSSynchronizedDwellTimeView.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"

#include "ui/IDEAS/IDEASPersistentView.h"
#include "ui/IDEAS/IDEASXASScanConfigurationView.h"
#include "ui/IDEAS/IDEASXRFDetailedDetectorView.h"
#include "ui/IDEAS/IDEASSampleCameraPanel.h"

IDEASAppController::IDEASAppController(QObject *parent)
	: AMAppController(parent)
{

}

bool IDEASAppController::startup()
{
	getUserDataFolderFromDialog();

	// Start up the main program.
	if(AMAppController::startup()) {

		// Initialize central beamline object
		IDEASBeamline::ideas();
		// Initialize the periodic table object.
		AMPeriodicTable::table();
		// Initialize the storage ring.
		CLSStorageRing::sr1();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)){

//			AMRun firstRun(CLSBeamline::beamlineName(CLSBeamline::IDEASBeamline), CLSBeamline::IDEASBeamline); //5: Ideas Beamline
			AMRun firstRun(CLSFacilityID::IDEASBeamlineName, CLSFacilityID::IDEASBeamline); //5: Ideas Beamline
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		setupExporterOptions();
		setupUserInterface();
		makeConnections();

		// Github setup for adding VESPERS specific comment.
		additionalIssueTypesAndAssignees_.append("I think it's a IDEAS specific issue", "epengr");

		return true;
	}
	else
		return false;
}

void IDEASAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void IDEASAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<IDEASXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<IDEASXRFScanConfiguration>();
}

void IDEASAppController::setupExporterOptions()
{
	QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "IDEAS Default XAS");

	AMExporterOptionGeneralAscii *ideasDefaultXAS = new AMExporterOptionGeneralAscii();

	if (matchIDs.count() != 0)
			ideasDefaultXAS->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

	ideasDefaultXAS->setName("IDEAS Default XAS");
	ideasDefaultXAS->setFileName("$name_$number.dat");
	ideasDefaultXAS->setHeaderText("Scan: $name #$number\nDate: $dateTime\n\nRing Current: $control[ringCurrent]\nInitial I_0: $control[I0Current]");
	ideasDefaultXAS->setHeaderIncluded(true);
	ideasDefaultXAS->setColumnHeader("$dataSetName $dataSetInfoDescription");
	ideasDefaultXAS->setColumnHeaderIncluded(true);
	ideasDefaultXAS->setColumnHeaderDelimiter("");
	ideasDefaultXAS->setSectionHeader("");
	ideasDefaultXAS->setSectionHeaderIncluded(true);
	ideasDefaultXAS->setIncludeAllDataSources(true);
	ideasDefaultXAS->setFirstColumnOnly(true);
	ideasDefaultXAS->setIncludeHigherDimensionSources(true);
	ideasDefaultXAS->setSeparateHigherDimensionalSources(true);
	ideasDefaultXAS->setSeparateSectionFileName("$name_$dataSetName_$number.dat");
	ideasDefaultXAS->setHigherDimensionsInRows(true);
	ideasDefaultXAS->storeToDb(AMDatabase::database("user"));

	if(ideasDefaultXAS->id() > 0)
			AMAppControllerSupport::registerClass<IDEASXASScanConfiguration, AMExporterAthena, AMExporterOptionGeneralAscii>(ideasDefaultXAS->id());

}

void IDEASAppController::setupUserInterface()
{
	// Create panes in the main window:
	////////////////////////////////////

	mw_->insertHeading("General", 0);

	mw_->insertHeading("XRF Detectors", 1);

	ideasKETEKDetailedDetectorView_ = new IDEASXRFDetailedDetectorView(IDEASBeamline::ideas()->ketek());
	ideasKETEKDetailedDetectorView_->buildDetectorView();
	ideasKETEKDetailedDetectorView_->setEnergyRange(1000, 20480);
	ideasKETEKDetailedDetectorView_->addEmissionLineNameFilter(QRegExp("1"));
	ideasKETEKDetailedDetectorView_->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	ideasKETEKDetailedDetectorView_->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
	mw_->addPane(ideasKETEKDetailedDetectorView_, "XRF Detectors", "KETEK", ":/system-search.png");

	ideas13ElementGeDetailedDetectorView_ = new IDEASXRFDetailedDetectorView(IDEASBeamline::ideas()->ge13Element());
	ideas13ElementGeDetailedDetectorView_->buildDetectorView();
	ideas13ElementGeDetailedDetectorView_->setEnergyRange(1000, 20480);
	ideas13ElementGeDetailedDetectorView_->addEmissionLineNameFilter(QRegExp("1"));
	ideas13ElementGeDetailedDetectorView_->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	ideas13ElementGeDetailedDetectorView_->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
	mw_->addPane(ideas13ElementGeDetailedDetectorView_, "XRF Detectors", "13-el Ge", ":/system-search.png");

	mw_->insertHeading("Scans", 2);

	IDEASPersistentView *persistentPanel = new IDEASPersistentView;
	mw_->addRightWidget(persistentPanel);

	xasScanConfigurationView_ = 0; //NULL
	xasScanConfigurationHolder3_ = new AMScanConfigurationViewHolder3(0, true);
	mw_->addPane(xasScanConfigurationHolder3_, "Scans", "IDEAS XAS Scan", ":/utilities-system-monitor.png");

	sampleCameraPanel_ = new IDEASSampleCameraPanel();
	mw_->addPane(sampleCameraPanel_, "Experiment Tools", "Sample Alignment",":/22x22/gnome-display-properties.png");

	connect(IDEASBeamline::ideas()->monoEnergyControl(), SIGNAL(connected(bool)), this, SLOT(onEnergyConnected(bool)));
	onEnergyConnected(false);
}

void IDEASAppController::makeConnections()
{
	connect(this, SIGNAL(scanEditorCreated(AMGenericScanEditor*)), this, SLOT(onScanEditorCreated(AMGenericScanEditor*)));

}

void IDEASAppController::onEnergyConnected(bool connected){
	Q_UNUSED(connected)
	if(IDEASBeamline::ideas()->monoEnergyControl() && IDEASBeamline::ideas()->monoEnergyControl()->isConnected() && !xasScanConfigurationView_){
		// Do New XAS
		IDEASXASScanConfiguration *xasScanConfiguration = new IDEASXASScanConfiguration(this);

		xasScanConfigurationView_ = new IDEASXASScanConfigurationView(xasScanConfiguration);
		xasScanConfigurationView_->setupDefaultXANESScanRegions();
		xasScanConfigurationHolder3_->setView(xasScanConfigurationView_);

		connect(xasScanConfiguration, SIGNAL(totalTimeChanged(double)), xasScanConfigurationHolder3_, SLOT(updateOverallScanTime(double)));
		xasScanConfigurationHolder3_->updateOverallScanTime(xasScanConfiguration->totalTime());
	}
}

void IDEASAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
	connect(CLSStorageRing::sr1(), SIGNAL(beamAvaliability(bool)), this, SLOT(onBeamAvailabilityChanged(bool)));
}

void IDEASAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
	disconnect(CLSStorageRing::sr1(), SIGNAL(beamAvaliability(bool)), this, SLOT(onBeamAvailabilityChanged(bool)));
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
	editor->setPlotRange(AMPeriodicTable::table()->elementBySymbol("K")->Kalpha().energy(), 20480);

	if (editor->using2DScanView())
		connect(editor, SIGNAL(dataPositionChanged(AMGenericScanEditor*,QPoint)), this, SLOT(onDataPositionChanged(AMGenericScanEditor*,QPoint)));
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

	editor->setPlotRange(AMPeriodicTable::table()->elementBySymbol("Al")->Kalpha().energy(), 20480);
}
