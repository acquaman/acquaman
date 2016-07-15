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


#include "BioXASMainBeamline.h"

BioXASMainBeamline::~BioXASMainBeamline()
{

}

bool BioXASMainBeamline::isConnected() const
{
	bool connected = (
				// Front-end BioXAS components.
				BioXASBeamline::isConnected() &&

				carbonFilterFarm_ && carbonFilterFarm_->isConnected() &&
				m1Mirror_ && m1Mirror_->isConnected() &&
				mono_ && mono_->isConnected() &&
				m2Mirror_ && m2Mirror_->isConnected() &&

				endStationKillSwitch_ && endStationKillSwitch_->isConnected() &&
				beWindow_ && beWindow_->isConnected() &&
				jjSlits_ && jjSlits_->isConnected() &&
				xiaFilters_ && xiaFilters_->isConnected() &&
				dbhrMirrors_ && dbhrMirrors_->isConnected() &&
				standardsWheel_ && standardsWheel_->isConnected() &&
				endstationTable_ && endstationTable_->isConnected() &&
				cryostatStage_ && cryostatStage_->isConnected() &&

				i0Keithley_ && i0Keithley_->isConnected() &&
				i1Keithley_ && i1Keithley_->isConnected() &&
				i2Keithley_ && i2Keithley_->isConnected() &&

				ge32DetectorInboard_ && ge32DetectorInboard_->isConnected() &&

				zebra_ && zebra_->isConnected() &&

				scaler_ && scaler_->isConnected() &&

				i0Detector_ && i0Detector_->isConnected() &&
				i1Detector_ && i1Detector_->isConnected() &&
				i2Detector_ && i2Detector_->isConnected()
				);

	return connected;
}

QList<AMControl *> BioXASMainBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType category)
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
		matchedMotors.append(mono_->maskUpperBlade());
		matchedMotors.append(mono_->maskLowerBlade());
		break;

	case BioXASBeamlineDef::MonoMotor:	// BioXAS Mono motors
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

