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
	bool newState = (
				// General BioXAS components.
				BioXASBeamline::isConnected() &&

				// M1 mirror
				m1Mirror_->isConnected() &&

				// Monochromator
				mono_->isConnected() &&

				// M2 mirror
				m2Mirror_->isConnected() &&

				// Scaler
				scaler_->isConnected()
				);

	return newState;
}

QList<AMControl *> BioXASMainBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType category)
{
	QList<AMControl *> matchedMotors;

	switch (category) {
	case BioXASBeamlineDef::FilterMotor: // BioXAS Filter motors
		matchedMotors.append(carbonFilterFarm1_);
		matchedMotors.append(carbonFilterFarm2_);
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
		break;

	default:
		qDebug() << "ERROR: invalid BioXAS Motor category: " << category;
		break;
	}

	return matchedMotors;
}

void BioXASMainBeamline::setupDiagnostics()
{

}

void BioXASMainBeamline::setupControlSets()
{

}

void BioXASMainBeamline::setupDetectors()
{
	i0Detector_ = new CLSBasicScalerChannelDetector("I0Detector", "I0 Detector", scaler_, 16, this);

	i1Detector_ = new CLSBasicScalerChannelDetector("I1Detector", "I1 Detector", scaler_, 17, this);

	i2Detector_ = new CLSBasicScalerChannelDetector("I2Detector", "I2 Detector", scaler_, 18, this);
}

void BioXASMainBeamline::setupSampleStage()
{

}

