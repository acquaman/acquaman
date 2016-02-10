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


#include "SGMBeamline.h"

#include <QStringBuilder>

#include "dataman/AMSamplePlatePre2013.h"
#include "dataman/AMUser.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "beamline/CLS/CLSAdvancedScalerChannelDetector.h"

#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMFastScanConfiguration.h"
#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "actions3/actions/AMScanAction.h"

#include "beamline/AMMotorGroup.h"
#include "actions3/actions/AMControlStopAction.h"


#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "beamline/CLS/CLSPGTDetectorV2.h"
#include "beamline/CLS/CLSQE65000Detector.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSAdvancedScalerChannelDetector.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/CLS/CLSSR570.h"

#include "beamline/CLS/CLSExclusiveStatesControl.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlWaitAction.h"


#include "util/AMErrorMonitor.h"

SGMBeamline::SGMBeamline() : CLSBeamline("SGMBeamline") {
	infoObject_ = SGMBeamlineInfo::sgmInfo();

	beamlineIsInitialized_ = false;
	setupNameToPVLookup();
	setupControls();

	beamlineWarnings_ = "";
	connect(this, SIGNAL(criticalConnectionsChanged()), this, SLOT(recomputeWarnings()));
	connect(energy(), SIGNAL(valueChanged(double)), this, SLOT(onEnergyValueChanged()));

	xFocus_ = AMNumber(AMNumber::Null);
	yFocus_ = AMNumber(AMNumber::Null);
	moveInFocus_ = false;

	addChildControl(energy_);
	addChildControl(energySpacingParam_);
	addChildControl(energyC1Param_);
	addChildControl(energyC2Param_);
	addChildControl(energySParam_);
	addChildControl(energyThetaParam_);
	addChildControl(exitSlitGap_);
	addChildControl(entranceSlitGap_);
	addChildControl(m4_);
	addChildControl(grating_);
	addChildControl(harmonic_);
	addChildControl(undulatorTracking_);
	addChildControl(monoTracking_);
	addChildControl(exitSlitTracking_);
	addChildControl(tfyHV_);
	addChildControl(pgtHV_);
	addChildControl(ssaManipulatorX_);
	addChildControl(ssaManipulatorY_);
	addChildControl(ssaManipulatorZ_);
	addChildControl(ssaManipulatorRot_);
	addChildControl(beamlineScanning_);
	connect(beamlineScanning_, SIGNAL(valueChanged(double)), this, SLOT(onBeamlineScanningValueChanged(double)));

	addChildControl(beamlineReady_);
	addChildControl(nextDwellTimeTrigger_);
	addChildControl(nextDwellTimeConfirmed_);
	addChildControl(energyMovingStatus_);
	addChildControl(fastShutterVoltage_);
	addChildControl(gratingVelocity_);
	addChildControl(gratingBaseVelocity_);
	addChildControl(gratingAcceleration_);
	addChildControl(ea1CloseVacuum1_);
	addChildControl(ea1CloseVacuum2_);
	addChildControl(ea2CloseVacuum_);
	addChildControl(beamOn_);
	addChildControl(visibleLightToggle_);

	addChildControl(vvr1611_3_I10_01Shutter_);
	addChildControl(vvr1611_3_I10_02Shutter_);
	addChildControl(vvr1611_3_I10_03Shutter_);
	addChildControl(vvr1611_3_I10_04Shutter_);
	addChildControl(psh1611_3_I10_01Shutter_);
	addChildControl(vvr1611_4_I10_01Shutter_);
	addChildControl(vvr1611_4_I10_02Shutter_);
	addChildControl(vvr1611_4_I10_03Shutter_);
	addChildControl(vvr1611_4_I10_04Shutter_);
	addChildControl(vvr1611_4_I10_05Shutter_);
	addChildControl(vvr1611_4_I10_06Shutter_);
	addChildControl(vvr1611_4_I10_07Shutter_);
	addChildControl(vvr1611_4_I10_08Shutter_);
	addChildControl(psh1411_I00_01Shutter_);
	addChildControl(vvr1411_I00_01Shutter_);
	addChildControl(vvf1411_I00_01Shutter_);
	addChildControl(psh1411_I00_02Shutter_);
	addChildControl(ssh1411_I00_01Shutter_);
	addChildControl(vvr1611_3_I00_01Shutter_);

	connect(visibleLightToggle_, SIGNAL(valueChanged(double)), this, SLOT(onVisibleLightChanged(double)));
	addChildControl(visibleLightStatus_);
	connect(visibleLightStatus_, SIGNAL(valueChanged(double)), this, SLOT(onVisibleLightChanged(double)));
	addChildControl(scalerIntegrationTime_);
	connect(activeEndstation_, SIGNAL(valueChanged(double)), this, SLOT(onActiveEndstationChanged(double)));
	addChildControl(ssaIllumination_);
	connect(mirrorStripeSelection_, SIGNAL(valueChanged(double)), this, SLOT(onMirrorStripeChanged(double)));

	shutterControlSet_ = new AMControlSet(this);
	shutterControlSet_->setName("Shutter State Controls");
	shutterControlSet_->addControl(vvr1611_3_I10_01Shutter_);
	shutterControlSet_->addControl(vvr1611_3_I10_02Shutter_);
	shutterControlSet_->addControl(vvr1611_3_I10_03Shutter_);
	shutterControlSet_->addControl(vvr1611_3_I10_04Shutter_);
	shutterControlSet_->addControl(psh1611_3_I10_01Shutter_);
	shutterControlSet_->addControl(vvr1611_4_I10_01Shutter_);
	shutterControlSet_->addControl(vvr1611_4_I10_02Shutter_);
	shutterControlSet_->addControl(vvr1611_4_I10_03Shutter_);
	shutterControlSet_->addControl(vvr1611_4_I10_04Shutter_);
	shutterControlSet_->addControl(vvr1611_4_I10_05Shutter_);
	shutterControlSet_->addControl(vvr1611_4_I10_06Shutter_);
	shutterControlSet_->addControl(vvr1611_4_I10_07Shutter_);
	shutterControlSet_->addControl(vvr1611_4_I10_08Shutter_);
	shutterControlSet_->addControl(psh1411_I00_01Shutter_);
	shutterControlSet_->addControl(vvr1411_I00_01Shutter_);
	shutterControlSet_->addControl(vvf1411_I00_01Shutter_);
	shutterControlSet_->addControl(psh1411_I00_02Shutter_);
	shutterControlSet_->addControl(ssh1411_I00_01Shutter_);
	shutterControlSet_->addControl(vvr1611_3_I00_01Shutter_);
	shutterControlSet_->addControl(vvr1611_3_I00_02Shutter_);

	criticalControlsSet_ = new AMControlSet(this);
	criticalControlsSet_->setName("Critical Beamline Controls");
	criticalControlsSet_->addControl(energy_);
	criticalControlsSet_->addControl(exitSlitGap_);
	criticalControlsSet_->addControl(m4_);
	criticalControlsSet_->addControl(grating_);
	criticalControlsSet_->addControl(harmonic_);
	criticalControlsSet_->addControl(undulatorTracking_);
	criticalControlsSet_->addControl(monoTracking_);
	criticalControlsSet_->addControl(exitSlitTracking_);
	criticalControlsSet_->addControl(beamlineScanning_);

	criticalControlsSet_->addControl(beamlineReady_);
	criticalControlsSet_->addControl(energyMovingStatus_);

	unconnectedSets_.append(criticalControlsSet_);
	connect(criticalControlsSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));
	connect(criticalControlsSet_, SIGNAL(controlConnectedChanged(bool,AMControl*)), this, SLOT(onCriticalControlsConnectedChanged(bool,AMControl*)));

	connect(criticalControlsSet_, SIGNAL(connected(bool)), this, SLOT(onCriticalsConnectedChanged()));

	beamOnControlSet_ = new AMControlSet(this);
	beamOnControlSet_->setName("Beam On Controls");
	beamOnControlSet_->addControl(beamOn_);
	beamOnControlSet_->addControl(fastShutterVoltage_);
	unconnectedSets_.append(beamOnControlSet_);
	connect(beamOnControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	CLSSR570 *tempSR570;

	scaler_ = new CLSSIS3820Scaler("BL1611-ID-1:mcs", this);

	tempSR570 = new CLSSR570("TEY", "Amp1611-4-21", this);
	scaler_->channelAt(0)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(0)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(0)->setCustomChannelName("TEY");

	tempSR570 = new CLSSR570("I0", "Amp1611-4-22", this);
	scaler_->channelAt(1)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(1)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(1)->setCustomChannelName("I0");

	tempSR570 = new CLSSR570("TFY PD", "Amp1611-4-23", this);
	scaler_->channelAt(2)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(2)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(2)->setCustomChannelName("TFY PD ");

	tempSR570 = new CLSSR570("PD", "Amp1611-4-24", this);
	scaler_->channelAt(3)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(3)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(3)->setCustomChannelName("PD");

	scaler_->channelAt(4)->setCustomChannelName("UP");
	scaler_->channelAt(5)->setCustomChannelName("DOWN");
	scaler_->channelAt(6)->setCustomChannelName("FPD1");
	scaler_->channelAt(7)->setCustomChannelName("FPD2");
	scaler_->channelAt(8)->setCustomChannelName("FPD3");
	scaler_->channelAt(9)->setCustomChannelName("FPD4");
	scaler_->channelAt(10)->setCustomChannelName("FPD5");

	newAmptekSDD1_ = new CLSAmptekSDD123DetectorNew("AmptekSDD1", "Amptek SDD 1", "amptek:sdd1", this);
	newAmptekSDD2_ = new CLSAmptekSDD123DetectorNew("AmptekSDD2", "Amptek SDD 2", "amptek:sdd2", this);
	newAmptekSDD3_ = new CLSAmptekSDD123DetectorNew("AmptekSDD3", "Amptek SDD 3", "amptek:sdd3", this);
	newAmptekSDD4_ = new CLSAmptekSDD123DetectorNew("AmptekSDD4", "Amptek SDD 4", "amptek:sdd4", this);
	newAmptekSDD5_ = new CLSAmptekSDD123DetectorNew("AmptekSDD5", "Amptek SDD 5", "amptek:sdd5", this);
	newAmptekSDD5_->setEVPerBin(2.25);

	newPGTDetector_ = new CLSPGTDetectorV2("PGT", "PGT", "MCA1611-01", this);
	newQE65000Detector_ = new CLSQE65000Detector("QE65000", "QE 65000", "SA0000-03", this);
	newTEYDetector_ = new CLSAdvancedScalerChannelDetector("TEY", "TEY", scaler_, 0, this);
	scaler_->channelAt(0)->setDetector(newTEYDetector_);

	newTFYDetector_ = new CLSAdvancedScalerChannelDetector("TFY", "TFY", scaler_, 2, this);
	newI0Detector_ = new CLSAdvancedScalerChannelDetector("I0", "I0", scaler_, 1, this);
	newPDDetector_ = new CLSAdvancedScalerChannelDetector("PD", "PD", scaler_, 3, this);
	newFilteredPD1Detector_ = new CLSAdvancedScalerChannelDetector("FilteredPD1", "FilteredPD1", scaler_, 6, this);
	newFilteredPD2Detector_ = new CLSAdvancedScalerChannelDetector("FilteredPD2", "FilteredPD2", scaler_, 7, this);
	newFilteredPD3Detector_ = new CLSAdvancedScalerChannelDetector("FilteredPD3", "FilteredPD3", scaler_, 8, this);
	newFilteredPD4Detector_ = new CLSAdvancedScalerChannelDetector("FilteredPD4", "FilteredPD4", scaler_, 9, this);
	newFilteredPD5Detector_ = new CLSAdvancedScalerChannelDetector("FilteredPD5", "FilteredPD5", scaler_, 10, this);

	newEncoderUpDetector_ = new CLSAdvancedScalerChannelDetector("EncoderUp", "Encoder Up", scaler_, 5, this);
	newEncoderDownDetector_ = new CLSAdvancedScalerChannelDetector("EncoderDown", "Encoder Down", scaler_, 4, this);
	AMControl *energyFeedbackControl = new AMReadOnlyPVControl("EnergyFeedback", "BL1611-ID-1:Energy:fbk", this, "Energy Feedback");
	energyFeedbackDetector_ = new AMBasicControlDetectorEmulator("EnergyFeedback", "Energy Feedback", energyFeedbackControl, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	AMControl *gratingEncoderFeedbackControl = new AMReadOnlyPVControl("GratingEncoderFeedback", "SMTR16114I1002:enc:fbk", this, "Grating Encoder Feedback");
	gratingEncoderDetector_ = new AMBasicControlDetectorEmulator("GratingEncoderFeedback", "Grating Encoder Feedback", gratingEncoderFeedbackControl, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	AMControl* scalerDwellTime = new AMReadOnlyPVControl("ScalerDwellTime", "BL1611-ID-1:mcs:delay", this, "Scaler Dwell Time");
	dwellTimeDetector_ = new AMBasicControlDetectorEmulator("DwellTimeFeedback", "Dwell Time Feedback", scalerDwellTime, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);


	allDetectorGroup_ = new AMDetectorGroup("All Detectors", this);
	allDetectorGroup_->addDetector(newAmptekSDD1_);
	allDetectorGroup_->addDetector(newAmptekSDD2_);
	allDetectorGroup_->addDetector(newAmptekSDD3_);
	allDetectorGroup_->addDetector(newAmptekSDD4_);
	allDetectorGroup_->addDetector(newAmptekSDD5_);
	allDetectorGroup_->addDetector(newPGTDetector_);
	allDetectorGroup_->addDetector(newQE65000Detector_);
	allDetectorGroup_->addDetector(newTEYDetector_);
	allDetectorGroup_->addDetector(newTFYDetector_);
	allDetectorGroup_->addDetector(newI0Detector_);
	allDetectorGroup_->addDetector(newPDDetector_);
	allDetectorGroup_->addDetector(newFilteredPD1Detector_);
	allDetectorGroup_->addDetector(newFilteredPD2Detector_);
	allDetectorGroup_->addDetector(newFilteredPD3Detector_);
	allDetectorGroup_->addDetector(newFilteredPD4Detector_);
	allDetectorGroup_->addDetector(newFilteredPD5Detector_);
	allDetectorGroup_->addDetector(energyFeedbackDetector_);
	allDetectorGroup_->addDetector(dwellTimeDetector_);

	connect(allDetectorGroup_, SIGNAL(allDetectorsResponded()), this, SLOT(onAllDetectorsGroupAllDetectorResponded()));

	XASDetectorGroup_ = new AMDetectorGroup("XAS Detectors", this);
	XASDetectorGroup_->addDetector(newAmptekSDD1_);
	XASDetectorGroup_->addDetector(newAmptekSDD2_);
	XASDetectorGroup_->addDetector(newAmptekSDD3_);
	XASDetectorGroup_->addDetector(newAmptekSDD4_);
	XASDetectorGroup_->addDetector(newAmptekSDD5_);
	XASDetectorGroup_->addDetector(newPGTDetector_);
	XASDetectorGroup_->addDetector(newQE65000Detector_);
	XASDetectorGroup_->addDetector(newTEYDetector_);
	XASDetectorGroup_->addDetector(newTFYDetector_);
	XASDetectorGroup_->addDetector(newI0Detector_);
	XASDetectorGroup_->addDetector(newPDDetector_);
	XASDetectorGroup_->addDetector(newFilteredPD1Detector_);
	XASDetectorGroup_->addDetector(newFilteredPD2Detector_);
	XASDetectorGroup_->addDetector(newFilteredPD3Detector_);
	XASDetectorGroup_->addDetector(newFilteredPD4Detector_);
	XASDetectorGroup_->addDetector(newFilteredPD5Detector_);
	XASDetectorGroup_->addDetector(energyFeedbackDetector_);
	XASDetectorGroup_->addDetector(dwellTimeDetector_);

	FastDetectorGroup_ = new AMDetectorGroup("Fast Detectors", this);
	FastDetectorGroup_->addDetector(newTEYDetector_);
	FastDetectorGroup_->addDetector(newI0Detector_);
	FastDetectorGroup_->addDetector(newTFYDetector_);
	FastDetectorGroup_->addDetector(newPDDetector_);
	FastDetectorGroup_->addDetector(newFilteredPD1Detector_);
	FastDetectorGroup_->addDetector(newFilteredPD2Detector_);
	FastDetectorGroup_->addDetector(newFilteredPD3Detector_);
	FastDetectorGroup_->addDetector(newFilteredPD4Detector_);
	FastDetectorGroup_->addDetector(newFilteredPD5Detector_);

	criticalDetectorSet_ = new AMDetectorSet();
	criticalDetectorSet_->addDetector(newAmptekSDD1_);
	criticalDetectorSet_->addDetector(newAmptekSDD2_);
	criticalDetectorSet_->addDetector(newAmptekSDD3_);
	criticalDetectorSet_->addDetector(newAmptekSDD4_);
	criticalDetectorSet_->addDetector(newAmptekSDD5_);
	criticalDetectorSet_->addDetector(newTEYDetector_);
	criticalDetectorSet_->addDetector(newTFYDetector_);
	criticalDetectorSet_->addDetector(newI0Detector_);
	criticalDetectorSet_->addDetector(newPDDetector_);
	criticalDetectorSet_->addDetector(newFilteredPD1Detector_);
	criticalDetectorSet_->addDetector(newFilteredPD2Detector_);
	criticalDetectorSet_->addDetector(newFilteredPD3Detector_);
	criticalDetectorSet_->addDetector(newFilteredPD4Detector_);
	criticalDetectorSet_->addDetector(newFilteredPD5Detector_);
	criticalDetectorSet_->addDetector(energyFeedbackDetector_);
	criticalDetectorSet_->addDetector(dwellTimeDetector_);

	connect(criticalDetectorSet_, SIGNAL(connected(bool)), this, SLOT(onCriticalDetectorSetConnectedChanged(bool)));
	connect(allDetectorGroup_, SIGNAL(detectorBecameConnected(AMDetector*)), this, SLOT(onAllDetectorGroupDetectorBecameConnected(AMDetector*)));
	connect(allDetectorGroup_, SIGNAL(detectorBecameUnconnected(AMDetector*)), this, SLOT(onAllDetectorGroupDetectorBecameUnconnected(AMDetector*)));

	synchronizedDwellTime_ = new CLSSynchronizedDwellTime("BL1611-ID-1:dwell", this);
	synchronizedDwellTime_->addElement(0);
	synchronizedDwellTime_->addElement(1);
	synchronizedDwellTime_->addElement(2);
	synchronizedDwellTime_->addElement(3);
	synchronizedDwellTime_->addElement(4);

	trackingSet_ = new AMControlSet(this);
	trackingSet_->setName("Tracking");
	trackingSet_->addControl(undulatorTracking_);
	trackingSet_->addControl(monoTracking_);
	trackingSet_->addControl(exitSlitTracking_);

	fluxResolutionSet_ = new AMControlSet(this);
	fluxResolutionSet_->setName("Flux/Resolution");
	fluxResolutionSet_->addControl(grating_);
	fluxResolutionSet_->addControl(harmonic_);
	fluxResolutionSet_->addControl(exitSlitGap_);

	unconnectedSets_.append(trackingSet_);
	unconnectedSets_.append(fluxResolutionSet_);
	connect(trackingSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	ssaManipulatorSet_ = new AMControlSet(this);
	ssaManipulatorSet_->setName("SSA Manipulator");
	ssaManipulatorSet_->addControl(ssaManipulatorX_);
	ssaManipulatorSet_->addControl(ssaManipulatorY_);
	ssaManipulatorSet_->addControl(ssaManipulatorZ_);
	ssaManipulatorSet_->addControl(ssaManipulatorRot_);
	ssaManipulatorSampleTolerances_ << 1.0 << 1.0 << 1.0 << 15.0;
	unconnectedSets_.append(ssaManipulatorSet_);
	connect(ssaManipulatorSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	sampleManipulator_ = new AMControlSetSampleManipulator(ssaManipulatorSet_);

	currentSamplePlate_ = 0;//NULL

	setupExposedControls();
	setupExposedDetectors();
}

SGMBeamline::~SGMBeamline()
{
}

bool SGMBeamline::isConnected() const{
	return criticalControlsSet_->isConnected() && criticalDetectorSet_->isConnnected();
}

bool SGMBeamline::isReady() const{
	if(isConnected())
		if( (energy()->value() > 150) && (energy()->value() < 2100) )
			return true;
	return false;
}

SGMBeamlineInfo::sgmGrating SGMBeamline::currentGrating() const{
	return (SGMBeamlineInfo::sgmGrating)grating()->value();
}

QStringList SGMBeamline::unconnectedCriticals() const{
	QStringList allUnconnected;
	allUnconnected.append(criticalControlsSet_->unconnected());
	return allUnconnected;
}

AMDetectorSet* SGMBeamline::criticalDetectorSet(const QObject *privilegedRequester){
	if(QString("%1").arg(privilegedRequester->metaObject()->className()) == "SGMDetectorsMasterView")
		return criticalDetectorSet_;
	return 0; //NULL
}

bool SGMBeamline::detectorConnectedByName(QString name){
	for(int x = 0; x < unconnectedSets_.count(); x++){
		if( QString::compare(unconnectedSets_.at(x)->name().section(' ', 0, 0), name, Qt::CaseInsensitive) == 0)
			return false;
	}
	return true;
}

QString SGMBeamline::beamlineWarnings(){
	return beamlineWarnings_;
}

QString SGMBeamline::currentEndstation() const{
	if(activeEndstation_->value() == 0)
		return infoObject_->sgmEndstationName(SGMBeamlineInfo::scienta);
	else if(activeEndstation_->value() == 1)
		return infoObject_->sgmEndstationName(SGMBeamlineInfo::ssa);
	else
		return infoObject_->sgmEndstationName((SGMBeamlineInfo::sgmEndstation)272727);
}

AMAction3::ActionValidity SGMBeamline::validateAction(AMAction3 *action){
	AMScanAction *scanAction = qobject_cast<AMScanAction*>(action);
	if(scanAction){
		AMScanActionInfo *scanActionInfo = qobject_cast<AMScanActionInfo*>(scanAction->info());
		SGMXASScanConfiguration *oldXASScanConfiguration = qobject_cast<SGMXASScanConfiguration*>(scanActionInfo->configuration());
		SGMFastScanConfiguration *oldFastScanConfiguration = qobject_cast<SGMFastScanConfiguration*>(scanActionInfo->configuration());
		if(oldXASScanConfiguration || oldFastScanConfiguration)
			return AMAction3::ActionNeverValid;
		SGMFastScanConfiguration2013 *currentFastScanConfiguration = qobject_cast<SGMFastScanConfiguration2013*>(scanActionInfo->configuration());
		if(currentFastScanConfiguration && qFuzzyIsNull(undulatorForcedOpen_->value() - 1.0))
			return AMAction3::ActionNotCurrentlyValid;

	}

	return AMAction3::ActionCurrentlyValid;
}

QString SGMBeamline::validateActionMessage(AMAction3 *action){
	AMScanAction *scanAction = qobject_cast<AMScanAction*>(action);
	if(scanAction){
		AMScanActionInfo *scanActionInfo = qobject_cast<AMScanActionInfo*>(scanAction->info());
		SGMXASScanConfiguration *oldXASScanConfiguration = qobject_cast<SGMXASScanConfiguration*>(scanActionInfo->configuration());
		SGMFastScanConfiguration *oldFastScanConfiguration = qobject_cast<SGMFastScanConfiguration*>(scanActionInfo->configuration());
		if(oldXASScanConfiguration)
			return QString("The SGM Beamline no longer supports this type of XAS Scan configuration. While you may inspect the configuration you cannot run it. Please transfer these settings to a new scan configuration.");
		if(oldFastScanConfiguration)
			return QString("The SGM Beamline no longer supports this type of Fast Scan configuration. While you may inspect the configuration you cannot run it. Please transfer these settings to a new scan configuration.");
		SGMFastScanConfiguration2013 *currentFastScanConfiguration = qobject_cast<SGMFastScanConfiguration2013*>(scanActionInfo->configuration());
		if(currentFastScanConfiguration && qFuzzyIsNull(undulatorForcedOpen_->value() - 1.0))
			return QString("The Undulator has been forced open, and as such running a Fast Scan is not currently possible.");
	}

	return QString("Action is Currently Valid");
}

int SGMBeamline::currentSamplePlateId() const{
	if(currentSamplePlate_)
		return currentSamplePlate()->id();
	return -1;
}

int SGMBeamline::currentSampleId(){
	if(currentSamplePlate_)
		return currentSamplePlate_->sampleIdAtPosition(currentSamplePositioner()->toInfoList(), ssaManipulatorSampleTolerances_);
	return -1;
}

QString SGMBeamline::currentSampleDescription(){
	int currentId = currentSampleId();
	if(currentId == -1)
		return "<Unknown Sample>";
	else
		return AMSamplePre2013(currentId, AMUser::user()->database()).name();
}

AMDetector* SGMBeamline::newAmptekSDD1() const {
	return newAmptekSDD1_;
}

AMDetector* SGMBeamline::newAmptekSDD2() const {
	return newAmptekSDD2_;
}

AMDetector* SGMBeamline::newAmptekSDD3() const {
	return newAmptekSDD3_;
}

AMDetector* SGMBeamline::newAmptekSDD4() const {
	return newAmptekSDD4_;
}

AMDetector* SGMBeamline::newAmptekSDD5() const {
	return newAmptekSDD5_;
}

AMDetector* SGMBeamline::newPGTDetector() const {
	return newPGTDetector_;
}

AMDetector* SGMBeamline::newQE65000Detector() const {
	return newQE65000Detector_;
}

AMDetector* SGMBeamline::newTEYDetector() const {
	return newTEYDetector_;
}

AMDetector* SGMBeamline::newTFYDetector() const {
	return newTFYDetector_;
}

AMDetector* SGMBeamline::newI0Detector() const {
	return newI0Detector_;
}

AMDetector* SGMBeamline::newPDDetector() const {
	return newPDDetector_;
}

AMDetector* SGMBeamline::newFilteredPD1Detector() const {
	return newFilteredPD1Detector_;
}

AMDetector* SGMBeamline::newFilteredPD2Detector() const {
	return newFilteredPD2Detector_;
}

AMDetector* SGMBeamline::newFilteredPD3Detector() const {
	return newFilteredPD3Detector_;
}

AMDetector* SGMBeamline::newFilteredPD4Detector() const {
	return newFilteredPD4Detector_;
}

AMDetector* SGMBeamline::newFilteredPD5Detector() const {
	return newFilteredPD5Detector_;
}

AMDetector* SGMBeamline::newEncoderUpDetector() const {
	return newEncoderUpDetector_;
}

AMDetector* SGMBeamline::newEncoderDownDetector() const {
	return newEncoderDownDetector_;
}

AMDetector* SGMBeamline::energyFeedbackDetector() const {
	return energyFeedbackDetector_;
}

AMDetector* SGMBeamline::gratingEncoderDetector() const {
	return gratingEncoderDetector_;
}

AMDetector* SGMBeamline::dwellTimeDetector() const {
	return dwellTimeDetector_;
}

AMAction3* SGMBeamline::createBeamOnActions3(){
	if(!beamOnControlSet_->isConnected() || !shutterControlSet_->isConnected())
		return 0;

	AMListAction3 *beamOnActionsList = new AMListAction3(new AMListActionInfo3("SGM Beam On", "SGM Beam On"), AMListAction3::Parallel);

	beamOnActionsList->addSubAction(AMActionSupport::buildControlMoveAction(beamOn_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlMoveAction(fastShutterVoltage_, 0));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlMoveAction(frontBypassValve_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlMoveAction(backBypassValve_, 1));

	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_3_I10_01Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_3_I10_02Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_3_I10_03Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_3_I10_04Shutter_, 1));

	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(psh1611_3_I10_01Shutter_, 1));

	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_4_I10_01Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_4_I10_02Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_4_I10_03Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_4_I10_04Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_4_I10_05Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_4_I10_06Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_4_I10_07Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_4_I10_08Shutter_, 1));

	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(psh1411_I00_01Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1411_I00_01Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvf1411_I00_01Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(psh1411_I00_02Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(ssh1411_I00_01Shutter_, 1));

	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_3_I00_01Shutter_, 1));
	beamOnActionsList->addSubAction(AMActionSupport::buildControlWaitAction(vvr1611_3_I00_02Shutter_, 1));

	return beamOnActionsList;
}

