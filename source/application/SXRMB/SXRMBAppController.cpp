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


#include "SXRMBAppController.h"

#include "beamline/SXRMB/SXRMBBeamline.h"
#include "application/SXRMB/SXRMB.h"

#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "acquaman/SXRMB/SXRMBEXAFSScanConfiguration.h"

#include "application/AMAppControllerSupport.h"

#include "dataman/AMScan.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/AMSMAKExporter.h"
#include "dataman/AMRun.h"
#include "dataman/SXRMB/SXRMBUserConfiguration.h"
#include "dataman/AMRegionOfInterest.h"

#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/SXRMB/SXRMBXRFDetailedDetectorView.h"
#include "ui/SXRMB/SXRMBPersistentView.h"
#include "ui/SXRMB/SXRMBEXAFSScanConfigurationView.h"
#include "ui/SXRMB/SXRMB2DMapScanConfigurationView.h"
#include "ui/SXRMB/SXRMB2DOxidationMapScanConfigurationView.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "ui/SXRMB/SXRMBChooseDataFolderDialog.h"
#include "ui/acquaman/SXRMB/SXRMBOxidationMapScanConfigurationViewHolder.h"

#include "util/AMPeriodicTable.h"

#include <QMessageBox>

SXRMBAppController::SXRMBAppController(QObject *parent)
	: AMAppController(parent)
{
	userConfiguration_ = 0;
	moveImmediatelyAction_ = 0;
}

bool SXRMBAppController::startup()
{
	// Get a destination folder.
	if (!SXRMBChooseDataFolderDialog::getDataFolder())
		return false;

	// Start up the main program.
	if(AMAppController::startup()) {


		// Initialize central beamline object
		SXRMBBeamline::sxrmb();
		// Initialize the periodic table object.
		AMPeriodicTable::table();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)){

			AMRun firstRun("SXRMB", 9);	/// \todo For now, we know that 7 is the ID of the BioXAS main endstation facility, but this is a hardcoded hack.
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		setupExporterOptions();
		setupUserInterface();
		makeConnections();

		return true;
	}
	else
		return false;
}

void SXRMBAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void SXRMBAppController::onBeamlineConnected(bool connected)
{
	if (connected && !exafsScanConfigurationView_) {
		exafsScanConfiguration_ = new SXRMBEXAFSScanConfiguration();

		exafsScanConfiguration_->setNormalPosition(SXRMBBeamline::sxrmb()->microprobeSampleStageY()->value());
		exafsScanConfiguration_->setMicroprobeSampleStageX(SXRMBBeamline::sxrmb()->microprobeSampleStageX()->value());
		exafsScanConfiguration_->setMicroprobeSampleStageZ(SXRMBBeamline::sxrmb()->microprobeSampleStageZ()->value());

		exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(-11);
		exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.5);
		exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(9);
		exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);

		exafsScanConfigurationView_ = new SXRMBEXAFSScanConfigurationView(exafsScanConfiguration_);
		exafsScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3(exafsScanConfigurationView_);

		mw_->addPane(exafsScanConfigurationViewHolder_, "Scans", "EXAFS Scan", ":/utilites-system-monitor.png");
	}

	if (connected && !microProbe2DScanConfigurationView_) {
		microProbe2DScanConfiguration_ = new SXRMB2DMapScanConfiguration();

		microProbe2DScanConfiguration_->setExcitationEnergy(SXRMBBeamline::sxrmb()->energy()->value());

		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0.0);
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.01);
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(0.1);

		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(0.0);
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.01);
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(0.1);

		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(1.0);

		microProbe2DScanConfigurationView_ = new SXRMB2DMapScanConfigurationView(microProbe2DScanConfiguration_);
		microProbe2DScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3(microProbe2DScanConfigurationView_);

		mw_->addPane(microProbe2DScanConfigurationViewHolder_, "Scans", "2D Scan", ":/utilites-system-monitor.png");
	}

	if (connected && !microProbe2DOxidationScanConfigurationView_) {
		microProbe2DOxidationScanConfiguration_ = new SXRMB2DMapScanConfiguration();
		microProbe2DOxidationScanConfiguration_->setName("Oxidation Map");
		microProbe2DOxidationScanConfiguration_->setUserScanName("Oxidation Map");

		microProbe2DScanConfiguration_->setExcitationEnergy(SXRMBBeamline::sxrmb()->energy()->value());

		microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0.0);
		microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.01);
		microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(0.1);

		microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(0.0);
		microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.01);
		microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(0.1);

		microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);
		microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(1.0);

		microProbe2DOxidationScanConfigurationView_ = new SXRMB2DOxidationMapScanConfigurationView(microProbe2DOxidationScanConfiguration_);
		microProbe2DOxidationScanConfigurationViewHolder_ = new SXRMBOxidationMapScanConfigurationViewHolder(microProbe2DOxidationScanConfigurationView_);

		mw_->addPane(microProbe2DOxidationScanConfigurationViewHolder_, "Scans", "Oxidation Map", ":/utilites-system-monitor.png");
	}

	if (connected && !sxrmbPersistentView_){

		sxrmbPersistentView_ = new SXRMBPersistentView();
		mw_->addRightWidget(sxrmbPersistentView_);
	}

	if (connected && !userConfiguration_){
		qDebug() << "Going to set user configuration for the first time";
		userConfiguration_ = new SXRMBUserConfiguration(this);

		// It is sufficient to only connect the user configuration to the single element because the single element and four element are synchronized together.
		connect(userConfiguration_, SIGNAL(loadedFromDb()), this, SLOT(onUserConfigurationLoadedFromDb()));

		if (!userConfiguration_->loadFromDb(AMDatabase::database("user"), 1)){
			userConfiguration_->storeToDb(AMDatabase::database("user"));

			AMDetector *detector = SXRMBBeamline::sxrmb()->brukerDetector();
			// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
			connect(detector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
			connect(detector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
		}
	}
}

void SXRMBAppController::onScalerConnected(bool isConnected){
	if(isConnected && SXRMBBeamline::sxrmb()->isConnected()){
		if(!scalerView_){

			scalerView_ = new CLSSIS3820ScalerView(SXRMBBeamline::sxrmb()->scaler(), false);
			scalerView_->setAmplifierViewFormat('g');
			scalerView_->setAmplifierViewPrecision(3);
		}

		QGroupBox *scalerGroupBox = new QGroupBox;
		scalerGroupBox->setFlat(true);

		QHBoxLayout *scalerHorizontalSqueezeLayout = new QHBoxLayout;
		scalerHorizontalSqueezeLayout->addStretch();
		scalerHorizontalSqueezeLayout->addWidget(scalerView_);
		scalerHorizontalSqueezeLayout->addStretch();

		QVBoxLayout *scalerVerticalSqueezeLayout = new QVBoxLayout;
		scalerVerticalSqueezeLayout->addWidget(new AMTopFrame("Scaler"));
		scalerVerticalSqueezeLayout->addStretch();
		scalerVerticalSqueezeLayout->addLayout(scalerHorizontalSqueezeLayout);
		scalerVerticalSqueezeLayout->addStretch();
		scalerGroupBox->setLayout(scalerVerticalSqueezeLayout);
		mw_->addPane(scalerGroupBox, "Detectors", "Scaler", ":/system-search.png", true);
	}
	else if(scalerView_)
		mw_->removePane(scalerView_);
}

void SXRMBAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<SXRMBScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<SXRMBEXAFSScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SXRMB2DMapScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SXRMBUserConfiguration>();
}

