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

#include <QDebug>

BioXASMainBeamline::~BioXASMainBeamline()
{

}

bool BioXASMainBeamline::isConnected() const
{
	bool connected = (
				// Front-end BioXAS components.
				BioXASBeamline::isConnected() &&

				safetyShutterES_ && safetyShutterES_->isConnected() &&
				m1Mirror_ && m1Mirror_->isConnected() &&
				mono_ && mono_->isConnected() &&
				m2Mirror_ && m2Mirror_->isConnected() &&
				carbonFilterFarm_ && carbonFilterFarm_->isConnected() &&
				jjSlits_ && jjSlits_->isConnected() &&
				xiaFilters_ && xiaFilters_->isConnected() &&
				dbhrMirrors_ && dbhrMirrors_->isConnected() &&
				standardsWheel_ && standardsWheel_->isConnected() &&
				endstationTable_ && endstationTable_->isConnected() &&

				scaler_ && scaler_->isConnected() &&
				i0Keithley_ && i0Keithley_->isConnected() &&
				i0Detector_ && i0Detector_->isConnected() &&
				i1Keithley_ && i1Keithley_->isConnected() &&
				i1Detector_ && i1Detector_->isConnected() &&
				i2Keithley_ && i2Keithley_->isConnected() &&
				i2Detector_ && i2Detector_->isConnected() &&

				utilities_ && utilities_->isConnected()
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
		matchedMotors.append(m1Mirror_->upstreamInboardMotorControl());
		matchedMotors.append(m1Mirror_->upstreamOutboardMotorControl());
		matchedMotors.append(m1Mirror_->downstreamMotorControl());
		matchedMotors.append(m1Mirror_->stripeSelectMotorControl());
		matchedMotors.append(m1Mirror_->yawMotorControl());
		matchedMotors.append(m1Mirror_->benderUpstreamMotorControl());
		matchedMotors.append(m1Mirror_->benderDownstreamMotorControl());
		matchedMotors.append(m1Mirror_->upperSlitBladeMotorControl());
		break;

	case BioXASBeamlineDef::MaskMotor:	// BioXAS Variable Mask motors
		matchedMotors.append(mono_->upperSlitBladeMotor());
		matchedMotors.append(mono_->lowerSlitBladeMotor());
		break;

	case BioXASBeamlineDef::MonoMotor:	// BioXAS Mono motors
		matchedMotors.append(mono_->paddleMotor());
		matchedMotors.append(mono_->braggMotor());
		matchedMotors.append(mono_->verticalMotor());
		matchedMotors.append(mono_->lateralMotor());
		matchedMotors.append(mono_->crystalChangeMotor());
		matchedMotors.append(mono_->crystal1PitchMotor());
		matchedMotors.append(mono_->crystal1RollMotor());
		matchedMotors.append(mono_->crystal2PitchMotor());
		matchedMotors.append(mono_->crystal2RollMotor());
		break;

	case BioXASBeamlineDef::M2Motor:	// BioXAS M2 motors
		matchedMotors.append(m2Mirror_->upstreamInboardMotorControl());
		matchedMotors.append(m2Mirror_->upstreamOutboardMotorControl());
		matchedMotors.append(m2Mirror_->downstreamMotorControl());
		matchedMotors.append(m2Mirror_->stripeSelectMotorControl());
		matchedMotors.append(m2Mirror_->yawMotorControl());
		matchedMotors.append(m2Mirror_->benderUpstreamMotorControl());
		matchedMotors.append(m2Mirror_->benderDownstreamMotorControl());
		break;

	case BioXASBeamlineDef::PseudoM1Motor: // BioXAS Pseudo M1 motor
		matchedMotors.append(m1Mirror_->rollControl());
		matchedMotors.append(m1Mirror_->pitchControl());
		matchedMotors.append(m1Mirror_->heightControl());
		matchedMotors.append(m1Mirror_->yawControl());
		matchedMotors.append(m1Mirror_->lateralControl());
		break;

	case BioXASBeamlineDef::PseudoM2Motor: // BioXAS Pseudo M2 motor
		matchedMotors.append(m2Mirror_->rollControl());
		matchedMotors.append(m2Mirror_->pitchControl());
		matchedMotors.append(m2Mirror_->yawControl());
		matchedMotors.append(m2Mirror_->heightControl());
		matchedMotors.append(m2Mirror_->lateralControl());
		break;

	case BioXASBeamlineDef::PseudoMonoMotor: // BioXAS Pseudo Mono motor
		matchedMotors.append(mono_->energyControl());
		matchedMotors.append(mono_->regionControl());
		break;

	default:
		qDebug() << "ERROR: invalid BioXAS Motor category: " << category;
		break;
	}

	return matchedMotors;
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::scalerDwellTimeDetector() const
{
	return detectorForControl(scaler_->dwellTimeControl());
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::energySetpointDetector() const
{
	return energySetpointDetector_;
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::encoderEnergyFeedbackDetector() const
{
	return detectorForControl(mono_->encoderEnergyControl());
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::stepEnergyFeedbackDetector() const
{
	return detectorForControl(mono_->stepEnergyControl());
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::braggDetector() const
{
	return detectorForControl(mono_->braggMotor());
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::braggStepSetpointDetector() const
{
	return detectorForControl(mono_->braggMotor()->stepSetpointControl());
}

AMBasicControlDetectorEmulator* BioXASMainBeamline::braggEncoderStepDegFeedbackDetector() const
{
	return detectorForControl(mono_->encoderStepsDiffControl());
}

void BioXASMainBeamline::setupComponents()
{
	// Endstation safety shutter.
	safetyShutterES_ = new  CLSBiStateControl("MainShutter", "MainShutter", "SSH1607-5-I21-01:state", "SSH1607-5-I21-01:opr:open", "SSH1607-5-I21-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( safetyShutterES_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// M1 Mirror.
	m1Mirror_ = new BioXASMainM1Mirror(this);
	connect( m1Mirror_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Mono.
	mono_ = new BioXASMainMonochromator(this);
	mono_->setM1MirrorPitchControl(m1Mirror_->pitchControl());
	connect( mono_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// M2 Mirror.
	m2Mirror_ = new BioXASMainM2Mirror(this);
	connect( m2Mirror_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Carbon filter farm.
	carbonFilterFarm_ = new BioXASMainCarbonFilterFarm(this);
	connect( carbonFilterFarm_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// JJ slits.
	jjSlits_ = new CLSJJSlits("JJSlits", "SMTR1607-7-I21-11", "SMTR1607-7-I21-10", "SMTR1607-7-I21-12", "SMTR1607-7-I21-13", this);
	connect( jjSlits_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// XIA filters.
	xiaFilters_ = new BioXASMainXIAFilters(this);
	connect( xiaFilters_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// DBHR mirrors.
	dbhrMirrors_ = new BioXASMainDBHRMirrors(this);
	connect( dbhrMirrors_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Standards wheel.
	standardsWheel_ = new BioXASMainStandardsWheel(this);
	connect( standardsWheel_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// Endstation table.
	endstationTable_ = new BioXASEndstationTable("MainEndstationTable", "BL1607-7-I21", true, this);
	connect( endstationTable_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Scaler
	scaler_ = new CLSSIS3820Scaler("MCS1607-701:mcs", this);
	connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// Scaler channel detectors.
	i0Detector_ = new CLSBasicScalerChannelDetector("I0Detector", "I0 Detector", scaler_, 16, this);
	i1Detector_ = new CLSBasicScalerChannelDetector("I1Detector", "I1 Detector", scaler_, 17, this);
	i2Detector_ = new CLSBasicScalerChannelDetector("I2Detector", "I2 Detector", scaler_, 18, this);

	// I0 channel amplifier
	i0Keithley_ = new CLSKeithley428("I0 Channel", "AMP1607-701", this);
	scaler_->channelAt(16)->setCustomChannelName("I0 Channel");
	scaler_->channelAt(16)->setCurrentAmplifier(i0Keithley_);
	scaler_->channelAt(16)->setDetector(i0Detector_);

	// I1 channel amplifier
	i1Keithley_ = new CLSKeithley428("I1 Channel", "AMP1607-702", this);
	scaler_->channelAt(17)->setCustomChannelName("I1 Channel");
	scaler_->channelAt(17)->setCurrentAmplifier(i1Keithley_);
	scaler_->channelAt(17)->setDetector(i1Detector_);

	// I2 channel amplifier
	i2Keithley_ = new CLSKeithley428("I2 Channel", "AMP1607-703", this);
	scaler_->channelAt(18)->setCustomChannelName("I2 Channel");
	scaler_->channelAt(18)->setCurrentAmplifier(i2Keithley_);
	scaler_->channelAt(18)->setDetector(i2Detector_);

	// Utilities
	utilities_ = new BioXASBeamlineUtilities(this);
	connect( utilities_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

void BioXASMainBeamline::setupControlsAsDetectors()
{
	addControlAsDetector("ScalerDwellTimeFeedback", "ScalerDwellTimeFeedback", scaler_->dwellTimeControl());
	addControlAsDetector("MonoEncoderEnergyFeedback", "MonoEncoderEnergyFeedback", mono_->encoderEnergyControl());
	addControlAsDetector("MonoStepEnergyFeedback", "MonoStepEnergyFeedback", mono_->stepEnergyControl());
	addControlAsDetector("MonoStepAngleFeedback", "MonoStepAngleFeedback", mono_->stepBraggControl());
	addControlAsDetector("MonoStepSetpoint", "MonoStepSetpoint", mono_->braggMotor()->stepSetpointControl());
	addControlAsDetector("MonoEncoderStepsDegFeedback", "MonoEncoderStepsDegFeedback", mono_->encoderStepsDiffControl());

	energySetpointDetector_ = new AMBasicControlDetectorEmulator("EnergySetpoint", "EnergySetpoint", mono_->energyControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	energySetpointDetector_->setControlProperty(AMBasicControlDetectorEmulator::Control::Setpoint);
}

void BioXASMainBeamline::setupExposedControls()
{
	// M1 mirror controls

	addExposedControl(m1Mirror_->upstreamInboardMotorControl());
	addExposedControl(m1Mirror_->upstreamOutboardMotorControl());
	addExposedControl(m1Mirror_->downstreamMotorControl());
	addExposedControl(m1Mirror_->stripeSelectMotorControl());
	addExposedControl(m1Mirror_->yawMotorControl());
	addExposedControl(m1Mirror_->benderUpstreamMotorControl());
	addExposedControl(m1Mirror_->benderDownstreamMotorControl());
	addExposedControl(m1Mirror_->upperSlitBladeMotorControl());

	addExposedControl(m1Mirror_->rollControl());
	addExposedControl(m1Mirror_->pitchControl());
	addExposedControl(m1Mirror_->heightControl());
	addExposedControl(m1Mirror_->yawControl());
	addExposedControl(m1Mirror_->lateralControl());
	addExposedControl(m1Mirror_->bendControl());

	// Mono controls.

	addExposedControl(mono_->encoderEnergyControl());
	addExposedControl(mono_->stepEnergyControl());
	addExposedControl(mono_->regionControl());
	addExposedControl(mono_->braggMotor());
	addExposedControl(mono_->braggMotor()->EGUVelocityControl());
	addExposedControl(mono_->braggMotor()->EGUBaseVelocityControl());
	addExposedControl(mono_->braggMotor()->EGUAccelerationControl());
	addExposedControl(mono_->braggMotor()->preDeadBandControl());
	addExposedControl(mono_->braggMotor()->postDeadBandControl());
	addExposedControl(mono_->braggMotor()->encoderMovementTypeControl());
	addExposedControl(mono_->braggMotor()->encoderStepSoftRatioControl());
	addExposedControl(mono_->braggMotor()->encoderCalibrationSlopeControl());
	addExposedControl(mono_->braggMotor()->stepCalibrationSlopeControl());
	addExposedControl(mono_->braggMotor()->retries());
	addExposedControl(mono_->verticalMotor());
	addExposedControl(mono_->lateralMotor());
	addExposedControl(mono_->crystal1PitchMotor());
	addExposedControl(mono_->crystal1RollMotor());
	addExposedControl(mono_->crystal2PitchMotor());
	addExposedControl(mono_->crystal2RollMotor());
	addExposedControl(mono_->encoderStepsDiffControl());

	// M2 mirror controls.

	addExposedControl(m2Mirror_->upstreamInboardMotorControl());
	addExposedControl(m2Mirror_->upstreamOutboardMotorControl());
	addExposedControl(m2Mirror_->downstreamMotorControl());
	addExposedControl(m2Mirror_->stripeSelectMotorControl());
	addExposedControl(m2Mirror_->yawMotorControl());
	addExposedControl(m2Mirror_->benderUpstreamMotorControl());
	addExposedControl(m2Mirror_->benderDownstreamMotorControl());
	addExposedControl(m2Mirror_->screenMotorControl());

	addExposedControl(m2Mirror_->rollControl());
	addExposedControl(m2Mirror_->pitchControl());
	addExposedControl(m2Mirror_->heightControl());
	addExposedControl(m2Mirror_->yawControl());
	addExposedControl(m2Mirror_->lateralControl());
	addExposedControl(m2Mirror_->bendControl());

	// Carbon filter farm controls.

	addExposedControl(carbonFilterFarm_->upstreamActuator()->position());
	addExposedControl(carbonFilterFarm_->upstreamActuator()->window());
	addExposedControl(carbonFilterFarm_->upstreamActuator()->filter());

	addExposedControl(carbonFilterFarm_->downstreamActuator()->position());
	addExposedControl(carbonFilterFarm_->downstreamActuator()->window());
	addExposedControl(carbonFilterFarm_->downstreamActuator()->filter());

	addExposedControl(carbonFilterFarm_->filter());

	// JJ slits controls.

	addExposedControl(jjSlits_->verticalCenterControl());
	addExposedControl(jjSlits_->verticalGapControl());
	addExposedControl(jjSlits_->horizontalCenterControl());
	addExposedControl(jjSlits_->horizontalGapControl());

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
	// Add detectors.

	addExposedDetector(i0Detector_);
	addExposedDetector(i1Detector_);
	addExposedDetector(i2Detector_);

	// Add controls as detectors.

	foreach (AMDetector *detector, controlDetectorMap_.values())
		addExposedDetector(detector);

	// Add any remaining detectors.

	addExposedDetector(energySetpointDetector_);
}

BioXASMainBeamline::BioXASMainBeamline()
	: BioXASBeamline("BioXAS Beamline - Main Endstation")
{
	setupComponents();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();
}
