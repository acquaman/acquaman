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

BioXASImagingBeamline::~BioXASImagingBeamline()
{

}

bool BioXASImagingBeamline::isConnected() const
{
	bool connected = (
				carbonFilterFarm_ && carbonFilterFarm_->isConnected()
				);

	return connected;
}

QList<AMControl *> BioXASImagingBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType category)
{
	QList<AMControl *> matchedMotors;

	switch (category) {
	case BioXASBeamlineDef::FilterMotor: // BioXAS Filter motors
		matchedMotors.append(carbonFilterFarm_->upstreamMotor());
		matchedMotors.append(carbonFilterFarm_->downstreamMotor());
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

	default:
		qDebug() << "ERROR: invalid BioXAS Motor category: " << category;
		break;
	}

	return matchedMotors;
}

void BioXASImagingBeamline::setupComponents()
{
	/// Carbon filter farm.

	carbonFilterFarm_ = new BioXASImagingCarbonFilterFarm(this);
	connect( carbonFilterFarm_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// BioXAS filter motors
	imagingCarbonFilterFarm1_ = new CLSMAXvMotor(QString("SMTR1607-5-I00-05 Filter 1"), QString("SMTR1607-5-I00-05"), QString("SMTR1607-5-I00-05 Filter 1"), true, 0.05, 2.0, this, QString(":mm"));
	imagingCarbonFilterFarm2_ = new CLSMAXvMotor(QString("SMTR1607-5-I00-06 Filter 2"), QString("SMTR1607-5-I00-06"), QString("SMTR1607-5-I00-06 Filter 2"), true, 0.05, 2.0, this, QString(":mm"));

	// BioXAS M1 motors
	m1VertUpStreamINB_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-01 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I10-01"), QString("SMTR1607-5-I10-01 VERT INB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m1VertUpStreamOUTB_ = new CLSMAXvMotor( QString("SMTR1607-5-I10-02 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I10-02"), QString("SMTR1607-5-I10-02 VERT OUTB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m1VertDownStream_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-03 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I10-03"), QString("SMTR1607-5-I10-03 VERT (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m1StripeSelect_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-04 STRIPE SELECT"), QString("SMTR1607-5-I10-04"), QString("SMTR1607-5-I10-04 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	m1Yaw_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-05 YAW"), QString("SMTR1607-5-I10-05"), QString("SMTR1607-5-I10-05 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	m1BenderUpstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-06 BENDER (UPSTREAM)"), QString("SMTR1607-5-I10-06"), QString("SMTR1607-5-I10-06 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	m1BenderDownStream_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-07 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I10-07"), QString("SMTR1607-5-I10-07 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));

	// BioXAS Variable Mask motors
	variableMaskVertUpperBlade_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-08 VERT UPPER BLADE"), QString("SMTR1607-5-I10-08"), QString("SMTR1607-5-I10-08 VERT UPPER BLADE"), true, 0.05, 2.0, this, QString(":mm"));
	variableMaskVertLowerBlade_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-09 VERT LOWER BLADE"), QString("SMTR1607-5-I10-09"), QString("SMTR1607-5-I10-09 VERT LOWER BLADE"), true, 0.05, 2.0, this, QString(":mm"));

	// BioXAS Mono motors
	monoCrystal2Z_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-10 CRYSTAL 2 Z"), QString("SMTR1607-5-I10-10"), QString("SMTR1607-5-I10-10 CRYSTAL 2 Z"), true, 0.05, 2.0, this, QString(":mm"));
	monoCrystal2Y_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-11 CRYSTAL 2 Y"), QString("SMTR1607-5-I10-11"), QString("SMTR1607-5-I10-11 CRYSTAL 2 Y"), true, 0.05, 2.0, this, QString(":mm"));
	monoCrystal2Pitch_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-12 CRYSTAL 2 PITCH"), QString("SMTR1607-5-I10-12"), QString("SMTR1607-5-I10-12 CRYSTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	monoCrystal2Roll_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-13 CRYSTAL 2 ROLL"), QString("SMTR1607-5-I10-13"), QString("SMTR1607-5-I10-13 CRYSTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V"));
	monoBragg_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-14 BRAGG"), QString("SMTR1607-5-I10-14"), QString("SMTR1607-5-I10-14 BRAGG"), true, 0.05, 2.0, this, QString(":deg"));
	monoVertical_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-15 VERTICAL (Y)"), QString("SMTR1607-5-I10-15"), QString("SMTR1607-5-I10-15 VERTICAL (Y)"), true, 0.05, 2.0, this, QString(":mm"));
	monoLateral_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-16 LATERAL (X)"), QString("SMTR1607-5-I10-16"), QString("SMTR1607-5-I10-16 LATERAL (X)"), true, 0.05, 2.0, this, QString(":mm"));

	// BioXAS M2 motors
	m2VertUpstreamINB_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-17 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I10-17"), QString("SMTR1607-5-I10-17 VERT INB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m2VertUpstreamOUTB_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-18 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I10-18"), QString("SMTR1607-5-I10-18 VERT OUTB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m2VertDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-19 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I10-19"), QString("SMTR1607-5-I10-19 VERT (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	m2StripeSelect_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-20 STRIPE SELECT"), QString("SMTR1607-5-I10-20"), QString("SMTR1607-5-I10-20 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	m2Yaw_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-21 YAW"), QString("SMTR1607-5-I10-21"), QString("SMTR1607-5-I10-21 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	m2BenderUpstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-22 BENDER (UPSTREAM)"), QString("SMTR1607-5-I10-22"), QString("SMTR1607-5-I10-22 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	m2BenderDownStream_ = new CLSMAXvMotor(QString("SMTR1607-5-I10-23 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I10-23"), QString("SMTR1607-5-I10-23 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));

	// BioXAS M1 Pseudo motors					   name,				   pvBaseName,				readPVname,	writePVname, movingPVname,	enabledPVname, stopPVname, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description, parent = 0
	m1PseudoRoll_ = new BioXASPseudoMotorControl("BL1607-5-I10 Imaging M1 Roll", "BL1607-5-I10:M1:Roll", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m1PseudoPitch_ = new BioXASPseudoMotorControl("BL1607-5-I10 Imaging M1 Pitch", "BL1607-5-I10:M1:Pitch", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m1PseudoHeight_ = new BioXASPseudoMotorControl("BL1607-5-I10 Imaging M1 Height", "BL1607-5-I10:M1:Height", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");
	m1PseudoYaw_ = new BioXASPseudoMotorControl("BL1607-5-I10 Imaging M1 Yaw", "BL1607-5-I10:M1:Yaw", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m1PseudoLateral_ = new BioXASPseudoMotorControl("BL1607-5-I10 Imaging M1 Lateral", "BL1607-5-I10:M1:Lateral", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");

	// BioXAS M2 Pseudo motors					   name,				   pvBaseName,				readPVname,	writePVname, movingPVname,	enabledPVname, stopPVname, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description, parent = 0
	m2PseudoRoll_ = new BioXASPseudoMotorControl("BL1607-5-I10 Imaging M2 Roll", "BL1607-5-I10:M2:Roll", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m2PseudoPitch_ = new BioXASPseudoMotorControl("BL1607-5-I10 Imaging M2 Pitch", "BL1607-5-I10:M2:Pitch", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m2PseudoHeight_ = new BioXASPseudoMotorControl("BL1607-5-I10 Imaging M2 Height", "BL1607-5-I10:M2:Height", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");
	m2PseudoYaw_ = new BioXASPseudoMotorControl("BL1607-5-I10 Imaging M2 Yaw", "BL1607-5-I10:M2:Yaw", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	m2PseudoLateral_ = new BioXASPseudoMotorControl("BL1607-5-I10 Imaging M2 Lateral", "BL1607-5-I10:M2:Lateral", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");
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

BioXASImagingBeamline::BioXASImagingBeamline()
	: BioXASBeamline("BioXAS Beamline - Imaging Endstation")
{
	setupComponents();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();
}