AMAction3* SGMBeamline::createTurnOffBeamActions(){
	if(!SGMBeamline::sgm()->isConnected()) {
		return 0;
	}

	return AMActionSupport::buildControlMoveAction(fastShutterVoltage_, 5);
}

AMAction3* SGMBeamline::createStopMotorsActions3(){
	if(!beamOnControlSet_->isConnected())
		return 0;

	AMListAction3 *stopMotorsActionsList = new AMListAction3(new AMListActionInfo3("SGM Stop Motors", "SGM Stop Motors"), AMListAction3::Parallel);

	AMControlInfo stopMonoMotorInfo = mono()->toInfo();
	AMControlInfo stopExitSlitMotorInfo = exitSlit()->toInfo();
	AMControlInfo stopUndulatorMotorInfo = undulator()->toInfo();

	AMControlStopActionInfo *stopMonoActionInfo = new AMControlStopActionInfo(stopMonoMotorInfo);
	AMControlStopActionInfo *stopExitSlitActionInfo = new AMControlStopActionInfo(stopExitSlitMotorInfo);
	AMControlStopActionInfo *stopUndulatorActionInfo = new AMControlStopActionInfo(stopUndulatorMotorInfo);

	AMControlStopAction *stopMonoAction = new AMControlStopAction(stopMonoActionInfo, mono());
	AMControlStopAction *stopExitSlitAction = new AMControlStopAction(stopExitSlitActionInfo, exitSlit());
	AMControlStopAction *stopUndulatorAction = new AMControlStopAction(stopUndulatorActionInfo, undulator());

	stopMotorsActionsList->addSubAction(stopMonoAction);
	stopMotorsActionsList->addSubAction(stopExitSlitAction);
	stopMotorsActionsList->addSubAction(stopUndulatorAction);

	return stopMotorsActionsList;
}

