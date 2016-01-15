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


#include "BioXASSideBeamline.h"

#include "beamline/AMDetector.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "util/AMPeriodicTable.h"

#include "beamline/AMDetectorTriggerSource.h"

BioXASSideBeamline::~BioXASSideBeamline()
{

}

bool BioXASSideBeamline::isConnected() const
{
	bool connected = (
				// Front-end BioXAS components.
				BioXASBeamline::isConnected() &&

				carbonFilterFarm_ && carbonFilterFarm_->isConnected() &&
				m1Mirror_ && m1Mirror_->isConnected() &&
				mono_ && mono_->isConnected() &&
				m2Mirror_ && m2Mirror_->isConnected() &&
				endstationShutter_ && endstationShutter_->isConnected() &&

				shutters_ && shutters_->isConnected() &&

				beamStatus_ && beamStatus_->isConnected() &&

				jjSlits_ && jjSlits_->isConnected() &&
				xiaFilters_ && xiaFilters_->isConnected() &&
				dbhrMirrors_ && dbhrMirrors_->isConnected() &&
				standardsWheel_ && standardsWheel_->isConnected() &&
				cryostatStage_ && cryostatStage_->isConnected() &&
				endstationTable_ && endstationTable_->isConnected() &&
				filterFlipper_ && filterFlipper_->isConnected() &&

				scaler_ && scaler_->isConnected() &&
				i0Keithley_ && i0Keithley_->isConnected() &&
				i0Detector_ && i0Detector_->isConnected() &&
				i1Keithley_ && i1Keithley_->isConnected() &&
				i1Detector_ && i1Detector_->isConnected() &&
				i2Keithley_ && i2Keithley_->isConnected() &&
				i2Detector_ && i2Detector_->isConnected() &&

				detectorStageLateral_ && detectorStageLateral_->isConnected() &&

				utilities_ && utilities_->isConnected() &&

				zebra_ && zebra_->isConnected() &&

				fastShutter_ && fastShutter_->isConnected() &&

				ge32ElementDetector_ && ge32ElementDetector_->isConnected()
				);

	return connected;
}

QList<AMControl *> BioXASSideBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType category) const
{
	QList<AMControl *> matchedMotors;

	switch (category) {
	case BioXASBeamlineDef::FilterMotor: // BioXAS Filter motors
		matchedMotors.append(carbonFilterFarm_->upstreamActuator()->position());
		matchedMotors.append(carbonFilterFarm_->downstreamActuator()->position());
		break;

	case BioXASBeamlineDef::M1Motor:	// BioXAS M1 motors
		matchedMotors.append(m1Mirror_->upstreamInboardMotor());
		matchedMotors.append(m1Mirror_->upstreamOutboardMotor());
		matchedMotors.append(m1Mirror_->downstreamMotor());
		matchedMotors.append(m1Mirror_->stripeSelectMotor());
		matchedMotors.append(m1Mirror_->yawMotor());
		matchedMotors.append(m1Mirror_->upstreamBenderMotor());
		matchedMotors.append(m1Mirror_->downstreamBenderMotor());
		matchedMotors.append(m1Mirror_->mask()->upperSlitBlade());
		break;

	case BioXASBeamlineDef::MaskMotor:	// BioXAS Variable Mask motors
		matchedMotors.append(mono_->mask()->upperBlade());
		matchedMotors.append(mono_->mask()->lowerBlade());
		break;

	case BioXASBeamlineDef::MonoMotor:	// Mono motors
		matchedMotors.append(mono_->paddle());
		matchedMotors.append(mono_->bragg());
		matchedMotors.append(mono_->vertical());
		matchedMotors.append(mono_->lateral());
		matchedMotors.append(mono_->crystalChange());
		matchedMotors.append(mono_->crystal1Pitch());
		matchedMotors.append(mono_->crystal1Roll());
		matchedMotors.append(mono_->crystal2Pitch());
		matchedMotors.append(mono_->crystal2Roll());
		break;

	case BioXASBeamlineDef::M2Motor:	// BioXAS M2 motors
		matchedMotors.append(m2Mirror_->upstreamInboardMotor());
		matchedMotors.append(m2Mirror_->upstreamOutboardMotor());
		matchedMotors.append(m2Mirror_->downstreamMotor());
		matchedMotors.append(m2Mirror_->stripeSelectMotor());
		matchedMotors.append(m2Mirror_->yawMotor());
		matchedMotors.append(m2Mirror_->upstreamBenderMotor());
		matchedMotors.append(m2Mirror_->downstreamBenderMotor());
		break;

	case BioXASBeamlineDef::PseudoM1Motor: // BioXAS Pseudo M1 motor
		matchedMotors.append(m1Mirror_->roll());
		matchedMotors.append(m1Mirror_->pitch());
		matchedMotors.append(m1Mirror_->height());
		matchedMotors.append(m1Mirror_->yaw());
		matchedMotors.append(m1Mirror_->lateral());
		break;

	case BioXASBeamlineDef::PseudoM2Motor: // BioXAS Pseudo M2 motor
		matchedMotors.append(m2Mirror_->roll());
		matchedMotors.append(m2Mirror_->pitch());
		matchedMotors.append(m2Mirror_->yaw());
		matchedMotors.append(m2Mirror_->height());
		matchedMotors.append(m2Mirror_->lateral());
		break;

	case BioXASBeamlineDef::PseudoMonoMotor: // BioXAS Pseudo Mono motor
		matchedMotors.append(mono_->energy());
		matchedMotors.append(mono_->region());
		break;

	default:
		qDebug() << "ERROR: invalid BioXAS Motor category: " << category;
		break;
	}

	return matchedMotors;
}