void SXRMBAppController::setupExporterOptions()
{
	AMExporterOptionGeneralAscii *sxrmbExportOptions = SXRMB::buildStandardExporterOption("SXRMBXASDefault", true, true, true, true);
	if(sxrmbExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<SXRMBEXAFSScanConfiguration, AMExporterAthena, AMExporterOptionGeneralAscii>(sxrmbExportOptions->id());

	sxrmbExportOptions = SXRMB::buildSMAKExporterOption("SXRMB2DDefault", true, false, false, true);
	if(sxrmbExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<SXRMB2DMapScanConfiguration, AMSMAKExporter, AMExporterOptionSMAK>(sxrmbExportOptions->id());
}

void SXRMBAppController::setupUserInterface()
{
	exafsScanConfiguration_ = 0; //NULL
	exafsScanConfigurationView_ = 0; //NULL
	exafsScanConfigurationViewHolder_ = 0; //NULL

	microProbe2DScanConfiguration_ = 0; //NULL
	microProbe2DScanConfigurationView_ = 0; //NULL
	microProbe2DScanConfigurationViewHolder_ = 0; //NULL

	microProbe2DOxidationScanConfiguration_ = 0; //NULL
	microProbe2DOxidationScanConfigurationView_ = 0; //NULL
	microProbe2DOxidationScanConfigurationViewHolder_ = 0; //NULL

	scalerView_ = 0; //NULL
	sxrmbPersistentView_ = 0;


	// Create panes in the main window:
	////////////////////////////////////


	mw_->insertHeading("General", 0);
	mw_->insertHeading("Detectors", 1);

	SXRMBXRFDetailedDetectorView *brukerView = new SXRMBXRFDetailedDetectorView(SXRMBBeamline::sxrmb()->brukerDetector());
	brukerView->buildDetectorView();
	brukerView->setEnergyRange(1700, 10000);
	brukerView->addEmissionLineNameFilter(QRegExp("1"));
	brukerView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	brukerView->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
	brukerView->enableDeadTimeDisplay();

	mw_->addPane(brukerView, "Detectors", "Bruker", ":/system-search.png");

	mw_->insertHeading("Scans", 2);

	connect(SXRMBBeamline::sxrmb(), SIGNAL(connected(bool)), this, SLOT(onBeamlineConnected(bool)));
	connect(SXRMBBeamline::sxrmb()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected(bool)));

	if(SXRMBBeamline::sxrmb()->isConnected()){
		onBeamlineConnected(true);
		if(SXRMBBeamline::sxrmb()->scaler()->isConnected())
			onScalerConnected(true);
	}
}

void SXRMBAppController::makeConnections()
{
	connect(this, SIGNAL(scanEditorCreated(AMGenericScanEditor*)), this, SLOT(onScanEditorCreated(AMGenericScanEditor*)));
}

void SXRMBAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
	userConfiguration_->storeToDb(AMDatabase::database("user"));
}

void SXRMBAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)
	userConfiguration_->storeToDb(AMDatabase::database("user"));
}

void SXRMBAppController::onUserConfigurationLoadedFromDb()
{
	AMXRFDetector *detector = SXRMBBeamline::sxrmb()->brukerDetector();

	foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){
		detector->addRegionOfInterest(region->createCopy());
		microProbe2DScanConfiguration_->addRegionOfInterest(region);
		exafsScanConfiguration_->addRegionOfInterest(region);
		microProbe2DOxidationScanConfiguration_->addRegionOfInterest(region);
	}

	// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
	connect(detector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
	connect(detector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
}

void SXRMBAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	userConfiguration_->addRegionOfInterest(region);
	microProbe2DScanConfiguration_->addRegionOfInterest(region);
	exafsScanConfiguration_->addRegionOfInterest(region);
	microProbe2DOxidationScanConfiguration_->addRegionOfInterest(region);
}

void SXRMBAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	userConfiguration_->removeRegionOfInterest(region);
	microProbe2DScanConfiguration_->removeRegionOfInterest(region);
	exafsScanConfiguration_->removeRegionOfInterest(region);
	microProbe2DOxidationScanConfiguration_->removeRegionOfInterest(region);
}

