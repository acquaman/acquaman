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

QList<AMControl *> BioXASMainBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType category)
{
	QList<AMControl *> matchedMotors;

	switch (category) {
	case BioXASBeamlineDef::FilterMotor: // BioXAS Filter motors
		matchedMotors.append(carbonFilterFarm1_);
		matchedMotors.append(carbonFilterFarm2_);
		break;

	case BioXASBeamlineDef::M1Motor:	// BioXAS M1 motors
		matchedMotors.append(m1VertUpStreamINB_);
		matchedMotors.append(m1VertUpStreamOUTB_);
		matchedMotors.append(m1VertDownStream_);
		matchedMotors.append(m1StripeSelect_);
		matchedMotors.append(m1Yaw_);
		matchedMotors.append(m1BenderUpstream_);
		matchedMotors.append(m1BenderDownStream_);
		matchedMotors.append(m1UpperSlitBlade_);
		break;

	case BioXASBeamlineDef::MaskMotor:	// BioXAS Variable Mask motors
		matchedMotors.append(variableMaskVertUpperBlade_);
		matchedMotors.append(variableMaskVertLowerBlade_);
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
		matchedMotors.append(m2Mirror_->verticalUpstreamInboundControl());
		matchedMotors.append(m2Mirror_->verticalUpstreamOutboundControl());
		matchedMotors.append(m2Mirror_->verticalDownstreamControl());
		matchedMotors.append(m2Mirror_->stripeSelectControl());
		matchedMotors.append(m2Mirror_->yawControl());
		matchedMotors.append(m2Mirror_->benderUpstreamControl());
		matchedMotors.append(m2Mirror_->benderDownstreamControl());
		break;

	case BioXASBeamlineDef::PseudoM1Motor: // BioXAS Pseudo M1 motor
		matchedMotors.append(m1PseudoRoll_);
		matchedMotors.append(m1PseudoPitch_);
		matchedMotors.append(m1PseudoHeight_);
		matchedMotors.append(m1PseudoYaw_);
		matchedMotors.append(m1PseudoLateral_);
		break;

	case BioXASBeamlineDef::PseudoM2Motor: // BioXAS Pseudo M2 motor
		matchedMotors.append(m2Mirror_->pseudoRollControl());
		matchedMotors.append(m2Mirror_->pseudoPitchControl());
		matchedMotors.append(m2Mirror_->pseudoYawControl());
		matchedMotors.append(m2Mirror_->pseudoHeightControl());
		matchedMotors.append(m2Mirror_->pseudoLateralControl());
		break;

	case BioXASBeamlineDef::PseudoMonoMotor: // BioXAS Pseudo Mono motor
		matchedMotors.append(monoPseudoEnergy_);
		matchedMotors.append(monoBraggAngle_);
		break;

	default:
		qDebug() << "ERROR: invalid BioXAS Motor category: " << category;
		break;
	}

	return matchedMotors;
}

