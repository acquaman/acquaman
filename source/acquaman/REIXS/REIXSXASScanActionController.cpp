#include "REIXSXASScanActionController.h"

#include "dataman/AMXASScan.h"
#include "beamline/REIXS/REIXSBeamline.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMWaitAction.h"

#include "dataman/AMSamplePre2013.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DNormalizationAB.h"
#include "analysis/AM1DCalibrationAB.h"

#include "beamline/CLS/CLSSIS3820Scaler.h"

REIXSXASScanActionController::~REIXSXASScanActionController(){}

REIXSXASScanActionController::REIXSXASScanActionController(REIXSXASScanConfiguration *cfg, QObject *parent) :
	AMStepScanActionController(cfg, parent)
{
	fileWriterIsBusy_ = false;
	configuration_ = cfg;

	scan_ = new AMXASScan();
	scan_->setFileFormat("amRegionAscii2013");
	scan_->setScanConfiguration(cfg);
	scan_->setName("REIXS XAS Scan");
	scan_->setSampleId(REIXSBeamline::bl()->currentSampleId());
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	QString rangeString;
	if(configuration_->scanAxisAt(0)->regionCount())
		rangeString = QString("%1-%2 eV").arg(double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart())).arg(double(configuration_->scanAxisAt(0)->regionAt(configuration_->scanAxisAt(0)->regionCount()-1)->regionEnd()));

	if(configuration_->namedAutomatically()) {
		int sampleId = REIXSBeamline::bl()->currentSampleId();
		if(sampleId >= 1) {
			scan_->setSampleId(sampleId);
			//QString sampleName = AMSample::sampleNameForId(AMDatabase::database("user"), sampleId); // scan_->sampleName() won't work until the scan is saved to the database.
			//AMSamplePre2013 *currentSample = new AMSamplePre2013(sampleId, AMDatabase::database("user"));
			QString sampleName = scan_->sampleName();
			scan_->setName(QString("%1 %2 %3").arg(sampleName).arg(configuration_->autoScanName()).arg(rangeString));
		}
		else {
			scan_->setName(QString("%1 %2").arg(configuration_->autoScanName()).arg(rangeString));
			scan_->setSampleId(-1);
		}
	}
	else {
		scan_->setName(configuration_->userScanName());
		if(scan_->name().isEmpty())
			scan_->setName(QString("%1 %2").arg(configuration_->autoScanName()).arg(rangeString));
		scan_->setSampleId(configuration_->sampleId());
	}

	AMControlInfoList list;
	list.append(REIXSBeamline::bl()->photonSource()->energy()->toInfo());
	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet reixsDetectors;
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("I0")->toInfo());
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("TEY")->toInfo());
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("TFY")->toInfo());
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("PFY")->toInfo());
	configuration_->setDetectorConfigurations(reixsDetectors);

	connect(this, SIGNAL(initialized()), this, SLOT(onInitializationActionSucceeded()));


	secondsElapsed_ = 0;
	secondsTotal_ = configuration_->totalTime(true);
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));

}

void REIXSXASScanActionController::onInitializationActionSucceeded(){

	AMControlInfoList positions;

	positions.append(REIXSBeamline::bl()->photonSource()->energy()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->userEnergyOffset()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->monoSlit()->toInfo());
	positions.append(REIXSBeamline::bl()->sampleChamber()->x()->toInfo());
	positions.append(REIXSBeamline::bl()->sampleChamber()->y()->toInfo());
	positions.append(REIXSBeamline::bl()->sampleChamber()->z()->toInfo());
	positions.append(REIXSBeamline::bl()->sampleChamber()->r()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->gratingMask()->toInfo());  //D
	positions.append(REIXSBeamline::bl()->spectrometer()->toInfo());
	// add the polarization selection, since it's not a "control" anywhere.
	AMControlInfo polarization("beamlinePolarization", REIXSBeamline::bl()->photonSource()->epuPolarization()->value(), 0, 0, "[choice]", 0.1, "EPU Polarization");
	polarization.setEnumString(REIXSBeamline::bl()->photonSource()->epuPolarization()->enumNameAt(REIXSBeamline::bl()->photonSource()->epuPolarization()->value()));
	positions.append(polarization);
		if(REIXSBeamline::bl()->photonSource()->epuPolarization()->value() == 5)
		{
			AMControlInfo polarizationAngle("beamlinePolarizationAngle", REIXSBeamline::bl()->photonSource()->epuPolarizationAngle()->value(), 0, 0, "degrees", 0.1, "EPU Polarization Angle");
			positions.append(polarizationAngle);
		}
	positions.append(REIXSBeamline::bl()->photonSource()->monoGratingSelector()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->monoMirrorSelector()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->spectrometerRotationDrive()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->detectorTranslation()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->detectorTiltDrive()->toInfo());
	// add the spectrometer grating selection, since it's not a "control" anywhere.
	AMControlInfo grating("spectrometerGrating", REIXSBeamline::bl()->spectrometer()->specifiedGrating(), 0, 0, "[choice]", 0.1, "Spectrometer Grating");
	grating.setEnumString(REIXSBeamline::bl()->spectrometer()->spectrometerCalibration()->gratingAt(grating.value()).name());
	positions.append(grating);
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->x()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->y()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->z()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->u()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->v()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->w()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->r()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->s()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->t()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->endstationTranslation()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->M5Pitch()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->M5Yaw()->toInfo());

	positions.append(REIXSBeamline::bl()->spectrometer()->tmSOE()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->tmMCPPreamp()->toInfo());
	positions.append(REIXSBeamline::bl()->sampleChamber()->tmSample()->toInfo());