AMAction3* SGMBeamline::createGoToTransferPositionActions3(){
	if(!beamOnControlSet_->isConnected())
		return 0;

	AMListAction3 *goToTransferPostionActionsList = new AMListAction3(new AMListActionInfo3("SGM Go To Tranfer", "SGM Go To Tranfer"), AMListAction3::Parallel);

	goToTransferPostionActionsList->addSubAction(AMActionSupport::buildControlMoveAction(ssaManipulatorX_, 0.0));
	goToTransferPostionActionsList->addSubAction(AMActionSupport::buildControlMoveAction(ssaManipulatorY_, -9.1));
	goToTransferPostionActionsList->addSubAction(AMActionSupport::buildControlMoveAction(ssaManipulatorZ_, 151.0));
	goToTransferPostionActionsList->addSubAction(AMActionSupport::buildControlMoveAction(ssaManipulatorRot_, 0.0));

	return goToTransferPostionActionsList;
}

AMAction3* SGMBeamline::createGoToMeasurementPositionActions3(){
	if(!beamOnControlSet_->isConnected())
		return 0;

	AMListAction3 *goToMeasurePostionActionsList = new AMListAction3(new AMListActionInfo3("SGM Go To Measure", "SGM Go To Measure"), AMListAction3::Parallel);

	goToMeasurePostionActionsList->addSubAction(AMActionSupport::buildControlMoveAction(ssaManipulatorX_, 0.0));
	goToMeasurePostionActionsList->addSubAction(AMActionSupport::buildControlMoveAction(ssaManipulatorY_, 0.0));
	goToMeasurePostionActionsList->addSubAction(AMActionSupport::buildControlMoveAction(ssaManipulatorZ_, 0.0));
	goToMeasurePostionActionsList->addSubAction(AMActionSupport::buildControlMoveAction(ssaManipulatorRot_, 0.0));

	return goToMeasurePostionActionsList;
}

