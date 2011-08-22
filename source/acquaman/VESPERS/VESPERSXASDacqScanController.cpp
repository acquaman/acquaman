#include "VESPERSXASDacqScanController.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "beamline/AMBeamlineActionsList.h"
#include "dataman/AMUser.h"
#include "analysis/AM1DExpressionAB.h"

#include <QDir>

VESPERSXASDacqScanController::VESPERSXASDacqScanController(VESPERSXASScanConfiguration *cfg, QObject *parent)
	: AMDacqScanController(cfg, parent)
{
	config_ = cfg;
	xasScan_ = new AMXASScan();
	xasScan_->setName(config_->name());
	xasScan_->setScanConfiguration(config_);
	xasScan_->setRunId(AMUser::user()->currentRunId());

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	xasScan_->rawData()->addMeasurement(AMMeasurementInfo(*(ionChambers->detectorAt((int)config_->incomingChoice())->toInfo())));
	xasScan_->addRawDataSource(new AMRawDataSource(xasScan_->rawData(), 0));
	xasScan_->rawData()->addMeasurement(AMMeasurementInfo(*(ionChambers->detectorAt((int)config_->transmissionChoice())->toInfo())));
	xasScan_->addRawDataSource(new AMRawDataSource(xasScan_->rawData(), 1));

	if (config_->fluorescenceDetectorChoice() == VESPERSXASScanConfiguration::SingleElement){

		XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF1E();

		// This is safe and okay because I always have the regions of interest set taking up 0-X where X is the count-1 of the number of regions of interest.
		for (int i = 0; i < detector->roiInfoList()->count(); i++){

			xasScan_->rawData()->addMeasurement(AMMeasurementInfo(detector->roiInfoList()->at(i).name().remove(" "), detector->roiInfoList()->at(i).name()));
			xasScan_->addRawDataSource(new AMRawDataSource(xasScan_->rawData(), i+2));
		}
	}
	else if (config_->fluorescenceDetectorChoice() == VESPERSXASScanConfiguration::FourElement){

		XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF4E();

		// This is safe and okay because I always have the regions of interest set taking up 0-X where X is the count-1 of the number of regions of interest.
		for (int i = 0; i < detector->roiInfoList()->count(); i++){

			xasScan_->rawData()->addMeasurement(AMMeasurementInfo(detector->roiInfoList()->at(i).name().remove(" "), detector->roiInfoList()->at(i).name()));
			xasScan_->addRawDataSource(new AMRawDataSource(xasScan_->rawData(), i+2));
		}
	}

	/// \todo add the analysis blocks.  Holding off until I know the raw data sources are working.
	/*AM1DExpressionAB* transmission = new AM1DExpressionAB("normalized");
	transmission->setDescription("Normalized Transmission");
	transmission->setInputDataSources(QList<AMDataSource *>() << xasScan_->rawDataSources()->at(0) << xasScan_->rawDataSources()->at(1));
	transmission->setExpression("ROI/IonChamberSplit2");

	xasScan_->addAnalyzedDataSource(transmission);*/
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

	return false; //return AMDacqScanController::startImplementation();
}

AMnDIndex VESPERSXASDacqScanController::toScanIndex(QMap<int, double> aeData)
{
	return AMnDIndex(xasScan_->rawData()->scanSize(0));
}

void VESPERSXASDacqScanController::onInitializationActionsSucceeded()
{

	setInitialized();
}

void VESPERSXASDacqScanController::onInitializationActionsFailed(int explanation)
{
	Q_UNUSED(explanation)

	setFailed();
}

void VESPERSXASDacqScanController::onInitializationActionsProgress(double elapsed, double total)
{
	Q_UNUSED(elapsed)
	Q_UNUSED(total)
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

	advAcq_->appendRecord("07B2_Mono_SineB_Egec:eV", true, false, 0);

	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->incomingChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->incomingChoice())->readMethod()));
	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->transmissionChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->transmissionChoice())->readMethod()));

	for (int i = 0; i < ionChambers->count(); i++)
		if (i != (int)config_->incomingChoice() || i != (int)config_->transmissionChoice())
			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	/// In order to mimic the current configs, I've hardcoded all the names so that the file matches the reference file.  These should and will be migrated to proper maps of detectors and controls names.
	// These will all likely change and be modified.
	advAcq_->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
	advAcq_->appendRecord("07B2_Mono_SineB_K", true, false, 0);
	advAcq_->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
	advAcq_->appendRecord("PCT1402-01:mA:fbk", true, false, 0);

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

	/// In order to mimic the current configs, I've hardcoded all the names so that the file matches the reference file.  These should and will be migrated to proper maps of detectors and controls names.
	// These will all likely change and be modified.
	advAcq_->appendRecord("07B2_Mono_SineB_Egec:eV", true, false, 0);

	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->incomingChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->incomingChoice())->readMethod()));
	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->transmissionChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->transmissionChoice())->readMethod()));

	int roiCount = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count();

	for (int i = 0; i < roiCount; i++)
		advAcq_->appendRecord("IOC1607-004:mca1.R"+QString::number(i), true, false, 1);

	for (int i = 0; i < ionChambers->count(); i++)
		if (i != (int)config_->incomingChoice() || i != (int)config_->transmissionChoice())
			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	advAcq_->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
	advAcq_->appendRecord("07B2_Mono_SineB_K", true, false, 0);
	advAcq_->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
	advAcq_->appendRecord("PCT1402-01:mA:fbk", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.DTIM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.ERTM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.ELTM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:dxp1.FAST_PEAKS", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:dxp1.SLOW_PEAKS", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.VAL", true, true, 1);

	// End of hardcoded.

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

	/// In order to mimic the current configs, I've hardcoded all the names so that the file matches the reference file.  These should and will be migrated to proper maps of detectors and controls names.
	// These will all likely change and be modified.
	advAcq_->appendRecord("07B2_Mono_SineB_Egec:eV", true, false, 0);

	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->incomingChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->incomingChoice())->readMethod()));
	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->transmissionChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->transmissionChoice())->readMethod()));

	int roiCount = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count();

	for (int i = 0; i < roiCount; i++)
		advAcq_->appendRecord("dxp1607-B21-04:mcaCorrected.R"+QString::number(i), true, false, 1);

	for (int i = 0; i < ionChambers->count(); i++)
		if (i != (int)config_->incomingChoice() || i != (int)config_->transmissionChoice())
			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	advAcq_->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
	advAcq_->appendRecord("07B2_Mono_SineB_K", true, false, 0);
	advAcq_->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
	advAcq_->appendRecord("PCT1402-01:mA:fbk", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca1.ERTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca2.ERTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca3.ERTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca4.ERTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca1.ELTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca2.ELTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca3.ELTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca4.ELTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:dxp1.FAST_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp2.FAST_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp3.FAST_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp4.FAST_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp1.SLOW_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp2.SLOW_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp3.SLOW_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp4.SLOW_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca1.DTIM", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca2.DTIM", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca3.DTIM", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca4.DTIM", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mcaCorrected.VAL", true, true, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca1.VAL", true, true, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca2.VAL", true, true, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca3.VAL", true, true, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca4.VAL", true, true, 1);

	// End of hardcored.

	return loadSuccess;
}
