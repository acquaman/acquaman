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
	bool loadSuccess = false;
	QString homeDir = QDir::homePath();
	if( QDir(homeDir+"/dev").exists())
		homeDir.append("/dev");
	else if( QDir(homeDir+"/beamline/programming").exists())
		homeDir.append("/beamline/programming");

	loadSuccess = advAcq_->setConfigFile(homeDir.append("/acquaman/devConfigurationFiles/VESPERS/XANES.cfg"));

	if(!loadSuccess){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				VESPERSXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers."));
		return false;
	}

	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(VESPERSBeamline::vespers()->iMini()->detectorName()), true, false, detectorReadMethodToDacqReadMethod(VESPERSBeamline::vespers()->iMini()->readMethod()));
	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(VESPERSBeamline::vespers()->iPost()->detectorName()), true, false, detectorReadMethodToDacqReadMethod(VESPERSBeamline::vespers()->iPost()->readMethod()));

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

	return AMDacqScanController::startImplementation();
}

AMnDIndex VESPERSXASDacqScanController::toScanIndex(QMap<int, double> aeData)
{
	return AMnDIndex();
}

void VESPERSXASDacqScanController::onInitializationActionsSucceeded()
{

}

void VESPERSXASDacqScanController::onInitializationActionsFailed(int explanation)
{

}

void VESPERSXASDacqScanController::onInitializationActionsProgress(double elapsed, double total)
{

}