AMAction3* SGMBeamline::createRestorePreFastScanDefaultActions(){
	if(!beamOnControlSet_->isConnected())
		return 0;

	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Restore beamline defaults"), QString("Restore beamline defaults")), AMListAction3::Parallel);

	retVal->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->gratingVelocity(), 10000));
	retVal->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->gratingBaseVelocity(), 0));
	retVal->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->gratingAcceleration(), 5000));

	retVal->addSubAction(rawScaler()->createDwellTimeAction3(1.0));
	retVal->addSubAction(rawScaler()->createScansPerBufferAction3(1));
	retVal->addSubAction(rawScaler()->createTotalScansAction3(1));

	retVal->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorTracking(), 1));
	retVal->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->exitSlitTracking(), 1));

	return retVal;
}

CLSSIS3820Scaler* SGMBeamline::scaler() const{
	if(scaler_->isConnected())
		return scaler_;
	return 0; //NULL
}

CLSSIS3820Scaler* SGMBeamline::rawScaler(){
	return scaler_;
}

bool SGMBeamline::isBeamlineScanning() const{
	if( fabs(beamlineScanning_->value() -1.0) < beamlineScanning_->tolerance() )
		return true;
	return false;
}

bool SGMBeamline::isVisibleLightOn() const{
	if(visibleLightToggle_->value() == 1)
		return true;
	return false;
}

bool SGMBeamline::validInFocusPair() const{
	return xFocus_.isValid() && yFocus_.isValid();
}

bool SGMBeamline::movingInFocus() const{
	return validInFocusPair() && moveInFocus_;
}

void SGMBeamline::setCurrentSamplePlate(AMSamplePlatePre2013 *newSamplePlate){
	if(currentSamplePlate_ != newSamplePlate){
		currentSamplePlate_ = newSamplePlate;
		emit currentSamplePlateChanged(currentSamplePlate_);
	}
}

void SGMBeamline::visibleLightOn(){
	visibleLightToggle_->move(1);
}

void SGMBeamline::visibleLightOff(){
	visibleLightToggle_->move(0);
}

void SGMBeamline::closeVacuum(){
	if(activeEndstation_->value() == 0){
		ea1CloseVacuum1_->move(1);
		ea1CloseVacuum2_->move(1);
	}
	else if(activeEndstation_->value() == 1)
		ea2CloseVacuum_->move(1);
}

void SGMBeamline::setCurrentEndstation(SGMBeamlineInfo::sgmEndstation endstation){
	if(endstation == SGMBeamlineInfo::scienta)
		activeEndstation_->move(0);
	else if(endstation == SGMBeamlineInfo::ssa)
		activeEndstation_->move(1);
	return;
}

void SGMBeamline::setCurrentMirrorStripe(SGMBeamlineInfo::sgmMirrorStripe mirrorStripe){
	if(mirrorStripe == SGMBeamlineInfo::carbonStripe)
		mirrorStripeSelectCarbon_->move(1);
	else if(mirrorStripe == SGMBeamlineInfo::siliconStripe)
		mirrorStripeSelectSilicon_->move(1);
	return;
}

void SGMBeamline::setInFocus(){
	if(isConnected()){
		xFocus_ = ssaManipulatorX()->value();
		yFocus_ = ssaManipulatorY()->value();
	}
}

void SGMBeamline::setMoveInFocus(bool moveInFocus){
	if(validInFocusPair() && moveInFocus_ != moveInFocus){
		moveInFocus_ = moveInFocus;
		if(moveInFocus_)
			connect(ssaManipulatorX(), SIGNAL(setpointChanged(double)), this, SLOT(moveToYFocusPosition(double)));
		else
			disconnect(ssaManipulatorX(), SIGNAL(setpointChanged(double)), this, SLOT(moveToYFocusPosition(double)));
		emit moveInFocusChanged(moveInFocus_);
	}
}

