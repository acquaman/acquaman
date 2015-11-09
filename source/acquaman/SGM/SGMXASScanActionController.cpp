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


#include "SGMXASScanActionController.h"

#include <QApplication>

#include "dataman/AMXASScan.h"
#include "beamline/SGM/SGMBeamline.h"
#include "dataman/AMSample.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/AMListAction3.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "beamline/AMCurrentAmplifier.h"

#include "actions3/AMActionSupport.h"

SGMXASScanActionController::SGMXASScanActionController(SGMXASScanConfiguration2013 *cfg, QObject *parent) :
	AMStepScanActionController(cfg, parent)
{
	configuration_ = cfg;

	scan_ = new AMXASScan();
	scan_->setFileFormat("amCDFv1");
	scan_->setScanConfiguration(cfg);
	scan_->setSample(SGMBeamline::sgm()->currentSample());
	if(SGMBeamline::sgm()->currentSample())
		SGMBeamline::sgm()->currentSample()->addScan(scan_);
	connect(scan_, SIGNAL(storedToDb()), SGMBeamline::sgm()->currentSample(), SLOT(forceStoreToDb()));
	scan_->setIndexType("fileSystem");

	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));
	AMControlInfoList list;
	list.append(SGMBeamline::sgm()->energy()->toInfo());
	configuration_->setAxisControlInfos(list);

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

	/* Dark current correction tests
	int dwellTimeIndex = scan_->indexOfDataSource(SGMBeamline::sgm()->dwellTimeDetector()->name());

	if (dwellTimeIndex != -1) {
		AMDataSource* dwellTimeSource = scan_->dataSourceAt(dwellTimeIndex);

		for (int i = 0, size = regionsConfiguration_->detectorConfigurations().count(); i < size; i++){
			AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(regionsConfiguration_->detectorConfigurations().at(i));

			if (detector) {
				int detectorIndex = scan_->indexOfDataSource(detector->name());

				if (detectorIndex != -1 && detector->rank() == 0 && detector->canDoDarkCurrentCorrection()) {

					AMDataSource* detectorSource = scan_->dataSourceAt(detectorIndex);

					AM1DDarkCurrentCorrectionAB *detectorCorrection = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(detector->name()));
					detectorCorrection->setDescription(QString("%1 Dark Current Correction").arg(detector->name()));
					detectorCorrection->setDataName(detectorSource->name());
					detectorCorrection->setDwellTimeName(dwellTimeSource->name());
					detectorCorrection->setInputDataSources(QList<AMDataSource*>() << detectorSource << dwellTimeSource);
					connect( detector, SIGNAL(newDarkCurrentMeasurementValueReady(double)), detectorCorrection, SLOT(setDarkCurrent(double)) );
					detectorCorrection->setTimeUnitMultiplier(.001);
					scan_->addAnalyzedDataSource(detectorCorrection, true, false);
				}
			}
		}
	}
	*/
}

