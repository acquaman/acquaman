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


#include "VESPERSEnergyDacqScanController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "actions/AMBeamlineActionsList.h"
#include "dataman/AMUser.h"
#include "actions/AMBeamlineParallelActionsList.h"
#include "util/VESPERS/VESPERSConfigurationFileBuilder.h"
#include "dataman/datastore/AMCDFDataStore.h"

#include <QDir>
#include <QStringBuilder>

VESPERSEnergyDacqScanController::VESPERSEnergyDacqScanController(VESPERSEnergyScanConfiguration *cfg, QObject *parent)
	: AMDacqScanController(cfg, parent)
{
	config_ = cfg;
	config_->setUserScanName(config_->name());

	setupXASAction_ = 0;
	cleanupXASAction_ = 0;

	secondsElapsed_ = 0;
	secondsTotal_ = config_->totalTime();
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));

	scan_ = new AMXASScan(); 	// MB: Moved from line 363 in startImplementation.
	scan_->setName(config_->name());
//	scan_->setFileFormat("vespers2012Energy");
	scan_->setScanConfiguration(config_);
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setIndexType("fileSystem");
	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime())+".cdf");
	scan_->setFileFormat("amCDFv1");
	scan_->replaceRawDataStore(new AMCDFDataStore(AMUserSettings::userDataFolder % scan_->filePath(), false));
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	QString notes;

	if (config_->ccdDetector() == VESPERSEnergyScanConfiguration::Roper)
		notes.append(QString("\nRoper CCD distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToRoperCCD(), 0, 'f', 1));

	switch(VESPERSBeamline::vespers()->currentBeam()){

	case VESPERSBeamline::None:
	case VESPERSBeamline::Pink:
		// This should never happen.
		break;

	case VESPERSBeamline::TenPercent:
		notes.append(QString("Beam used:\t10% bandpass\n"));
		break;

	case VESPERSBeamline::OnePointSixPercent:
		notes.append(QString("Beam used:\t1.6% bandpass\n"));
		break;

	case VESPERSBeamline::Si:
		notes.append(QString("Beam used:\tSi (%1E/E = 10^-4)\n").arg(QString::fromUtf8("Δ")));
		break;
	}

	notes.append(QString("Filter thickness (aluminum):\t%1 %2m\n").arg(VESPERSBeamline::vespers()->endstation()->filterThickness()).arg(QString::fromUtf8("μ")));
	notes.append(QString("Horizontal slit separation:\t%1 mm\n").arg(VESPERSBeamline::vespers()->intermediateSlits()->gapX()));
	notes.append(QString("Vertical slit separation:\t%1 mm\n").arg(VESPERSBeamline::vespers()->intermediateSlits()->gapZ()));
	notes.append(QString("Gas used in ion chambers:\tN2\n"));
	notes.append(QString("\nIon Chamber Gain Settings\n"));
	CLSSplitIonChamber *split = VESPERSBeamline::vespers()->iSplit();
	notes.append(QString("%1:\t%2 %3\n").arg(split->name()).arg(split->sensitivityValueA()).arg(split->sensitivityUnitsA()));
	CLSIonChamber *chamber = VESPERSBeamline::vespers()->iPreKB();
	notes.append(QString("%1:\t%2 %3\n").arg(chamber->name()).arg(chamber->sensitivityValue()).arg(chamber->sensitivityUnits()));
	chamber = VESPERSBeamline::vespers()->iMini();
	notes.append(QString("%1:\t%2 %3\n").arg(chamber->name()).arg(chamber->sensitivityValue()).arg(chamber->sensitivityUnits()));
	chamber = VESPERSBeamline::vespers()->iPost();
	notes.append(QString("%1:\t%2 %3\n").arg(chamber->name()).arg(chamber->sensitivityValue()).arg(chamber->sensitivityUnits()));

	scan_->setNotes(notes);

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();
	AMMeasurementInfo temp = AMMeasurementInfo("", "");

	for (int i = 0; i < ionChambers->count(); i++){

		temp = AMMeasurementInfo(*(ionChambers->detectorAt(i)->toInfo()));
		temp.name = ionChambers->detectorAt(i)->detectorName();
		scan_->rawData()->addMeasurement(temp);
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), true, false);
	}

	addExtraDatasources();
	useDwellTimes(VESPERSBeamline::vespers()->dwellTimeTrigger(), VESPERSBeamline::vespers()->dwellTimeConfirmed());
}

