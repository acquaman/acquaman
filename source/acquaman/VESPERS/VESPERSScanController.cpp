#include "VESPERSScanController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "acquaman/dacq3_3/qepicsadvacq.h"

#include <QStringBuilder>

VESPERSScanController::VESPERSScanController(VESPERSScanConfiguration *config)
{
	config_ = config;

	initializationAction_ = 0;
	cleanupAction_ = 0;
}

void VESPERSScanController::buildBaseInitializationAction(double timeStep)
{
	// To initialize the XAS scan, there are four stages.
	/*
		First: Enable/Disable all the pertinent detectors.  The scalar is ALWAYS enabled.
		Second: Set the mode to single shot,set the time on the synchronized dwell time.
	 */
	AMBeamlineParallelActionsList *setupActionsList = new AMBeamlineParallelActionsList;

	if (!initializationAction_)
		onInitializationActionFinished();

	initializationAction_ = new AMBeamlineListAction(setupActionsList);

	// First stage.
	setupActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Scalar
	setupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementByName("Scaler")->createEnableAction(true));
	// Single element vortex
	if ((config_->fluorescenceDetector() == VESPERS::SingleElement) || (config_->fluorescenceDetector() == (VESPERS::SingleElement | VESPERS::FourElement)))
		setupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementByName("1-El Vortex")->createEnableAction(true));
	else
		setupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementByName("1-El Vortex")->createEnableAction(false));
	// Roper CCD
	if (config_->ccdDetector() == VESPERS::Roper)
		setupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementByName("Roper CCD")->createEnableAction(true));
	else
		setupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementByName("Roper CCD")->createEnableAction(false));

	// Pilatus CCD
	if (config_->ccdDetector() == VESPERS::Pilatus)
		setupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementByName("Pilatus CCD")->createEnableAction(true));
	else
		setupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementByName("Pilatus CCD")->createEnableAction(false));

	// Four element vortex
	if ((config_->fluorescenceDetector() == VESPERS::FourElement) || (config_->fluorescenceDetector() == (VESPERS::SingleElement | VESPERS::FourElement)))
		setupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementByName("4-El Vortex")->createEnableAction(true));
	else
		setupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementByName("4-El Vortex")->createEnableAction(false));
	// Mar CCD
	if (config_->ccdDetector() == VESPERS::Mar)
		setupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementByName("Mar CCD")->createEnableAction(true));
	else
		setupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementByName("Mar CCD")->createEnableAction(false));

	// Second stage.
	setupActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	setupActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createStartAction(false));
	setupActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createScansPerBufferAction(1));
	setupActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createTotalScansAction(1));
	setupActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createModeAction(CLSSynchronizedDwellTime::SingleShot));
	setupActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createMasterTimeAction(timeStep));
}

void VESPERSScanController::buildCleanupAction(bool usingMono)
{
	// To cleanup the XAS scan, there is one stage.
	/*
		First: Only have the scalar running in the syncrhonized dwell time.
		Second: Set the dwell time to 1 second.  Disables the variable integration time.  Set the relative energy PV to 0.
		Third: Set the scan mode to continuous.  This starts the synchronized dwell time.
	 */
	AMBeamlineParallelActionsList *cleanupActionsList = new AMBeamlineParallelActionsList;

	if (!cleanupAction_)
		onCleanupActionFinished();

	cleanupAction_ = new AMBeamlineListAction(cleanupActionsList);

	// First stage.
	cleanupActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Scalar
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(0)->createEnableAction(true));
	// Single element vortex
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(1)->createEnableAction(false));
	// CCD
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(2)->createEnableAction(false));
	// Picoammeters
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(3)->createEnableAction(false));
	// Four element vortex
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(4)->createEnableAction(false));
	// Mar CCD
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(5)->createEnableAction(false));

	// Second stage.
	cleanupActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Synchronized dwell time.
	cleanupActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createMasterTimeAction(1.0));

	if (usingMono){
		// Variable integration time.
		cleanupActionsList->appendAction(1, VESPERSBeamline::vespers()->variableIntegrationTime()->createModeAction(CLSVariableIntegrationTime::Disabled));
		// Energy.
		cleanupActionsList->appendAction(1, VESPERSBeamline::vespers()->mono()->createDelEAction(0));
	}

	// Third stage.
	cleanupActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	// Start the synchronized dwell time.
	cleanupActionsList->appendAction(2, VESPERSBeamline::vespers()->synchronizedDwellTime()->createModeAction(CLSSynchronizedDwellTime::Continuous));
}

