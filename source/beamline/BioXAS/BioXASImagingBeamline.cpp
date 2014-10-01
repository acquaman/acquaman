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

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSBiStateControl.h"

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
	carbonFilterFarm1_ = new BioXASMAXvMotor(BioXASBeamlineDef::FilterMotor, QString("Carbon Filter Farm"), QString("SMTR1607-5-I00-05"), QString(":mm"), QString("SMTR1607-5-I00-05 Filter 1"));
	carbonFilterFarm2_ = new BioXASMAXvMotor(BioXASBeamlineDef::FilterMotor, QString("Carbon Filter Farm"), QString("SMTR1607-5-I00-06"), QString(":mm"), QString("SMTR1607-5-I00-06 Filter 2"));

	// BioXAS M1 motors
	m1VertUpStreamINB_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Image M1"), QString("SMTR1607-5-I10-01"), QString(":mm"), QString("SMTR1607-5-I10-01 VERT INB (UPSTREAM)"));
	m1VertUpStreamOUTB_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Image M1"), QString("SMTR1607-5-I10-02"), QString(":mm"), QString("SMTR1607-5-I10-02 VERT OUTB (UPSTREAM)"));
	m1VertDownStream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Image M1"), QString("SMTR1607-5-I10-03"), QString(":mm"), QString("SMTR1607-5-I10-03 VERT (DOWNSTREAM)"));
	m1StripeSelect_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Image M1"), QString("SMTR1607-5-I10-04"), QString(":mm"), QString("SMTR1607-5-I10-04 STRIPE SELECT"));
	m1Yaw_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Image M1"), QString("SMTR1607-5-I10-05"), QString(":mm"), QString("SMTR1607-5-I10-05 YAW"));
	m1BenderUpstream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Image M1"), QString("SMTR1607-5-I10-06"), QString(":lbs"), QString("SMTR1607-5-I10-06 BENDER (UPSTREAM)"));
	m1BenderDownStream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M1Motor, QString("Image M1"), QString("SMTR1607-5-I10-07"), QString(":lbs"), QString("SMTR1607-5-I10-07 BENDER (DOWNSTREAM)"));

	// BioXAS Variable Mask motors
	variableMaskVertUpperBlade_ = new BioXASMAXvMotor(BioXASBeamlineDef::MaskMotor, QString("Image Variable Mask"), QString("SMTR1607-5-I10-08"), QString(":mm"), QString("SMTR1607-5-I10-08 VERT UPPER BLADE"));
	variableMaskVertLowerBlade_ = new BioXASMAXvMotor(BioXASBeamlineDef::MaskMotor, QString("Image Variable Mask"), QString("SMTR1607-5-I10-09"), QString(":mm"), QString("SMTR1607-5-I10-09 VERT LOWER BLADE"));

	// BioXAS Mono motors
	monoCrystal2Z_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Image Mono"), QString("SMTR1607-5-I10-10"), QString(":mm"), QString("SMTR1607-5-I10-10 CRYSTAL 2 Z"));
	monoCrystal2Y_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Image Mono"), QString("SMTR1607-5-I10-11"), QString(":mm"), QString("SMTR1607-5-I10-11 CRYSTAL 2 Y"));
	monoCrystal2Pitch_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Image Mono"), QString("SMTR1607-5-I10-12"), QString(":V"), QString("SMTR1607-5-I10-12 CRYSTAL 2 PITCH"));
	monoCrystal2Roll_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Image Mono"), QString("SMTR1607-5-I10-13"), QString(":V"), QString("SMTR1607-5-I10-13 CRYSTAL 2 ROLL"));
	monoBragg_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Image Mono"), QString("SMTR1607-5-I10-14"), QString(":deg"), QString("SMTR1607-5-I10-14 BRAGG"));
	monoVertical_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Image Mono"), QString("SMTR1607-5-I10-15"), QString(":mm"), QString("SMTR1607-5-I10-15 VERTICAL (Y)"));
	monoLateral_ = new BioXASMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("Image Mono"), QString("SMTR1607-5-I10-16"), QString(":mm"), QString("SMTR1607-5-I10-16 LATERAL (X)"));

	// BioXAS M2 motors
	m2VertUpstreamINB_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Image M2"), QString("SMTR1607-5-I10-17"), QString(":mm"), QString("SMTR1607-5-I10-17 VERT INB (UPSTREAM)"));
	m2VertUpstreamOUTB_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Image M2"), QString("SMTR1607-5-I10-18"), QString(":mm"), QString("SMTR1607-5-I10-18 VERT OUTB (UPSTREAM)"));
	m2VertDownstream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Image M2"), QString("SMTR1607-5-I10-19"), QString(":mm"), QString("SMTR1607-5-I10-19 VERT (DOWNSTREAM)"));
	m2StripeSelect_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Image M2"), QString("SMTR1607-5-I10-20"), QString(":mm"), QString("SMTR1607-5-I10-20 STRIPE SELECT"));
	m2Yaw_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Image M2"), QString("SMTR1607-5-I10-21"), QString(":mm"), QString("SMTR1607-5-I10-21 YAW"));
	m2BenderUpstream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Image M2"), QString("SMTR1607-5-I10-22"), QString(":lbs"), QString("SMTR1607-5-I10-22 BENDER (UPSTREAM)"));
	m2BenderDownStream_ = new BioXASMAXvMotor(BioXASBeamlineDef::M2Motor, QString("Image M2"), QString("SMTR1607-5-I10-23"), QString(":lbs"), QString("SMTR1607-5-I10-23 BENDER (DOWNSTREAM)"));
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

QList<BioXASMAXvMotor *> BioXASImagingBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType category)
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

	return matchedMotors;
}
