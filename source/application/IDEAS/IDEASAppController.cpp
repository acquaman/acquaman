/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "beamline/IDEAS/IDEASBeamline.h"

#include "ui/IDEAS/IDEASScanConfigurationViewHolder3.h"

#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/IDEAS/IDEASPersistentView.h"
#include "dataman/AMScan.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "application/AMAppControllerSupport.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/AMRun.h"

#include "util/AMPeriodicTable.h"

//#include "ui/CLS/CLSSynchronizedDwellTimeView.h"
#include "ui/IDEAS/IDEASXASScanConfigurationView.h"
#include "ui/IDEAS/IDEASScanConfigurationViewHolder3.h"
#include "ui/IDEAS/IDEASXRFDetailedDetectorViewWithSave.h"
#include "acquaman/IDEAS/IDEASXASScanConfiguration.h"
#include "acquaman/IDEAS/IDEASXRFScanConfiguration.h"

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

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)){

						AMRun firstRun("IDEAS", 5);	/// \todo For now, we know that 5 is the ID of the IDEAS facility, but this is a hardcoded hack.
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

	mw_->insertHeading("Detectors", 1);

            IDEASXRFDetailedDetectorViewWithSave_ = new IDEASXRFDetailedDetectorViewWithSave(IDEASBeamline::ideas()->ketek());
                IDEASXRFDetailedDetectorViewWithSave_->buildDetectorView();
		IDEASXRFDetailedDetectorViewWithSave_->setEnergyRange(1000, 20480);
		IDEASXRFDetailedDetectorViewWithSave_->addEmissionLineNameFilter(QRegExp("1"));
		IDEASXRFDetailedDetectorViewWithSave_->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
		IDEASXRFDetailedDetectorViewWithSave_->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
		mw_->addPane(IDEASXRFDetailedDetectorViewWithSave_, "Detectors", "XRF Detector", ":/system-search.png");

    mw_->insertHeading("Scans", 2);

		IDEASPersistentView *persistentPanel = new IDEASPersistentView;
		mw_->addRightWidget(persistentPanel);

//	ideasSynchronizedDwellTimeView_ = 0; //NULL
//	connect(IDEASBeamline::ideas()->synchronizedDwellTime(), SIGNAL(connected(bool)), this, SLOT(onSynchronizedDwellTimeConnected(bool)));
//	onSynchronizedDwellTimeConnected(false);

	xasScanConfigurationView_ = 0; //NULL
	xasScanConfigurationHolder3_ = new IDEASScanConfigurationViewHolder3();
	mw_->addPane(xasScanConfigurationHolder3_, "Scans", "IDEAS XAS Scan", ":/utilities-system-monitor.png");

	connect(IDEASBeamline::ideas()->monoEnergyControl(), SIGNAL(connected(bool)), this, SLOT(onEnergyConnected(bool)));
	onEnergyConnected(false);
}

void IDEASAppController::makeConnections()
{
    connect(this, SIGNAL(scanEditorCreated(AMGenericScanEditor*)), this, SLOT(onScanEditorCreated(AMGenericScanEditor*)));

}

//void IDEASAppController::onSynchronizedDwellTimeConnected(bool connected){
//	Q_UNUSED(connected)
//	if(IDEASBeamline::ideas()->synchronizedDwellTime() && IDEASBeamline::ideas()->synchronizedDwellTime()->isConnected() && !ideasSynchronizedDwellTimeView_){
//		CLSSynchronizedDwellTime *clsDwellTime = qobject_cast<CLSSynchronizedDwellTime*>(IDEASBeamline::ideas()->synchronizedDwellTime());
//		if(clsDwellTime)
//			ideasSynchronizedDwellTimeView_ = new CLSSynchronizedDwellTimeView(clsDwellTime);

//		mw_->addPane(ideasSynchronizedDwellTimeView_, "Detectors", "IDEAS Sync Dwell", ":/system-software-update.png", true);
//		ideasSynchronizedDwellTimeView_->setAdvancedViewVisible(true);
//	}
//}

void IDEASAppController::onEnergyConnected(bool connected){
	Q_UNUSED(connected)
	if(IDEASBeamline::ideas()->monoEnergyControl() && IDEASBeamline::ideas()->monoEnergyControl()->isConnected() && !xasScanConfigurationView_){
//		double goodEnergy = 10 * floor(IDEASBeamline::ideas()->monoEnergyControl()->value() / 10);
		// Do New XAS
		IDEASXASScanConfiguration *xasScanConfiguration = new IDEASXASScanConfiguration(this);
//		xasScanConfiguration->xasRegions()->setEnergyControl(IDEASBeamline::ideas()->monoEnergyControl());
//		xasScanConfiguration->regions()->setDefaultTimeControl(IDEASBeamline::ideas()->masterDwellControl());
//                                xasScanConfiguration->addRegion(0, goodEnergy, 1, goodEnergy + 10, 1);


		xasScanConfigurationView_ = new IDEASXASScanConfigurationView(xasScanConfiguration);
		xasScanConfigurationView_->setupDefaultXANESScanRegions();
		xasScanConfigurationHolder3_->setView(xasScanConfigurationView_);
	}
}

void IDEASAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
}

void IDEASAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
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
	if (!exclusiveName.isNull())
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

//	if (spectraNames.contains("sumSpectra-1eland4el"))
//		editor->setSingleSpectrumViewDataSourceName("correctedRawSpectra-1el");

//	else if (spectraNames.contains("correctedSum-4el"))
//		editor->setSingleSpectrumViewDataSourceName("correctedSum-4el");

//	else if (spectraNames.contains("correctedRawSpectra-1el"))
//		editor->setSingleSpectrumViewDataSourceName("correctedRawSpectra-1el");

//	else if (!spectraNames.isEmpty())
	editor->setSingleSpectrumViewDataSourceName(spectraNames.first());

	editor->setPlotRange(AMPeriodicTable::table()->elementBySymbol("Al")->Kalpha().energy(), 20480);
}



//AMScan *scan = action->controller()->scan();
//openScanInEditor(scan, automaticBringScanEditorToFrontWithRunningScans());

//scanEditorScanMapping_.append(qMakePair(scan, scanEditorAt(scanEditorCount()-1)));
//connect(action, SIGNAL(stateChanged(int,int)), this, SLOT(updateScanEditorModelItem()));
//updateScanEditorModelItem();
