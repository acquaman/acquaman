#include "VESPERSScanController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
//#include "acquaman/dacq3_3/qepicsadvacq.h"
#include "actions3/AMListAction3.h"
#include "beamline/CLS/CLSSR570.h"

#include <QStringBuilder>


VESPERSScanController::~VESPERSScanController(){}

VESPERSScanController::VESPERSScanController(VESPERSScanConfiguration *configuration)
{
	config_ = configuration;
}

AMAction3 *VESPERSScanController::buildBaseInitializationAction(const AMDetectorInfoSet &detectorConfigurations)
{
	// To initialize the XAS scan, there are four stages.
	/*
		First: Enable/Disable all the pertinent detectors.  The scalar is ALWAYS enabled.
		Second: Set the mode to single shot,set the time on the synchronized dwell time.
	 */
	AMSequentialListAction3 *initializationAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("VESPERS Scan Initialization Actions", "VESPERS Scan Initialization Actions"));
	CLSSynchronizedDwellTime *synchronizedDwell = qobject_cast<CLSSynchronizedDwellTime*>(VESPERSBeamline::vespers()->synchronizedDwellTime());

	AMListAction3 *stage1 = new AMListAction3(new AMListActionInfo3("VESPERS Initialization Stage 1", "VESPERS Initialization Stage 1"), AMListAction3::Parallel);
	stage1->addSubAction(synchronizedDwell->createModeAction3(CLSSynchronizedDwellTime::SingleShot));
	stage1->addSubAction(synchronizedDwell->createScanningAction3(false));

	AMListAction3 *stage2 = new AMListAction3(new AMListActionInfo3("VESPERS Initialization Stage 2", "VESPERS Initialization Stage 2"), AMListAction3::Parallel);
	QStringList dwellKeys;

	for (int i = 0, size = detectorConfigurations.count(); i < size; i++)
		dwellKeys << VESPERSBeamline::vespers()->exposedDetectorByInfo(detectorConfigurations.at(i))->synchronizedDwellKey();

	dwellKeys.removeDuplicates();

	for (int i = 0, size = synchronizedDwell->elementCount(); i < size; i++){

		if (synchronizedDwell->enabledAt(i) && !dwellKeys.contains(synchronizedDwell->keyAt(i)))
			stage2->addSubAction(synchronizedDwell->elementAt(i)->createEnableAction3(false));

		else if (!synchronizedDwell->enabledAt(i) && dwellKeys.contains(synchronizedDwell->keyAt(i)))
			stage2->addSubAction(synchronizedDwell->elementAt(i)->createEnableAction3(true));
	}

	AMListAction3 *stage3 = new AMListAction3(new AMListActionInfo3("VESPERS Initialization Stage 3", "VESPERS Initialization Stage 3"), AMListAction3::Parallel);
	CLSSIS3820Scaler *scaler = VESPERSBeamline::vespers()->scaler();

	stage3->addSubAction(scaler->createStartAction3(false));
	stage3->addSubAction(scaler->createScansPerBufferAction3(1));
	stage3->addSubAction(scaler->createTotalScansAction3(1));

	initializationAction->addSubAction(stage1);
	initializationAction->addSubAction(stage2);
	initializationAction->addSubAction(stage3);

	return initializationAction;
}

