#include "REIXSXASScanActionController.h"

#include "dataman/AMXASScan.h"
#include "beamline/REIXS/REIXSBeamline.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"

#include "dataman/AMSample.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DNormalizationAB.h"
#include "analysis/AM1DCalibrationAB.h"

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "dataman/AMSample.h"

 REIXSXASScanActionController::~REIXSXASScanActionController(){}
REIXSXASScanActionController::REIXSXASScanActionController(REIXSXASScanConfiguration *cfg, QObject *parent) :
	AMRegionScanActionController(cfg, parent)
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
	if(configuration_->regionCount())
		rangeString = QString("%1-%2 eV").arg(configuration_->regionStart(0)).arg(configuration_->regionEnd(configuration_->regionCount()-1));

	if(configuration_->namedAutomatically()) {
		int sampleId = REIXSBeamline::bl()->currentSampleId();
		if(sampleId >= 1) {
			scan_->setSampleId(sampleId);
			//QString sampleName = AMSample::sampleNameForId(AMDatabase::database("user"), sampleId); // scan_->sampleName() won't work until the scan is saved to the database.
			AMSample *currentSample = new AMSample(sampleId, AMDatabase::database("user"));
			QString sampleName = currentSample->name();
			scan_->setName(QString("%1 %2 %3").arg(sampleName).arg(configuration_->autoScanName()).arg(rangeString));
			scan_->setNumber(scan_->largestNumberInScansWhere(AMDatabase::database("user"), QString("sampleId = %1").arg(sampleId))+1);
		}
		else {
			scan_->setName(QString("%1 %2").arg(configuration_->autoScanName()).arg(rangeString));
			scan_->setNumber(0);
			scan_->setSampleId(-1);
		}
	}
	else {
		scan_->setName(configuration_->userScanName());
		if(scan_->name().isEmpty())
			scan_->setName(QString("%1 %2").arg(configuration_->autoScanName()).arg(rangeString));
		scan_->setNumber(configuration_->scanNumber());
		scan_->setSampleId(configuration_->sampleId());
	}

	AMDetectorInfoSet reixsDetectors;
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("I0")->toInfo());
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("TEY")->toInfo());
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("TFY")->toInfo());
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("PFY")->toInfo());
	configuration_->setDetectorConfigurations(reixsDetectors);

	connect(this, SIGNAL(initialized()), this, SLOT(onInitializationActionSucceeded()));


	secondsElapsed_ = 0;
	secondsTotal_ = 1;
	for (int i = 0; i < configuration_->regions()->count(); i++)
		secondsTotal_ += ((configuration_->regions()->end(i) - configuration_->regions()->start(i))/configuration_->regions()->delta(i))*(configuration_->regions()->time(i) + 0.2);
	qDebug() << "Seconds Total is:" << secondsTotal_;
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





	AMListAction3 *initializationStage3 = new AMListAction3(new AMListActionInfo3("REIXS XAS Initialization Stage 3", "REIXS XAS Initialization Stage 3"), AMListAction3::Parallel);

	tmpControl = REIXSBeamline::bl()->photonSource()->energy();
	AMControlInfo initialEnergySetpoint = tmpControl->toInfo();
	initialEnergySetpoint.setValue(configuration_->startEnergy());
	moveActionInfo = new AMControlMoveActionInfo3(initialEnergySetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);




	initializationActions->addSubAction(initializationStage1);
	initializationActions->addSubAction(initializationStage2);
	initializationActions->addSubAction(initializationStage3);

	return initializationActions;
}

AMAction3* REIXSXASScanActionController::createCleanupActions(){
	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("REIXS XAS Cleanup Actions", "REIXS XAS Cleanup Actions"));//, AMListAction3::Parallel); sequentially set up scaler

	CLSSIS3820Scaler *scaler = REIXSBeamline::bl()->scaler();
//	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(scaler->isConti/*nuous()));
//	cleanupActions->addSubAction(scaler->createScansPerBufferAction3(scaler->scansPerBuffer()));
//	cleanupActions->addSubAction(scaler->createTotalScansAction3(scaler->totalScans()));
//	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(true));
	cleanupActions->addSubAction(scaler->createScansPerBufferAction3(1));
	cleanupActions->addSubAction(scaler->createTotalScansAction3(0));
	cleanupActions->addSubAction(scaler->createStartAction3(true));

	return cleanupActions;
}


void REIXSXASScanActionController::onScanTimerUpdate()
{

	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;
		qDebug() << secondsElapsed_ << " elasped of " << secondsTotal_ ;
		emit timeRemaining(secondsTotal_-secondsElapsed_);
		emit progress(secondsElapsed_, secondsTotal_);
	}
}
