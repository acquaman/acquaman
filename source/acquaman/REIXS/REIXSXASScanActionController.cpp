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
			QString sampleName = AMSample::sampleNameForId(AMDatabase::database("user"), sampleId); // scan_->sampleName() won't work until the scan is saved to the database.
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
}

void REIXSXASScanActionController::onInitializationActionSucceeded(){

	AMControlInfoList positions(REIXSBeamline::bl()->exposedControls()->toInfoList());

	// add the spectrometer grating selection, since it's not a "control" anywhere.
	AMControlInfo grating("spectrometerGrating", REIXSBeamline::bl()->spectrometer()->specifiedGrating(), 0, 0, "[choice]", 0.1, "Spectrometer Grating");
	grating.setEnumString(REIXSBeamline::bl()->spectrometer()->spectrometerCalibration()->gratingAt(grating.value()).name());
	positions.insert(9, grating);

	// add the polarization selection, since it's not a "control" anywhere.
	AMControlInfo polarization("beamlinePolarization", REIXSBeamline::bl()->photonSource()->epuPolarization()->value(), 0, 0, "[choice]", 0.1, "EPU Polarization");
	polarization.setEnumString(REIXSBeamline::bl()->photonSource()->epuPolarization()->enumNameAt(REIXSBeamline::bl()->photonSource()->epuPolarization()->value()));
	positions.append(polarization);

	if(REIXSBeamline::bl()->photonSource()->epuPolarization()->value() == 5)
	{
	AMControlInfo polarizationAngle("beamlinePolarizationAngle", REIXSBeamline::bl()->photonSource()->epuPolarizationAngle()->value(), 0, 0, "degrees", 0.1, "EPU Polarization Angle");
	positions.append(polarizationAngle);
	}


	//scan_->scanInitialConditions()->setValuesFrom(positions);
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

	if(configuration_->applyPolarization()){
		tmpControl = REIXSBeamline::bl()->photonSource()->epuPolarization();
		AMControlInfo polarizationSetpoint = tmpControl->toInfo();
		polarizationSetpoint.setValue(configuration_->polarization());
		moveActionInfo = new AMControlMoveActionInfo3(polarizationSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationStage1->addSubAction(moveAction);

		/* NEEDS TO BE FIXED TO GO IN SEQUENCE BEFORE POLARIZATION ENUM IS CHOOSEN
		tmpControl = REIXSBeamline::bl()->photonSource()->epuPolarizationAngle();
		AMControlInfo polarizationAngleSetpoint = tmpControl->toInfo();
		polarizationAngleSetpoint.setValue(configuration_->polarizationAngle());
		moveActionInfo = new AMControlMoveActionInfo3(polarizationAngleSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationStage1->addSubAction(moveAction);
		*/
	}

	initializationStage1->addSubAction(REIXSBeamline::bl()->scaler()->createStartAction3(false));
	initializationStage1->addSubAction(REIXSBeamline::bl()->scaler()->createContinuousEnableAction3(false));


	AMListAction3 *initializationStage2 = new AMListAction3(new AMListActionInfo3("REIXS XAS Initialization Stage 2", "REIXS XAS Initialization Stage 2"), AMListAction3::Parallel);
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
	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("REIXS XAS Cleanup Actions", "REIXS XAS Cleanup Actions"), AMListAction3::Parallel);

	CLSSIS3820Scaler *scaler = REIXSBeamline::bl()->scaler();
	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(scaler->isContinuous()));
	cleanupActions->addSubAction(scaler->createScansPerBufferAction3(scaler->scansPerBuffer()));
	cleanupActions->addSubAction(scaler->createTotalScansAction3(scaler->totalScans()));

	return cleanupActions;
}