AMAction3 *VESPERSScanController::buildCCDInitializationAction(VESPERS::CCDDetectors ccdChoice, const QString &ccdName)
{
	AMParallelListAction3 *action = new AMParallelListAction3(new AMParallelListActionInfo3("CCD Setup Action", "Sets the path, base file name and file number to the detector."));

	if (ccdChoice.testFlag(VESPERS::Roper)){

		VESPERSCCDDetector *ccd = VESPERSBeamline::vespers()->vespersRoperCCD();
		QString uniqueName = getUniqueCCDName(ccd->ccdFilePath(), ccdName);

		if (config_->ccdFileName() != uniqueName)
			config_->setCCDFileName(uniqueName);

		action->addSubAction(ccd->createFileNameAction(uniqueName));
		action->addSubAction(ccd->createFileNumberAction(1));
	}

	if (ccdChoice.testFlag(VESPERS::Mar)){

		VESPERSCCDDetector *ccd = VESPERSBeamline::vespers()->vespersMarCCD();
		QString uniqueName = getUniqueCCDName(ccd->ccdFilePath(), ccdName);

		if (config_->ccdFileName() != uniqueName)
			config_->setCCDFileName(uniqueName);

		action->addSubAction(ccd->createFileNameAction(uniqueName));
		action->addSubAction(ccd->createFileNumberAction(1));
	}

	if (ccdChoice.testFlag(VESPERS::Pilatus)){

		VESPERSCCDDetector *ccd = VESPERSBeamline::vespers()->vespersPilatusAreaDetector();

		QString dataFolder = AMUserSettings::userDataFolder;

		if (dataFolder.contains(QRegExp("\\d{2,2}-\\d{4,4}")))
			action->addSubAction(ccd->createFilePathAction("/ramdisk/" % dataFolder.mid(dataFolder.indexOf(QRegExp("\\d{2,2}-\\d{4,4}")), 7)));

		QString uniqueName = getUniqueCCDName(ccd->ccdFilePath(), ccdName);

		if (config_->ccdFileName() != uniqueName)
			config_->setCCDFileName(uniqueName);

		action->addSubAction(ccd->createFileNameAction(uniqueName));
		action->addSubAction(ccd->createFileNumberAction(1));
	}

	return action;
}

AMAction3 *VESPERSScanController::buildCleanupAction()
{
	// To cleanup the XAS scan, there is one stage.
	/*
		First: Only have the scalar running in the syncrhonized dwell time.
		Second: Set the dwell time to 1 second.  Set the scan mode to continuous.  This starts the synchronized dwell time.
		Third: Disables the variable integration time.  Set the relative energy PV to 0.
	 */
	AMListAction3 *cleanup = new AMListAction3(new AMListActionInfo3("VESPERS Cleanup", "VESPERS Cleanup"), AMListAction3::Sequential);
	CLSSynchronizedDwellTime *synchronizedDwell = qobject_cast<CLSSynchronizedDwellTime*>(VESPERSBeamline::vespers()->synchronizedDwellTime());

	AMListAction3 *stage1 = new AMListAction3(new AMListActionInfo3("VESPERS Cleanup Stage 2", "Setting synchronized dwell options"), AMListAction3::Parallel);
	stage1->addSubAction(synchronizedDwell->createMasterTimeAction3(1));
	stage1->addSubAction(synchronizedDwell->createModeAction3(CLSSynchronizedDwellTime::Continuous));

	AMListAction3 *stage2 = new AMListAction3(new AMListActionInfo3("VESPERS Cleanup Stage 1", "Disabling all but the scaler in the synchronized dwell."), AMListAction3::Parallel);

	// The scaler is always first.  Therefore, we know this will always only ensure the scaler is still enabled.
	for (int i = 0, size = synchronizedDwell->elementCount(); i < size; i++)
		stage2->addSubAction(synchronizedDwell->elementAt(i)->createEnableAction3(i == 0));

	cleanup->addSubAction(stage1);
	cleanup->addSubAction(stage2);

	return cleanup;
}