void SXRMBAppController::onScanEditorCreated(AMGenericScanEditor *editor)
{
	connect(editor, SIGNAL(scanAdded(AMGenericScanEditor*,AMScan*)), this, SLOT(onScanAddedToEditor(AMGenericScanEditor*,AMScan*)));
	editor->setPlotRange(1700, 10000);

	if (editor->using2DScanView())
		connect(editor, SIGNAL(dataPositionChanged(AMGenericScanEditor*,QPoint)), this, SLOT(onDataPositionChanged(AMGenericScanEditor*,QPoint)));
}

void SXRMBAppController::onScanAddedToEditor(AMGenericScanEditor *editor, AMScan *scan)
{
	QString exclusiveName = QString();

	for (int i = 0, count = scan->analyzedDataSourceCount(); i < count && exclusiveName.isNull(); i++){

		AMDataSource *source = scan->analyzedDataSources()->at(i);

		if (source->name().contains("norm_") && !source->name().contains("norm_PFY") && !source->hiddenFromUsers())
			exclusiveName = source->name();
	}

	if (!exclusiveName.isNull())
		editor->setExclusiveDataSourceByName(exclusiveName);

	else if (editor->scanAt(0)->analyzedDataSourceCount())
		editor->setExclusiveDataSourceByName(editor->scanAt(0)->analyzedDataSources()->at(editor->scanAt(0)->analyzedDataSourceCount()-1)->name());

	configureSingleSpectrumView(editor, scan);
}

void SXRMBAppController::configureSingleSpectrumView(AMGenericScanEditor *editor, AMScan *scan)
{
	int scanRank = scan->scanRank();
	QStringList spectraNames;

	for (int i = 0, size = scan->dataSourceCount(); i < size; i++)
		if (scan->dataSourceAt(i)->rank()-scanRank == 1)
			spectraNames << scan->dataSourceAt(i)->name();

	if (!spectraNames.isEmpty())
		editor->setSingleSpectrumViewDataSourceName(spectraNames.first());

	editor->setPlotRange(1700, 10000);
}

void SXRMBAppController::onDataPositionChanged(AMGenericScanEditor *editor, const QPoint &pos)
{
	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	SXRMB2DMapScanConfiguration *config = qobject_cast<SXRMB2DMapScanConfiguration *>(editor->currentScan()->scanConfiguration());

	if (!config)
		return;

	QString text;

	text = QString("Setup at (H,V,N): (%1 mm, %2 mm, %3 mm)")
			.arg(editor->dataPosition().x(), 0, 'f', 3)
			.arg(editor->dataPosition().y(), 0, 'f', 3)
			.arg(config->normalPosition());

	QMenu popup(text, editor);
	QAction *temp = popup.addAction(text);
	popup.addSeparator();
	popup.addAction("XANES scan");
	popup.addAction("EXAFS scan");
	popup.addSeparator();
	popup.addAction("Go to immediately");
	popup.addSeparator();
	temp = popup.addAction("2D XRF Scan");
	temp->setDisabled(editor->selectedRect().isNull());

	temp = popup.exec(pos);

	if (temp){

		if (temp->text() == "XANES scan")
			setupXASScan(editor, false);

		else if (temp->text() == "EXAFS scan")
			setupXASScan(editor, true);

		else if (temp->text() == "Go to immediately")
			moveImmediately(editor);

		else if (temp->text() == "2D XRF Scan")
			setup2DXRFScan(editor);
	}
}