void VESPERSEnergyDacqScanController::addExtraDatasources()
{
	// Common to all three types are Ea, K, dwell time and ring current.
	scan_->rawData()->addMeasurement(AMMeasurementInfo("Ea", "Energy Setpoint", "eV"));
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	scan_->rawData()->addMeasurement(AMMeasurementInfo("DwellTime", "Dwell Time", "s"));
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	scan_->rawData()->addMeasurement(AMMeasurementInfo("RingCurrent", "Ring Current", "mA"));
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);

	if (config_->ccdDetector() == VESPERSEnergyScanConfiguration::Roper || config_->ccdDetector() == VESPERSEnergyScanConfiguration::Mar){

		scan_->rawData()->addMeasurement(AMMeasurementInfo("CCDFileNumber", "CCD file number"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	}
}

bool VESPERSEnergyDacqScanController::initializeImplementation()
{
	// To initialize the XAS scan, there are four stages.
	/*
		First: Enable/Disable all the pertinent detectors.  The scalar is ALWAYS enabled.
		Second: Set the mode to single shot,set the time on the synchronized dwell time, and set the variable integration time if an EXAFS scan.
		Third: Make sure the relative energy postion is back to zero and move the sample stage to the correct location (if enabled).
		Fourth: Move the mono to the correct energy.
	 */
	AMBeamlineParallelActionsList *setupXASActionsList = new AMBeamlineParallelActionsList;

	if (!setupXASAction_)
		onInitializationActionFinished();

	setupXASAction_ = new AMBeamlineListAction(setupXASActionsList);

	// First stage.
	setupXASActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Scalar
	setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(0)->createEnableAction(true));
	// Single element vortex
	setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(1)->createEnableAction(false));
	// CCD
	if (config_->ccdDetector() == VESPERSEnergyScanConfiguration::Roper)
		setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(2)->createEnableAction(true));
	else
		setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(2)->createEnableAction(false));
	// Picoammeters
	setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(3)->createEnableAction(false));
	// Four element vortex
	setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(4)->createEnableAction(false));
	// Mar CCD
	if (config_->ccdDetector() == VESPERSEnergyScanConfiguration::Mar)
		setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(5)->createEnableAction(true));
	else
		setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(5)->createEnableAction(false));

	// Second stage.
	setupXASActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	setupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createStartAction(false));
	setupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createScansPerBufferAction(1));
	setupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createTotalScansAction(1));
	setupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createModeAction(CLSSynchronizedDwellTime::SingleShot));
	setupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createMasterTimeAction(config_->regionTime(0)));

	// Third stage.
	if (config_->goToPosition() && VESPERSBeamline::vespers()->experimentConfiguration()->sampleStageChoice()){

		setupXASActionsList->appendStage(new QList<AMBeamlineActionItem *>());
		setupXASActionsList->appendAction(2, VESPERSBeamline::vespers()->pseudoSampleStage()->createHorizontalMoveAction(config_->x()));
		setupXASActionsList->appendAction(2, VESPERSBeamline::vespers()->pseudoSampleStage()->createVerticalMoveAction(config_->y()));
	}

	else if (config_->goToPosition() && !VESPERSBeamline::vespers()->experimentConfiguration()->sampleStageChoice()){

		setupXASActionsList->appendStage(new QList<AMBeamlineActionItem *>());
		setupXASActionsList->appendAction(2, VESPERSBeamline::vespers()->realSampleStage()->createHorizontalMoveAction(config_->x()));
		setupXASActionsList->appendAction(2, VESPERSBeamline::vespers()->realSampleStage()->createVerticalMoveAction(config_->y()));
	}

	// Integrity check.  Make sure no actions are null.
	for (int i = 0; i < setupXASActionsList->stageCount(); i++){

		for (int j = 0; j < setupXASActionsList->stage(i)->size(); j++){

			if (setupXASActionsList->action(i, j) == 0){

				onInitializationActionsFailed(0);
				return false;
			}
		}
	}

	connect(setupXASAction_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsSucceeded()));
	connect(setupXASAction_, SIGNAL(failed(int)), this, SLOT(onInitializationActionsFailed(int)));
	connect(setupXASAction_, SIGNAL(progress(double,double)), this, SLOT(onInitializationActionsProgress(double,double)));
	setupXASAction_->start();

	return true;
}