AMBasicControlDetectorEmulator* BioXASMainBeamline::energySetpointDetector() const
{
	return energySetpointDetector_;
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::encoderEnergyFeedbackDetector() const
{
	return detectorForControl(mono_->encoderEnergy());
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::stepEnergyFeedbackDetector() const
{
	return detectorForControl(mono_->stepEnergy());
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::braggDetector() const
{
	return detectorForControl(mono_->bragg());
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::braggStepSetpointDetector() const
{
	return detectorForControl(mono_->bragg()->stepSetpointControl());
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::braggEncoderStepDegFeedbackDetector() const
{
	return detectorForControl(mono_->encoderStepsDiffControl());
}

bool BioXASMainBeamline::useDiodeDetector(bool useDetector)
{
	bool result = false;

	if (useDetector && setUsingDiodeDetector(true)) {

		// Remove the other 'optional' scaler channel detectors.

		usePIPSDetector(false);
		useLytleDetector(false);

		// Add the detector to the scaler.

		scaler_->addChannelDetector(19, "Diode", diodeDetector());

		result = true;

	} else if (!useDetector && setUsingDiodeDetector(false)) {

		// Remove the detector from the scaler.

		scaler_->removeChannelDetector(19);

		result = true;
	}

	return result;
}

bool BioXASMainBeamline::usePIPSDetector(bool useDetector)
{
	bool result = false;

	if (useDetector && setUsingPIPSDetector(true)) {

		// Remove the other 'optional' scaler channel detectors.

		useDiodeDetector(false);
		useLytleDetector(false);

		// Add the detector to the scaler.

		scaler_->addChannelDetector(19, "PIPS", pipsDetector());

		result = true;

	} else if (!useDetector && setUsingPIPSDetector(false)) {

		// Remove the detector from the scaler.

		scaler_->removeChannelDetector(19);

		result = true;
	}

	return result;
}

bool BioXASMainBeamline::useLytleDetector(bool useDetector)
{
	bool result = false;

	if (useDetector && setUsingLytleDetector(true)) {

		// Remove the other 'optional' scaler channel detectors.

		useDiodeDetector(false);
		usePIPSDetector(false);

		// Add the detector to the scaler.

		scaler_->addChannelDetector(19, "Lytle", lytleDetector());

		result = true;

	} else if (!useDetector && setUsingLytleDetector(false)) {

		// Remove the detector from the scaler.

		scaler_->removeChannelDetector(19);

		result = true;
	}

	return result;
}

void BioXASMainBeamline::setupComponents()
{
	// SOE shutter.

	setSOEShutter(new CLSExclusiveStatesControl("Endstation shutter", "SSH1607-5-I21-01:state", "SSH1607-5-I21-01:opr:open", "SSH1607-5-I21-01:opr:close", this));

	// Utilities - Main valves (non-beampath--beampath valves are added in BioXASBeamline).

	addValve(new CLSExclusiveStatesControl("VVR1607-5-I21-05", "VVR1607-5-I21-05:state", "VVR1607-5-I21-05:opr:open", "VVR1607-5-I21-05:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - Main ion pumps.

	addIonPump(new CLSIonPump("IOP1607-5-I21-01", "IOP1607-5-I21-01", this));
	addIonPump(new CLSIonPump("IOP1607-5-I21-02", "IOP1607-5-I21-02", this));
	addIonPump(new CLSIonPump("IOP1607-5-I21-03", "IOP1607-5-I21-03", this));
	addIonPump(new CLSIonPump("IOP1607-5-I21-04", "IOP1607-5-I21-04", this));
	addIonPump(new CLSIonPump("IOP1607-5-I21-05", "IOP1607-5-I21-05", this));
	addIonPump(new CLSIonPump("IOP1607-5-I21-06", "IOP1607-5-I21-06", this));
	addIonPump(new CLSIonPump("IOP1607-5-I00-06", "IOP1607-5-I00-06", this));

	// Carbon filter farm.

	carbonFilterFarm_ = new BioXASMainCarbonFilterFarm(this);
	connect( carbonFilterFarm_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	// M1 Mirror.

	m1Mirror_ = new BioXASMainM1Mirror(this);
	connect( m1Mirror_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	// Mono.

	mono_ = new BioXASMainMonochromator("BioXASMainMonochromator", this);
	mono_->setM1MirrorPitchControl(m1Mirror_->pitch());
	connect( mono_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	// M2 Mirror.

	m2Mirror_ = new BioXASMainM2Mirror(this);
	connect( m2Mirror_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	// The beam status.

	beamlineStatus_->addMirrorMaskControl(m1Mirror_->mask()->state(), CLSMirrorMaskState::Open);
	beamlineStatus_->addMonoMaskControl(mono_->maskState(), BioXASSSRLMonochromatorMaskState::Open);

	// Kill switch status.

	endStationKillSwitch_ = new AMReadOnlyPVControl("BioXASMainEndStationKillSwitch", "SWES1607-7-01:Em:Off", this);
	connect(endStationKillSwitch_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()));

	// Be window.

	beWindow_ = new CLSMAXvMotor("SMTR1607-7-I21-01", "SMTR1607-7-I21-01", "SMTR1607-7-I21-01", true, 0.01, 2.0, this);
	connect( beWindow_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	// JJ slits.

	jjSlits_ = new AMSlits("BioXASMainJJSlits", this);
	connect( jjSlits_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	jjSlits_->setUpperBlade(new CLSMAXvMotor("SMTR1607-7-I21-11", "SMTR1607-7-I21-11", "SMTR1607-7-I21-11", false, 0.05, 2.0, this));
	jjSlits_->setLowerBlade(new CLSMAXvMotor("SMTR1607-7-I21-10", "SMTR1607-7-I21-10", "SMTR1607-7-I21-10", false, 0.05, 2.0, this));
	jjSlits_->setInboardBlade(new CLSMAXvMotor("SMTR1607-7-I21-12", "SMTR1607-7-I21-12", "SMTR1607-7-I21-12", false, 0.05, 2.0, this));
	jjSlits_->setOutboardBlade(new CLSMAXvMotor("SMTR1607-7-I21-13", "SMTR1607-7-I21-13", "SMTR1607-7-I21-13", false, 0.05, 2.0, this));

	jjSlits_->setVerticalSlitOpenGapValue(30);
	jjSlits_->setVerticalSlitClosedGapValue(0);
	jjSlits_->setHorizontalSlitOpenGapValue(30);
	jjSlits_->setHorizontalSlitClosedGapValue(0);


	// XIA filters.
	xiaFilters_ = new BioXASMainXIAFilters(this);
	connect( xiaFilters_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	// DBHR mirrors.
	dbhrMirrors_ = new BioXASMainDBHRMirrors(this);
	connect( dbhrMirrors_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	// Standards wheel.
	standardsWheel_ = new BioXASMainStandardsWheel(this);
	connect( standardsWheel_, SIGNAL(connectedChanged(bool)), this, SLOT(onBeamlineComponentConnected()) );

	// Endstation table.
	endstationTable_ = new BioXASEndstationTable("MainEndstationTable", "BL1607-7-I21", true, this);
	connect( endstationTable_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	// The cryostat stage.

	cryostatStage_ = new BioXASMainCryostatStage("BioXASMainCryostatStage", this);
	connect( cryostatStage_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	// Zebra.

	zebra_ = new BioXASMainZebra("TRG1607-701", this);
	connect(zebra_, SIGNAL(connectedChanged(bool)), this, SLOT(onBeamlineComponentConnected()));

	// The Zebra trigger source.

	zebraTriggerSource_ = new AMZebraDetectorTriggerSource("ZebraTriggerSource", this);
	zebraTriggerSource_->setTriggerControl(zebra_->softInputControlAt(0));

	// Scaler.

	scaler_ = new BioXASSIS3820Scaler("MCS1607-701:mcs", zebra_->softInputControlAt(2), this);
	connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onBeamlineComponentConnected()) );

	scaler_->setTriggerSource(zebraTriggerSource_);
	scaler_->setInputsModeValuePreference(BioXASSIS3820Scaler::Mode1);
	scaler_->setTriggerSourceModeValuePreference(BioXASSIS3820Scaler::Hardware);

	// I0 channel.

	i0Keithley_ = new CLSKeithley428("AMP1607-701", "AMP1607-701", this);
	connect( i0Keithley_, SIGNAL(isConnected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	scaler_->channelAt(16)->setCurrentAmplifier(i0Keithley_);
	scaler_->channelAt(16)->setVoltagRange(0.1, 9.5);
	scaler_->channelAt(16)->setCountsVoltsSlopePreference(0.00001);

	i0Detector_ = new CLSBasicScalerChannelDetector("I0Detector", "I0", scaler_, 16, this);
	connect( i0Detector_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	addExposedScalerChannelDetector(i0Detector_);

	scaler_->addChannelDetector(16, "I0 Channel", i0Detector_);

	// I1 channel.

	i1Keithley_ = new CLSKeithley428("AMP1607-702", "AMP1607-702", this);
	connect( i1Keithley_, SIGNAL(isConnected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	scaler_->channelAt(17)->setCurrentAmplifier(i1Keithley_);
	scaler_->channelAt(17)->setVoltagRange(0.1, 9.5);
	scaler_->channelAt(17)->setCountsVoltsSlopePreference(0.00001);

	i1Detector_ = new CLSBasicScalerChannelDetector("I1Detector", "I1", scaler_, 17, this);
	connect( i1Detector_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	addExposedScalerChannelDetector(i1Detector_);

	scaler_->addChannelDetector(17, "I1 Channel", i1Detector_);

	// I2 channel.

	i2Keithley_ = new CLSKeithley428("AMP1607-703", "AMP1607-703", this);
	connect( i2Keithley_, SIGNAL(isConnected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	scaler_->channelAt(18)->setCurrentAmplifier(i2Keithley_);
	scaler_->channelAt(18)->setVoltagRange(0.1, 9.5);
	scaler_->channelAt(18)->setCountsVoltsSlopePreference(0.00001);

	i2Detector_ = new CLSBasicScalerChannelDetector("I2Detector", "I2", scaler_, 18, this);
	connect( i2Detector_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	addExposedScalerChannelDetector(i2Detector_);

	scaler_->channelAt(18)->setCustomChannelName("I2 Channel");
	scaler_->channelAt(18)->setCurrentAmplifier(i2Keithley_);
	scaler_->channelAt(18)->setDetector(i2Detector_);
	scaler_->channelAt(18)->setVoltagRange(0.1, 9.5);
	scaler_->channelAt(18)->setCountsVoltsSlopePreference(0.00001);

	scaler_->addChannelDetector(18, "I2 Channel", i2Detector_);

	// 'Misc' channel.

	miscKeithley_ = new CLSKeithley428("AMP1607-704", "AMP1607-704", this);
	connect( miscKeithley_, SIGNAL(isConnected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	scaler_->channelAt(19)->setCurrentAmplifier(miscKeithley_);
	scaler_->channelAt(19)->setVoltagRange(0.1, 9.5);
	scaler_->channelAt(19)->setCountsVoltsSlopePreference(0.00001);

	setDiodeDetector(new CLSBasicScalerChannelDetector("Diode", "Diode", scaler_, 19, this));
	setPIPSDetector(new CLSBasicScalerChannelDetector("PIPS", "PIPS", scaler_, 19, this));
	setLytleDetector(new CLSBasicScalerChannelDetector("Lytle", "Lytle", scaler_, 19, this));

	// The inboard 32Ge detector.

	ge32DetectorInboard_ = new BioXASMainInboard32ElementGeDetector("BioXASMainInboardDetector",
																	"Inboard Detector",
																	zebra_->softInputControlAt(0),
																	zebra_->pulseControlAt(2),
																	this);
	connect( ge32DetectorInboard_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	ge32DetectorInboard_->setTriggerSource(zebraTriggerSource_);

	addGe32Detector(ge32DetectorInboard_);

	// The scaler dwell time detector.

	setScalerDwellTimeDetector(createScalerDwellTimeDetector(scaler_));
}

void BioXASMainBeamline::setupControlsAsDetectors()
{
	addControlAsDetector("MonoEncoderEnergyFeedback", "Encoder-based Energy Feedback", mono_->encoderEnergy());
	addControlAsDetector("MonoStepEnergyFeedback", "Step-based Energy Feedback", mono_->stepEnergy());
	addControlAsDetector("MonoStepAngleFeedback", "Step-based Goniometer Angle Feedback", mono_->stepBragg());
	addControlAsDetector("MonoStepSetpoint", "Goniometer Step Setpoint", mono_->bragg()->stepSetpointControl());
	addControlAsDetector("MonoEncoderStepsDegFeedback", "Goniometer Angle Encoder-Steps Difference", mono_->encoderStepsDiffControl());

	energySetpointDetector_ = new AMBasicControlDetectorEmulator("EnergySetpoint", "EnergySetpoint", mono_->energy(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	energySetpointDetector_->setControlProperty(AMBasicControlDetectorEmulator::Control::Setpoint);
}

void BioXASMainBeamline::setupExposedControls()
{
	// M1 mirror controls

	addExposedControl(m1Mirror_->upstreamInboardMotor());
	addExposedControl(m1Mirror_->upstreamOutboardMotor());
	addExposedControl(m1Mirror_->downstreamMotor());
	addExposedControl(m1Mirror_->stripeSelectMotor());
	addExposedControl(m1Mirror_->yawMotor());
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
	addExposedControl(mono_->bragg()->encoderMovementTypeControl());
	addExposedControl(mono_->bragg()->encoderStepSoftRatioControl());
	addExposedControl(mono_->bragg()->encoderCalibrationSlopeControl());
	addExposedControl(mono_->bragg()->stepCalibrationSlopeControl());
	addExposedControl(mono_->bragg()->retries());
	addExposedControl(mono_->vertical());
	addExposedControl(mono_->lateral());
	addExposedControl(mono_->crystal1Pitch());
	addExposedControl(mono_->crystal1Roll());
	addExposedControl(mono_->crystal2Pitch());
	addExposedControl(mono_->crystal2Roll());
	addExposedControl(mono_->encoderStepsDiffControl());

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

	// Carbon filter farm controls.

	addExposedControl(carbonFilterFarm_->upstreamActuator()->position());
	addExposedControl(carbonFilterFarm_->upstreamActuator()->window());
	addExposedControl(carbonFilterFarm_->upstreamActuator()->filter());

	addExposedControl(carbonFilterFarm_->downstreamActuator()->position());
	addExposedControl(carbonFilterFarm_->downstreamActuator()->window());
	addExposedControl(carbonFilterFarm_->downstreamActuator()->filter());

	addExposedControl(carbonFilterFarm_->filter());

	// JJ slits controls.

	addExposedControl(jjSlits_->verticalCenter());
	addExposedControl(jjSlits_->verticalGap());
	addExposedControl(jjSlits_->horizontalCenter());
	addExposedControl(jjSlits_->horizontalGap());

	// XIA filters control.

	addExposedControl(xiaFilters_->filter1());
	addExposedControl(xiaFilters_->filter2());
	addExposedControl(xiaFilters_->filter3());
	addExposedControl(xiaFilters_->filter4());

	// DBHR mirrors controls.

	addExposedControl(dbhrMirrors_->m1VerticalControl());
	addExposedControl(dbhrMirrors_->m2VerticalControl());
	addExposedControl(dbhrMirrors_->pitchControl());

	// Standards wheel.

	addExposedControl(standardsWheel_->wheelControl());

	// Endstation table

	addExposedControl(endstationTable_->heightPVController());
	addExposedControl(endstationTable_->pitchPVController());
	addExposedControl(endstationTable_->lateralPVController());
	addExposedControl(endstationTable_->yawPVController());
}

void BioXASMainBeamline::setupExposedDetectors()
{
	// Add controls as detectors.

	foreach (AMDetector *detector, controlDetectorMap_.values())
		addExposedDetector(detector);

	// Add any remaining detectors.

	addExposedDetector(energySetpointDetector_);
}

BioXASMainBeamline::BioXASMainBeamline()
	: BioXASBeamline("BioXASMain", "BioXAS Beamline - Main Endstation")
{
	setupComponents();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();
}
