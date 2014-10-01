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

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSBiStateControl.h"

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
	// BioXAX Carbon Filter Farm Motors
	carbonFilterFarm1_ = new BioXASMAXvMotor(BioXASBeamlineDef::FilterMotor, QString("Carbon Filter Farm"), QString("SMTR1607-5-I00-03"), QString(":mm"), QString("SMTR1607-5-I00-03 Filter 1"));
	carbonFilterFarm2_ = new BioXASMAXvMotor(BioXASBeamlineDef::FilterMotor, QString("Carbon Filter Farm"), QString("SMTR1607-5-I00-04"), QString(":mm"), QString("SMTR1607-5-I00-04 Filter 2"));

	// BioXAX M1 Motors
	m1VertUpStreamINB_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Main M1"), QString("SMTR1607-5-I21-01"), QString(":mm"), QString("SMTR1607-5-I21-01 VERT INB (UPSTREAM)"));
	m1VertUpStreamOUTB_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Main M1"), QString("SMTR1607-5-I21-02"), QString(":mm"), QString("SMTR1607-5-I21-02 VERT OUTB (UPSTREAM)"));
	m1VertDownStream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Main M1"), QString("SMTR1607-5-I21-03"), QString(":mm"), QString("SMTR1607-5-I21-03 VERT (DOWNSTREAM)"));
	m1StripeSelect_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Main M1"), QString("SMTR1607-5-I21-04"), QString(":mm"), QString("SMTR1607-5-I21-04 STRIPE SELECT"));
	m1Yaw_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Main M1"), QString("SMTR1607-5-I21-05"), QString(":mm"), QString("SMTR1607-5-I21-05 YAW"));
	m1BenderUpstream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Main M1"), QString("SMTR1607-5-I21-06"), QString(":lbs"), QString("SMTR1607-5-I21-06 BENDER (UPSTREAM)"));
	m1BenderDownStream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Main M1"), QString("SMTR1607-5-I21-07"), QString(":lbs"), QString("SMTR1607-5-I21-07 BENDER (DOWNSTREAM)"));
	m1UpperSlitBlade_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Main M1"), QString("SMTR1607-5-I21-08"), QString(":mm"), QString("SMTR1607-5-I21-08 UPPER SLIT BLADE"));

	// BioXAS Variable Mask motors
	variableMaskVertUpperBlade_ = new BioXASMAXvMotor(BioXASBeamlineDef::MaskMotor, QString("Main Variable Mask"), QString("SMTR1607-5-I21-09"), QString(":mm"), QString("SMTR1607-5-I21-09 VERT UPPER BLADE"));
	variableMaskVertLowerBlade_ = new BioXASMAXvMotor(BioXASBeamlineDef::MaskMotor, QString("Main Variable Mask"), QString("SMTR1607-5-I21-10"), QString(":mm"), QString("SMTR1607-5-I21-10 VERT LOWER BLADE"));

	// BioXAS Mono motors
	monoPhosphorPaddle_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Main Mono"), QString("SMTR1607-5-I21-11"), QString(":mm"), QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"));
	monoBragg_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Main Mono"), QString("SMTR1607-5-I21-12"), QString(":deg"), QString("SMTR1607-5-I21-12 BRAGG"));
	monoVertical_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Main Mono"), QString("SMTR1607-5-I21-13"), QString(":mm"), QString("SMTR1607-5-I21-13 VERTICAL"));
	monoLateral_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Main Mono"), QString("SMTR1607-5-I21-14"), QString(":mm"), QString("SMTR1607-5-I21-14 LATERAL"));
	monoXtalXchage_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Main Mono"), QString("SMTR1607-5-I21-22"), QString(":mm"), QString("SMTR1607-5-I21-22 XTAL XCHAGE"));
	monoXtal1Pitch_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Main Mono"), QString("SMTR1607-5-I21-23"), QString(":V"), QString("SMTR1607-5-I21-23 XTAL 1 PITCH"));
	monoXtal1Roll_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Main Mono"), QString("SMTR1607-5-I21-24"), QString(":V"), QString("SMTR1607-5-I21-24 XTAL 1 ROLL"));
	monoXtal2Pitch_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Main Mono"), QString("SMTR1607-5-I21-25"), QString(":V"), QString("SMTR1607-5-I21-25 XTAL 2 PITCH"));
	monoXtal2Roll_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Main Mono"), QString("SMTR1607-5-I21-26"), QString(":V"), QString("SMTR1607-5-I21-26 XTAL 2 ROLL"));

	// BioXAS M2 motors
	m2VertUpstreamINB_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Main M2"), QString("SMTR1607-5-I21-15"), QString(":mm"), QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"));
	m2VertUpstreamOUTB_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Main M2"), QString("SMTR1607-5-I21-16"), QString(":mm"), QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"));
	m2VertDownstream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Main M2"), QString("SMTR1607-5-I21-17"), QString(":mm"), QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"));
	m2StripeSelect_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Main M2"), QString("SMTR1607-5-I21-18"), QString(":mm"), QString("SMTR1607-5-I21-18 STRIPE SELECT"));
	m2Yaw_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Main M2"), QString("SMTR1607-5-I21-19"), QString(":mm"), QString("SMTR1607-5-I21-19 YAW"));
	m2BenderUpstream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Main M2"), QString("SMTR1607-5-I21-20"), QString(":lbs"), QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"));
	m2BenderDownStream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Main M2"), QString("SMTR1607-5-I21-21"), QString(":lbs"), QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"));;
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

QList<BioXASMAXvMotor *> BioXASMainBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType category)
{
	QList<BioXASMAXvMotor *> matchedMotors;

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

	return matchedMotors;
}
