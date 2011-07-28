#include "VESPERSXASDacqScanController.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "beamline/AMBeamlineActionsList.h"
#include "dataman/AMUser.h"

#include <QDir>

VESPERSXASDacqScanController::VESPERSXASDacqScanController(VESPERSXASScanConfiguration *cfg, QObject *parent)
	: AMDacqScanController(cfg, parent)
{
	config_ = cfg;
	xasScan_ = new AMXASScan();
	xasScan_->setName("Boobies!! For teh win!");
	xasScan_->setScanConfiguration(config_);
	xasScan_->setRunId(AMUser::user()->currentRunId());
}

bool VESPERSXASDacqScanController::initializeImplementation()
{
	setInitialized();
	return true;
}

bool VESPERSXASDacqScanController::startImplementation()
{
	switch(config_->fluorescenceDetectorChoice()){

	case VESPERSXASScanConfiguration::None:
		if (!setupTransmissionXAS()){

			AMErrorMon::report(AMErrorReport(this,
					AMErrorReport::Alert,
					VESPERSXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
					"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers."));
			return false;
		}
		break;

	case VESPERSXASScanConfiguration::SingleElement:
		if (!setupSingleElementXAS()){

			AMErrorMon::report(AMErrorReport(this,
					AMErrorReport::Alert,
					VESPERSXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
					"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers."));
			return false;
		}
		break;

	case VESPERSXASScanConfiguration::FourElement:
		if (!setupFourElementXAS()){

			AMErrorMon::report(AMErrorReport(this,
					AMErrorReport::Alert,
					VESPERSXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
					"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers."));
			return false;
		}
		break;

	default:
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				VESPERSXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS XAS DACQ Scan Controller failed to start (Invalid Fluorescence Detector chosen). Please report this bug to the Acquaman developers."));
		return false;
	}

	for (int i = 0; i < config_->regionCount(); i++){

		if (advAcq_->getNumRegions() == i)
			advAcq_->addRegion(i, config_->regionStart(i), config_->regionDelta(i), config_->regionEnd(i), 1);
		else {

			advAcq_->setStart(i, config_->regionStart(i));
			advAcq_->setDelta(i, config_->regionDelta(i));
			advAcq_->setEnd(i, config_->regionEnd(i));
		}
	}

	generalScan_ = xasScan_;

	advAcq_->saveConfigFile("/home/hunterd/beamline/programming/acquaman/devConfigurationFiles/VESPERS/writeTest.cfg");

	return false;//return AMDacqScanController::startImplementation();
}

AMnDIndex VESPERSXASDacqScanController::toScanIndex(QMap<int, double> aeData)
{
	return AMnDIndex();
}

void VESPERSXASDacqScanController::onInitializationActionsSucceeded()
{
	/// \todo how do we get here?  why is this here?
	setInitialized();
}

void VESPERSXASDacqScanController::onInitializationActionsFailed(int explanation)
{
	/// \todo how do we get here? why is this here?
	setFailed();
}

void VESPERSXASDacqScanController::onInitializationActionsProgress(double elapsed, double total)
{

}

QString VESPERSXASDacqScanController::getHomeDirectory()
{
	// Find out which path we are using for acquaman (depends on whether you are on Mac or Linux or beamline OPI).
	QString homeDir = QDir::homePath();
	if(QDir(homeDir+"/dev").exists())
		homeDir.append("/dev");
	else if(QDir(homeDir+"/beamline/programming").exists())
		homeDir.append("/beamline/programming");

	return homeDir;
}

bool VESPERSXASDacqScanController::setupTransmissionXAS()
{
	bool loadSuccess = false;

	loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/XANES.cfg"));

	if(!loadSuccess){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				VESPERSXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers."));
		return false;
	}

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	return loadSuccess;
}

bool VESPERSXASDacqScanController::setupSingleElementXAS()
{
	bool loadSuccess = false;

	loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/XANES-1Elem.cfg"));

	if(!loadSuccess){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				VESPERSXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers."));
		return false;
	}

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	return loadSuccess;
}

bool VESPERSXASDacqScanController::setupFourElementXAS()
{
	bool loadSuccess = false;

	loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/XANES-4Elem.cfg"));

	if(!loadSuccess){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				VESPERSXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers."));
		return false;
	}

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	return loadSuccess;
}