AMBasicControlDetectorEmulator* BioXASSideBeamline::scalerDwellTimeDetector() const
{
	return detectorForControl(scaler_->dwellTimeControl());
}

AMBasicControlDetectorEmulator* BioXASSideBeamline::encoderEnergyFeedbackDetector() const
{
	return detectorForControl(mono_->encoderEnergy());
}

AMBasicControlDetectorEmulator* BioXASSideBeamline::stepEnergyFeedbackDetector() const
{
	return detectorForControl(mono_->stepEnergy());
}

AMBasicControlDetectorEmulator* BioXASSideBeamline::braggDetector() const
{
	return detectorForControl(mono_->bragg());
}

AMBasicControlDetectorEmulator* BioXASSideBeamline::braggStepSetpointDetector() const
{
	return detectorForControl(mono_->bragg()->stepSetpointControl());
}

void BioXASSideBeamline::setupComponents()
{
	// Carbon filter farm.

	carbonFilterFarm_ = new BioXASSideCarbonFilterFarm(this);
	connect( carbonFilterFarm_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// M1 mirror.

	m1Mirror_ = new BioXASSideM1Mirror(this);
	connect( m1Mirror_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Mono.

	mono_ = new BioXASSideMonochromator("BioXASSideMonochromator", this);
	connect( mono_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	mono_->setM1MirrorPitchControl(m1Mirror_->pitch());

	// M2 mirror.

	m2Mirror_ = new BioXASSideM2Mirror(this);
	connect( m2Mirror_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Endstation safety shutter.

	endstationShutter_ = new  BioXASEndstationShutter("BioXASSideEndstationShutter", "SSH1607-5-I22-01", this);
	connect( endstationShutter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Shutters.

	shutters_ = new BioXASShutters("BioXASSideShutters", this);
	connect( shutters_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	shutters_->setFrontEndShutters(frontEndShutters_);
	shutters_->setEndstationShutter(endstationShutter_);

	// Beam status.

	beamStatus_ = new BioXASBeamStatus("BioXASSideBeamStatus", this);
	connect( beamStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	beamStatus_->setShutters(shutters_);
	beamStatus_->setValves(valves());
	beamStatus_->setMirrorMaskState(m1Mirror_->mask()->state());
	beamStatus_->setMonoMaskState(mono_->mask()->state());

	// JJ slits.

	jjSlits_ = new CLSJJSlits("JJSlits", "SMTR1607-6-I22-10", "SMTR1607-6-I22-09", "SMTR1607-6-I22-11", "SMTR1607-6-I22-12", this);
	connect( jjSlits_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// XIA filters.

	xiaFilters_ = new BioXASSideXIAFilters(this);
	connect( xiaFilters_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// DBHR mirrors.

	dbhrMirrors_ = new BioXASSideDBHRMirrors(this);
	connect( dbhrMirrors_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Standards wheel.

	standardsWheel_ = new CLSStandardsWheel("StandardsWheel", "SMTR1607-6-I22-19", this);
	connect( standardsWheel_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	standardsWheel_->setName(0, "Cr");
	standardsWheel_->setName(1, "Mn");
	standardsWheel_->setName(2, "Fe");
	standardsWheel_->setName(3, "Co");
	standardsWheel_->setName(4, "Ni");
	standardsWheel_->setName(5, "Cu");
	standardsWheel_->setName(6, "Zn");
	standardsWheel_->setName(7, "As");
	standardsWheel_->setName(8, "Se");
	standardsWheel_->setName(9, "Mo");
	standardsWheel_->setName(10, "Cd");
	standardsWheel_->setName(11, "None");

	// Endstation table.

	endstationTable_ = new BioXASEndstationTable("SideBL endstation table", "BL1607-6-I22", false, this);
	connect( endstationTable_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Detector stage.

	detectorStageLateral_ = new CLSMAXvMotor("SMTR1607-6-I22-16 Side Detector Lateral", "SMTR1607-6-I22-16", "SMTR1607-6-I22-16 Side Detector Lateral", true, 0.05, 2.0, this, ":mm");
	connect( detectorStageLateral_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Cryostat stage.

	cryostatStage_ = new BioXASSideCryostatStage(this);
	connect( cryostatStage_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Filter flipper.
	filterFlipper_ = new BioXASSideFilterFlipper(this);
	connect( filterFlipper_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	filterFlipper_->filters()->setFilter(1, "Cr", 3);
	filterFlipper_->filters()->setFilter(2, "Cr", 6);
	filterFlipper_->filters()->setFilter(3, "Ni", 3);
	filterFlipper_->filters()->setFilter(4, "Ni", 6);
	filterFlipper_->filters()->setFilter(5, "Cu", 3);
	filterFlipper_->filters()->setFilter(6, "Cu", 6);
	filterFlipper_->filters()->setFilter(7, "Zr", 3);
	filterFlipper_->filters()->setFilter(8, "Zr", 6);
	filterFlipper_->filters()->setFilter(9, "Ag", 3);
	filterFlipper_->filters()->setFilter(10, "Ag", 6);

	// Scaler.
	scaler_ = new BioXASSIS3820Scaler("MCS1607-601:mcs", this);
	connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// Scaler channel detectors.

	i0Detector_ = new CLSBasicScalerChannelDetector("I0Detector", "I0 Detector", scaler_, 16, this);
	connect( i0Detector_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	i1Detector_ = new CLSBasicScalerChannelDetector("I1Detector", "I1 Detector", scaler_, 17, this);
	connect( i1Detector_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	i2Detector_ = new CLSBasicScalerChannelDetector("I2Detector", "I2 Detector", scaler_, 18, this);
	connect( i2Detector_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// I0 channel amplifier.

	i0Keithley_ = new CLSKeithley428("I0 Channel", "AMP1607-601", this);
	connect( i0Keithley_, SIGNAL(isConnected(bool)), this, SLOT(updateConnected()) );

	scaler_->channelAt(16)->setCustomChannelName("I0 Channel");
	scaler_->channelAt(16)->setCurrentAmplifier(i0Keithley_);
	scaler_->channelAt(16)->setDetector(i0Detector_);
	scaler_->channelAt(16)->setVoltagRange(0.1, 9.5);
	scaler_->channelAt(16)->setCountsVoltsSlopePreference(0.00001);

	// I1 channel amplifier.

	i1Keithley_ = new CLSKeithley428("I1 Channel", "AMP1607-602", this);
	connect( i1Keithley_, SIGNAL(isConnected(bool)), this, SLOT(updateConnected()) );

	scaler_->channelAt(17)->setCustomChannelName("I1 Channel");
	scaler_->channelAt(17)->setCurrentAmplifier(i1Keithley_);
	scaler_->channelAt(17)->setDetector(i1Detector_);
	scaler_->channelAt(17)->setVoltagRange(0.1, 9.5);
	scaler_->channelAt(17)->setCountsVoltsSlopePreference(0.00001);

	// I2 channel amplifier.

	i2Keithley_ = new CLSKeithley428("I2 Channel", "AMP1607-603", this);
	connect( i2Keithley_, SIGNAL(isConnected(bool)), this, SLOT(updateConnected()) );

	scaler_->channelAt(18)->setCustomChannelName("I2 Channel");
	scaler_->channelAt(18)->setCurrentAmplifier(i2Keithley_);
	scaler_->channelAt(18)->setDetector(i2Detector_);
	scaler_->channelAt(18)->setVoltagRange(0.1, 9.5);
	scaler_->channelAt(18)->setCountsVoltsSlopePreference(0.00001);

	// Zebra
	zebra_ = new BioXASZebra("TRG1607-601", this);
	connect(zebra_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()));

	// The germanium detector.

	ge32ElementDetector_ = new BioXAS32ElementGeDetector("Ge32Element",
							     "Ge 32 Element",
							     zebra_->softInputControlAt(0),
							     zebra_->pulseControlAt(2),
							     this);
	connect( ge32ElementDetector_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	zebraTriggerSource_ = new AMZebraDetectorTriggerSource("ZebraTriggerSource", this);
	zebraTriggerSource_->setTriggerControl(zebra_->softInputControlAt(0));
	scaler_->setTriggerSource(zebraTriggerSource_);
	ge32ElementDetector_->setTriggerSource(zebraTriggerSource_);

	addSynchronizedXRFDetector(ge32ElementDetector_);

	// Utilities.

	utilities_ = new BioXASSideBeamlineUtilities(this);
	connect( utilities_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// The fast shutter.

	fastShutter_ = new BioXASFastShutter("BioXASSideFastShutter", this);
	connect( fastShutter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	fastShutter_->setStatus(new AMSinglePVControl("BioXASSideFastShutterState", "TRG1607-601:OUT2_TTL:STA", this));
	fastShutter_->setOperator(zebra_->softInputControlAt(1));
}

void BioXASSideBeamline::setupControlsAsDetectors()
{
	addControlAsDetector("ScalerDwellTimeFeedback", "ScalerDwellTimeFeedback", scaler_->dwellTimeControl());
	addControlAsDetector("MonoEncoderEnergyFeedback", "MonoEncoderEnergyFeedback", mono_->encoderEnergy());
	addControlAsDetector("MonoStepEnergyFeedback", "MonoStepEnergyFeedback", mono_->stepEnergy());
	addControlAsDetector("MonoStepAngleFeedback", "MonoStepAngleFeedback", mono_->stepBragg());
	addControlAsDetector("MonoStepSetpoint", "MonoStepSetpoint", mono_->bragg()->stepSetpointControl());
}

void BioXASSideBeamline::setupExposedControls()
{
	// M1 mirror controls

	addExposedControl(m1Mirror_->upstreamInboardMotor());
	addExposedControl(m1Mirror_->upstreamOutboardMotor());
	addExposedControl(m1Mirror_->downstreamMotor());
	addExposedControl(m1Mirror_->stripeSelectMotor());
	addExposedControl(m1Mirror_->yaw());
	addExposedControl(m1Mirror_->upstreamBenderMotor());
	addExposedControl(m1Mirror_->downstreamBenderMotor());
	addExposedControl(m1Mirror_->mask()->upperSlitBlade());

	addExposedControl(m1Mirror_->roll());
	addExposedControl(m1Mirror_->pitch());
	addExposedControl(m1Mirror_->height());
	addExposedControl(m1Mirror_->yaw());
	addExposedControl(m1Mirror_->lateral());
	addExposedControl(m1Mirror_->bend());

	// Mono controls.

	addExposedControl(mono_->encoderEnergy());
	addExposedControl(mono_->stepEnergy());
	addExposedControl(mono_->region());
	addExposedControl(mono_->bragg());
	addExposedControl(mono_->bragg()->EGUVelocityControl());
	addExposedControl(mono_->bragg()->EGUBaseVelocityControl());
	addExposedControl(mono_->bragg()->EGUAccelerationControl());
	addExposedControl(mono_->bragg()->preDeadBandControl());
	addExposedControl(mono_->bragg()->postDeadBandControl());
	addExposedControl(mono_->bragg()->encoderFeedbackControl());
	addExposedControl(mono_->bragg()->encoderMovementTypeControl());
	addExposedControl(mono_->bragg()->encoderStepSoftRatioControl());
	addExposedControl(mono_->bragg()->encoderCalibrationSlopeControl());
	addExposedControl(mono_->bragg()->stepCalibrationSlopeControl());
	addExposedControl(mono_->bragg()->retries());
	addExposedControl(mono_->bragg()->stepMotorFeedbackControl());
	addExposedControl(mono_->vertical());
	addExposedControl(mono_->lateral());
	addExposedControl(mono_->crystal1Pitch());
	addExposedControl(mono_->crystal1Roll());
	addExposedControl(mono_->crystal2Pitch());
	addExposedControl(mono_->crystal2Roll());

	// M2 mirror controls.

	addExposedControl(m2Mirror_->upstreamInboardMotor());
	addExposedControl(m2Mirror_->upstreamOutboardMotor());
	addExposedControl(m2Mirror_->downstreamMotor());
	addExposedControl(m2Mirror_->stripeSelectMotor());
	addExposedControl(m2Mirror_->yawMotor());
	addExposedControl(m2Mirror_->upstreamBenderMotor());
	addExposedControl(m2Mirror_->downstreamBenderMotor());
	addExposedControl(m2Mirror_->screen());

	addExposedControl(m2Mirror_->roll());
	addExposedControl(m2Mirror_->pitch());
	addExposedControl(m2Mirror_->height());
	addExposedControl(m2Mirror_->yaw());
	addExposedControl(m2Mirror_->lateral());
	addExposedControl(m2Mirror_->bend());

	// JJ slit controls.

	addExposedControl(jjSlits_->verticalCenterControl());
	addExposedControl(jjSlits_->verticalGapControl());
	addExposedControl(jjSlits_->horizontalCenterControl());
	addExposedControl(jjSlits_->horizontalGapControl());

	// Carbon filter farm controls.

	addExposedControl(carbonFilterFarm_->upstreamActuator()->position());
	addExposedControl(carbonFilterFarm_->upstreamActuator()->window());
	addExposedControl(carbonFilterFarm_->upstreamActuator()->filter());

	addExposedControl(carbonFilterFarm_->downstreamActuator()->position());
	addExposedControl(carbonFilterFarm_->downstreamActuator()->window());
	addExposedControl(carbonFilterFarm_->downstreamActuator()->filter());

	addExposedControl(carbonFilterFarm_->filter());

	// DBHR controls.

	addExposedControl(dbhrMirrors_->pitchControl());
	addExposedControl(dbhrMirrors_->m1VerticalControl());
	addExposedControl(dbhrMirrors_->m2VerticalControl());

	// Detector stage controls.

	addExposedControl(detectorStageLateral_);

	// Cryostat stage controls.

	addExposedControl(cryostatStage_->stageXControl());
	addExposedControl(cryostatStage_->stageYControl());
	addExposedControl(cryostatStage_->stageZControl());

	// Standards wheel.

	addExposedControl(standardsWheel_->wheelControl());

	// Endstation table

	addExposedControl(endstationTable_->heightPVController());
	addExposedControl(endstationTable_->pitchPVController());
	addExposedControl(endstationTable_->lateralPVController());
	addExposedControl(endstationTable_->yawPVController());
}

void BioXASSideBeamline::setupExposedDetectors()
{
	// Add detectors.

	addExposedDetector(i0Detector_);
	addExposedDetector(i1Detector_);
	addExposedDetector(i2Detector_);
	addExposedDetector(ge32ElementDetector_);

	// Add controls as detectors.

	foreach (AMDetector *detector, controlDetectorMap_.values())
		addExposedDetector(detector);
}

BioXASSideBeamline::BioXASSideBeamline()
	: BioXASBeamline("BioXAS Beamline - Side Endstation")
{
	setupComponents();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();
}