QString VESPERSScanController::buildNotes()
{
	// Build the notes for the scan.
	QString notes;

	switch ((int)config_->fluorescenceDetector()){

	case VESPERS::NoXRF:
		break;

	case VESPERS::SingleElement:
		notes.append(QString("\nFluorescence detector distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToSingleElementVortex(), 0, 'f', 1));
		break;

	case VESPERS::FourElement:
		notes.append(QString("\nFluorescence detector distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToFourElementVortex(), 0, 'f', 1));
		break;

	case VESPERS::SingleElement | VESPERS::FourElement:
		notes.append(QString("\nFluorescence detector (1-el Vortex) distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToSingleElementVortex(), 0, 'f', 1));
		notes.append(QString("\nFluorescence detector (4-el Vortex) distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToFourElementVortex(), 0, 'f', 1));
		break;
	}

	if (config_->ccdDetector() == VESPERS::Roper)
		notes.append(QString("\nRoper CCD distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToRoperCCD(), 0, 'f', 1));

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
	CLSSplitIonChamber *split = VESPERSBeamline::vespers()->iSplit();
	notes.append(QString("%1:\t%2 %3\n").arg(split->name()).arg(split->sensitivityValueA()).arg(split->sensitivityUnitsA()));
	CLSIonChamber *chamber = VESPERSBeamline::vespers()->iPreKB();
	notes.append(QString("%1:\t%2 %3\n").arg(chamber->name()).arg(chamber->sensitivityValue()).arg(chamber->sensitivityUnits()));
	chamber = VESPERSBeamline::vespers()->iMini();
	notes.append(QString("%1:\t%2 %3\n").arg(chamber->name()).arg(chamber->sensitivityValue()).arg(chamber->sensitivityUnits()));
	chamber = VESPERSBeamline::vespers()->iPost();
	notes.append(QString("%1:\t%2 %3\n").arg(chamber->name()).arg(chamber->sensitivityValue()).arg(chamber->sensitivityUnits()));

	return notes;
}

void VESPERSScanController::addStandardMeasurements(AMScan *scan, bool addEaAndDwellTime, bool addK)
{
	if (addEaAndDwellTime && addK){

		scan->rawData()->addMeasurement(AMMeasurementInfo("Ea", "Energy Setpoint", "eV"));
		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
		scan->rawData()->addMeasurement(AMMeasurementInfo("K", "K Setpoint"));
		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
		scan->rawData()->addMeasurement(AMMeasurementInfo("DwellTime", "Dwell Time", "s"));
		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	}

	else if (addEaAndDwellTime){

		scan->rawData()->addMeasurement(AMMeasurementInfo("Ea", "Energy Setpoint", "eV"));
		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
		scan->rawData()->addMeasurement(AMMeasurementInfo("DwellTime", "Dwell Time", "s"));
		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	}

	scan->rawData()->addMeasurement(AMMeasurementInfo("RingCurrent", "Ring Current", "mA"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
}

void VESPERSScanController::addSingleElementDeadTimeMeasurements(AMScan *scan)
{
	// Dead time, real time, live time, fast peaks, slow peaks, spectrum index.
	scan->rawData()->addMeasurement(AMMeasurementInfo("DeadTime", "Dead Time", "%"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("RealTime", "Real Time", "s"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("LiveTime", "Live Time", "s"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks", "Fast Peaks"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks", "Slow Peaks"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
}

void VESPERSScanController::addSingleElementRegionsOfInterestMeasurements(AMScan *scan, AMROIInfoList list, bool addSuffix)
{
	for (int i = 0, roiCount = list.count(); i < roiCount; i++){

		if (addSuffix)
			scan->rawData()->addMeasurement(AMMeasurementInfo(list.at(i).name().remove(" ") % QString("-1el"), list.at(i).name()));
		else
			scan->rawData()->addMeasurement(AMMeasurementInfo(list.at(i).name().remove(" "), list.at(i).name()));

		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount() - 1), false, true);
	}
}

void VESPERSScanController::addSingleElementSpectraMeasurments(AMScan *scan, const AMMeasurementInfo &info)
{
	AMMeasurementInfo temp = info;
	temp.name = "rawSpectra-1el";
	temp.description = "Raw Spectrum 1-el";
	scan->rawData()->addMeasurement(temp);
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
}

void VESPERSScanController::addFourElementDeadTimeMeasurements(AMScan *scan)
{
	// Real time (x4), Live time (x4), fast peaks (x4), slow peaks (x4), dead time (x4)
	scan->rawData()->addMeasurement(AMMeasurementInfo("RealTime1", "Real Time 1", "s"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("RealTime2", "Real Time 2", "s"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("RealTime3", "Real Time 3", "s"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("RealTime4", "Real Time 4", "s"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("LiveTime1", "Live Time 1", "s"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("LiveTime2", "Live Time 2", "s"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("LiveTime3", "Live Time 3", "s"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("LiveTime4", "Live Time 4", "s"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks1", "Fast Peaks 1"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks2", "Fast Peaks 2"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks3", "Fast Peaks 3"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks4", "Fast Peaks 4"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks1", "Slow Peaks 1"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks2", "Slow Peaks 2"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks3", "Slow Peaks 3"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks4", "Slow Peaks 4"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("DeadTime1", "Dead Time 1", "%"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("DeadTime2", "Dead Time 2", "%"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("DeadTime3", "Dead Time 3", "%"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	scan->rawData()->addMeasurement(AMMeasurementInfo("DeadTime4", "Dead Time 4", "%"));
	scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
}

void VESPERSScanController::addFourElementRegionsOfInterestMeasurements(AMScan *scan, AMROIInfoList list, bool addSuffix)
{
	for (int i = 0, roiCount = list.count(); i < roiCount; i++){

		if (addSuffix)
			scan->rawData()->addMeasurement(AMMeasurementInfo(list.at(i).name().remove(" ") % "-4el", list.at(i).name()));
		else
			scan->rawData()->addMeasurement(AMMeasurementInfo(list.at(i).name().remove(" "), list.at(i).name()));

		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount() - 1), false, false);
	}
}

void VESPERSScanController::addFourElementSpectraMeasurments(AMScan *scan, const AMMeasurementInfo &info)
{
	AMMeasurementInfo temp = info;
	temp.name = "correctedSum-4el";
	temp.description = "Corrected Sum 4-el";
	scan->rawData()->addMeasurement(temp);
	if (scan->scanRank() == 2)
		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), false, true);
	else
		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount()-1), true, false);

	for (int i = 0; i < 4; i++){

		temp = info;
		temp.name = QString("raw%1-4el").arg(i+1);
		temp.description = QString("Raw Spectrum %1 4-el").arg(i+1);
		scan->rawData()->addMeasurement(temp);
		scan->addRawDataSource(new AMRawDataSource(scan->rawData(), scan->rawData()->measurementCount() - 1), false, true);
	}
}

void VESPERSScanController::addStandardExtraPVs(QEpicsAdvAcq *advAcq, bool addEaAndDwellTime, bool addK)
{
	if (addEaAndDwellTime && addK){

		advAcq->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
		advAcq->appendRecord("07B2_Mono_SineB_K", true, false, 0);
		advAcq->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
	}

	else if (addEaAndDwellTime){

		advAcq->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
		advAcq->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
	}

	advAcq->appendRecord("PCT1402-01:mA:fbk", true, false, 0);
}

void VESPERSScanController::addSingleElementDeadTimePVs(QEpicsAdvAcq *advAcq)
{
	advAcq->appendRecord("IOC1607-004:mca1.DTIM", true, false, 0);
	advAcq->appendRecord("IOC1607-004:mca1.ERTM", true, false, 0);
	advAcq->appendRecord("IOC1607-004:mca1.ELTM", true, false, 0);
	advAcq->appendRecord("IOC1607-004:dxp1.FAST_PEAKS", true, false, 0);
	advAcq->appendRecord("IOC1607-004:dxp1.SLOW_PEAKS", true, false, 0);
}

void VESPERSScanController::addSingleElementRegionsOfInterestPVs(QEpicsAdvAcq *advAcq, int roiCount)
{
	for (int i = 0; i < roiCount; i++)
		advAcq->appendRecord("IOC1607-004:mca1.R"+QString::number(i), true, false, 1);
}

void VESPERSScanController::addSingleElementSpectraPVs(QEpicsAdvAcq *advAcq)
{
	advAcq->appendRecord("IOC1607-004:mca1", true, true, 1);
}

void VESPERSScanController::addFourElementDeadTimePVs(QEpicsAdvAcq *advAcq)
{
	advAcq->appendRecord("dxp1607-B21-04:dxp1:ElapsedRealTime", true, false, 0);
	advAcq->appendRecord("dxp1607-B21-04:dxp2:ElapsedRealTime", true, false, 0);
	advAcq->appendRecord("dxp1607-B21-04:dxp3:ElapsedRealTime", true, false, 0);
	advAcq->appendRecord("dxp1607-B21-04:dxp4:ElapsedRealTime", true, false, 0);
	advAcq->appendRecord("dxp1607-B21-04:dxp1:ElapsedTriggerLiveTime", true, false, 0);
	advAcq->appendRecord("dxp1607-B21-04:dxp2:ElapsedTriggerLiveTime", true, false, 0);
	advAcq->appendRecord("dxp1607-B21-04:dxp3:ElapsedTriggerLiveTime", true, false, 0);
	advAcq->appendRecord("dxp1607-B21-04:dxp4:ElapsedTriggerLiveTime", true, false, 0);
	advAcq->appendRecord("dxp1607-B21-04:dxp1:Triggers", true, false, 1);
	advAcq->appendRecord("dxp1607-B21-04:dxp2:Triggers", true, false, 1);
	advAcq->appendRecord("dxp1607-B21-04:dxp3:Triggers", true, false, 1);
	advAcq->appendRecord("dxp1607-B21-04:dxp4:Triggers", true, false, 1);
	advAcq->appendRecord("dxp1607-B21-04:dxp1:Events", true, false, 1);
	advAcq->appendRecord("dxp1607-B21-04:dxp2:Events", true, false, 1);
	advAcq->appendRecord("dxp1607-B21-04:dxp3:Events", true, false, 1);
	advAcq->appendRecord("dxp1607-B21-04:dxp4:Events", true, false, 1);
	advAcq->appendRecord("dxp1607-B21-04:dxp1:NetDTP", true, false, 1);
	advAcq->appendRecord("dxp1607-B21-04:dxp2:NetDTP", true, false, 1);
	advAcq->appendRecord("dxp1607-B21-04:dxp3:NetDTP", true, false, 1);
	advAcq->appendRecord("dxp1607-B21-04:dxp4:NetDTP", true, false, 1);
}

void VESPERSScanController::addFourElementRegionsOfInterestPVs(QEpicsAdvAcq *advAcq, int roiCount)
{
	for (int i = 0; i < roiCount; i++)
		advAcq->appendRecord("dxp1607-B21-04:mcaCorrected.R"+QString::number(i), true, false, 1);
}

void VESPERSScanController::addFourElementSpectraPVs(QEpicsAdvAcq *advAcq)
{
	advAcq->appendRecord("dxp1607-B21-04:mcaCorrected", true, true, 0);
	advAcq->appendRecord("dxp1607-B21-04:mca1", true, true, 0);
	advAcq->appendRecord("dxp1607-B21-04:mca2", true, true, 0);
	advAcq->appendRecord("dxp1607-B21-04:mca3", true, true, 0);
	advAcq->appendRecord("dxp1607-B21-04:mca4", true, true, 0);
}

void VESPERSScanController::onInitializationActionFinished()
{
	if (initializationAction_ == 0)
		return;

	// Disconnect all signals and return all memory.
	initializationAction_->disconnect();
	AMBeamlineParallelActionsList *actionList = initializationAction_->list();

	for (int i = 0; i < actionList->stageCount(); i++){

		while (actionList->stage(i)->size())
			actionList->stage(i)->takeAt(0)->deleteLater();
	}

	initializationAction_->deleteLater();
	initializationAction_ = 0;
}

void VESPERSScanController::onCleanupActionFinished()
{
	if (cleanupAction_ == 0)
		return;

	// Disconnect all signals and return all memory.
	cleanupAction_->disconnect();
	AMBeamlineParallelActionsList *actionList = cleanupAction_->list();

	for (int i = 0; i < actionList->stageCount(); i++){

		while (actionList->stage(i)->size())
			actionList->stage(i)->takeAt(0)->deleteLater();
	}

	cleanupAction_->deleteLater();
	cleanupAction_ = 0;
}

QString VESPERSScanController::getUniqueCCDName(const QString &path, const QString &name) const
{
	// Because the paths that go into the PV's are not the paths we use to check, we need to create the right path.  So far, that is /mnt/aurora
	QString newPath = path;

	if (config_->ccdDetector() == VESPERS::Roper){

		newPath.replace("Y:\\", "/nas/vespers/");
		newPath.replace('\\', '/');
	}

	if (!VESPERS::fileNameExists(newPath, name))
		return name;

	QString newName = name;

	while (VESPERS::fileNameExists(newPath, newName)){

		newName = VESPERS::appendUniqueIdentifier(name);
	}

	return newName;
}