QString SGMXASScanActionController::buildNotes()
{
	QString notes;
	// Add X,Y,Z, Theta manip
	notes.append(QString("%1:\t%2\n").arg("Manipulator X").arg(SGMBeamline::sgm()->ssaManipulatorX()->value()));
	notes.append(QString("%1:\t%2\n").arg("Manipulator Y").arg(SGMBeamline::sgm()->ssaManipulatorY()->value()));
	notes.append(QString("%1:\t%2\n").arg("Manipulator Z").arg(SGMBeamline::sgm()->ssaManipulatorZ()->value()));
	notes.append(QString("%1:\t%2\n").arg("Manipulator Theta").arg(SGMBeamline::sgm()->ssaManipulatorRot()->value()));

	// Add Exit and Entrance Slits
	notes.append(QString("%1:\t%2\n").arg("Exit Slit Gap").arg(this->configuration_->exitSlitGap()));
	notes.append(QString("%1:\t%2\n").arg("Entrance Slit Gap").arg(SGMBeamline::sgm()->entranceSlitGap()->value()));


	// Add Grating
	notes.append(QString("%1:\t%2\n").arg("Grating").arg(SGMBeamlineInfo::sgmInfo()->sgmGratingDescription(configuration_->grating())));

	// Add Detector Info
	// Scaler (gets scalar from beamline, iterates through each channel, if channel has a sr570 connected then adds its name, value and the units)
	notes.append("Scaler:\n");
	CLSSIS3820Scaler* scaler = SGMBeamline::sgm()->scaler();
	for(int iChannel = 0; iChannel < scaler->channels().count(); iChannel++)
	{
		CLSSIS3820ScalerChannel* currentChannel = scaler->channelAt(iChannel);
		if(currentChannel->currentAmplifier() != 0)
		{
			AMCurrentAmplifier *channelSR570 = currentChannel->currentAmplifier();
			if(channelSR570)
				notes.append(QString("%1:\t%2 %3\n").arg(currentChannel->customChannelName()).arg(channelSR570->value()).arg(channelSR570->units()));
		}
	}

	//Amptek (gets info for each AmptekSDD123 Detector from the beamline and adds its: Gain, Temperature, Pile-Upon Rejection, Peaking Time, Fast and Slow Thresholds)
	notes.append("Amptek detectors:\n");
	CLSAmptekSDD123DetectorNew* newAmptekSDD1 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD1());
	if(newAmptekSDD1 != 0)
	{
		notes.append("SDD1:\n");
		notes.append(QString("%1:\t%2\n").arg("Gain").arg(newAmptekSDD1->detectorTotalGain()));
		notes.append(QString("%1:\t%2\n").arg("Temperature").arg(newAmptekSDD1->detectorTemperature()));
		notes.append(QString("%1:\t%2\n").arg("Pile-upon Rejection").arg(newAmptekSDD1->pileUpRejection()));
		notes.append(QString("%1:\t%2\n").arg("Peaking Time").arg(newAmptekSDD1->peakingTime()));
		notes.append(QString("%1:\t%2\n").arg("Fast Threshold").arg(newAmptekSDD1->fastThreshold()));
		notes.append(QString("%1:\t%2\n").arg("Slow Threshold").arg(newAmptekSDD1->slowThreshold()));
	}

	CLSAmptekSDD123DetectorNew* newAmptekSDD2 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD2());
	if(newAmptekSDD2 != 0)
	{
		notes.append("SDD2:\n");
		notes.append(QString("%1:\t%2\n").arg("Gain").arg(newAmptekSDD2->detectorTotalGain()));
		notes.append(QString("%1:\t%2\n").arg("Temperature").arg(newAmptekSDD2->detectorTemperature()));
		notes.append(QString("%1:\t%2\n").arg("Pile-upon Rejection").arg(newAmptekSDD2->pileUpRejection()));
		notes.append(QString("%1:\t%2\n").arg("Peaking Time").arg(newAmptekSDD2->peakingTime()));
		notes.append(QString("%1:\t%2\n").arg("Fast Threshold").arg(newAmptekSDD2->fastThreshold()));
		notes.append(QString("%1:\t%2\n").arg("Slow Threshold").arg(newAmptekSDD2->slowThreshold()));
	}

	CLSAmptekSDD123DetectorNew* newAmptekSDD3 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD3());
	if(newAmptekSDD3 != 0)
	{
		notes.append("SDD3:\n");
		notes.append(QString("%1:\t%2\n").arg("Gain").arg(newAmptekSDD3->detectorTotalGain()));
		notes.append(QString("%1:\t%2\n").arg("Temperature").arg(newAmptekSDD3->detectorTemperature()));
		notes.append(QString("%1:\t%2\n").arg("Pile-upon Rejection").arg(newAmptekSDD3->pileUpRejection()));
		notes.append(QString("%1:\t%2\n").arg("Peaking Time").arg(newAmptekSDD3->peakingTime()));
		notes.append(QString("%1:\t%2\n").arg("Fast Threshold").arg(newAmptekSDD3->fastThreshold()));
		notes.append(QString("%1:\t%2\n").arg("Slow Threshold").arg(newAmptekSDD3->slowThreshold()));
	}

	CLSAmptekSDD123DetectorNew* newAmptekSDD4 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD4());
	if(newAmptekSDD4 != 0)
	{
		notes.append("SDD4:\n");
		notes.append(QString("%1:\t%2\n").arg("Gain").arg(newAmptekSDD4->detectorTotalGain()));
		notes.append(QString("%1:\t%2\n").arg("Temperature").arg(newAmptekSDD4->detectorTemperature()));
		notes.append(QString("%1:\t%2\n").arg("Pile-upon Rejection").arg(newAmptekSDD4->pileUpRejection()));
		notes.append(QString("%1:\t%2\n").arg("Peaking Time").arg(newAmptekSDD4->peakingTime()));
		notes.append(QString("%1:\t%2\n").arg("Fast Threshold").arg(newAmptekSDD4->fastThreshold()));
		notes.append(QString("%1:\t%2\n").arg("Slow Threshold").arg(newAmptekSDD4->slowThreshold()));
	}

	CLSAmptekSDD123DetectorNew* newAmptekSDD5 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD5());
	if(newAmptekSDD5 != 0)
	{
		notes.append("SDD5:\n");
		notes.append(QString("%1:\t%2\n").arg("Gain").arg(newAmptekSDD5->detectorTotalGain()));
		notes.append(QString("%1:\t%2\n").arg("Temperature").arg(newAmptekSDD5->detectorTemperature()));
		notes.append(QString("%1:\t%2\n").arg("Pile-upon Rejection").arg(newAmptekSDD5->pileUpRejection()));
		notes.append(QString("%1:\t%2\n").arg("Peaking Time").arg(newAmptekSDD5->peakingTime()));
		notes.append(QString("%1:\t%2\n").arg("Fast Threshold").arg(newAmptekSDD5->fastThreshold()));
		notes.append(QString("%1:\t%2\n").arg("Slow Threshold").arg(newAmptekSDD5->slowThreshold()));
	}

	return notes;
}