//	AMControlInfoList positions(REIXSBeamline::bl()->exposedControls()->toInfoList());

//	// add the spectrometer grating selection, since it's not a "control" anywhere.
//	AMControlInfo grating("spectrometerGrating", REIXSBeamline::bl()->spectrometer()->specifiedGrating(), 0, 0, "[choice]", 0.1, "Spectrometer Grating");
//	grating.setEnumString(REIXSBeamline::bl()->spectrometer()->spectrometerCalibration()->gratingAt(grating.value()).name());
//	positions.insert(9, grating);

//	// add the polarization selection, since it's not a "control" anywhere.
//	AMControlInfo polarization("beamlinePolarization", REIXSBeamline::bl()->photonSource()->epuPolarization()->value(), 0, 0, "[choice]", 0.1, "EPU Polarization");
//	polarization.setEnumString(REIXSBeamline::bl()->photonSource()->epuPolarization()->enumNameAt(REIXSBeamline::bl()->photonSource()->epuPolarization()->value()));
//	positions.append(polarization);

//	if(REIXSBeamline::bl()->photonSource()->epuPolarization()->value() == 5)
//	{
//	AMControlInfo polarizationAngle("beamlinePolarizationAngle", REIXSBeamline::bl()->photonSource()->epuPolarizationAngle()->value(), 0, 0, "degrees", 0.1, "EPU Polarization Angle");
//	positions.append(polarizationAngle);
//	}


////	AMControlInfo SOEtemp("SOETemp", REIXSBeamline::bl()->spectrometer()->tmSOE(), 0, 0, "C", 0.1, "SOE Temperature");
////	positions.append(SOEtemp);
////	AMControlInfo MCPtemp("MCPtemp", REIXSBeamline::bl()->spectrometer()->tmMCPPreamp(), 0, 0, "C", 0.1, "MCP Preamp Temperature");
////	positions.append(MCPtemp);
////	AMControlInfo Sampletemp("Sampletemp", REIXSBeamline::bl()->sampleChamber()->tmSample(), 0, 0, "C", 0.1, "Sample Temperature");
////	positions.append(Sampletemp);
//	positions.append(REIXSBeamline::bl()->spectrometer()->tmSOE()->toInfo());
//	positions.append(REIXSBeamline::bl()->spectrometer()->tmMCPPreamp()->toInfo());
//	positions.append(REIXSBeamline::bl()->sampleChamber()->tmSample()->toInfo());

//	//scan_->scanInitialConditions()->setValuesFrom(positions);
	scan_->setScanInitialConditions(positions);
}