void BioXASMainBeamline::setupComponents()
{
	// The Main endstation safety shutter.
	safetyShutterDownstream_ = new  CLSBiStateControl("MainShutter", "MainShutter", "SSH1607-5-I21-01:state", "SSH1607-5-I21-01:opr:open", "SSH1607-5-I21-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( safetyShutterDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// M1 Mirror.
	m1Mirror_ = new BioXASMainM1Mirror(this);
	connect( m1Mirror_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// Mono.
	mono_ = new BioXASMainMonochromator(this);
	mono_->setM1MirrorPitchControl(m1Mirror_->pitchControl());
	connect( mono_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// M2 Mirror.
	m2Mirror_ = new BioXASMainM2Mirror(this);
	connect( m2Mirror_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// Scaler
	scaler_ = new CLSSIS3820Scaler("BL1607-5-I21:mcs", this);
	connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	scalerDwellTime_ = new AMReadOnlyPVControl("ScalerDwellTime", "BL1607-5-I21:mcs:delay", this, "Scaler dwell time");

	// Detectors
	setupDetectors();

	// Amplifiers
	i0Keithley_ = new CLSKeithley428("I0 Channel", "AMP1607-701", this);
	scaler_->channelAt(16)->setCustomChannelName("I0 Channel");
	scaler_->channelAt(16)->setCurrentAmplifier(i0Keithley_);
	scaler_->channelAt(16)->setDetector(i0Detector_);

	i1Keithley_ = new CLSKeithley428("I1 Channel", "AMP1607-702", this);
	scaler_->channelAt(17)->setCustomChannelName("I1 Channel");
	scaler_->channelAt(17)->setCurrentAmplifier(i1Keithley_);
	scaler_->channelAt(17)->setDetector(i1Detector_);

	i2Keithley_ = new CLSKeithley428("I2 Channel", "AMP1607-703", this);
	scaler_->channelAt(18)->setCustomChannelName("I2 Channel");
	scaler_->channelAt(18)->setCurrentAmplifier(i2Keithley_);
	scaler_->channelAt(18)->setDetector(i2Detector_);
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
}

void BioXASMainBeamline::setupExposedDetectors()
{
	addExposedDetector(dwellTimeDetector_);
	addExposedDetector(i0Detector_);
	addExposedDetector(i1Detector_);
	addExposedDetector(i2Detector_);
	addExposedDetector(encoderEnergySetpointDetector_);
	addExposedDetector(encoderEnergyFeedbackDetector_);
	addExposedDetector(stepEnergyFeedbackDetector_);
	addExposedDetector(braggDetector_);
	addExposedDetector(braggEncoderFeedbackDetector_);
	addExposedDetector(braggMoveRetriesDetector_);
	addExposedDetector(braggStepSetpointDetector_);
}

void BioXASMainBeamline::setupMotorGroup()
{
	// BioXAS filter motors
	carbonFilterFarm1_ = new CLSMAXvMotor(QString("SMTR1607-5-I00-03 Filter 1"), QString("SMTR1607-5-I00-03"), QString("SMTR1607-5-I00-03 Filter 1"), true, 0.05, 2.0, this, QString(":mm"));
	carbonFilterFarm2_ = new CLSMAXvMotor(QString("SMTR1607-5-I00-04 Filter 2"), QString("SMTR1607-5-I00-04"), QString("SMTR1607-5-I00-04 Filter 2"), true, 0.05, 2.0, this, QString(":mm"));
}

void BioXASMainBeamline::setupControlsAsDetectors()
{
	encoderEnergySetpointDetector_ = new AMBasicControlDetectorEmulator("EncoderEnergySetpoint", "EncoderEnergySetpoint", mono_->encoderEnergyControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	encoderEnergySetpointDetector_->setControlProperty(AMBasicControlDetectorEmulator::Control::Setpoint);
	encoderEnergySetpointDetector_->setHiddenFromUsers(false);
	encoderEnergySetpointDetector_->setIsVisible(true);

	encoderEnergyFeedbackDetector_ = new AMBasicControlDetectorEmulator("EncoderEnergyFeedback", "EncoderEnergyFeedback", mono_->encoderEnergyControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	encoderEnergyFeedbackDetector_->setHiddenFromUsers(false);
	encoderEnergyFeedbackDetector_->setIsVisible(true);

	stepEnergyFeedbackDetector_ = new AMBasicControlDetectorEmulator("StepEnergyFeedback", "StepEnergyFeedback", mono_->stepEnergyControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	stepEnergyFeedbackDetector_->setHiddenFromUsers(false);
	stepEnergyFeedbackDetector_->setIsVisible(true);

	dwellTimeDetector_ = new AMBasicControlDetectorEmulator("DwellTimeFeedback", "Dwell Time Feedback", scalerDwellTime_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

	braggDetector_ = new AMBasicControlDetectorEmulator("GoniometerMotorFeedback", "Goniometer Motor Feedback", mono_->braggMotor(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	braggDetector_->setHiddenFromUsers(false);
	braggDetector_->setIsVisible(true);

	braggEncoderFeedbackDetector_ = new AMBasicControlDetectorEmulator("GoniometerMotorEncoderFeedback", "Goniometer Motor Encoder Feedback", mono_->braggMotor()->encoderFeedbackControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	braggEncoderFeedbackDetector_->setHiddenFromUsers(false);
	braggEncoderFeedbackDetector_->setIsVisible(true);

	braggMoveRetriesDetector_ = new AMBasicControlDetectorEmulator("GoniometerMotorMoveRetries", "Number of goniometer motor move retries", mono_->braggMotor()->retries(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	braggMoveRetriesDetector_->setHiddenFromUsers(false);
	braggMoveRetriesDetector_->setIsVisible(true);

	braggStepSetpointDetector_ = new AMBasicControlDetectorEmulator("GoniometerMotorStepSetpoint", "Goniometer motor step setpoint", mono_->braggMotor()->stepSetpointControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	braggStepSetpointDetector_->setHiddenFromUsers(false);
	braggStepSetpointDetector_->setIsVisible(true);
}

BioXASMainBeamline::BioXASMainBeamline()
	: BioXASBeamline("BioXAS Beamline - Main Endstation")
{
	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupControlSets();
	setupMotorGroup();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();
}
