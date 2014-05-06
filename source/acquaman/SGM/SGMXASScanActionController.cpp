#include "SGMXASScanActionController.h"

#include "dataman/AMXASScan.h"
#include "beamline/SGM/SGMBeamline.h"
#include "dataman/AMSample.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/AMListAction3.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "beamline/CLS/CLSSR570.h"

SGMXASScanActionController::SGMXASScanActionController(SGMXASScanConfiguration2013 *cfg, QObject *parent) :
	AMRegionScanActionController(cfg, parent)
{
	configuration_ = cfg;

	scan_ = new AMXASScan();
	scan_->setFileFormat("amRegionAscii2013");
	scan_->setScanConfiguration(cfg);
	//scan_->setSampleId(SGMBeamline::sgm()->currentSampleId());
	scan_->setSample(SGMBeamline::sgm()->currentSample());
	if(SGMBeamline::sgm()->currentSample())
		SGMBeamline::sgm()->currentSample()->addScan(scan_);
	connect(scan_, SIGNAL(storedToDb()), SGMBeamline::sgm()->currentSample(), SLOT(forceStoreToDb()));
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	QString scanName;
	QString sampleName;
	if(scan_->sampleId() == -1)
		sampleName = "Unknown Sample";
	else if(scan_->sample())
		sampleName = scan_->sample()->name();
	else
		sampleName = AMSample(scan_->sampleId(), AMUser::user()->database()).name();
	if(configuration_->userScanName() == ""){
		scanName = configuration_->autoScanName();
		scan_->setName(QString("%1 - %2").arg(sampleName).arg(scanName));
	}
	else{
		scanName = configuration_->userScanName();
		scan_->setName(QString("%1 - %2").arg(scanName).arg(sampleName));
	}
	scan_->setNotes(buildNotes());
}

SGMXASScanActionController::~SGMXASScanActionController(){}

void SGMXASScanActionController::buildScanControllerImplementation()
{
}