void REIXSXASScanActionController::buildScanControllerImplementation()
{
	QList<AMDataSource*> raw1DDataSources;
	for(int i=0; i<scan_->rawDataSources()->count(); i++)
		if(scan_->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << scan_->rawDataSources()->at(i);

	int rawTeyIndex = scan_->rawDataSources()->indexOfKey("TEY");
	int rawTfyIndex = scan_->rawDataSources()->indexOfKey("TFY");
	int rawPfyIndex = scan_->rawDataSources()->indexOfKey("PFY");
	int rawI0Index = scan_->rawDataSources()->indexOfKey("I0");

	if(rawTeyIndex != -1 && rawI0Index != -1) {
		AM1DCalibrationAB* teyChannel = new AM1DCalibrationAB("TEYNorm");
		teyChannel->setDescription("Normalized TEY");
		teyChannel->setInputDataSources(raw1DDataSources);
		teyChannel->setDataName("TEY");
		teyChannel->setNormalizationName("I0");

		scan_->addAnalyzedDataSource(teyChannel);
	}

	if(rawTfyIndex != -1 && rawI0Index != -1) {
		AM1DCalibrationAB* tfyChannel = new AM1DCalibrationAB("TFYNorm");
		tfyChannel->setDescription("Normalized TFY");
		tfyChannel->setInputDataSources(raw1DDataSources);
		tfyChannel->setDataName("TFY");
		tfyChannel->setNormalizationName("I0");

		scan_->addAnalyzedDataSource(tfyChannel);
	}

	if(rawPfyIndex != -1 && rawI0Index != -1) {
		AM1DCalibrationAB* pfyChannel = new AM1DCalibrationAB("PFYNorm");
		pfyChannel->setDescription("Normalized PFY");
		pfyChannel->setInputDataSources(raw1DDataSources);
		pfyChannel->setDataName("PFY");
		pfyChannel->setNormalizationName("I0");


		scan_->addAnalyzedDataSource(pfyChannel);
	}
}

AMAction3* REIXSXASScanActionController::createInitializationActions(){

	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("REIXS XAS Initialization Actions", "REIXS XAS Initialization Actions"));


	AMListAction3 *initializationStage1 = new AMListAction3(new AMListActionInfo3("REIXS XAS Initialization Stage 1", "REIXS XAS Initialization Stage 1"), AMListAction3::Parallel);

	if(configuration_->applySlitWidth()){
		tmpControl = REIXSBeamline::bl()->photonSource()->monoSlit();
		AMControlInfo monoSlitSetpoint = tmpControl->toInfo();
		monoSlitSetpoint.setValue(configuration_->slitWidth());
		moveActionInfo = new AMControlMoveActionInfo3(monoSlitSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationStage1->addSubAction(moveAction);
	}

	if(configuration_->applyPolarization() && configuration_->polarization() == 5 && configuration_->polarizationAngle() != REIXSBeamline::bl()->photonSource()->epuPolarizationAngle()->value()){
		tmpControl = REIXSBeamline::bl()->photonSource()->epuPolarizationAngle();
		AMControlInfo polarizationAngleSetpoint = tmpControl->toInfo();
		polarizationAngleSetpoint.setValue(configuration_->polarizationAngle());
		moveActionInfo = new AMControlMoveActionInfo3(polarizationAngleSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationStage1->addSubAction(moveAction);

	}

	initializationStage1->addSubAction(REIXSBeamline::bl()->scaler()->createStartAction3(false));
	initializationStage1->addSubAction(REIXSBeamline::bl()->scaler()->createContinuousEnableAction3(false));





	AMListAction3 *initializationStage2 = new AMListAction3(new AMListActionInfo3("REIXS XAS Initialization Stage 2", "REIXS XAS Initialization Stage 2"), AMListAction3::Parallel);

	if(configuration_->applyPolarization() && REIXSBeamline::bl()->photonSource()->epuPolarization()->value() != configuration_->polarization()){
		tmpControl = REIXSBeamline::bl()->photonSource()->epuPolarization();
		AMControlInfo polarizationSetpoint = tmpControl->toInfo();
		polarizationSetpoint.setValue(configuration_->polarization());
		moveActionInfo = new AMControlMoveActionInfo3(polarizationSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationStage2->addSubAction(moveAction);
	}

	initializationStage2->addSubAction(REIXSBeamline::bl()->scaler()->createScansPerBufferAction3(1));
	initializationStage2->addSubAction(REIXSBeamline::bl()->scaler()->createTotalScansAction3(1));

	initializationActions->addSubAction(initializationStage1);
	initializationActions->addSubAction(initializationStage2);

	return initializationActions;
}

AMAction3* REIXSXASScanActionController::createCleanupActions(){
	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("REIXS XAS Cleanup Actions", "REIXS XAS Cleanup Actions"));//, AMListAction3::Parallel); sequentially set up scaler

	CLSSIS3820Scaler *scaler = REIXSBeamline::bl()->scaler();
//	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(scaler->isConti/*nuous()));
//	cleanupActions->addSubAction(scaler->createScansPerBufferAction3(scaler->scansPerBuffer()));
//	cleanupActions->addSubAction(scaler->createTotalScansAction3(scaler->totalScans()));
//	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(true));
//	cleanupActions->addSubAction(scaler->createScansPerBufferAction3(1));
//	cleanupActions->addSubAction(scaler->createTotalScansAction3(0));
	cleanupActions->addSubAction(scaler->createStartAction3(false));
	cleanupActions->addSubAction(new AMWaitAction(new AMWaitActionInfo(1.0)));
	cleanupActions->addSubAction(scaler->createDwellTimeAction3(1.0));
	cleanupActions->addSubAction(new AMWaitAction(new AMWaitActionInfo(1.0)));
	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(true));

	return cleanupActions;
}

void REIXSXASScanActionController::cancelImplementation(){

	AMStepScanActionController::cancelImplementation();

	AMAction3 *cleanupActions = createCleanupActions();
	cleanupActions->start();
	//setCancelled();  //should be handled by AMStepScanActionController::cancelImplementation();

}



void REIXSXASScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		//emit timeRemaining(secondsTotal_-secondsElapsed_); Not in IDEAS controller?
		//qDebug() << "scan timer updated to" << secondsElapsed_ << "of" << secondsTotal_;
		emit progress(secondsElapsed_, secondsTotal_);
	}
}
