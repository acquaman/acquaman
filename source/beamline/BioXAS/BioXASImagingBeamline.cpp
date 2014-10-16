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


#include "BioXASImagingBeamline.h"

#include <QDebug>

BioXASImagingBeamline::BioXASImagingBeamline()
	: AMBeamline("BioXAS Beamline - Imaging Endstation")
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

void BioXASImagingBeamline::setupDiagnostics()
{

}

void BioXASImagingBeamline::setupSampleStage()
{

}

void BioXASImagingBeamline::setupMotorGroup()
{
	// BioXAS filter motors
	imagingCarbonFilterFarm1_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::FilterMotor, QString("SMTR1607-5-I00-05 Filter 1"), QString("SMTR1607-5-I00-05"), QString("SMTR1607-5-I00-05 Filter 1"), QString(":mm"),  true, 0.05, 2.0, this);
	imagingCarbonFilterFarm2_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::FilterMotor, QString("SMTR1607-5-I00-06 Filter 2"), QString("SMTR1607-5-I00-06"), QString("SMTR1607-5-I00-06 Filter 2"), QString(":mm"),  true, 0.05, 2.0, this);

	// BioXAS M1 motors
	m1VertUpStreamINB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I10-01 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I10-01"), QString("SMTR1607-5-I10-01 VERT INB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m1VertUpStreamOUTB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I10-02 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I10-02"), QString("SMTR1607-5-I10-02 VERT OUTB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m1VertDownStream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I10-03 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I10-03"), QString("SMTR1607-5-I10-03 VERT (DOWNSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m1StripeSelect_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I10-04 STRIPE SELECT"), QString("SMTR1607-5-I10-04"), QString("SMTR1607-5-I10-04 STRIPE SELECT"), QString(":mm"),  true, 0.05, 2.0, this);
	m1Yaw_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I10-05 YAW"), QString("SMTR1607-5-I10-05"), QString("SMTR1607-5-I10-05 YAW"), QString(":mm"),  true, 0.05, 2.0, this);
	m1BenderUpstream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I10-06 BENDER (UPSTREAM)"), QString("SMTR1607-5-I10-06"), QString("SMTR1607-5-I10-06 BENDER (UPSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);
	m1BenderDownStream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I10-07 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I10-07"), QString("SMTR1607-5-I10-07 BENDER (DOWNSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);

	// BioXAS Variable Mask motors
	variableMaskVertUpperBlade_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MaskMotor,   QString("SMTR1607-5-I10-08 VERT UPPER BLADE"), QString("SMTR1607-5-I10-08"), QString("SMTR1607-5-I10-08 VERT UPPER BLADE"), QString(":mm"),  true, 0.05, 2.0, this);
	variableMaskVertLowerBlade_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MaskMotor,   QString("SMTR1607-5-I10-09 VERT LOWER BLADE"), QString("SMTR1607-5-I10-09"), QString("SMTR1607-5-I10-09 VERT LOWER BLADE"), QString(":mm"),  true, 0.05, 2.0, this);

	// BioXAS Mono motors
	monoCrystal2Z_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I10-10 CRYSTAL 2 Z"), QString("SMTR1607-5-I10-10"), QString("SMTR1607-5-I10-10 CRYSTAL 2 Z"), QString(":mm"),  true, 0.05, 2.0, this);
	monoCrystal2Y_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I10-11 CRYSTAL 2 Y"), QString("SMTR1607-5-I10-11"), QString("SMTR1607-5-I10-11 CRYSTAL 2 Y"), QString(":mm"),  true, 0.05, 2.0, this);
	monoCrystal2Pitch_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I10-12 CRYSTAL 2 PITCH"), QString("SMTR1607-5-I10-12"), QString("SMTR1607-5-I10-12 CRYSTAL 2 PITCH"), QString(":V"),   true, 0.05, 2.0, this);
	monoCrystal2Roll_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I10-13 CRYSTAL 2 ROLL"), QString("SMTR1607-5-I10-13"), QString("SMTR1607-5-I10-13 CRYSTAL 2 ROLL"), QString(":V"),   true, 0.05, 2.0, this);
	monoBragg_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I10-14 BRAGG"), QString("SMTR1607-5-I10-14"), QString("SMTR1607-5-I10-14 BRAGG"), QString(":deg"), true, 0.05, 2.0, this);
	monoVertical_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I10-15 VERTICAL (Y)"), QString("SMTR1607-5-I10-15"), QString("SMTR1607-5-I10-15 VERTICAL (Y)"), QString(":mm"),  true, 0.05, 2.0, this);
	monoLateral_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I10-16 LATERAL (X)"), QString("SMTR1607-5-I10-16"), QString("SMTR1607-5-I10-16 LATERAL (X)"), QString(":mm"),  true, 0.05, 2.0, this);

	// BioXAS M2 motors
	m2VertUpstreamINB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I10-17 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I10-17"), QString("SMTR1607-5-I10-17 VERT INB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m2VertUpstreamOUTB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I10-18 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I10-18"), QString("SMTR1607-5-I10-18 VERT OUTB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m2VertDownstream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I10-19 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I10-19"), QString("SMTR1607-5-I10-19 VERT (DOWNSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m2StripeSelect_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I10-20 STRIPE SELECT"), QString("SMTR1607-5-I10-20"), QString("SMTR1607-5-I10-20 STRIPE SELECT"), QString(":mm"),  true, 0.05, 2.0, this);
	m2Yaw_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I10-21 YAW"), QString("SMTR1607-5-I10-21"), QString("SMTR1607-5-I10-21 YAW"), QString(":mm"),  true, 0.05, 2.0, this);
	m2BenderUpstream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I10-22 BENDER (UPSTREAM)"), QString("SMTR1607-5-I10-22"), QString("SMTR1607-5-I10-22 BENDER (UPSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);
	m2BenderDownStream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I10-23 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I10-23"), QString("SMTR1607-5-I10-23 BENDER (DOWNSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);
}