QString VESPERSScanController::buildNotes()
{
	// Build the notes for the scan.
	QString notes;
	VESPERS::FluorescenceDetectors xrfFlag = config_->fluorescenceDetector();

	if (xrfFlag == VESPERS::SingleElement)
		notes.append(QString("\nFluorescence detector (1-el Vortex) distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToSingleElementVortex(), 0, 'f', 1));

	if (xrfFlag == VESPERS::FourElement)
		notes.append(QString("\nFluorescence detector (4-el Vortex) distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToFourElementVortex(), 0, 'f', 1));

	if (config_->ccdDetector() == VESPERS::Pilatus)
		notes.append(QString("\nPilatus distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToCCD(), 0, 'f', 1));

	switch(VESPERSBeamline::vespers()->currentBeam()){

	case VESPERS::NoBeam:
		// This should never happen.
		break;

	case VESPERS::Pink:
		notes.append("Beam used:\tPink\n");
		break;

	case VESPERS::TenPercent:
		notes.append(QString("Beam used:\t10% bandpass\nMonochromator energy:\t%1 eV\n").arg(VESPERSBeamline::vespers()->mono()->energy(), 0, 'f', 2));
		break;

	case VESPERS::OnePointSixPercent:
		notes.append(QString("Beam used:\t1.6% bandpass\nMonochromator energy:\t%1 eV\n").arg(VESPERSBeamline::vespers()->mono()->energy(), 0, 'f', 2));
		break;

	case VESPERS::Si:
		notes.append(QString("Beam used:\tSi (%2E/E = 10^-4)\nMonochromator energy:\t%1 eV\n").arg(VESPERSBeamline::vespers()->mono()->energy(), 0, 'f', 2).arg(QString::fromUtf8("Δ")));
		break;
	}

	notes.append(QString("Filter thickness (aluminum):\t%1 %2m\n").arg(VESPERSBeamline::vespers()->endstation()->filterThickness()).arg(QString::fromUtf8("μ")));
	notes.append(QString("Horizontal slit separation:\t%1 mm\n").arg(VESPERSBeamline::vespers()->intermediateSlits()->gapX()));
	notes.append(QString("Vertical slit separation:\t%1 mm\n").arg(VESPERSBeamline::vespers()->intermediateSlits()->gapZ()));
	notes.append(QString("Gas used in ion chambers:\tN2\n"));
	notes.append(QString("\nIon Chamber Gain Settings\n"));

	CLSSIS3820ScalerChannel *channel = VESPERSBeamline::vespers()->scaler()->channelAt(5);
    CLSSR570 *sr570 = qobject_cast<CLSSR570 *>(channel->currentAmplifier());
    if (sr570)
        notes.append(QString("%1:\t%2 %3\n").arg("Split").arg(sr570->value()).arg(sr570->units()));
	channel = VESPERSBeamline::vespers()->scaler()->channelAt(7);
    sr570 = qobject_cast<CLSSR570 *>(channel->currentAmplifier());
        if (sr570)
    notes.append(QString("%1:\t%2 %3\n").arg("Pre-KB").arg(sr570->value()).arg(sr570->units()));
	channel = VESPERSBeamline::vespers()->scaler()->channelAt(8);
    sr570 = qobject_cast<CLSSR570 *>(channel->currentAmplifier());
        if (sr570)
    notes.append(QString("%1:\t%2 %3\n").arg("Mini").arg(sr570->value()).arg(sr570->units()));
	channel = VESPERSBeamline::vespers()->scaler()->channelAt(9);
    sr570 = qobject_cast<CLSSR570 *>(channel->currentAmplifier());
        if (sr570)
    notes.append(QString("%1:\t%2 %3\n").arg("Post").arg(sr570->value()).arg(sr570->units()));

	return notes;
}

//void VESPERSScanController::addStandardMeasurements(AMScan *scan, bool addEaAndDwellTime, bool addK)
//{
//	if (addEaAndDwellTime && addK){

//		scan->rawData()->addMeasurement(AMMeasurementInfo("Ea", "Energy Setpoint", "eV"));
//		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//		scan->rawData()->addMeasurement(AMMeasurementInfo("K", "K Setpoint"));
//		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//		scan->rawData()->addMeasurement(AMMeasurementInfo("DwellTime", "Dwell Time", "s"));
//		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	}

//	else if (addEaAndDwellTime){

//		scan->rawData()->addMeasurement(AMMeasurementInfo("Ea", "Energy Setpoint", "eV"));
//		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//		scan->rawData()->addMeasurement(AMMeasurementInfo("DwellTime", "Dwell Time", "s"));
//		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	}

//	scan->rawData()->addMeasurement(AMMeasurementInfo("RingCurrent", "Ring Current", "mA"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//}

//void VESPERSScanController::addSingleElementDeadTimeMeasurements(AMScan *scan)
//{
//	// Dead time, real time, live time, fast peaks, slow peaks, spectrum index.
//	scan->rawData()->addMeasurement(AMMeasurementInfo("DeadTime", "Dead Time", "%"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("RealTime", "Real Time", "s"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("LiveTime", "Live Time", "s"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks", "Fast Peaks"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks", "Slow Peaks"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//}

//void VESPERSScanController::addSingleElementRegionsOfInterestMeasurements(AMScan *scan, AMROIInfoList list, bool addSuffix)
//{
//	for (int i = 0, roiCount = list.count(); i < roiCount; i++){

//		if (addSuffix)
//			scan->rawData()->addMeasurement(AMMeasurementInfo(list.at(i).name().remove(" ") % QString("-1el"), list.at(i).name()));
//		else
//			scan->rawData()->addMeasurement(AMMeasurementInfo(list.at(i).name().remove(" "), list.at(i).name()));

//		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount() - 1), false, true);
//	}
//}

//void VESPERSScanController::addSingleElementSpectraMeasurments(AMScan *scan, const AMMeasurementInfo &info)
//{
//	AMMeasurementInfo temp = info;
//	temp.name = "rawSpectra-1el";
//	temp.description = "Raw Spectrum 1-el";
//	scan->rawData()->addMeasurement(temp);
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//}

//void VESPERSScanController::addFourElementDeadTimeMeasurements(AMScan *scan)
//{
//	// Real time (x4), Live time (x4), fast peaks (x4), slow peaks (x4), dead time (x4)
//	scan->rawData()->addMeasurement(AMMeasurementInfo("RealTime1", "Real Time 1", "s"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("RealTime2", "Real Time 2", "s"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("RealTime3", "Real Time 3", "s"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("RealTime4", "Real Time 4", "s"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("LiveTime1", "Live Time 1", "s"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("LiveTime2", "Live Time 2", "s"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("LiveTime3", "Live Time 3", "s"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("LiveTime4", "Live Time 4", "s"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks1", "Fast Peaks 1"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks2", "Fast Peaks 2"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks3", "Fast Peaks 3"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks4", "Fast Peaks 4"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks1", "Slow Peaks 1"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks2", "Slow Peaks 2"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks3", "Slow Peaks 3"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks4", "Slow Peaks 4"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("DeadTime1", "Dead Time 1", "%"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("DeadTime2", "Dead Time 2", "%"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("DeadTime3", "Dead Time 3", "%"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	scan->rawData()->addMeasurement(AMMeasurementInfo("DeadTime4", "Dead Time 4", "%"));
//	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//}

//void VESPERSScanController::addFourElementRegionsOfInterestMeasurements(AMScan *scan, AMROIInfoList list, bool addSuffix)
//{
//	for (int i = 0, roiCount = list.count(); i < roiCount; i++){

//		if (addSuffix)
//			scan->rawData()->addMeasurement(AMMeasurementInfo(list.at(i).name().remove(" ") % "-4el", list.at(i).name()));
//		else
//			scan->rawData()->addMeasurement(AMMeasurementInfo(list.at(i).name().remove(" "), list.at(i).name()));

//		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount() - 1), false, true);
//	}
//}

//void VESPERSScanController::addFourElementSpectraMeasurments(AMScan *scan, const AMMeasurementInfo &info)
//{
//	AMMeasurementInfo temp = info;
//	temp.name = "correctedSum-4el";
//	temp.description = "Corrected Sum 4-el";
//	scan->rawData()->addMeasurement(temp);
//	if (scan->scanRank() == 2 || scan->scanRank() == 3)
//		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
//	else
//		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), true, false);

//	for (int i = 0; i < 4; i++){

//		temp = info;
//		temp.name = QString("raw%1-4el").arg(i+1);
//		temp.description = QString("Raw Spectrum %1 4-el").arg(i+1);
//		scan->rawData()->addMeasurement(temp);
//		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount() - 1), false, true);
//	}
//}

//void VESPERSScanController::addStandardExtraPVs(QEpicsAdvAcq *advAcq, bool addEaAndDwellTime, bool addK)
//{
//	if (addEaAndDwellTime && addK){

//		advAcq->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
//		advAcq->appendRecord("07B2_Mono_SineB_K", true, false, 0);
//		advAcq->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
//	}

//	else if (addEaAndDwellTime){

//		advAcq->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
//		advAcq->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
//	}

//	advAcq->appendRecord("PCT1402-01:mA:fbk", true, false, 0);
//}

//void VESPERSScanController::addSingleElementDeadTimePVs(QEpicsAdvAcq *advAcq)
//{
//	advAcq->appendRecord("IOC1607-004:mca1.DTIM", true, false, 0);
//	advAcq->appendRecord("IOC1607-004:mca1.ERTM", true, false, 0);
//	advAcq->appendRecord("IOC1607-004:mca1.ELTM", true, false, 0);
//	advAcq->appendRecord("IOC1607-004:dxp1.FAST_PEAKS", true, false, 0);
//	advAcq->appendRecord("IOC1607-004:dxp1.SLOW_PEAKS", true, false, 0);
//}

//void VESPERSScanController::addSingleElementRegionsOfInterestPVs(QEpicsAdvAcq *advAcq, int roiCount)
//{
//	for (int i = 0; i < roiCount; i++)
//		advAcq->appendRecord("IOC1607-004:mca1.R"+QString::number(i), true, false, 1);
//}

//void VESPERSScanController::addSingleElementSpectraPVs(QEpicsAdvAcq *advAcq)
//{
//	advAcq->appendRecord("IOC1607-004:mca1", true, true, 1);
//}

//void VESPERSScanController::addFourElementDeadTimePVs(QEpicsAdvAcq *advAcq)
//{
//	advAcq->appendRecord("dxp1607-B21-04:dxp1:ElapsedRealTime", true, false, 0);
//	advAcq->appendRecord("dxp1607-B21-04:dxp2:ElapsedRealTime", true, false, 0);
//	advAcq->appendRecord("dxp1607-B21-04:dxp3:ElapsedRealTime", true, false, 0);
//	advAcq->appendRecord("dxp1607-B21-04:dxp4:ElapsedRealTime", true, false, 0);
//	advAcq->appendRecord("dxp1607-B21-04:dxp1:ElapsedTriggerLiveTime", true, false, 0);
//	advAcq->appendRecord("dxp1607-B21-04:dxp2:ElapsedTriggerLiveTime", true, false, 0);
//	advAcq->appendRecord("dxp1607-B21-04:dxp3:ElapsedTriggerLiveTime", true, false, 0);
//	advAcq->appendRecord("dxp1607-B21-04:dxp4:ElapsedTriggerLiveTime", true, false, 0);
//	advAcq->appendRecord("dxp1607-B21-04:dxp1:Triggers", true, false, 1);
//	advAcq->appendRecord("dxp1607-B21-04:dxp2:Triggers", true, false, 1);
//	advAcq->appendRecord("dxp1607-B21-04:dxp3:Triggers", true, false, 1);
//	advAcq->appendRecord("dxp1607-B21-04:dxp4:Triggers", true, false, 1);
//	advAcq->appendRecord("dxp1607-B21-04:dxp1:Events", true, false, 1);
//	advAcq->appendRecord("dxp1607-B21-04:dxp2:Events", true, false, 1);
//	advAcq->appendRecord("dxp1607-B21-04:dxp3:Events", true, false, 1);
//	advAcq->appendRecord("dxp1607-B21-04:dxp4:Events", true, false, 1);
//	advAcq->appendRecord("dxp1607-B21-04:dxp1:NetDTP", true, false, 1);
//	advAcq->appendRecord("dxp1607-B21-04:dxp2:NetDTP", true, false, 1);
//	advAcq->appendRecord("dxp1607-B21-04:dxp3:NetDTP", true, false, 1);
//	advAcq->appendRecord("dxp1607-B21-04:dxp4:NetDTP", true, false, 1);
//}

//void VESPERSScanController::addFourElementRegionsOfInterestPVs(QEpicsAdvAcq *advAcq, int roiCount)
//{
//	for (int i = 0; i < roiCount; i++)
//		advAcq->appendRecord("dxp1607-B21-04:mcaCorrected.R"+QString::number(i), true, false, 1);
//}

//void VESPERSScanController::addFourElementSpectraPVs(QEpicsAdvAcq *advAcq)
//{
//	advAcq->appendRecord("dxp1607-B21-04:mcaCorrected", true, true, 0);
//	advAcq->appendRecord("dxp1607-B21-04:mca1", true, true, 0);
//	advAcq->appendRecord("dxp1607-B21-04:mca2", true, true, 0);
//	advAcq->appendRecord("dxp1607-B21-04:mca3", true, true, 0);
//	advAcq->appendRecord("dxp1607-B21-04:mca4", true, true, 0);
//}

QString VESPERSScanController::getUniqueCCDName(const QString &path, const QString &name) const
{
	// Because the paths that go into the PV's are not the paths we use to check, we need to create the right path.  So far, that is /nas/vespers
	QString newPath = path;

	if (config_->ccdDetector() == VESPERS::Roper){

		newPath.replace("Y:\\", "/nas/vespers/");
		newPath.replace('\\', '/');
	}

	else if (config_->ccdDetector() == VESPERS::Pilatus)
		newPath.replace("/ramdisk/", "/nas/pilatus/500_500/");

	if (!VESPERS::fileNameExists(newPath, name))
		return name;

	QString newName = name;

	while (VESPERS::fileNameExists(newPath, newName))
		newName = VESPERS::appendUniqueIdentifier(name);

	return newName;
}