bool VESPERSEnergyDacqScanController::startImplementation()
{
	currentRegionIndex_ = 0;

	if (VESPERSBeamline::vespers()->experimentConfiguration()->sampleStageChoice()){

		scan_->scanInitialConditions()->append(VESPERSBeamline::vespers()->sampleStageHorizontal()->toInfo());
		scan_->scanInitialConditions()->append(VESPERSBeamline::vespers()->sampleStageVertical()->toInfo());
	}
	else{

		scan_->scanInitialConditions()->append(VESPERSBeamline::vespers()->sampleStageX()->toInfo());
		scan_->scanInitialConditions()->append(VESPERSBeamline::vespers()->sampleStageZ()->toInfo());
	}

	// Setup the real config.
	switch(config_->ccdDetector()){

	case VESPERSEnergyScanConfiguration::Roper:
		if (!setupRoperScan()){

			AMErrorMon::alert(this,
					VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
					"Error, VESPERS Energy DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
			return false;
		}
		break;

	case VESPERSEnergyScanConfiguration::Mar:
		if (!setupMarScan()){

			AMErrorMon::alert(this,
					VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
					"Error, VESPERS Energy DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
			return false;
		}
		break;

	default:
		AMErrorMon::alert(this,
				VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS EXAFS DACQ Scan Controller failed to start (Invalid Fluorescence Detector chosen). Please report this bug to the Acquaman developers.");
		return false;
	}

	for (int i = 0; i < config_->regionCount(); i++){

		if (advAcq_->getNumRegions() == i){

			AMPVwStatusControl *control = 0;

			control = qobject_cast<AMPVwStatusControl *>(config_->regions()->defaultControl());

			if (i == 0)
				advAcq_->addRegion(i, control->writePVName(), config_->regionStart(i), config_->regionDelta(i), config_->regionEnd(i), 1);
			else
				advAcq_->addRegion(i, control->writePVName(), config_->regionStart(i)+config_->regionDelta(i), config_->regionDelta(i), config_->regionEnd(i), 1);
		}

		else {

			if (i == 0)
				advAcq_->setStart(i, config_->regionStart(i));
			else
				advAcq_->setStart(i, config_->regionStart(i)+config_->regionDelta(i));

			advAcq_->setDelta(i, config_->regionDelta(i));
			advAcq_->setEnd(i, config_->regionEnd(i));
		}
	}
	advAcq_->saveConfigFile("/home/hunterd/Desktop/writeTest.cfg");

	return AMDacqScanController::startImplementation();
}

void VESPERSEnergyDacqScanController::cleanup()
{
	// To cleanup the XAS scan, there is one stage.
	/*
		First: Only have the scalar running in the syncrhonized dwell time.
		Second: Set the dwell time to 1 second.  Disables the variable integration time.  Set the relative energy PV to 0.
		Third: Set the scan mode to continuous.  This starts the synchronized dwell time.
	 */
	AMBeamlineParallelActionsList *cleanupXASActionsList = new AMBeamlineParallelActionsList;

	if (!cleanupXASAction_)
		onCleanupActionFinished();

	cleanupXASAction_ = new AMBeamlineListAction(cleanupXASActionsList);

	// First stage.
	cleanupXASActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Scalar
	cleanupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(0)->createEnableAction(true));
	// Single element vortex
	cleanupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(1)->createEnableAction(false));
	// CCD
	cleanupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(2)->createEnableAction(false));
	// Picoammeters
	cleanupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(3)->createEnableAction(false));
	// Four element vortex
	cleanupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(4)->createEnableAction(false));
	// Mar CCD
	cleanupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(5)->createEnableAction(false));

	// Second stage.
	cleanupXASActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Synchronized dwell time.
	cleanupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createMasterTimeAction(1.0));

	// Third stage.
	cleanupXASActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	// Start the synchronized dwell time.
	cleanupXASActionsList->appendAction(2, VESPERSBeamline::vespers()->synchronizedDwellTime()->createModeAction(CLSSynchronizedDwellTime::Continuous));

	connect(cleanupXASAction_, SIGNAL(succeeded()), this, SLOT(onCleanupFinished()));
	connect(cleanupXASAction_, SIGNAL(failed(int)), this, SLOT(onCleanupFinished()));
	cleanupXASAction_->start();
}

void VESPERSEnergyDacqScanController::onCleanupFinished()
{
	AMDacqScanController::onDacqStop();
}

