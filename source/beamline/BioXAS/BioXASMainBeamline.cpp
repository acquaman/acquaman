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

BioXASMainBeamline::BioXASMainBeamline()
	: AMBeamline("BioXAS Beamline - Main Endstation")
{
	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupDetectors();
	setupControlSets();
	setupMono();
	setupMotorGroup();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();
}

BioXASMainBeamline::~BioXASMainBeamline()
{

}

void BioXASMainBeamline::setupDiagnostics()
{

}

void BioXASMainBeamline::setupSampleStage()
{

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

	// BioXAS Mono motors
	monoPhosphorPaddle_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I21-11"), QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), true, 0.05, 2.0, this, QString(":mm"));
	monoBragg_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-12 BRAGG"), QString("SMTR1607-5-I21-12"), QString("SMTR1607-5-I21-12 BRAGG"), true, 0.05, 2.0, this, QString(":deg"));
	monoVertical_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-13 VERTICAL"), QString("SMTR1607-5-I21-13"), QString("SMTR1607-5-I21-13 VERTICAL"), true, 0.05, 2.0, this, QString(":mm"));
	monoLateral_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-14 LATERAL"), QString("SMTR1607-5-I21-14"), QString("SMTR1607-5-I21-14 LATERAL"), true, 0.05, 2.0, this, QString(":mm"));
	monoXtalXchage_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-22 XTAL XCHAGE"), QString("SMTR1607-5-I21-22"), QString("SMTR1607-5-I21-22 XTAL XCHAGE"), true, 0.05, 2.0, this, QString(":mm"));
	monoXtal1Pitch_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), QString("SMTR1607-5-I21-23"), QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	monoXtal1Roll_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-24 XTAL 1 ROLL"), QString("SMTR1607-5-I21-24"), QString("SMTR1607-5-I21-24 XTAL 1 ROLL"), true, 0.05, 2.0, this, QString(":V"));
	monoXtal2Pitch_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), QString("SMTR1607-5-I21-25"), QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	monoXtal2Roll_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), QString("SMTR1607-5-I21-26"), QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V"));

	// BioXAS M2 motors
	m2VertUpstreamINB_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I21-15"), QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m2VertUpstreamOUTB_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I21-16"), QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m2VertDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I21-17"), QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m2StripeSelect_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-18 STRIPE SELECT"), QString("SMTR1607-5-I21-18"), QString("SMTR1607-5-I21-18 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	m2Yaw_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-19 YAW"), QString("SMTR1607-5-I21-19"), QString("SMTR1607-5-I21-19 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	m2BenderUpstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I21-20"), QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	m2BenderDownStream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I21-21"), QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));

	// BioXAS M1 Pseudo motors					   name,				   pvBaseName,				readPVname,	writePVname, movingPVname,	enabledPVname, stopPVname, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description, parent = 0
	m1PseudoRoll_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M1 Roll", "BL1607-5-I21:M1:Roll", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m1PseudoPitch_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M1 Pitch", "BL1607-5-I21:M1:Pitch", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m1PseudoHeight_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M1 Height", "BL1607-5-I21:M1:Height", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");
	m1PseudoYaw_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M1 Yaw", "BL1607-5-I21:M1:Yaw", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m1PseudoLateral_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M1 Lateral", "BL1607-5-I21:M1:Lateral", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");

	// BioXAS M2 Pseudo motors					   name,				   pvBaseName,				readPVname,	writePVname, movingPVname,	enabledPVname, stopPVname, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description, parent = 0
	m2PseudoRoll_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M2 Roll", "BL1607-5-I21:M2:Roll", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m2PseudoPitch_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M2 Pitch", "BL1607-5-I21:M2:Pitch", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m2PseudoHeight_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M2 Height", "BL1607-5-I21:M2:Height", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");
	m2PseudoYaw_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M2 Yaw", "BL1607-5-I21:M2:Yaw", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m2PseudoLateral_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M2 Lateral", "BL1607-5-I21:M2:Lateral", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");

	// BioXAS Mono Pseudo motors					   name,				   pvBaseName,				readPVname,	writePVname, movingPVname,	enabledPVname, stopPVname, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description, parent = 0
	monoPseudoEnergy_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main Mono Energy", "BL1607-5-I21:Energy", ":EV:fbk", ":EV", ":status", ":enabled", ":stop");
}