AMAction3* SGMXASScanActionController::createInitializationActions(){
	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Actions", "SGM XAS Initialization Actions"));

	AMListAction3 *initializationStage1 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 1", "SGM XAS Initialization Stage 1"), AMListAction3::Parallel);

	AMAction3 *gratingAction = AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->grating(), configuration_->fluxResolutionGroup().controlNamed(SGMBeamline::sgm()->grating()->name()).value());
	initializationStage1->addSubAction(gratingAction);

	AMAction3 *exitSlipGapAction = AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->exitSlitGap(), configuration_->fluxResolutionGroup().controlNamed(SGMBeamline::sgm()->exitSlitGap()->name()).value());
	initializationStage1->addSubAction(exitSlipGapAction);

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

	AMAction3 *undulatorTrackingAction = AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorTracking(), configuration_->trackingGroup().controlNamed(SGMBeamline::sgm()->undulatorTracking()->name()).value());
	initializationStage3->addSubAction(undulatorTrackingAction);

	AMAction3 *exitSlitAction = AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->exitSlitTracking(), configuration_->trackingGroup().controlNamed(SGMBeamline::sgm()->exitSlitTracking()->name()).value());
	initializationStage3->addSubAction(exitSlitAction);

	AMAction3 *monoTrackingAction = AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->monoTracking(), configuration_->trackingGroup().controlNamed(SGMBeamline::sgm()->monoTracking()->name()).value());
	initializationStage3->addSubAction(monoTrackingAction);

	//really? here?

	AMAction3 *harmonicAction = AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->harmonic(), configuration_->fluxResolutionGroup().controlNamed(SGMBeamline::sgm()->harmonic()->name()).value());
	initializationStage3->addSubAction(harmonicAction);

	/* Dark current correction testing
	AMListAction3* initializationStage4 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 4", "SGM XAS Initialization Stage 4"), AMListAction3::Sequential);

	for (int i = 0, size = regionsConfiguration_->detectorConfigurations().count(); i < size; i++){
		AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(regionsConfiguration_->detectorConfigurations().at(i));

		bool sharedSourceFound = false;

		if (detector) {
			int detectorIndex = scan_->indexOfDataSource(detector->name());

			if (detectorIndex != -1 && detector->rank() == 0 && detector->canDoDarkCurrentCorrection()) {
				bool isSourceShared = detector->sharesDetectorTriggerSource();

				if (isSourceShared && !sharedSourceFound) {
					sharedSourceFound = true;
					initializationStage4->addSubAction(detector->createDarkCurrentCorrectionActions(10));

				} else if (!isSourceShared) {
					initializationStage4->addSubAction(detector->createDarkCurrentCorrectionActions(10));

				}
			}
		}
	}
	*/

	initializationActions->addSubAction(initializationStage1);
	initializationActions->addSubAction(initializationStage2);
	initializationActions->addSubAction(initializationStage3);
	/* Dark current correction testing
	initializationActions->addSubAction(initializationStage4);
	*/

	QStringList applicationArguments = QApplication::instance()->arguments();
	if(!applicationArguments.contains("--enableTesting")){
		AMListAction3 *initializationStage5 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 5", "SGM XAS Initialization Stage 5"), AMListAction3::Parallel);
		initializationStage5->addSubAction(SGMBeamline::sgm()->createBeamOnActions3());

		initializationActions->addSubAction(initializationStage5);
	}

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

//	qDebug() << "Will go back to " << syncDwell->time();

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