void BioXASImagingBeamline::setupDetectors()
{

}

void BioXASImagingBeamline::setupControlSets()
{

}

void BioXASImagingBeamline::setupMono()
{

}

void BioXASImagingBeamline::setupSynchronizedDwellTime()
{

}

void BioXASImagingBeamline::setupComponents()
{

}

void BioXASImagingBeamline::setupControlsAsDetectors()
{

}

void BioXASImagingBeamline::setupExposedControls()
{

}

void BioXASImagingBeamline::setupExposedDetectors()
{

}

BioXASImagingBeamline::~BioXASImagingBeamline()
{

}

QList<BioXASCLSMAXvMotor *> BioXASImagingBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType category)
{
	QList<BioXASCLSMAXvMotor *> matchedMotors;

	switch (category) {
	case BioXASBeamlineDef::FilterMotor: // BioXAS Filter motors
		matchedMotors.append(imagingCarbonFilterFarm1_);
		matchedMotors.append(imagingCarbonFilterFarm2_);
		break;

	case BioXASBeamlineDef::M1Motor:	// BioXAS M1 motors
		matchedMotors.append(m1VertUpStreamINB_);
		matchedMotors.append(m1VertUpStreamOUTB_);
		matchedMotors.append(m1VertDownStream_);
		matchedMotors.append(m1StripeSelect_);
		matchedMotors.append(m1Yaw_);
		matchedMotors.append(m1BenderUpstream_);
		matchedMotors.append(m1BenderDownStream_);
		break;

	case BioXASBeamlineDef::MaskMotor:	// BioXAS Variable Mask motors
		matchedMotors.append(variableMaskVertUpperBlade_);
		matchedMotors.append(variableMaskVertLowerBlade_);
		break;

	case BioXASBeamlineDef::MonoMotor:	// BioXAS Mono motors
		matchedMotors.append(monoCrystal2Z_);
		matchedMotors.append(monoCrystal2Y_);
		matchedMotors.append(monoCrystal2Pitch_);
		matchedMotors.append(monoCrystal2Roll_);
		matchedMotors.append(monoBragg_);
		matchedMotors.append(monoVertical_);
		matchedMotors.append(monoLateral_);
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