QString SGMXASScanActionController::buildNotes()
{
	QString notes;
	// Add X,Y,Z, Theta manip
	notes.append(QString("%1:\t%2\n").arg("Manipulator X").arg(SGMBeamline::sgm()->ssaManipulatorX()->value()));
	notes.append(QString("\n%1:\t%2\n").arg("Manipulator Y").arg(SGMBeamline::sgm()->ssaManipulatorY()->value()));
	notes.append(QString("\n%1:\t%2\n").arg("Manipulator Z").arg(SGMBeamline::sgm()->ssaManipulatorZ()->value()));
	notes.append(QString("\n%1:\t%2\n").arg("Manipulator Theta").arg(SGMBeamline::sgm()->ssaManipulatorRot()->value()));

	// Add Exit and Entrance Slits
	notes.append(QString("\n%1:\t%2\n").arg("Exit Slit Gap").arg(this->configuration_->exitSlitGap()));
	notes.append(QString("\n%1:\t%2\n").arg("Entrance Slit Gap").arg(SGMBeamline::sgm()->entranceSlitGap()->value()));


	// Add Grating
	notes.append(QString("\n%1:\t%2\n").arg("Grating").arg(SGMBeamlineInfo::sgmInfo()->sgmGratingDescription(configuration_->grating())));

	// Add Detector Info
	// Scaler (gets scalar from beamline, iterates through each channel, if channel has a sr570 connected then adds its name, value and the units)
	notes.append("Scaler:\n");
	CLSSIS3820Scaler* scaler = SGMBeamline::sgm()->scaler();
	for(int iChannel = 0; iChannel < scaler->channels().count(); iChannel++)
	{
		CLSSIS3820ScalerChannel* currentChannel = scaler->channelAt(iChannel);
		if(currentChannel->sr570() != 0)
		{
			notes.append(QString("\n%1:\t%2 %3\n").arg(currentChannel->customChannelName()).arg(currentChannel->sr570()->value()).arg(currentChannel->sr570()->units()));
		}
	}

	//Amptek (gets info for each AmptekSDD123 Detector from the beamline and adds its: Gain, Temperature, Pile-Upon Rejection, Peaking Time, Fast and Slow Thresholds)
	notes.append("\nAmptek detectors:\n");
	CLSAmptekSDD123DetectorNew* newAmptekSDD1 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD1());
	if(newAmptekSDD1 != 0)
	{
		notes.append("\nSDD1:\n");
		notes.append(QString("\n%1:\t%2").arg("Gain").arg(newAmptekSDD1->detectorTotalGain()));
		notes.append(QString("\n%1:\t%2").arg("Temperature").arg(newAmptekSDD1->detectorTemperature()));
		notes.append(QString("\n%1:\t%2").arg("Pile-upon Rejection").arg(newAmptekSDD1->pileUpRejection()));
		notes.append(QString("\n%1:\t%2").arg("Peaking Time").arg(newAmptekSDD1->peakingTime()));
		notes.append(QString("\n%1:\t%2").arg("Fast Threshold").arg(newAmptekSDD1->fastThreshold()));
		notes.append(QString("\n%1:\t%2").arg("Slow Threshold").arg(newAmptekSDD1->slowThreshold()));
	}

	CLSAmptekSDD123DetectorNew* newAmptekSDD2 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD2());
	if(newAmptekSDD2 != 0)
	{
		notes.append("\nSDD2:\n");
		notes.append(QString("\n%1:\t%2").arg("Gain").arg(newAmptekSDD2->detectorTotalGain()));
		notes.append(QString("\n%1:\t%2").arg("Temperature").arg(newAmptekSDD2->detectorTemperature()));
		notes.append(QString("\n%1:\t%2").arg("Pile-upon Rejection").arg(newAmptekSDD2->pileUpRejection()));
		notes.append(QString("\n%1:\t%2").arg("Peaking Time").arg(newAmptekSDD2->peakingTime()));
		notes.append(QString("\n%1:\t%2").arg("Fast Threshold").arg(newAmptekSDD2->fastThreshold()));
		notes.append(QString("\n%1:\t%2").arg("Slow Threshold").arg(newAmptekSDD2->slowThreshold()));
	}

	CLSAmptekSDD123DetectorNew* newAmptekSDD3 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD3());
	if(newAmptekSDD3 != 0)
	{
		notes.append("\nSDD3:\n");
		notes.append(QString("\n%1:\t%2").arg("Gain").arg(newAmptekSDD3->detectorTotalGain()));
		notes.append(QString("\n%1:\t%2").arg("Temperature").arg(newAmptekSDD3->detectorTemperature()));
		notes.append(QString("\n%1:\t%2").arg("Pile-upon Rejection").arg(newAmptekSDD3->pileUpRejection()));
		notes.append(QString("\n%1:\t%2").arg("Peaking Time").arg(newAmptekSDD3->peakingTime()));
		notes.append(QString("\n%1:\t%2").arg("Fast Threshold").arg(newAmptekSDD3->fastThreshold()));
		notes.append(QString("\n%1:\t%2").arg("Slow Threshold").arg(newAmptekSDD3->slowThreshold()));
	}

	CLSAmptekSDD123DetectorNew* newAmptekSDD4 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD4());
	if(newAmptekSDD4 != 0)
	{
		notes.append("\nSDD4:\n");
		notes.append(QString("\n%1:\t%2").arg("Gain").arg(newAmptekSDD4->detectorTotalGain()));
		notes.append(QString("\n%1:\t%2").arg("Temperature").arg(newAmptekSDD4->detectorTemperature()));
		notes.append(QString("\n%1:\t%2").arg("Pile-upon Rejection").arg(newAmptekSDD4->pileUpRejection()));
		notes.append(QString("\n%1:\t%2").arg("Peaking Time").arg(newAmptekSDD4->peakingTime()));
		notes.append(QString("\n%1:\t%2").arg("Fast Threshold").arg(newAmptekSDD4->fastThreshold()));
		notes.append(QString("\n%1:\t%2").arg("Slow Threshold").arg(newAmptekSDD4->slowThreshold()));
	}

	CLSAmptekSDD123DetectorNew* newAmptekSDD5 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD5());
	if(newAmptekSDD5 != 0)
	{
		notes.append("\nSDD5:\n");
		notes.append(QString("\n%1:\t%2").arg("Gain").arg(newAmptekSDD5->detectorTotalGain()));
		notes.append(QString("\n%1:\t%2").arg("Temperature").arg(newAmptekSDD5->detectorTemperature()));
		notes.append(QString("\n%1:\t%2").arg("Pile-upon Rejection").arg(newAmptekSDD5->pileUpRejection()));
		notes.append(QString("\n%1:\t%2").arg("Peaking Time").arg(newAmptekSDD5->peakingTime()));
		notes.append(QString("\n%1:\t%2").arg("Fast Threshold").arg(newAmptekSDD5->fastThreshold()));
		notes.append(QString("\n%1:\t%2").arg("Slow Threshold").arg(newAmptekSDD5->slowThreshold()));
	}

	return notes;
}