void SXRMBAppController::moveImmediately(const AMGenericScanEditor *editor)
{
	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	SXRMB2DMapScanConfiguration *config = qobject_cast<SXRMB2DMapScanConfiguration *>(editor->currentScan()->scanConfiguration());

	if (!config)
		return;

	moveImmediatelyAction_ = new AMListAction3(new AMListActionInfo3("Move immediately", "Moves sample stage to given coordinates."), AMListAction3::Sequential);
	moveImmediatelyAction_->addSubAction(SXRMBBeamline::sxrmb()->microprobeSampleStageMotorGroupObject()->createHorizontalMoveAction(editor->dataPosition().x()));
	moveImmediatelyAction_->addSubAction(SXRMBBeamline::sxrmb()->microprobeSampleStageMotorGroupObject()->createVerticalMoveAction(editor->dataPosition().y()));
	moveImmediatelyAction_->addSubAction(SXRMBBeamline::sxrmb()->microprobeSampleStageMotorGroupObject()->createNormalMoveAction(config->normalPosition()));

	connect(moveImmediatelyAction_, SIGNAL(succeeded()), this, SLOT(onMoveImmediatelySuccess()));
	connect(moveImmediatelyAction_, SIGNAL(failed()), this, SLOT(onMoveImmediatelyFailure()));
	moveImmediatelyAction_->start();
}

void SXRMBAppController::onMoveImmediatelySuccess()
{
	cleanMoveImmediatelyAction();
}

void SXRMBAppController::onMoveImmediatelyFailure()
{
	cleanMoveImmediatelyAction();
	QMessageBox::warning(mw_, "Sample Stage Error", "The sample stage was unable to complete the desired movement.");
}

void SXRMBAppController::cleanMoveImmediatelyAction()
{
	if (moveImmediatelyAction_ == 0)
		return;

	// Disconnect all signals and return all memory.
	moveImmediatelyAction_->disconnect();
	moveImmediatelyAction_->deleteLater();
	moveImmediatelyAction_ = 0;
}

void SXRMBAppController::setupXASScan(const AMGenericScanEditor *editor, bool setupEXAFS)
{
	// THIS NEEDS REAL COORDINATES.

	QString edge = editor->exclusiveDataSourceName();
	edge = edge.remove("norm_");
	edge.chop(2);
	edge.insert(edge.size()-1, " ");

	if (edge.at(edge.size()-1) == 'L')
		edge.append("1");

	exafsScanConfiguration_->setEdge(edge);
	exafsScanConfiguration_->setMicroprobeSampleStageX(editor->dataPosition().x());
	exafsScanConfiguration_->setMicroprobeSampleStageZ(editor->dataPosition().y());

	qDebug() << "Config " << exafsScanConfiguration_->microprobeSampleStageX() << exafsScanConfiguration_->microprobeSampleStageZ() << " editor " << editor->dataPosition().x() << editor->dataPosition().y();

	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	SXRMB2DMapScanConfiguration *configuration = qobject_cast<SXRMB2DMapScanConfiguration *>(editor->currentScan()->scanConfiguration());
	if (configuration){

		exafsScanConfiguration_->setName(configuration->name());
		exafsScanConfiguration_->setNormalPosition(configuration->normalPosition());
	}

	if (setupEXAFS)
		exafsScanConfigurationView_->setupDefaultEXAFSScanRegions();

	else
		exafsScanConfigurationView_->setupDefaultXANESScanRegions();

	mw_->undock(exafsScanConfigurationViewHolder_);
}

void SXRMBAppController::setup2DXRFScan(const AMGenericScanEditor *editor)
{
	QRectF mapRect = editor->selectedRect();

	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	SXRMB2DMapScanConfiguration *config = qobject_cast<SXRMB2DMapScanConfiguration *>(editor->currentScan()->scanConfiguration());

	if (config){

		microProbe2DScanConfiguration_->setName(config->name());
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(mapRect.left());
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.01);
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(mapRect.right());
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(config->scanAxisAt(0)->regionAt(0)->regionTime());
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(mapRect.bottom());
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.01);
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(mapRect.top());
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(config->scanAxisAt(1)->regionAt(0)->regionTime());
		microProbe2DScanConfiguration_->setNormalPosition(config->normalPosition());
		microProbe2DScanConfigurationView_->updateMapInfo();
	}

	mw_->undock(microProbe2DScanConfigurationViewHolder_);
}