void BioXASMainBeamline::setupDetectors()
{
    i0Detector_ = new CLSBasicScalerChannelDetector("I0Detector", "I0 Detector", scaler_, 0, this);
    scaler_->channelAt(0)->setDetector(i0Detector_);

    iTDetector_ = new CLSBasicScalerChannelDetector("ITDetector", "IT Detector", scaler_, 1, this);
    scaler_->channelAt(1)->setDetector(iTDetector_);

    i2Detector_ = new CLSBasicScalerChannelDetector("I2Detector", "I2 Detector", scaler_, 15, this);
    scaler_->channelAt(15)->setDetector(i2Detector_);
}

void BioXASMainBeamline::setupControlSets()
{

}

void BioXASMainBeamline::setupMono()
{

}

void BioXASMainBeamline::setupSynchronizedDwellTime()
{

}

void BioXASMainBeamline::setupComponents()
{
    scaler_ = new CLSSIS3820Scaler("BL1607-5-I21:mcs00", this);

    scalerDwellTime_ = new AMReadOnlyPVControl("ScalerDwellTime", "BL1607-5-I21:mcs00:delay", this, "Scaler dwell time");

    i0Keithley_ = new CLSKeithley428("I0 Channel", "AMP1607-701:Gain", this);
    scaler_->channelAt(0)->setCustomChannelName("I0 Channel");
    scaler_->channelAt(0)->setCurrentAmplifier(i0Keithley_);

    iTKeithley_ = new CLSKeithley428("IT Channel", "AMP1607-702:Gain", this);
    scaler_->channelAt(1)->setCustomChannelName("IT Channel");
    scaler_->channelAt(1)->setCurrentAmplifier(iTKeithley_);

    i2Keithley_ = new CLSKeithley428("I2 Channel", "AMP1607-703:Gain", this);
    scaler_->channelAt(15)->setCustomChannelName("I2 Channel");
    scaler_->channelAt(15)->setCurrentAmplifier(iTKeithley_);
}

void BioXASMainBeamline::setupControlsAsDetectors()
{

}

void BioXASMainBeamline::setupExposedControls()
{

}

void BioXASMainBeamline::setupExposedDetectors()
{
    addExposedDetector(i0Detector_);
    addExposedDetector(iTDetector_);
    addExposedDetector(i2Detector_);
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
		matchedMotors.append(monoPhosphorPaddle_);
		matchedMotors.append(monoBragg_);
		matchedMotors.append(monoVertical_);
		matchedMotors.append(monoLateral_);
		matchedMotors.append(monoXtalXchage_);
		matchedMotors.append(monoXtal1Pitch_);
		matchedMotors.append(monoXtal1Roll_);
		matchedMotors.append(monoXtal2Pitch_);
		matchedMotors.append(monoXtal2Roll_);
		break;

	case BioXASBeamlineDef::M2Motor:	// BioXAS M2 motors
		matchedMotors.append(m2VertUpstreamINB_);
		matchedMotors.append(m2VertUpstreamOUTB_);
		matchedMotors.append(m2VertDownstream_);
		matchedMotors.append(m2StripeSelect_);
		matchedMotors.append(m2Yaw_);
		matchedMotors.append(m2BenderUpstream_);
		matchedMotors.append(m2BenderDownStream_);
		break;

	case BioXASBeamlineDef::PseudoM1Motor: // BioXAS Pseudo M1 motor
		matchedMotors.append(m1PseudoRoll_);
		matchedMotors.append(m1PseudoPitch_);
		matchedMotors.append(m1PseudoHeight_);
		matchedMotors.append(m1PseudoYaw_);
		matchedMotors.append(m1PseudoLateral_);
		break;

	case BioXASBeamlineDef::PseudoM2Motor: // BioXAS Pseudo M2 motor
		matchedMotors.append(m2PseudoRoll_);
		matchedMotors.append(m2PseudoPitch_);
		matchedMotors.append(m2PseudoHeight_);
		matchedMotors.append(m2PseudoYaw_);
		matchedMotors.append(m2PseudoLateral_);
		break;

	case BioXASBeamlineDef::PseudoMonoMotor: // BioXAS Pseudo Mono motor
		matchedMotors.append(monoPseudoEnergy_);
		break;

	default:
		qDebug() << "ERROR: invalid BioXAS Motor category: " << category;
		break;
	}

	return matchedMotors;
}