AMAction3* SGMXASScanActionController::createInitializationActions(){
	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Actions", "SGM XAS Initialization Actions"));


	AMListAction3 *initializationStage1 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 1", "SGM XAS Initialization Stage 1"), AMListAction3::Parallel);

	tmpControl = SGMBeamline::sgm()->grating();
	AMControlInfo gratingSetpoint = tmpControl->toInfo();
	gratingSetpoint.setValue(configuration_->fluxResolutionGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(gratingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage1->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->exitSlitGap();
	AMControlInfo exitSlitGapSetpoint = tmpControl->toInfo();
	exitSlitGapSetpoint.setValue(configuration_->fluxResolutionGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(exitSlitGapSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage1->addSubAction(moveAction);

	CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(SGMBeamline::sgm()->synchronizedDwellTime());
	QList<QString> allKeys;
	for(int x = 0; x < configuration_->detectorConfigurations().count(); x++)
		if(!allKeys.contains(SGMBeamline::sgm()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(x))->synchronizedDwellKey()))
			allKeys.append(SGMBeamline::sgm()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(x))->synchronizedDwellKey());

	for(int x = 0; x < syncDwell->elementCount(); x++){
		if(syncDwell->enabledAt(x) && !allKeys.contains(syncDwell->keyAt(x)) )
			initializationStage1->addSubAction(syncDwell->elementAt(x)->createEnableAction3(false));
		else if(!syncDwell->enabledAt(x) && allKeys.contains(syncDwell->keyAt(x)) )
			initializationStage1->addSubAction(syncDwell->elementAt(x)->createEnableAction3(true));
	}

	CLSAmptekSDD123DetectorNew *amptek1 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD1());
	if(amptek1)
		initializationStage1->addSubAction(amptek1->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD1()->name())));
	CLSAmptekSDD123DetectorNew *amptek2 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD2());
	if(amptek2)
		initializationStage1->addSubAction(amptek2->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD2()->name())));
	CLSAmptekSDD123DetectorNew *amptek3 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD3());
	if(amptek3)
		initializationStage1->addSubAction(amptek3->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD3()->name())));
	CLSAmptekSDD123DetectorNew *amptek4 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD4());
	if(amptek4)
		initializationStage1->addSubAction(amptek4->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD4()->name())));

	initializationStage1->addSubAction(SGMBeamline::sgm()->scaler()->createStartAction3(false));
	initializationStage1->addSubAction(SGMBeamline::sgm()->scaler()->createContinuousEnableAction3(false));


	AMListAction3 *initializationStage2 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 2", "SGM XAS Initialization Stage 2"), AMListAction3::Parallel);
	initializationStage2->addSubAction(SGMBeamline::sgm()->scaler()->createScansPerBufferAction3(1));
	initializationStage2->addSubAction(SGMBeamline::sgm()->scaler()->createTotalScansAction3(1));


	AMListAction3 *initializationStage3 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 3", "SGM XAS Initialization Stage 3"), AMListAction3::Parallel);

	tmpControl = SGMBeamline::sgm()->undulatorTracking();
	AMControlInfo undulatorTrackingSetpoint = tmpControl->toInfo();
	undulatorTrackingSetpoint.setValue(configuration_->trackingGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(undulatorTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	qDebug() << "Undulator tracking " << configuration_->trackingGroup().controlNamed(tmpControl->name()).value();

	tmpControl = SGMBeamline::sgm()->exitSlitTracking();
	AMControlInfo exitSlitTrackingSetpoint = tmpControl->toInfo();
	exitSlitTrackingSetpoint.setValue(configuration_->trackingGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(exitSlitTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	qDebug() << "Exitslit tracking " << configuration_->trackingGroup().controlNamed(tmpControl->name()).value();

	tmpControl = SGMBeamline::sgm()->monoTracking();
	AMControlInfo monoTrackingSetpoint = tmpControl->toInfo();
	monoTrackingSetpoint.setValue(configuration_->trackingGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(monoTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	qDebug() << "Mono tracking " << configuration_->trackingGroup().controlNamed(tmpControl->name()).value();

	//really? here?
	tmpControl = SGMBeamline::sgm()->harmonic();
	AMControlInfo harmonicSetpoint = tmpControl->toInfo();
	harmonicSetpoint.setValue(configuration_->fluxResolutionGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(harmonicSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	AMListAction3 *initializationStage4 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 4", "SGM XAS Initialization Stage 4"), AMListAction3::Parallel);
	initializationStage4->addSubAction(SGMBeamline::sgm()->createBeamOnActions3());

	initializationActions->addSubAction(initializationStage1);
	initializationActions->addSubAction(initializationStage2);
	initializationActions->addSubAction(initializationStage3);
	initializationActions->addSubAction(initializationStage4);

	return initializationActions;
}

AMAction3* SGMXASScanActionController::createCleanupActions(){
	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("SGM XAS Cleanup Actions", "SGM XAS Cleanup Actions"), AMListAction3::Parallel);

	CLSSIS3820Scaler *scaler = SGMBeamline::sgm()->scaler();
	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(scaler->isContinuous()));
	cleanupActions->addSubAction(scaler->createScansPerBufferAction3(scaler->scansPerBuffer()));
	cleanupActions->addSubAction(scaler->createTotalScansAction3(scaler->totalScans()));

	CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(SGMBeamline::sgm()->synchronizedDwellTime());
	for(int x = 0; x < syncDwell->elementCount(); x++)
		cleanupActions->addSubAction(syncDwell->elementAt(x)->createEnableAction3(syncDwell->enabledAt(x)));
	cleanupActions->addSubAction(syncDwell->createMasterTimeAction3(syncDwell->time()));

	CLSAmptekSDD123DetectorNew *amptek1 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD1());
	if(amptek1)
		cleanupActions->addSubAction(amptek1->createEnableAction3(amptek1->isEnabled()));
	CLSAmptekSDD123DetectorNew *amptek2 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD2());
	if(amptek2)
		cleanupActions->addSubAction(amptek2->createEnableAction3(amptek2->isEnabled()));
	CLSAmptekSDD123DetectorNew *amptek3 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD3());
	if(amptek3)
		cleanupActions->addSubAction(amptek3->createEnableAction3(amptek3->isEnabled()));
	CLSAmptekSDD123DetectorNew *amptek4 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD4());
	if(amptek4)
		cleanupActions->addSubAction(amptek4->createEnableAction3(amptek4->isEnabled()));

	AMControlInfo fastShutterSetpoint = SGMBeamline::sgm()->fastShutterVoltage()->toInfo();
	AMControlMoveAction3 *fastShutterAction = new AMControlMoveAction3(new AMControlMoveActionInfo3(fastShutterSetpoint), SGMBeamline::sgm()->fastShutterVoltage());
	cleanupActions->addSubAction(fastShutterAction);

	return cleanupActions;
}