void VESPERSEnergyDacqScanController::onDwellTimeTriggerChanged(double newValue)
{
	if( fabs(newValue - 1.0) < 0.1){

		VESPERSBeamline::vespers()->synchronizedDwellTime()->setTime(config_->regionTime(currentRegionIndex_));
		currentRegionIndex_++;
		dwellTimeTrigger_->move(0);
		dwellTimeConfirmed_->move(1);
	}
}

AMnDIndex VESPERSEnergyDacqScanController::toScanIndex(QMap<int, double> aeData)
{
	Q_UNUSED(aeData)
	return AMnDIndex(scan_->rawData()->scanSize(0));
}

void VESPERSEnergyDacqScanController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}

void VESPERSEnergyDacqScanController::onInitializationActionsSucceeded()
{
	setInitialized();
}

void VESPERSEnergyDacqScanController::onInitializationActionsFailed(int explanation)
{
	Q_UNUSED(explanation)

	AMErrorMon::alert(this, 0, "Energy scan failed to initialize.");
	onInitializationActionFinished();
	setFailed();
}

void VESPERSEnergyDacqScanController::onInitializationActionsProgress(double elapsed, double total)
{
	Q_UNUSED(elapsed)
	Q_UNUSED(total)
}

QString VESPERSEnergyDacqScanController::getHomeDirectory()
{
	// Find out which path we are using for acquaman (depends on whether you are on Mac or Linux or beamline OPI).
	QString homeDir = QDir::homePath();
	if(QDir(homeDir+"/dev").exists())
		homeDir.append("/dev");
	else if(QDir(homeDir+"/beamline/programming").exists())
		homeDir.append("/beamline/programming");

	return homeDir;
}

bool VESPERSEnergyDacqScanController::setupRoperScan()
{
	VESPERSConfigurationFileBuilder builder;
	builder.setDimensions(1);
	builder.setRoperCCD(true);
	builder.setPvNameAxis1("07B2_Mono_SineB_Ea");
	builder.buildConfigurationFile();

	bool loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/EnergyScan-Roper.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(1)){}

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	/// In order to mimic the current configs, I've hardcoded all the names so that the file matches the reference file.  These should and will be migrated to proper maps of detectors and controls names.
	// These will all likely change and be modified.
	advAcq_->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
	advAcq_->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
	advAcq_->appendRecord("PCT1402-01:mA:fbk", true, false, 0);
	advAcq_->appendRecord("IOC1607-003:det1:FileNumber", true, false, 0);

	return loadSuccess;
}

bool VESPERSEnergyDacqScanController::setupMarScan()
{
	VESPERSConfigurationFileBuilder builder;
	builder.setDimensions(1);
	builder.setMarCCD(true);
	builder.setPvNameAxis1("07B2_Mono_SineB_Ea");
	builder.buildConfigurationFile();

	bool loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(1)){}

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	/// In order to mimic the current configs, I've hardcoded all the names so that the file matches the reference file.  These should and will be migrated to proper maps of detectors and controls names.
	// These will all likely change and be modified.
	advAcq_->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
	advAcq_->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
	advAcq_->appendRecord("PCT1402-01:mA:fbk", true, false, 0);
	advAcq_->appendRecord("ccd1607-002:cam1:FileNumber", true, false, 0);

	return loadSuccess;
}

void VESPERSEnergyDacqScanController::onInitializationActionFinished()
{
	if (setupXASAction_ == 0)
		return;

	// Disconnect all signals and return all memory.
	setupXASAction_->disconnect();
	AMBeamlineParallelActionsList *actionList = setupXASAction_->list()	;

	for (int i = 0; i < actionList->stageCount(); i++){

		while (actionList->stage(i)->size())
			actionList->stage(i)->takeAt(0)->deleteLater();
	}

	setupXASAction_->deleteLater();
	setupXASAction_ = 0;
}

void VESPERSEnergyDacqScanController::onCleanupActionFinished()
{
	if (cleanupXASAction_ == 0)
		return;

	// Disconnect all signals and return all memory.
	cleanupXASAction_->disconnect();
	AMBeamlineParallelActionsList *actionList = cleanupXASAction_->list()	;

	for (int i = 0; i < actionList->stageCount(); i++){

		while (actionList->stage(i)->size())
			actionList->stage(i)->takeAt(0)->deleteLater();
	}

	cleanupXASAction_->deleteLater();
	cleanupXASAction_ = 0;
}
