/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "dataman/AMSamplePlate.h"
#include "dataman/AMUser.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "beamline/CLS/CLSCAEN2527HVChannel.h"
#include "beamline/CLS/CLSPGT8000HVChannel.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "SGMOptimizationSupport.h"
#include "beamline/CLS/CLSAdvancedScalerChannelDetector.h"

#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMFastScanConfiguration.h"
#include "actions3/actions/AMScanAction.h"

#include "beamline/AMOldDetector.h"
#include "beamline/AMSingleControlDetector.h"
#include "beamline/SGM/SGMMCPDetector.h"
#include "beamline/CLS/CLSPGTDetector.h"
#include "beamline/CLS/CLSOceanOptics65000Detector.h"
#include "beamline/CLS/CLSAmptekSDD123Detector.h"

#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "beamline/CLS/CLSPGTDetectorV2.h"
#include "beamline/CLS/CLSQE65000Detector.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSAdvancedScalerChannelDetector.h"
#include "beamline/AMBasicControlDetectorEmulator.h"

SGMBeamline::SGMBeamline() : AMBeamline("SGMBeamline") {
	infoObject_ = SGMBeamlineInfo::sgmInfo();

	beamlineIsInitialized_ = false;
	setupNameToPVLookup();
	setupControls();

	beamlineWarnings_ = "";
	connect(this, SIGNAL(criticalConnectionsChanged()), this, SLOT(recomputeWarnings()));
	connect(energy(), SIGNAL(valueChanged(double)), this, SLOT(onEnergyValueChanged()));

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
	connect(visibleLightToggle_, SIGNAL(valueChanged(double)), this, SLOT(onVisibleLightChanged(double)));
	addChildControl(visibleLightStatus_);
	connect(visibleLightStatus_, SIGNAL(valueChanged(double)), this, SLOT(onVisibleLightChanged(double)));
	addChildControl(scalerIntegrationTime_);
	connect(activeEndstation_, SIGNAL(valueChanged(double)), this, SLOT(onActiveEndstationChanged(double)));
	addChildControl(ssaIllumination_);
	connect(mirrorStripeSelection_, SIGNAL(valueChanged(double)), this, SLOT(onMirrorStripeChanged(double)));

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

	scaler_ = new CLSSIS3820Scaler("BL1611-ID-1:mcs", this);
	scaler_->channelAt(0)->setCustomChannelName("TEY");
	scaler_->channelAt(1)->setCustomChannelName("I0");
	scaler_->channelAt(2)->setCustomChannelName("TFY PD ");
	scaler_->channelAt(3)->setCustomChannelName("PD");
	scaler_->channelAt(4)->setCustomChannelName("UP");
	scaler_->channelAt(5)->setCustomChannelName("DOWN");
	scaler_->channelAt(6)->setCustomChannelName("FPD1");
	scaler_->channelAt(7)->setCustomChannelName("FPD2");
	scaler_->channelAt(8)->setCustomChannelName("FPD3");
	scaler_->channelAt(9)->setCustomChannelName("FPD4");
	scaler_->channelAt(10)->setCustomChannelName("FPD5");

	detectorMap_ = new QMultiMap<AMOldDetector*, QPair<AMOldDetectorSet*, bool> >();

	criticalDetectorsSet_ = new AMOldDetectorSet(this);
	criticalDetectorsSet_->setName("Critical Beamline Detectors");
	rawDetectorsSet_ = new AMOldDetectorSet(this);
	rawDetectorsSet_->setName("All possible detectors for SGM Beamline");
	connect(criticalDetectorsSet_, SIGNAL(connected(bool)), this, SLOT(onCriticalsConnectedChanged()));
	connect(criticalDetectorsSet_, SIGNAL(detectorAdded(int)), this, SLOT(recomputeWarnings()));
	connect(criticalDetectorsSet_, SIGNAL(detectorRemoved(int)), this, SLOT(recomputeWarnings()));

	allDetectors_ = new AMOldDetectorSet(this);
	allDetectors_->setName("All Detectors");

	feedbackDetectors_ = new AMOldDetectorSet(this);
	feedbackDetectors_->setName("Feedback Detectors");

	XASDetectors_ = new AMOldDetectorSet(this);
	XASDetectors_->setName("XAS Detectors");

	FastDetectors_ = new AMOldDetectorSet(this);
	FastDetectors_->setName("Fast Scan Detectors");

	teyScalerDetector_ = new AMSingleReadOnlyControlDetector("OLDteyScaler", "BL1611-ID-1:mcs00:fbk", AMOldDetector::WaitRead, this);
	teyScalerDetector_->setDescription("OLD TEY");
	detectorRegistry_.append(teyScalerDetector_);
	detectorMap_->insert(teyScalerDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(teyScalerDetector_, qMakePair(XASDetectors(), true));
	detectorMap_->insert(teyScalerDetector_, qMakePair(FastDetectors(), true));
	criticalDetectorsSet_->addDetector(teyScalerDetector_);
	rawDetectorsSet_->addDetector(teyScalerDetector_);
	connect(teyScalerDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));


	tfyScalerDetector_ = new AMSingleReadOnlyControlDetector("OLDtfyScaler", "BL1611-ID-1:mcs02:fbk", AMOldDetector::WaitRead, this);
	tfyScalerDetector_->setDescription("OLD TFY Diode");
	detectorRegistry_.append(tfyScalerDetector_);
	detectorMap_->insert(tfyScalerDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(tfyScalerDetector_, qMakePair(XASDetectors(), true));
	detectorMap_->insert(tfyScalerDetector_, qMakePair(FastDetectors(), true));
	criticalDetectorsSet_->addDetector(tfyScalerDetector_);
	rawDetectorsSet_->addDetector(tfyScalerDetector_);
	connect(tfyScalerDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));
	//connect(tfyHVToggle_, SIGNAL(valueChanged(double)), this, SIGNAL(detectorHVChanged()));

	/*
	pgtDetector_ = new CLSPGTDetector("OLDpgt", "MCA1611-01", createHVPGTOnActions(), createHVPGTOffActions(), AMOldDetector::WaitRead, this);
	pgtDetector_->setDescription("OLD SDD");
	detectorRegistry_.append(pgtDetector_);
	connect(pgtDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));
	detectorMap_->insert(pgtDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(pgtDetector_, qMakePair(XASDetectors(), false));
	// PGT looks broken, so don't include it as a critical detector any more
	//criticalDetectorsSet_->addDetector(pgtDetector_);
	rawDetectorsSet_->addDetector(pgtDetector_);
	*/

	oos65000Detector_ = new CLSOceanOptics65000Detector("OLDoos65000", "SA0000-03", AMOldDetector::WaitRead, this);
	oos65000Detector_->setDescription("OLD OceanOptics 65000");
	detectorRegistry_.append(oos65000Detector_);
	connect(oos65000Detector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));
	detectorMap_->insert(oos65000Detector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(oos65000Detector_, qMakePair(XASDetectors(), false));
	rawDetectorsSet_->addDetector(oos65000Detector_);

	i0ScalerDetector_ = new AMSingleReadOnlyControlDetector("OLDI0Scaler", "BL1611-ID-1:mcs01:fbk", AMOldDetector::WaitRead, this);
	i0ScalerDetector_->setDescription("OLD I0");
	detectorRegistry_.append(i0ScalerDetector_);
	detectorMap_->insert(i0ScalerDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(i0ScalerDetector_, qMakePair(feedbackDetectors(), false));
	criticalDetectorsSet_->addDetector(i0ScalerDetector_);
	rawDetectorsSet_->addDetector(i0ScalerDetector_);
	connect(i0ScalerDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	eVFbkDetector_ = new AMSingleReadOnlyControlDetector("OLDeVFbk", "BL1611-ID-1:Energy:fbk", AMOldDetector::ImmediateRead, this);
	eVFbkDetector_->setDescription("OLD Energy Feedback");
	detectorRegistry_.append(eVFbkDetector_);
	detectorMap_->insert(eVFbkDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(eVFbkDetector_, qMakePair(feedbackDetectors(), false));
	criticalDetectorsSet_->addDetector(eVFbkDetector_);
	rawDetectorsSet_->addDetector(eVFbkDetector_);
	connect(eVFbkDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	photodiodeScalerDetector_ = new AMSingleReadOnlyControlDetector("OLDphotodiodeScaler", "BL1611-ID-1:mcs03:fbk", AMOldDetector::WaitRead, this);
	photodiodeScalerDetector_->setDescription("OLD Photodiode");
	detectorRegistry_.append(photodiodeScalerDetector_);
	detectorMap_->insert(photodiodeScalerDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(photodiodeScalerDetector_, qMakePair(feedbackDetectors(), false));
	criticalDetectorsSet_->addDetector(photodiodeScalerDetector_);
	rawDetectorsSet_->addDetector(photodiodeScalerDetector_);
	connect(photodiodeScalerDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	encoderUpDetector_ = new AMSingleReadOnlyControlDetector("OLDencoderUp", "BL1611-ID-1:mcs04:fbk", AMOldDetector::WaitRead, this);
	encoderUpDetector_->setDescription("OLD Encoder Up Counts");
	detectorRegistry_.append(encoderUpDetector_);
	detectorMap_->insert(encoderUpDetector_, qMakePair(allDetectors(), false));
	rawDetectorsSet_->addDetector(encoderUpDetector_);
	connect(encoderUpDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	encoderDownDetector_ = new AMSingleReadOnlyControlDetector("OLDencoderDown", "BL1611-ID-1:mcs04:fbk", AMOldDetector::WaitRead, this);
	encoderDownDetector_->setDescription("OLD Encoder Down Counts");
	detectorRegistry_.append(encoderDownDetector_);
	rawDetectorsSet_->addDetector(encoderDownDetector_);
	detectorMap_->insert(encoderDownDetector_, qMakePair(allDetectors(), false));
	connect(encoderDownDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	ringCurrentDetector_ = new AMSingleReadOnlyControlDetector("OLDringCurrent", "PCT1402-01:mA:fbk", AMOldDetector::ImmediateRead, this);
	ringCurrentDetector_->setDescription("OLD Ring Current");
	detectorRegistry_.append(ringCurrentDetector_);
	rawDetectorsSet_->addDetector(ringCurrentDetector_);
	detectorMap_->insert(ringCurrentDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(ringCurrentDetector_, qMakePair(feedbackDetectors(), false));
	connect(ringCurrentDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	filterPD1ScalarDetector_ = new AMSingleReadOnlyControlDetector("OLDfilterPD1Current", "BL1611-ID-1:mcs06:fbk", AMOldDetector::ImmediateRead, this);
	filterPD1ScalarDetector_->setDescription("OLD Aux 1 Diode");
	detectorRegistry_.append(filterPD1ScalarDetector_);
	rawDetectorsSet_->addDetector(filterPD1ScalarDetector_);
	detectorMap_->insert(filterPD1ScalarDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(filterPD1ScalarDetector_, qMakePair(XASDetectors(), false));
	connect(filterPD1ScalarDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	filterPD2ScalarDetector_ = new AMSingleReadOnlyControlDetector("OLDfilterPD2Current", "BL1611-ID-1:mcs07:fbk", AMOldDetector::ImmediateRead, this);
	filterPD2ScalarDetector_->setDescription("OLD Aux 2 Diode");
	detectorRegistry_.append(filterPD2ScalarDetector_);
	rawDetectorsSet_->addDetector(filterPD2ScalarDetector_);
	detectorMap_->insert(filterPD2ScalarDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(filterPD2ScalarDetector_, qMakePair(XASDetectors(), false));
	connect(filterPD2ScalarDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	filterPD3ScalarDetector_ = new AMSingleReadOnlyControlDetector("OLDfilterPD3Current", "BL1611-ID-1:mcs08:fbk", AMOldDetector::ImmediateRead, this);
	filterPD3ScalarDetector_->setDescription("OLD Aux 3 Diode");
	detectorRegistry_.append(filterPD3ScalarDetector_);
	rawDetectorsSet_->addDetector(filterPD3ScalarDetector_);
	detectorMap_->insert(filterPD3ScalarDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(filterPD3ScalarDetector_, qMakePair(XASDetectors(), false));
	connect(filterPD3ScalarDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	filterPD4ScalarDetector_ = new AMSingleReadOnlyControlDetector("OLDfilterPD4Current", "BL1611-ID-1:mcs09:fbk", AMOldDetector::ImmediateRead, this);
	filterPD4ScalarDetector_->setDescription("OLD Aux 4 Diode");
	detectorRegistry_.append(filterPD4ScalarDetector_);
	rawDetectorsSet_->addDetector(filterPD4ScalarDetector_);
	detectorMap_->insert(filterPD4ScalarDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(filterPD4ScalarDetector_, qMakePair(XASDetectors(), false));
	connect(filterPD4ScalarDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	newAmptekSDD1_ = new CLSAmptekSDD123DetectorNew("AmptekSDD1", "Amptek SDD 1", "amptek:sdd1", this);
	newAmptekSDD2_ = new CLSAmptekSDD123DetectorNew("AmptekSDD2", "Amptek SDD 2", "amptek:sdd2", this);
	newAmptekSDD3_ = new CLSAmptekSDD123DetectorNew("AmptekSDD3", "Amptek SDD 3", "amptek:sdd3", this);
	newAmptekSDD4_ = new CLSAmptekSDD123DetectorNew("AmptekSDD4", "Amptek SDD 4", "amptek:sdd4", this);
	newAmptekSDD5_ = new CLSAmptekSDD123DetectorNew("AmptekSDD5", "Amptek SDD 5", "amptek:sdd5", this);
	newAmptekSDD5_->setEVPerBin(2.25);

	//newPGTDetector_ = new CLSPGTDetectorV2("PGT", "PGT", "MCA1611-01", this);
	newQE65000Detector_ = new CLSQE65000Detector("QE65000", "QE 65000", "SA0000-03", this);
	newTEYDetector_ = new CLSAdvancedScalerChannelDetector("TEY", "TEY", scaler_, 0, this);
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

	newDetectorSet_ = new AMDetectorGroup("All Detectors", this);
	newDetectorSet_->addDetector(newAmptekSDD1_);
	newDetectorSet_->addDetector(newAmptekSDD2_);
	newDetectorSet_->addDetector(newAmptekSDD3_);
	newDetectorSet_->addDetector(newAmptekSDD4_);
	newDetectorSet_->addDetector(newAmptekSDD5_);
	//newDetectorSet_->addDetector(newPGTDetector_);
	newDetectorSet_->addDetector(newQE65000Detector_);
	newDetectorSet_->addDetector(newTEYDetector_);
	newDetectorSet_->addDetector(newTFYDetector_);
	newDetectorSet_->addDetector(newI0Detector_);
	newDetectorSet_->addDetector(newPDDetector_);

	XASDetectorGroup_ = new AMDetectorGroup("XAS Detectors", this);
	XASDetectorGroup_->addDetector(newAmptekSDD1_);
	XASDetectorGroup_->addDetector(newAmptekSDD2_);
	XASDetectorGroup_->addDetector(newAmptekSDD3_);
	XASDetectorGroup_->addDetector(newAmptekSDD4_);
	XASDetectorGroup_->addDetector(newAmptekSDD5_);
	//XASDetectorGroup_->addDetector(newPGTDetector_);
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

	unrespondedDetectors_ = detectorRegistry_;
	QTimer::singleShot(10000, this, SLOT(ensureDetectorTimeout()));

	synchronizedDwellTime_ = new CLSSynchronizedDwellTime("BL1611-ID-1:dwell", this);
	synchronizedDwellTime_->addElement(0);
	synchronizedDwellTime_->addElement(1);
	synchronizedDwellTime_->addElement(2);
	synchronizedDwellTime_->addElement(3);
	synchronizedDwellTime_->addElement(4);

	fluxOptimization_ = new SGMFluxOptimization(this);
	fluxOptimization_->setDescription("Flux");
	resolutionOptimization_ = new SGMResolutionOptimization(this);
	resolutionOptimization_->setDescription("Resolution");
	fluxResolutionSet_ = new AMControlOptimizationSet(this);
	fluxResolutionSet_->setName("Flux/Resolution");
	fluxResolutionSet_->addControl(grating_);
	fluxResolutionSet_->addControl(harmonic_);
	fluxResolutionSet_->addControl(exitSlitGap_);
	unconnectedSets_.append(fluxResolutionSet_);
	connect(fluxResolutionSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));
	((AMControlOptimizationSet*)fluxResolutionSet_)->addOptimization(fluxOptimization_);
	((AMControlOptimizationSet*)fluxResolutionSet_)->addOptimization(resolutionOptimization_);

	trackingSet_ = new AMControlSet(this);
	trackingSet_->setName("Tracking");
	trackingSet_->addControl(undulatorTracking_);
	trackingSet_->addControl(monoTracking_);
	trackingSet_->addControl(exitSlitTracking_);
	unconnectedSets_.append(trackingSet_);
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

	connect(this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)), this, SLOT(onDetectorAvailabilityChanged(AMOldDetector*,bool)));

	currentSamplePlate_ = 0;//NULL

	setupExposedControls();
	setupExposedDetectors();
}

SGMBeamline::~SGMBeamline()
{
}

bool SGMBeamline::isConnected() const{
	return criticalControlsSet_->isConnected() && criticalDetectorsSet_->isConnected();
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
	allUnconnected.append(criticalDetectorsSet_->unconnected());
	return allUnconnected;
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

int SGMBeamline::synchronizedDwellTimeDetectorIndex(AMOldDetector *detector) const{
	/*if(detector == pgtDetector_)
		return 2;
	else*/ if(detector == oos65000Detector_)
		return 3;
	else
		return -1;
}

AMAction3::ActionValidity SGMBeamline::validateAction(AMAction3 *action){
	AMScanAction *scanAction = qobject_cast<AMScanAction*>(action);
	if(scanAction){
		AMScanActionInfo *scanActionInfo = qobject_cast<AMScanActionInfo*>(scanAction->info());
		SGMXASScanConfiguration *oldXASScanConfiguration = qobject_cast<SGMXASScanConfiguration*>(scanActionInfo->config());
		SGMFastScanConfiguration *oldFastScanConfiguration = qobject_cast<SGMFastScanConfiguration*>(scanActionInfo->config());
		if(oldXASScanConfiguration || oldFastScanConfiguration)
			return AMAction3::ActionNeverValid;
	}

	return AMAction3::ActionCurrentlyValid;
}

QString SGMBeamline::validateActionMessage(AMAction3 *action){
	AMScanAction *scanAction = qobject_cast<AMScanAction*>(action);
	if(scanAction){
		AMScanActionInfo *scanActionInfo = qobject_cast<AMScanActionInfo*>(scanAction->info());
		SGMXASScanConfiguration *oldXASScanConfiguration = qobject_cast<SGMXASScanConfiguration*>(scanActionInfo->config());
		SGMFastScanConfiguration *oldFastScanConfiguration = qobject_cast<SGMFastScanConfiguration*>(scanActionInfo->config());
		if(oldXASScanConfiguration)
			return QString("The SGM Beamline no longer supports this type of XAS Scan configuration. While you may inspect the configuration you cannot run it. Please transfer these settings to a new scan configuration.");
		if(oldFastScanConfiguration)
			return QString("The SGM Beamline no longer supports this type of Fast Scan configuration. While you may inspect the configuration you cannot run it. Please transfer these settings to a new scan configuration.");
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
		return AMSample(currentId, AMUser::user()->database()).name();
}

AMOldDetector* SGMBeamline::teyDetector() const {
	return teyScalerDetector_;
}

AMOldDetector* SGMBeamline::tfyDetector() const {
	return tfyScalerDetector_;
}

/*
AMOldDetector* SGMBeamline::pgtDetector() const {
	return pgtDetector_;
}
*/

AMOldDetector* SGMBeamline::oos65000Detector() const {
	return oos65000Detector_;
}

AMOldDetector* SGMBeamline::i0Detector() const {
	return i0ScalerDetector_;
}

AMOldDetector* SGMBeamline::eVFbkDetector() const {
	return eVFbkDetector_;
}

AMOldDetector* SGMBeamline::photodiodeDetector() const {
	return photodiodeScalerDetector_;
}

AMOldDetector* SGMBeamline::encoderUpDetector() const {
	return encoderUpDetector_;
}

AMOldDetector* SGMBeamline::encoderDownDetector() const {
	return encoderDownDetector_;
}

AMOldDetector* SGMBeamline::ringCurrentDetector() const {
	return ringCurrentDetector_;
}

AMOldDetector* SGMBeamline::filterPD1ScalarDetector() const {
	return filterPD1ScalarDetector_;
}

AMOldDetector* SGMBeamline::filterPD2ScalarDetector() const {
	return filterPD2ScalarDetector_;
}

AMOldDetector* SGMBeamline::filterPD3ScalarDetector() const {
	return filterPD3ScalarDetector_;
}

AMOldDetector* SGMBeamline::filterPD4ScalarDetector() const {
	return filterPD4ScalarDetector_;
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

/*
AMDetector* SGMBeamline::newPGTDetector() const {
	return newPGTDetector_;
}
*/

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

AMBeamlineListAction* SGMBeamline::createBeamOnActions(){
	if(!beamOnControlSet_->isConnected())
		return 0;// NULL
	AMBeamlineParallelActionsList *beamOnActionsList = new AMBeamlineParallelActionsList();
	AMBeamlineListAction *beamOnAction = new AMBeamlineListAction(beamOnActionsList);
	// Action to turn on beam for SGM:
	// Set beamOn to "1"
	// Set fastShutterVoltage to "0 V"
	AMBeamlineControlMoveAction *beamOnAction1 = new AMBeamlineControlMoveAction(beamOn());
	beamOnAction1->setSetpoint(1);
	AMBeamlineControlMoveAction *beamOnAction2 = new AMBeamlineControlMoveAction(fastShutterVoltage());
	beamOnAction2->setSetpoint(0);

	beamOnActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	beamOnActionsList->appendAction(0, beamOnAction1);
	beamOnActionsList->appendAction(0, beamOnAction2);
	return beamOnAction;
}

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
AMAction3* SGMBeamline::createBeamOnActions3(){
	if(!beamOnControlSet_->isConnected())
		return 0;

	AMListAction3 *beamOnActionsList = new AMListAction3(new AMListActionInfo3("SGM Beam On", "SGM Beam On"), AMListAction3::Parallel);

	AMControlInfo beamOnSetpoint = beamOn_->toInfo();
	beamOnSetpoint.setValue(1);
	AMControlMoveAction3 *beamOnAction = new AMControlMoveAction3(new AMControlMoveActionInfo3(beamOnSetpoint), beamOn_);
	beamOnActionsList->addSubAction(beamOnAction);

	AMControlInfo fastShutterSetpoint = fastShutterVoltage_->toInfo();
	fastShutterSetpoint.setValue(0);
	AMControlMoveAction3 *fastShutterAction = new AMControlMoveAction3(new AMControlMoveActionInfo3(fastShutterSetpoint), fastShutterVoltage_);
	beamOnActionsList->addSubAction(fastShutterAction);

	return beamOnActionsList;
}

AMBeamlineListAction* SGMBeamline::createStopMotorsAction(){
	AMBeamlineParallelActionsList *stopMotorsActionsList = new AMBeamlineParallelActionsList();
	AMBeamlineListAction *stopMotorsAction = new AMBeamlineListAction(stopMotorsActionsList);
	// Action to stop motors for SGM:
	// Stop mono, exit slit,
	AMBeamlineControlStopAction *stopMotorsAction1 = new AMBeamlineControlStopAction(mono());
	AMBeamlineControlStopAction *stopMotorsAction2 = new AMBeamlineControlStopAction(exitSlit());
	AMBeamlineControlStopAction *stopMotorsAction3 = new AMBeamlineControlStopAction(undulator());

	stopMotorsActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	stopMotorsActionsList->appendAction(0, stopMotorsAction1);
	stopMotorsActionsList->appendAction(0, stopMotorsAction2);
	stopMotorsActionsList->appendAction(0, stopMotorsAction3);
	return stopMotorsAction;
}

AMBeamlineListAction* SGMBeamline::createGoToTransferPositionActions(){
	AMBeamlineParallelActionsList *gotoTransferPositionActionsList = new AMBeamlineParallelActionsList();
	AMBeamlineListAction *gotoTransferPositionAction = new AMBeamlineListAction(gotoTransferPositionActionsList);
	// Action to stop motors for SGM:
	// Stop mono, exit slit,
	AMBeamlineControlMoveAction *gotoTransferPositionAction1 = new AMBeamlineControlMoveAction(ssaManipulatorX());
	gotoTransferPositionAction1->setSetpoint(0.0);
	AMBeamlineControlMoveAction *gotoTransferPositionAction2 = new AMBeamlineControlMoveAction(ssaManipulatorY());
	gotoTransferPositionAction2->setSetpoint(-13.17);
	AMBeamlineControlMoveAction *gotoTransferPositionAction3 = new AMBeamlineControlMoveAction(ssaManipulatorZ());
	gotoTransferPositionAction3->setSetpoint(-77.0);
	AMBeamlineControlMoveAction *gotoTransferPositionAction4 = new AMBeamlineControlMoveAction(ssaManipulatorRot());
	gotoTransferPositionAction4->setSetpoint(0.0);

	gotoTransferPositionActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	gotoTransferPositionActionsList->appendAction(0, gotoTransferPositionAction1);
	gotoTransferPositionActionsList->appendAction(0, gotoTransferPositionAction2);
	gotoTransferPositionActionsList->appendAction(0, gotoTransferPositionAction3);
	gotoTransferPositionActionsList->appendAction(0, gotoTransferPositionAction4);
	return gotoTransferPositionAction;
}

AMBeamlineListAction* SGMBeamline::createGoToMeasurementPositionActions(){
	AMBeamlineParallelActionsList *gotoMeasurementPositionActionsList = new AMBeamlineParallelActionsList();
	AMBeamlineListAction *gotoMeasurementPositionAction = new AMBeamlineListAction(gotoMeasurementPositionActionsList);
	// Action to stop motors for SGM:
	// Stop mono, exit slit,
	AMBeamlineControlMoveAction *gotoMeasurementPositionAction1 = new AMBeamlineControlMoveAction(ssaManipulatorX());
	gotoMeasurementPositionAction1->setSetpoint(0.0);
	AMBeamlineControlMoveAction *gotoMeasurementPositionAction2 = new AMBeamlineControlMoveAction(ssaManipulatorY());
	gotoMeasurementPositionAction2->setSetpoint(0.0);
	AMBeamlineControlMoveAction *gotoMeasurementPositionAction3 = new AMBeamlineControlMoveAction(ssaManipulatorZ());
	gotoMeasurementPositionAction3->setSetpoint(0.0);
	AMBeamlineControlMoveAction *gotoMeasurementPositionAction4 = new AMBeamlineControlMoveAction(ssaManipulatorRot());
	gotoMeasurementPositionAction4->setSetpoint(0.0);

	gotoMeasurementPositionActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	gotoMeasurementPositionActionsList->appendAction(0, gotoMeasurementPositionAction1);
	gotoMeasurementPositionActionsList->appendAction(0, gotoMeasurementPositionAction2);
	gotoMeasurementPositionActionsList->appendAction(0, gotoMeasurementPositionAction3);
	gotoMeasurementPositionActionsList->appendAction(0, gotoMeasurementPositionAction4);
	return gotoMeasurementPositionAction;
}

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHV106OnActions(){
	AMBeamlineHighVoltageChannelToggleAction *onAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannel106());
	onAction->setSetpoint(AMHighVoltageChannel::isPowerOn);
	return onAction;
}

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHV106OffActions(){
	AMBeamlineHighVoltageChannelToggleAction *offAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannel106());
	offAction->setSetpoint(AMHighVoltageChannel::isPowerOff);
	return offAction;
}

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHV109OnActions(){
	AMBeamlineHighVoltageChannelToggleAction *onAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannel109());
	onAction->setSetpoint(AMHighVoltageChannel::isPowerOn);
	return onAction;
}

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHV109OffActions(){
	AMBeamlineHighVoltageChannelToggleAction *offAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannel109());
	offAction->setSetpoint(AMHighVoltageChannel::isPowerOff);
	return offAction;
}

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHVPGTOnActions(){
	AMBeamlineHighVoltageChannelToggleAction *onAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannelPGT());
	onAction->setSetpoint(AMHighVoltageChannel::isPowerOn);
	return onAction;
}

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHVPGTOffActions(){
	AMBeamlineHighVoltageChannelToggleAction *offAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannelPGT());
	offAction->setSetpoint(AMHighVoltageChannel::isPowerOff);
	return offAction;
}

CLSSIS3820Scaler* SGMBeamline::scaler(){
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

QList<AMOldDetector*> SGMBeamline::possibleDetectorsForSet(AMOldDetectorSet *set){
	QPair<AMOldDetectorSet*, bool> tempDetectorSet;
	QList<AMOldDetector*> retVal;
	QMultiMap<AMOldDetector*, QPair<AMOldDetectorSet*, bool> >::iterator i = detectorMap_->begin();
	while(i != detectorMap_->end()){
		tempDetectorSet = i.value();
		if(tempDetectorSet.first == set)
			retVal.append(i.key());
		++i;
	}
	return retVal;
}

void SGMBeamline::setCurrentSamplePlate(AMSamplePlate *newSamplePlate){
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
	//qdebug() << "Critical controls are connected: " << criticalControlsSet_->isConnected();
	//qdebug() << "Critical detectors are connected: " << criticalDetectorsSet_->isConnected();
	emit criticalConnectionsChanged();
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

void SGMBeamline::onActiveEndstationChanged(double value){
	emit currentEndstationChanged((SGMBeamlineInfo::sgmEndstation)value);
}

void SGMBeamline::onMirrorStripeChanged(double value){
	emit currentMirrorStripeChanged((SGMBeamlineInfo::sgmMirrorStripe)value);
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

void SGMBeamline::onDetectorAvailabilityChanged(AMOldDetector *detector, bool isAvailable){
	QList<QPair<AMOldDetectorSet*, bool> > detectorSets = detectorMap_->values(detector);
	if(isAvailable){

		bool hasDetector;
		for(int x = 0; x < detectorSets.count(); x++){
			hasDetector = false;
			for(int y = 0; y < detectorSets.at(x).first->count(); y++)
				if(detectorSets.at(x).first->detectorAt(y)->detectorName() == detector->detectorName())
					hasDetector = true;
			if(!hasDetector){
				//qdebug() << "Adding detector " << detector->detectorName() << " as default " << detectorSets.at(x).second;
				detectorSets.at(x).first->addDetector(detector, detectorSets.at(x).second);
			}
		}
	}
	else{
		for(int x = 0; x < detectorSets.count(); x++){
			for(int y = 0; y < detectorSets.at(x).first->count(); y++)
				if(detectorSets.at(x).first->detectorAt(y)->detectorName() == detector->detectorName())
					detectorSets.at(x).first->removeDetector(detector);
		}
	}

	if(unrespondedDetectors_.contains(detector))
		unrespondedDetectors_.removeAll(detector);

	if(unrespondedDetectors_.count() == 0 && !beamlineIsInitialized_)
		computeBeamlineInitialized();
}

void SGMBeamline::ensureDetectorTimeout(){
	if(unrespondedDetectors_.count() == 0)
		return;

	while(unrespondedDetectors_.count() > 0)
		onDetectorAvailabilityChanged(unrespondedDetectors_.last(), false);
}

void SGMBeamline::computeBeamlineInitialized(){
	if(!beamlineIsInitialized_){
		beamlineIsInitialized_ = true;
		emit beamlineInitialized();
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
	//energy_ = new AMPVwStatusControl("energy", sgmPVName+":fbk", sgmPVName, "BL1611-ID-1:ready", sgmPVName, this, 0.25);
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
	AMPVwStatusControl *mono = new AMPVwStatusControl("mono", sgmPVName+":enc:fbk", sgmPVName+":encTarget", sgmPVName+":status", "SMTR16114I1002:stop", energy_, 5, 2.0, new AMControlStatusCheckerCLSMAXv(), 1);
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
	//exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", sgmPVName+":Y:mm:fbk", sgmPVName+":Y:mm:encsp", "SMTR16114I1017:status", "SMTR16114I1017:stop", this, 0.5);
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", sgmPVName+":Y:mm:fbk", "BL1611-ID-1:AddOns:ExitSlitGap:Y:mm", "BL1611-ID-1:AddOns:ExitSlitGap:Y:status", "SMTR16114I1017:stop", this, 0.5);
	//((AMPVwStatusControl*)exitSlitGap_)->setMoveStartTolerance(0.02);
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

	hvChannel106_ = new CLSCAEN2527HVChannel("Ch 6+", "PS1611401:106", AMHighVoltageChannel::positive, this);
	hvChannel109_ = new CLSCAEN2527HVChannel("Ch 9-", "PS1611401:109", AMHighVoltageChannel::negative, this);
	hvChannelPGT_ = new CLSPGT8000HVChannel("SGM PGT", "MCA1611-01", this);

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


	if(amNames2pvNames_.lookupFailed())
		AMErrorMon::alert(this, SGMBEAMLINE_PV_NAME_LOOKUPS_FAILED, "PV Name lookups in the SGM Beamline failed");
}

void SGMBeamline::setupExposedControls(){
	addExposedControl(ssaManipulatorX_);
	addExposedControl(ssaManipulatorY_);
	addExposedControl(ssaManipulatorZ_);
	addExposedControl(energy_);
	addExposedControl(masterDwell_);
}

void SGMBeamline::setupExposedDetectors(){
	addExposedDetector(newAmptekSDD1_);
	addExposedDetector(newAmptekSDD2_);
	addExposedDetector(newAmptekSDD3_);
	addExposedDetector(newAmptekSDD4_);
	addExposedDetector(newAmptekSDD5_);
	//addExposedDetector(newPGTDetector_);
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

	addExposedDetectorGroup(XASDetectorGroup_);
	addExposedDetectorGroup(FastDetectorGroup_);
}

SGMBeamline* SGMBeamline::sgm() {

	if(instance_ == 0)
		instance_ = new SGMBeamline();

	return static_cast<SGMBeamline*>(instance_);

}