void BioXASMainBeamline::onConnectedChanged()
{
	bool newState = (
				// General BioXAS components.
				BioXASBeamline::isConnected() &&

				// M2 mirror
				m2Mirror_->isConnected() &&

				// Monochromator
				mono_->isConnected() &&

				// Scaler
				scaler_->isConnected()
				);

	if (connected_ != newState) {
		connected_ = newState;
		emit connected(connected_);
	}
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

void BioXASMainBeamline::setupMono()
{
	mono_ = new BioXASMainMonochromator(this);
	connect( mono_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
}

void BioXASMainBeamline::setupComponents()
{
	// Setup general BioXAS components.

	BioXASBeamline::setupComponents();

	// Scaler

	scaler_ = new CLSSIS3820Scaler("BL1607-5-I21:mcs", this);
	connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onConnectedChanged()) );

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

	// M2 Mirror.

	m2Mirror_ = new BioXASMainM2Mirror(this);
	connect( m2Mirror_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
}

void BioXASMainBeamline::setupExposedControls()
{
	// M2 mirror controls.
	addExposedControl(m2Mirror_->screenControl());

	// Mono controls.
	addExposedControl(mono_->energyControl());
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
}

void BioXASMainBeamline::setupExposedDetectors()
{
	addExposedDetector(dwellTimeDetector_);
	addExposedDetector(i0Detector_);
	addExposedDetector(i1Detector_);
	addExposedDetector(i2Detector_);
	addExposedDetector(energySetpointDetector_);
	addExposedDetector(energyFeedbackDetector_);
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

	// BioXAS M1 motors
	m1VertUpStreamINB_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-01 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I21-01"), QString("SMTR1607-5-I21-01 VERT INB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m1VertUpStreamOUTB_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-02 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I21-02"), QString("SMTR1607-5-I21-02 VERT OUTB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m1VertDownStream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-03 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I21-03"), QString("SMTR1607-5-I21-03 VERT (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m1StripeSelect_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-04 STRIPE SELECT"), QString("SMTR1607-5-I21-04"), QString("SMTR1607-5-I21-04 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	m1Yaw_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-05 YAW"), QString("SMTR1607-5-I21-05"), QString("SMTR1607-5-I21-05 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	m1BenderUpstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-06 BENDER (UPSTREAM)"), QString("SMTR1607-5-I21-06"), QString("SMTR1607-5-I21-06 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	m1BenderDownStream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-07 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I21-07"), QString("SMTR1607-5-I21-07 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	m1UpperSlitBlade_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-08 UPPER SLIT BLADE"), QString("SMTR1607-5-I21-08"), QString("SMTR1607-5-I21-08 UPPER SLIT BLADE"), true, 0.05, 2.0, this, QString(":mm"));

	// BioXAS Variable Mask motors
	variableMaskVertUpperBlade_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), QString("SMTR1607-5-I21-09"), QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), true, 0.05, 2.0, this, QString(":mm"));
	variableMaskVertLowerBlade_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), QString("SMTR1607-5-I21-10"), QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), true, 0.05, 2.0, this, QString(":mm"));

	// BioXAS M1 Pseudo motors					   name,				   pvBaseName,				readPVname,	writePVname, movingPVname,	enabledPVname, stopPVname, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description, parent = 0
	m1PseudoRoll_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M1 Roll", "BL1607-5-I21:M1:Roll", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m1PseudoPitch_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M1 Pitch", "BL1607-5-I21:M1:Pitch", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m1PseudoHeight_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M1 Height", "BL1607-5-I21:M1:Height", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");
	m1PseudoYaw_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M1 Yaw", "BL1607-5-I21:M1:Yaw", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m1PseudoLateral_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M1 Lateral", "BL1607-5-I21:M1:Lateral", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");

	// BioXAS Mono Pseudo motors					   name,				   pvBaseName,				readPVname,	writePVname, movingPVname,	enabledPVname, stopPVname, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description, parent = 0
	monoPseudoEnergy_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main Mono Energy", "BL1607-5-I21:Energy", ":EV:fbk", ":EV", ":status", ":enabled", ":stop");
	monoBraggAngle_ = new AMPVwStatusControl("BL1607-5-I21 Main Mono Bragg Angle", "BL1607-5-I21:Energy:EV:fbk:tr.K", "BL1607-5-I21:Energy:EV:sp:tr.E", "BL1607-5-I21:Energy:status", "BL1607-5-I21:Energy:stop", this, 0.05);
}

void BioXASMainBeamline::setupControlsAsDetectors()
{
	energySetpointDetector_ = new AMBasicControlDetectorEmulator("EnergySetpoint", "EnergySetpoint", new AMReadOnlyPVControl("EnergySetpoint", "BL1607-5-I21:Energy:EV", this), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	energySetpointDetector_->setHiddenFromUsers(false);
	energySetpointDetector_->setIsVisible(true);

	energyFeedbackDetector_ = new AMBasicControlDetectorEmulator("EnergyFeedback", "EnergyFeedback", new AMReadOnlyPVControl("EnergyFeedback", "BL1607-5-I21:Energy:EV:fbk", this), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	energyFeedbackDetector_->setHiddenFromUsers(false);
	energyFeedbackDetector_->setIsVisible(true);

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
	connected_ = false;

	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupControlSets();
	setupMono();
	setupMotorGroup();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();
}