void SGMBeamline::onBeamlineScanningValueChanged(double value){
	bool isScanning;
	if( fabs(value - 1.0) < beamlineScanning_->tolerance() )
		isScanning = true;
	else
		isScanning = false;
	emit beamlineScanningChanged(isScanning);
}

void SGMBeamline::onControlSetConnected(bool csConnected){
	AMControlSet *ctrlSet = (AMControlSet*)QObject::sender();

	if(csConnected){
		unconnectedSets_.removeAll(ctrlSet);
		if(ctrlSet->name() == "SSA Manipulator"){
			AMControlInfoList ssaInfoList = ssaManipulatorSet_->toInfoList();
			ssaInfoList[0].setValue(-1.0);
			ssaInfoList[1].setValue( 0.0);
			ssaInfoList[2].setValue(-1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 0.0);
			ssaInfoList[2].setValue(-1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 1.0);
			ssaInfoList[2].setValue(-1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue(-1.0);
			ssaInfoList[2].setValue( 0.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 0.0);
			ssaInfoList[2].setValue( 0.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 1.0);
			ssaInfoList[2].setValue( 0.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue(-1.0);
			ssaInfoList[2].setValue( 1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 0.0);
			ssaInfoList[2].setValue( 1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 1.0);
			ssaInfoList[2].setValue( 1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
		}
		emit controlSetConnectionsChanged();
	}
	else{
		if( !unconnectedSets_.contains(ctrlSet) ){
			unconnectedSets_.append(ctrlSet);
			emit controlSetConnectionsChanged();
		}
	}
}

void SGMBeamline::onCriticalControlsConnectedChanged(bool isConnected, AMControl *control){
	Q_UNUSED(isConnected)
	Q_UNUSED(control)
	emit criticalControlsConnectionsChanged();
}

void SGMBeamline::onCriticalsConnectedChanged(){
	emit criticalConnectionsChanged();
	reviewConnected();
}

void SGMBeamline::onEnergyValueChanged(){
	if(isConnected()){
		bool wasOutsideGoodRange = false;
		bool isOutsideGoodRange = false;
		if( (lastEnergyValue_ < 150) || (lastEnergyValue_ > 2100) )
			wasOutsideGoodRange = true;
		if( (energy()->value() < 150) || (energy()->value() > 2100) )
			isOutsideGoodRange = true;

		if(wasOutsideGoodRange != isOutsideGoodRange)
			emit beamlineReadyChanged();

		lastEnergyValue_ = energy()->value();
	}
}

void SGMBeamline::onCriticalDetectorSetConnectedChanged(bool connected){
	Q_UNUSED(connected)
	emit criticalConnectionsChanged();
	reviewConnected();
}

void SGMBeamline::onAllDetectorGroupDetectorBecameConnected(AMDetector *detector){
	emit detectorAvailabilityChanged(detector, true);
}

void SGMBeamline::onAllDetectorGroupDetectorBecameUnconnected(AMDetector *detector){
	emit detectorAvailabilityChanged(detector, false);
}

void SGMBeamline::onActiveEndstationChanged(double value){
	emit currentEndstationChanged(SGMBeamlineInfo::sgmEndstation(int(value)));
}

void SGMBeamline::onMirrorStripeChanged(double value){
	emit currentMirrorStripeChanged(SGMBeamlineInfo::sgmMirrorStripe(int(value)));
}

void SGMBeamline::recomputeWarnings(){
	if(!isConnected()){
		beamlineWarnings_ = "Warning some critical beamline\ncontrols are not connected:\n";
		foreach(QString ctrlName, unconnectedCriticals())
			beamlineWarnings_.append("  "+ctrlName+"\n");
	}
	else
		beamlineWarnings_ = "";

	emit beamlineWarningsChanged(beamlineWarnings_);
}

void SGMBeamline::onVisibleLightChanged(double value){
	Q_UNUSED(value);
	AMPVwStatusControl *lMono = qobject_cast<AMPVwStatusControl*>(mono());
	if(visibleLightToggle_->value() == 1 && visibleLightStatus_->value() == 8)
		emit visibleLightStatusChanged("Visible Light\n is ON");
	else if( visibleLightToggle_->value() == 1 && visibleLightStatus_->value() == 0 && lMono->movingPV()->getString() == "MOVING - CCW" )
		emit visibleLightStatusChanged("Visible Light\n is moving to ON");
	else if( visibleLightToggle_->value() == 1 && visibleLightStatus_->value() == 0 && lMono->movingPV()->getString() == "MOVING +  CW" )
		visibleLightToggle_->move(0);
	else if( visibleLightStatus_->value() == 8)
		emit visibleLightStatusChanged("Visible Light\n is moving to OFF");
	else if( visibleLightStatus_->value() == 0)
		emit visibleLightStatusChanged("Visible Light\n is OFF");
}

void SGMBeamline::computeBeamlineInitialized(){
	if(!beamlineIsInitialized_){
		beamlineIsInitialized_ = true;
		emit beamlineInitialized();
	}
}

void SGMBeamline::onAllDetectorsGroupAllDetectorResponded(){
	if(!beamlineIsInitialized_)
		computeBeamlineInitialized();
}

void SGMBeamline::moveToYFocusPosition(double xSetpoint){
	if(isConnected()){
		double currentRotationRadians = (ssaManipulatorRot()->value()*3.14159)/180.0;
		double yFocusSetpoint = double(yFocus_) + (xSetpoint - double(xFocus_))*tan(currentRotationRadians);
		ssaManipulatorY()->move(yFocusSetpoint);
	}
}

void SGMBeamline::setupNameToPVLookup(){
	amNames2pvNames_.set("energy", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("energySpacingParam", "SG16114I1001:selI");
	amNames2pvNames_.set("energyC1Param", "SG16114I1001:selG");
	amNames2pvNames_.set("energyC2Param", "SG16114I1001:selH");
	amNames2pvNames_.set("energySParam", "SG16114I1001:selE");
	amNames2pvNames_.set("energyThetaParam", "SG16114I1001:selF");
	amNames2pvNames_.set("eV_Fbk", "BL1611-ID-1:Energy:fbk");
	amNames2pvNames_.set("eVFbk", "BL1611-ID-1:Energy:fbk");
	amNames2pvNames_.set("mono", "SMTR16114I1002");
	amNames2pvNames_.set("undulator", "UND1411-01");
	amNames2pvNames_.set("exitSlit", "PSL16114I1003");
	amNames2pvNames_.set("exitSlitGap", "PSL16114I1004");
	amNames2pvNames_.set("entranceSlitGap", "PSL16114I1001");
	amNames2pvNames_.set("M4", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("M4Inboard", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("M4Outboard", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("M4Downstream", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("grating", "BL1611-ID-1:AddOns:grating");
	amNames2pvNames_.set("harmonic", "BL1611-ID-1:AddOns:harmonic");
	amNames2pvNames_.set("undulatorMotor", "SMTR1411-01");
	amNames2pvNames_.set("undulatorTracking", "UND1411-01:Energy:track");
	amNames2pvNames_.set("monoTracking", "SG16114I1001:Energy:track");
	amNames2pvNames_.set("exitSlitTracking", "PSL16114I1003:Energy:track");
	amNames2pvNames_.set("tfyScaler", "BL1611-ID-1:mcs02:fbk");
	amNames2pvNames_.set("tfyHV", "PS1611401:109");
	amNames2pvNames_.set("pgtBase", "MCA1611-01");
	amNames2pvNames_.set("pgtHV", "MCA1611-01:Bias:Volt");
	amNames2pvNames_.set("amptekSDD1", "amptek:sdd1");
	amNames2pvNames_.set("amptekSDD2", "amptek:sdd2");

	amNames2pvNames_.set("ssaManipulatorX", "SMTR16114I1022");
	amNames2pvNames_.set("ssaManipulatorY", "SMTR16114I1023");
	amNames2pvNames_.set("ssaManipulatorZ", "SMTR16114I1024");
	amNames2pvNames_.set("ssaManipulatorRot", "SMTR16114I1025");

	amNames2pvNames_.set("beamlineScanning", "BL1611-ID-1:scanning");
	amNames2pvNames_.set("beamlineReady", "BL1611-ID-1:beam:status");
	amNames2pvNames_.set("nextDwellTimeTrigger", "BL1611-ID-1:AddOns:trigger:dwellTime");
	amNames2pvNames_.set("nextDwellTimeConfirmed", "BL1611-ID-1:AddOns:confirmed:dwellTime");
	amNames2pvNames_.set("energyMovingStatus", "BL1611-ID-1:ready");
	amNames2pvNames_.set("fastShutterVoltage", "PSH16114I1001:V");
	amNames2pvNames_.set("scaler", "BL1611-ID-1:mcs:scan");
	amNames2pvNames_.set("scalerIntegrationTime", "BL1611-ID-1:mcs:delay");
	amNames2pvNames_.set("gratingVelocity", "SMTR16114I1002:velo");
	amNames2pvNames_.set("gratingBaseVelocity", "SMTR16114I1002:veloBase");
	amNames2pvNames_.set("gratingAcceleration", "SMTR16114I1002:accel");
	amNames2pvNames_.set("ea1CloseVacuum1", "VVR1611-4-I10-05:opr:close");
	amNames2pvNames_.set("ea1CloseVacuum2", "VVR1611-4-I10-06:opr:close");
	amNames2pvNames_.set("ea2CloseVacuum", "VVR1611-4-I10-08:opr:close");
	amNames2pvNames_.set("beamOn", "BL1611-ID-1:AddOns:beamon:trigger");
	amNames2pvNames_.set("visibleLightToggle", "BL1611-ID-1:visible");
	amNames2pvNames_.set("visibleLightStatus", "BL1611-ID-1:visible:cal");
	amNames2pvNames_.set("activeEndstation", "BL1611-ID-1:AddOns:endstation:active");
	amNames2pvNames_.set("ssaIllumination", "ILC1611-4-I10-02");
	amNames2pvNames_.set("mirrorStripeSelection", "BL1611-ID-1:stripe");
	amNames2pvNames_.set("masterDwell", "BL1611-ID-1:dwell:setTime");
}

void SGMBeamline::setupControls(){
	amNames2pvNames_.resetLookup();

	QString sgmPVName = amNames2pvNames_.valueF("energy");
	energy_ = new AMPVwStatusControl("energy", sgmPVName+":fbk", sgmPVName, "BL1611-ID-1:ready", "SMTR16114I1002:stop", this, 0.25);
	energy_->setDescription("Energy");
	sgmPVName = amNames2pvNames_.valueF("energySpacingParam");
	energySpacingParam_ = new AMReadOnlyPVControl("energySpacingParam", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energyC1Param");
	energyC1Param_ = new AMReadOnlyPVControl("energyC1Param", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energyC2Param");
	energyC2Param_ = new AMReadOnlyPVControl("energyC2Param", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energySParam");
	energySParam_ = new AMReadOnlyPVControl("energySParam", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energyThetaParam");
	energyThetaParam_ = new AMReadOnlyPVControl("energyThetaParam", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("mono");
	AMPVwStatusControl *mono = new AMPVwStatusControl("mono", sgmPVName+":enc:fbk", sgmPVName+":encTarget", sgmPVName+":status", "SMTR16114I1002:stop", energy_, 5, 2.0, new CLSMAXvControlStatusChecker(), 1);
	mono->setDescription("Monochromator");
	sgmPVName = amNames2pvNames_.valueF("undulator");
	AMPVwStatusControl *undulator = new AMPVwStatusControl("undulator", sgmPVName+":gap:mm:fbk", sgmPVName+":gap:mm", sgmPVName+":moveStatus", "UND1411-01:stop", energy_, 0.1);
	undulator->setDescription("Undulator");
	sgmPVName = amNames2pvNames_.valueF("exitSlit");
	AMPVwStatusControl *exitSlit = new AMPVwStatusControl("exitSlit", sgmPVName+":Y:mm:fbk", sgmPVName+":Y:mm:encsp", "SMTR16114I1003:status", "SMTR16114I1003:stop", energy_, 0.1, 2.0, new AMControlStatusCheckerDefault(1));
	exitSlit->setDescription("Exit Slit Position");
	energy_->addChildControl(mono);
	energy_->addChildControl(undulator);
	energy_->addChildControl(exitSlit);

	sgmPVName = amNames2pvNames_.valueF("exitSlitGap");
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", sgmPVName+":Y:mm:fbk", "BL1611-ID-1:AddOns:ExitSlitGap:Y:mm", "BL1611-ID-1:AddOns:ExitSlitGap:Y:status", "SMTR16114I1017:stop", this, 0.5);
	exitSlitGap_->setDescription("Exit Slit Gap");
	sgmPVName = amNames2pvNames_.valueF("entranceSlitGap");
	entranceSlitGap_ = new AMPVwStatusControl("entranceSlitGap", sgmPVName+":Y:mm:fbk", sgmPVName+":Y:mm:encsp", "SMTR16114I1001:status", "SMTR16114I1001:stop", this, 0.1);
	entranceSlitGap_->setDescription("Entrance Slit Gap");
	sgmPVName = amNames2pvNames_.valueF("M4");
	m4_ = new AMReadOnlyPVwStatusControl("M4", sgmPVName, sgmPVName, this);
	m4_->setDescription("M4 Mirror");
	sgmPVName = amNames2pvNames_.valueF("M4Inboard");
	AMPVwStatusControl *m4inboard = new AMPVwStatusControl("M4Inboard", sgmPVName, sgmPVName, sgmPVName, "", this, 0.1);
	m4inboard->setDescription("M4 Inboard");
	sgmPVName = amNames2pvNames_.valueF("M4Outboard");
	AMPVwStatusControl *m4outboard = new AMPVwStatusControl("M4Outboard", sgmPVName, sgmPVName, sgmPVName, "", this, 0.1);
	m4outboard->setDescription("M4 Outboard");
	sgmPVName = amNames2pvNames_.valueF("M4Downstream");
	AMPVwStatusControl *m4downstream = new AMPVwStatusControl("M4Downstream", sgmPVName, sgmPVName, sgmPVName, "", this, 0.1);
	m4downstream->setDescription("M4 Downstream");
	m4_->addChildControl(m4inboard);
	m4_->addChildControl(m4outboard);
	m4_->addChildControl(m4downstream);

	sgmPVName = amNames2pvNames_.valueF("grating");
	grating_ = new AMPVwStatusControl("grating", sgmPVName, sgmPVName, "SMTR16114I1016:state", "SMTR16114I1016:emergStop", this, 0.1, 2.0, new AMControlStatusCheckerStopped(0));
	grating_->setDescription("Grating Selection");
	grating_->setAttemptMoveWhenWithinTolerance(false);
	sgmPVName = amNames2pvNames_.valueF("harmonic");
	harmonic_ = new AMPVwStatusControl("harmonic", sgmPVName, sgmPVName, "UND1411-01:moveStatus", "", this, 0.1);
	harmonic_->setDescription("Harmonic");
	harmonic_->setAttemptMoveWhenWithinTolerance(false);

	sgmPVName = amNames2pvNames_.valueF("undulatorMotor");
	undulatorStep_ = new AMPVControl("undulatorStep", sgmPVName+":step:sp", sgmPVName+":step", sgmPVName+":stop", this, 20, 20 );
	undulatorRelativeStepStorage_ = new AMPVControl("undulatorRelativeStepStorage", "BL1611-ID-1:AddOns:UndulatorRelativeStorage", "BL1611-ID-1:AddOns:UndulatorRelativeStorage", QString(), this, 1);
	undulatorVelocity_ = new AMPVControl("undulatorVelocity", sgmPVName+":velo:sp", sgmPVName+":velo", QString(), this, 1);
	undulatorFastTracking_ = new AMPVControl("undulatorFastTracking", "BL1611-ID-1:AddOns:UndulatorTrigger", "BL1611-ID-1:AddOns:UndulatorTrigger", QString(), this, 0.5);
	undulatorFastTrackingTrigger_ = new AMPVControl("undulatorFastTrackingTrigger", "BL1611-ID-1:AddOns:UndulatorTriggerPropogate", "BL1611-ID-1:AddOns:UndulatorTriggerPropogate", QString(), this, 0.5);

	sgmPVName = amNames2pvNames_.valueF("undulatorTracking");
	undulatorTracking_ = new AMPVControl("undulatorTracking", sgmPVName, sgmPVName, "", this, 0.1);
	undulatorTracking_->setDescription("Undulator Tracking");
	undulatorTracking_->setContextKnownDescription("Undulator");
	sgmPVName = amNames2pvNames_.valueF("monoTracking");
	monoTracking_ = new AMPVControl("monoTracking", sgmPVName, sgmPVName, "", this, 0.1, 10);
	monoTracking_->setDescription("Mono Tracking");
	monoTracking_->setContextKnownDescription("Mono");
	sgmPVName = amNames2pvNames_.valueF("exitSlitTracking");
	exitSlitTracking_ = new AMPVControl("exitSlitTracking", sgmPVName, sgmPVName, "", this, 0.1);
	exitSlitTracking_->setDescription("Exit Slit Tracking");
	exitSlitTracking_->setContextKnownDescription("Exit Slit");

	sgmPVName = amNames2pvNames_.valueF("tfyHV");
	tfyHV_ = new AMPVControl("tfyHV", sgmPVName+":vmon", sgmPVName+":v0set", QString(), this, 5.0);
	tfyHV_->setDescription("TFY High Voltage");
	tfyHV_->setContextKnownDescription("Voltage");

	tfyHVToggle_ = new AMPVControl("tfyHVToggle", sgmPVName+":status", sgmPVName+":pwonoff", QString(), this, 0.1);
	tfyHVToggle_->setDescription("TFY High Voltage Toggle");
	tfyHVToggle_->setContextKnownDescription("Toggle");

	sgmPVName = amNames2pvNames_.valueF("pgtHV");
	pgtHV_ = new AMPVControl("pgtHV", sgmPVName+"Actual:fbk", sgmPVName, QString(), this, 0.5);
	pgtHV_->setDescription("SDD High Voltage");
	pgtHV_->setContextKnownDescription("Voltage");

	ssaManipulatorX_ = new SGMMAXvMotor("ssaManipulatorX", amNames2pvNames_.valueF("ssaManipulatorX"), "SSA Inboard/Outboard", true, 0.2, 2.0, this);
	ssaManipulatorX_->setContextKnownDescription("X");

	ssaManipulatorY_ = new SGMMAXvMotor("ssaManipulatorY", amNames2pvNames_.valueF("ssaManipulatorY"), "SSA Upstream/Downstream", true, 0.2, 2.0, this);
	ssaManipulatorY_->setContextKnownDescription("Y");

	ssaManipulatorZ_ = new SGMMAXvMotor("ssaManipulatorZ", amNames2pvNames_.valueF("ssaManipulatorZ"), "SSA Up/Down", true, 0.2, 2.0, this);
	ssaManipulatorZ_->setContextKnownDescription("Z");

	ssaManipulatorRot_ = new SGMMAXvMotor("ssaManipulatorRot", amNames2pvNames_.valueF("ssaManipulatorRot"), "SSA Rotation", false, 0.2, 2.0, this);
	ssaManipulatorRot_->setContextKnownDescription("R");


	sgmPVName = amNames2pvNames_.valueF("beamlineScanning");
	beamlineScanning_ = new AMPVControl("beamlineScanning", sgmPVName, sgmPVName, "", this, 0.1);
	beamlineScanning_->setDescription("Beamline Scanning");
	sgmPVName = amNames2pvNames_.valueF("beamlineReady");
	beamlineReady_ = new AMReadOnlyPVControl("beamlineReady", sgmPVName, this);
	beamlineReady_->setDescription("Beamline Status");
	sgmPVName = amNames2pvNames_.valueF("nextDwellTimeTrigger");
	nextDwellTimeTrigger_ = new AMPVControl("nextDwellTimeTrigger", sgmPVName, sgmPVName, "", this, 0.1 );
	nextDwellTimeTrigger_->setDescription("Next Dwell Time Trigger");
	sgmPVName = amNames2pvNames_.valueF("nextDwellTimeConfirmed");
	nextDwellTimeConfirmed_ = new AMPVControl("nextDwellTimeConfirmed", sgmPVName, sgmPVName, "", this, 0.1 );
	nextDwellTimeConfirmed_->setDescription("Next Dwell Time Confirmed");

	sgmPVName = amNames2pvNames_.valueF("energyMovingStatus");
	energyMovingStatus_ = new AMReadOnlyPVControl("energyMovingStatus", sgmPVName, this);
	energyMovingStatus_->setDescription("Energy Status");
	sgmPVName = amNames2pvNames_.valueF("fastShutterVoltage");
	fastShutterVoltage_ = new AMPVControl("fastShutterVoltage", sgmPVName, sgmPVName, "", this, 0.1);
	fastShutterVoltage_->setDescription("Fast Shutter Voltage");
	sgmPVName = amNames2pvNames_.valueF("gratingVelocity");
	gratingVelocity_ = new AMPVControl("gratingVelocity", sgmPVName, sgmPVName, "", this, 1);
	gratingVelocity_->setDescription("Grating Motor Velocity");
	sgmPVName = amNames2pvNames_.valueF("gratingBaseVelocity");
	gratingBaseVelocity_ = new AMPVControl("gratingBaseVelocity", sgmPVName, sgmPVName, "", this, 1);
	gratingBaseVelocity_->setDescription("Grating Motor Base Velocity");
	sgmPVName = amNames2pvNames_.valueF("gratingAcceleration");
	gratingAcceleration_ = new AMPVControl("gratingAcceleration", sgmPVName, sgmPVName, "", this, 1);
	gratingAcceleration_->setDescription("Grating Motor Acceleration");
	sgmPVName = amNames2pvNames_.valueF("ea1CloseVacuum1");
	ea1CloseVacuum1_ = new AMPVControl("ea1CloseVacuum1", sgmPVName, sgmPVName, "", this);
	ea1CloseVacuum1_->setDescription("XPS Upstream Close Vacuum Valve");
	sgmPVName = amNames2pvNames_.valueF("ea1CloseVacuum2");
	ea1CloseVacuum2_ = new AMPVControl("ea1CloseVacuum2", sgmPVName, sgmPVName, "", this);
	ea1CloseVacuum2_->setDescription("XPS Downstream Close Vacuum Valve");
	sgmPVName = amNames2pvNames_.valueF("ea2CloseVacuum");
	ea2CloseVacuum_ = new AMPVControl("ea2CloseVacuum", sgmPVName, sgmPVName, "", this);
	ea2CloseVacuum_->setDescription("SSA Close Vacuum Valve");
	sgmPVName = amNames2pvNames_.valueF("beamOn");
	beamOn_ = new AMPVControl("beamOn", sgmPVName, sgmPVName, "", this, 0.5);
	beamOn_->setDescription("Beam On");
	sgmPVName = amNames2pvNames_.valueF("visibleLightToggle");
	visibleLightToggle_ = new AMPVControl("visibleLightToggle", sgmPVName, sgmPVName, "", this);
	visibleLightToggle_->setDescription("Visible Light On/Off");
	sgmPVName = amNames2pvNames_.valueF("visibleLightStatus");
	visibleLightStatus_ = new AMReadOnlyPVControl("visibleLightStatus", sgmPVName, this);
	visibleLightStatus_->setDescription("Visible Light Status");
	sgmPVName = amNames2pvNames_.valueF("activeEndstation");
	activeEndstation_ = new AMPVControl("activeEndstation", sgmPVName, sgmPVName, "", this);
	activeEndstation_->setDescription("Endstation Selection");

	sgmPVName = amNames2pvNames_.valueF("scalerIntegrationTime");
	scalerIntegrationTime_ = new AMPVControl("scalerIntegrationTime", sgmPVName, sgmPVName, "", this, 0.1);
	scalerIntegrationTime_->setDescription("Scaler Integration Time");
	scalerIntegrationTime_->setContextKnownDescription("Integration Time");

	sgmPVName = amNames2pvNames_.valueF("ssaIllumination");
	ssaIllumination_ = new AMPVControl("ssaIllumination", sgmPVName, sgmPVName, "", this, 0.5);
	ssaIllumination_->setDescription("SSA Illumination");

	sgmPVName = amNames2pvNames_.valueF("mirrorStripeSelection");
	mirrorStripeSelection_ = new AMReadOnlyPVControl("mirrorStripeSelection", sgmPVName%":fbk", this);
	mirrorStripeSelection_->setDescription("Mirror Stripe");

	mirrorStripeSelectCarbon_ = new AMPVControl("mirrorStripeSelectCarbon", sgmPVName%":c_fanout.PROC", sgmPVName%":c_fanout.PROC", QString(), this, 0.5);
	mirrorStripeSelectCarbon_->setDescription("Select Carbon Stripe");
	mirrorStripeSelectSilicon_ = new AMPVControl("mirrorStripeSelectSilicon", sgmPVName%":si_fanout.PROC", sgmPVName%":si_fanout.PROC", QString(), this, 0.5);
	mirrorStripeSelectSilicon_->setDescription("Select Silicon Stripe");

	sgmPVName = amNames2pvNames_.valueF("undulator");
	undulatorOffset_ = new AMPVControl("undulatorOffset", sgmPVName%":corr_A", sgmPVName%":corr_A", QString(), this, 0.01);
	undulatorOffset_->setDescription("Undulator Offset");

	sgmPVName = amNames2pvNames_.valueF("masterDwell");
	masterDwell_ = new AMPVControl("masterDwell", sgmPVName, sgmPVName, QString(), this, 0.01);
	masterDwell_->setDescription("Master Dwell Time");

	undulatorRelativeStep_ = new AMPVControl("undulatorRelativeStep", "SMTR1411-01:step:rel", "SMTR1411-01:step:rel", QString(), this, 5);
	undulatorRelativeStep_->setDescription("Undulator Relative Step");

	m2VerticalUpstreamStep_ = new AMReadOnlyPVControl("m2VerticalUpstreamStep", "SMTR16113I1010:step:fbk", this);
	m2VerticalDownstreamStep_ = new AMReadOnlyPVControl("m2VerticalDownstreamStep", "SMTR16113I1011:step:fbk", this);
	m2HorizontalUpstreamStep_ = new AMReadOnlyPVControl("m2HorizontalUpstreamsStep", "SMTR16113I1012:step:fbk", this);
	m2HorizontalDownstreamStep_ = new AMReadOnlyPVControl("m2HorizontalDownstreamStep_", "SMTR16113I1013:step:fbk", this);
	m2RotationalStep_ = new AMReadOnlyPVControl("m2RotationalStep_", "SMTR16113I1014:step:fbk", this);
	m3VerticalUpstreamStep_ = new AMReadOnlyPVControl("m3VerticalUpstreamStep", "SMTR16113I1015:step:sp", this);
	m3VerticalDownstreamStep_ = new AMReadOnlyPVControl("m3VerticalDownstreamStep", "SMTR16113I1016:step:sp", this);
	m3HorizontalUpstreamStep_ = new AMReadOnlyPVControl("m3HorizontalUpstreamStep", "SMTR16113I1017:step:sp", this);
	m3HorizontalDownstreamStep_ = new AMReadOnlyPVControl("m3HorizontalDownstreamStep", "SMTR16113I1018:step:sp", this);
	m3RotationalStep_ = new AMReadOnlyPVControl("m3RotationalStep", "SMTR16113I1019:step:fbk", this);

	m2VerticalUpstreamEncoder_ = new AMReadOnlyPVControl("m2VerticalUpstreamEncoder", "SMTR16113I1010:encod:fbk", this);
	m2VerticalDownstreamEncoder_ = new AMReadOnlyPVControl("m2VerticalDownstreamEncoder", "SMTR16113I1011:encod:fbk", this);
	m2HorizontalUpstreamEncoder_ = new AMReadOnlyPVControl("m2HorizontalUpstreamsEncoder", "SMTR16113I1012:encod:fbk", this);
	m2HorizontalDownstreamEncoder_ = new AMReadOnlyPVControl("m2HorizontalDownstreamEncoder_", "SMTR16113I1013:encod:fbk", this);
	m2RotationalEncoder_ = new AMReadOnlyPVControl("m2RotationalEncoder_", "SMTR16113I1014:encod:fbk", this);
	m3VerticalUpstreamEncoder_ = new AMReadOnlyPVControl("m3VerticalUpstreamEncoder", "SMTR16113I1015:enc:fbk", this);
	m3VerticalDownstreamEncoder_ = new AMReadOnlyPVControl("m3VerticalDownstreamEncoder", "SMTR16113I1016:enc:fbk", this);
	m3HorizontalUpstreamEncoder_ = new AMReadOnlyPVControl("m3HorizontalUpstreamEncoder", "SMTR16113I1017:enc:fbk", this);
	m3HorizontalDownstreamEncoder_ = new AMReadOnlyPVControl("m3HorizontalDownstreamEncoder", "SMTR16113I1018:enc:fbk", this);
	m3RotationalEncoder_ = new AMReadOnlyPVControl("m3RotationalEncoder", "SMTR16113I1019:encod:fbk", this);

	frontBypassValve_ = new CLSExclusiveStatesControl("FrontBypassValve", "VVR1611-4-I10-09:state", "VVR1611-4-I10-09:opr:open", "VVR1611-4-I10-09:opr:close", this);
	frontBypassValve_->setDescription("Before Bypass Valve");
	backBypassValve_ = new CLSExclusiveStatesControl("BackBypassValve", "VVR1611-4-I10-10:state", "VVR1611-4-I10-10:opr:open", "VVR1611-4-I10-10:opr:close", this);
	backBypassValve_->setDescription("Behind Bypass Valve");

	undulatorForcedOpen_ = new AMReadOnlyPVControl("UndulatorForcedOpen", "UND1411-01:openID", this);

	vvr1611_3_I10_01Shutter_ = new AMReadOnlyPVControl("VVR1611-3-I10-01Open", "VVR1611-3-I10-01:state", this);
	vvr1611_3_I10_02Shutter_ = new AMReadOnlyPVControl("VVR1611-3-I10-02Open", "VVR1611-3-I10-02:state", this);
	vvr1611_3_I10_03Shutter_= new AMReadOnlyPVControl("VVR1611-3-I10-03Open", "VVR1611-3-I10-03:state", this);
	vvr1611_3_I10_04Shutter_= new AMReadOnlyPVControl("VVR1611-3-I10-04Open", "VVR1611-3-I10-04:state", this);
	psh1611_3_I10_01Shutter_ = new AMReadOnlyPVControl("PSH1611-3-I10-01Open", "PSH1611-3-I10-01:state", this);
	vvr1611_4_I10_01Shutter_ = new AMReadOnlyPVControl("VVR1611-4-I10-01Open", "VVR1611-4-I10-01:state", this);
	vvr1611_4_I10_02Shutter_ = new AMReadOnlyPVControl("VVR1611-4-I10-02Open", "VVR1611-4-I10-02:state", this);
	vvr1611_4_I10_03Shutter_ = new AMReadOnlyPVControl("VVR1611-4-I10-03Open", "VVR1611-4-I10-03:state", this);
	vvr1611_4_I10_04Shutter_ = new AMReadOnlyPVControl("VVR1611-4-I10-04Open", "VVR1611-4-I10-04:state", this);
	vvr1611_4_I10_05Shutter_ = new AMReadOnlyPVControl("VVR1611-4-I10-05Open", "VVR1611-4-I10-05:state", this);
	vvr1611_4_I10_06Shutter_ = new AMReadOnlyPVControl("VVR1611-4-I10-06Open", "VVR1611-4-I10-06:state", this);
	vvr1611_4_I10_07Shutter_ = new AMReadOnlyPVControl("VVR1611-4-I10-07Open", "VVR1611-4-I10-07:state", this);
	vvr1611_4_I10_08Shutter_ = new AMReadOnlyPVControl("VVR1611-4-I10-08Open", "VVR1611-4-I10-08:state", this);
	psh1411_I00_01Shutter_ = new AMReadOnlyPVControl("PSH1411-I00-01Open", "PSH1411-I00-01:state", this);
	vvr1411_I00_01Shutter_ = new AMReadOnlyPVControl("VVR1411-I00-01Open", "VVR1411-I00-01:state", this);
	vvf1411_I00_01Shutter_ = new AMReadOnlyPVControl("VVF1411-I00-01Open", "VVF1411-I00-01:state", this);
	psh1411_I00_02Shutter_ = new AMReadOnlyPVControl("PSH1411-I00-01Open", "PSH1411-I00-02:state", this);
	ssh1411_I00_01Shutter_ = new AMReadOnlyPVControl("SSH1411-I00-01Open", "SSH1411-I00-01:state", this);
	vvr1611_3_I00_01Shutter_ = new AMReadOnlyPVControl("VVR1611-3-I00-01Open", "VVR1611-3-I00-01:state", this);
	vvr1611_3_I00_02Shutter_ = new AMReadOnlyPVControl("VVR1611-3-I00-02Open", "VVR1611-3-I00-01:state", this);

	if(amNames2pvNames_.lookupFailed())
		AMErrorMon::alert(this, SGMBEAMLINE_PV_NAME_LOOKUPS_FAILED, "PV Name lookups in the SGM Beamline failed");

	motorGroup_ = new AMMotorGroup(this);

	AMMotorGroupObject* sampleManipulatorGroupObject =
			new AMMotorGroupObject("Manipulator", this);

	sampleManipulatorGroupObject->setDirectionAxis(AMMotorGroupObject::HorizontalMotion,
										  "X", ssaManipulatorX_,
										  "", 0);

	sampleManipulatorGroupObject->setDirectionAxis(AMMotorGroupObject::NormalMotion,
										  "Y", ssaManipulatorY_,
										  "", 0);

	sampleManipulatorGroupObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion,
										  "Z", ssaManipulatorZ_,
										  "rZ", ssaManipulatorRot_);

	sampleManipulatorGroupObject->axis(AMMotorGroupObject::VerticalMotion)->setRotationPositionUnits("deg");

	motorGroup_->addMotorGroupObject(sampleManipulatorGroupObject);
}

void SGMBeamline::setupExposedControls(){
	addExposedControl(ssaManipulatorX_);
	addExposedControl(ssaManipulatorY_);
	addExposedControl(ssaManipulatorZ_);
	addExposedControl(ssaManipulatorRot_);
	addExposedControl(energy_);
	addExposedControl(masterDwell_);
	addExposedControl(exitSlitGap_);
}

void SGMBeamline::setupExposedDetectors(){
	addExposedDetector(newAmptekSDD1_);
	addExposedDetector(newAmptekSDD2_);
	addExposedDetector(newAmptekSDD3_);
	addExposedDetector(newAmptekSDD4_);
	addExposedDetector(newAmptekSDD5_);
	addExposedDetector(newQE65000Detector_);
	addExposedDetector(newTEYDetector_);
	addExposedDetector(newI0Detector_);
	addExposedDetector(newTFYDetector_);
	addExposedDetector(newPDDetector_);
	addExposedDetector(newFilteredPD1Detector_);
	addExposedDetector(newFilteredPD2Detector_);
	addExposedDetector(newFilteredPD3Detector_);
	addExposedDetector(newFilteredPD4Detector_);
	addExposedDetector(newFilteredPD5Detector_);
	addExposedDetector(newEncoderUpDetector_);
	addExposedDetector(newEncoderDownDetector_);
	addExposedDetector(energyFeedbackDetector_);
	addExposedDetector(gratingEncoderDetector_);
	addExposedDetector(dwellTimeDetector_);

	addExposedDetectorGroup(XASDetectorGroup_);
	addExposedDetectorGroup(FastDetectorGroup_);
}

void SGMBeamline::reviewConnected(){
	if(criticalControlsSet_->isConnected() && criticalDetectorSet_->isConnnected())
		emit beamlineConnected(true);
	else
		emit beamlineConnected(false);
}

SGMBeamline* SGMBeamline::sgm() {

	if(instance_ == 0){
		instance_ = new SGMBeamline();
		instance_->initializeBeamlineSupport();
	}

	return static_cast<SGMBeamline*>(instance_);

}
