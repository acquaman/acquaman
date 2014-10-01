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

BioXASMainBeamline* BioXASMainBeamline::bioXASMainBLInstance_ = 0;

BioXASMainBeamline::BioXASMainBeamline()
	: AMBeamline("BioXAS Beamline - Main Endstation")
{
	setupSynchronizedDwellTime();
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

void BioXASMainBeamline::setupDiagnostics()
{

}

void BioXASMainBeamline::setupSampleStage()
{

}

void BioXASMainBeamline::setupMotorGroup()
{
	// BioXAS filter motors
	carbonFilterFarm1_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::FilterMotor, QString("SMTR1607-5-I00-03 Filter 1"), QString("SMTR1607-5-I00-03"), QString("SMTR1607-5-I00-03 Filter 1"), QString(":mm"),  true, 0.05, 2.0, this);
	carbonFilterFarm2_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::FilterMotor, QString("SMTR1607-5-I00-04 Filter 2"), QString("SMTR1607-5-I00-04"), QString("SMTR1607-5-I00-04 Filter 2"), QString(":mm"),  true, 0.05, 2.0, this);

	// BioXAS M1 motors
	m1VertUpStreamINB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I21-01 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I21-01"), QString("SMTR1607-5-I21-01 VERT INB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m1VertUpStreamOUTB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I21-02 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I21-02"), QString("SMTR1607-5-I21-02 VERT OUTB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m1VertDownStream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I21-03 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I21-03"), QString("SMTR1607-5-I21-03 VERT (DOWNSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m1StripeSelect_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I21-04 STRIPE SELECT"), QString("SMTR1607-5-I21-04"), QString("SMTR1607-5-I21-04 STRIPE SELECT"), QString(":mm"),  true, 0.05, 2.0, this);
	m1Yaw_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I21-05 YAW"), QString("SMTR1607-5-I21-05"), QString("SMTR1607-5-I21-05 YAW"), QString(":mm"),  true, 0.05, 2.0, this);
	m1BenderUpstream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I21-06 BENDER (UPSTREAM)"), QString("SMTR1607-5-I21-06"), QString("SMTR1607-5-I21-06 BENDER (UPSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);
	m1BenderDownStream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I21-07 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I21-07"), QString("SMTR1607-5-I21-07 BENDER (DOWNSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);
	m1UpperSlitBlade_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I21-08 UPPER SLIT BLADE"), QString("SMTR1607-5-I21-08"), QString("SMTR1607-5-I21-08 UPPER SLIT BLADE"), QString(":mm"),  true, 0.05, 2.0, this);

	// BioXAS Variable Mask motors
	variableMaskVertUpperBlade_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MaskMotor,   QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), QString("SMTR1607-5-I21-09"), QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), QString(":mm"),  true, 0.05, 2.0, this);
	variableMaskVertLowerBlade_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MaskMotor,   QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), QString("SMTR1607-5-I21-10"), QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), QString(":mm"),  true, 0.05, 2.0, this);

	// BioXAS Mono motors
	monoPhosphorPaddle_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I21-11"), QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), QString(":mm"),  true, 0.05, 2.0, this);
	monoBragg_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I21-12 BRAGG"), QString("SMTR1607-5-I21-12"), QString("SMTR1607-5-I21-12 BRAGG"), QString(":deg"), true, 0.05, 2.0, this);
	monoVertical_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I21-13 VERTICAL"), QString("SMTR1607-5-I21-13"), QString("SMTR1607-5-I21-13 VERTICAL"), QString(":mm"),  true, 0.05, 2.0, this);
	monoLateral_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I21-14 LATERAL"), QString("SMTR1607-5-I21-14"), QString("SMTR1607-5-I21-14 LATERAL"), QString(":mm"),  true, 0.05, 2.0, this);
	monoXtalXchage_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I21-22 XTAL XCHAGE"), QString("SMTR1607-5-I21-22"), QString("SMTR1607-5-I21-22 XTAL XCHAGE"), QString(":mm"),  true, 0.05, 2.0, this);
	monoXtal1Pitch_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), QString("SMTR1607-5-I21-23"), QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), QString(":V"),   true, 0.05, 2.0, this);
	monoXtal1Roll_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I21-24 XTAL 1 ROLL"), QString("SMTR1607-5-I21-24"), QString("SMTR1607-5-I21-24 XTAL 1 ROLL"), QString(":V"),   true, 0.05, 2.0, this);
	monoXtal2Pitch_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), QString("SMTR1607-5-I21-25"), QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), QString(":V"),   true, 0.05, 2.0, this);
	monoXtal2Roll_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), QString("SMTR1607-5-I21-26"), QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), QString(":V"),   true, 0.05, 2.0, this);

	// BioXAS M2 motors
	m2VertUpstreamINB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I21-15"), QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m2VertUpstreamOUTB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I21-16"), QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m2VertDownstream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I21-17"), QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m2StripeSelect_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I21-18 STRIPE SELECT"), QString("SMTR1607-5-I21-18"), QString("SMTR1607-5-I21-18 STRIPE SELECT"), QString(":mm"),  true, 0.05, 2.0, this);
	m2Yaw_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I21-19 YAW"), QString("SMTR1607-5-I21-19"), QString("SMTR1607-5-I21-19 YAW"), QString(":mm"),  true, 0.05, 2.0, this);
	m2BenderUpstream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I21-20"), QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);
	m2BenderDownStream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I21-21"), QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);
}

void BioXASMainBeamline::setupDetectors()
{

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

}

void BioXASMainBeamline::setupControlsAsDetectors()
{

}

void BioXASMainBeamline::setupExposedControls()
{

}

void BioXASMainBeamline::setupExposedDetectors()
{

}

BioXASMainBeamline::~BioXASMainBeamline()
{

}

QList<BioXASCLSMAXvMotor *> BioXASMainBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType category)
{
	QList<BioXASCLSMAXvMotor *> matchedMotors;

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

	default:
		qDebug() << "ERROR: invalid BioXAS Motor category: " << category;
		break;
	}

	return matchedMotors;
}
