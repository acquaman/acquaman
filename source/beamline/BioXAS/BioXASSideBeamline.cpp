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


#include "BioXASSideBeamline.h"

#include <QDebug>

#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMBasicControlDetectorEmulator.h"

BioXASSideBeamline::BioXASSideBeamline()
	: AMBeamline("BioXAS Beamline - Side Endstation")
{
	wasConnected_ = false;

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

void BioXASSideBeamline::setupDiagnostics()
{
	// Shutter controls
	psh1_ = new CLSBiStateControl("IPSH1", "First Photon Shutter", "IPSH1407-I00-01:state", "IPSH1407-I00-01:opr:open", "IPSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(0), this);
	psh2_ = new CLSBiStateControl("IPSH2", "Second Photon Shutter", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(0), this);
	ssh1_ = new CLSBiStateControl("SSH", "First Safety Shutter", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(0), this);

	sshSide1_ = new CLSBiStateControl("SSH Side", "Side Safety Shutter", "SSH1607-5-I22-01:state", "SSH1607-5-I22-01:opr:open", "SSH1607-5-I22-01:opr:close", new AMControlStatusCheckerDefault(0), this);

	// Pressure controls
	ccg1_ = new AMReadOnlyPVwStatusControl("Pressure 1", "CCG1407-I00-01:vac:p", "CCG1407-I00-01:vac", this, new AMControlStatusCheckerDefault(0));
	ccg2_ = new AMReadOnlyPVwStatusControl("Pressure 2", "CCG1407-I00-02:vac:p", "CCG1407-I00-02:vac", this, new AMControlStatusCheckerDefault(0));
	ccg3_ = new AMReadOnlyPVwStatusControl("Pressure 3", "CCG1607-5-I00-02:vac:p", "CCG1607-5-I00-02:vac", this, new AMControlStatusCheckerDefault(0));

	ccgSide1_ = new AMReadOnlyPVwStatusControl("Side pressure 1", "CCG1607-5-I00-03:vac:p", "CCG1607-5-I00-03:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide2_ = new AMReadOnlyPVwStatusControl("Side pressure 2", "CCG1607-5-I00-04:vac:p", "CCG1607-5-I00-04:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide3_ = new AMReadOnlyPVwStatusControl("Side pressure 3", "CCG1607-5-I00-05:vac:p", "CCG1607-5-I00-05:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide4_ = new AMReadOnlyPVwStatusControl("Side pressure 4", "CCG1607-5-I22-01:vac:p", "CCG1607-5-I22-01:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide5_ = new AMReadOnlyPVwStatusControl("Side pressure 5", "CCG1607-5-I22-02:vac:p", "CCG1607-5-I22-02:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide6_ = new AMReadOnlyPVwStatusControl("Side pressure 6", "CCG1607-5-I22-03:vac:p", "CCG1607-5-I22-03:vac", this, new AMControlStatusCheckerDefault(0));

	// Vaccum valve controls
	vvr1_ = new CLSBiStateControl("Valve control 1", "Valve control 1", "VVR1407-I00-01:state", "VVR1407-I00-01:opr:open", "VVR1407-I00-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr2_ = new CLSBiStateControl("Valve control 2", "Valve control 2", "VVR1607-5-I00-01:state", "VVR1607-5-I00-01:opr:open", "VVR1607-5-I00-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr3_ = new CLSBiStateControl("Valve control 3", "Valve control 3", "VVR1607-5-I22-01:state", "VVR1607-5-I22-01:opr:open", "VVR1607-5-I22-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr4_ = new CLSBiStateControl("Valve control 4", "Valve control 4", "VVR1607-5-I21-01:state", "VVR1607-5-I21-01:opr:open", "VVR1607-5-I21-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr5_ = new CLSBiStateControl("Valve control 5", "Valve control 5", "VVR1607-5-I10-01:state", "VVR1607-5-I10-01:opr:open", "VVR1607-5-I10-01:opr:close", new AMControlStatusCheckerDefault(4), this);

	vvrSide1_ = new CLSBiStateControl("Side valve control 1", "Side valve control 1", "VVR1607-5-I22-02:state", "VVR1607-5-I22-02:opr:open", "VVR1607-5-I22-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide2_ = new CLSBiStateControl("Side valve control 2", "Side valve control 2", "VVR1607-5-I22-03:state", "VVR1607-5-I22-03:opr:open", "VVR1607-5-I22-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide3_ = new CLSBiStateControl("Side valve control 3", "Side valve control 3", "VVR1607-5-I22-04:state", "VVR1607-5-I22-04:opr:open", "VVR1607-5-I22-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide4_ = new CLSBiStateControl("Side valve control 4", "Side valve control 4", "VVR1607-5-I22-05:state", "VVR1607-5-I22-05:opr:open", "VVR1607-5-I22-05:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide5_ = new CLSBiStateControl("Side valve control 5", "Side valve control 5", "VVR1607-5-I22-06:state", "VVR1607-5-I22-06:opr:open", "VVR1607-5-I22-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide6_ = new CLSBiStateControl("Side valve control 6", "Side valve control 6", "VVR1607-5-I22-07:state", "VVR1607-5-I22-07:opr:open", "VVR1607-5-I22-07:opr:close", new AMControlStatusCheckerDefault(4), this);

	// Ion pump controls
	iop1_ = new AMReadOnlyPVControl("Ion pump 1", "IOP1407-I00-01", this);
	iop2_ = new AMReadOnlyPVControl("Ion pump 2", "IOP1407-I00-02", this);
	iop3_ = new AMReadOnlyPVControl("Ion pump 3", "IOP1407-I00-03", this);
	iop4_ = new AMReadOnlyPVControl("Ion pump 4", "IOP1607-5-I00-01", this);
	iop5_ = new AMReadOnlyPVControl("Ion pump 5", "IOP1607-5-I00-02", this);

	iopSide1_ = new AMReadOnlyPVControl("Side ion pump 1", "IOP1607-5-I00-03", this);
	iopSide2_ = new AMReadOnlyPVControl("Side ion pump 2", "IOP1607-5-I00-04", this);
	iopSide3_ = new AMReadOnlyPVControl("Side ion pump 3", "IOP1607-5-I00-05", this);
	iopSide4_ = new AMReadOnlyPVControl("Side ion pump 4", "IOP1607-5-I22-01", this);
	iopSide5_ = new AMReadOnlyPVControl("Side ion pump 5", "IOP1607-5-I22-02", this);
	iopSide6_ = new AMReadOnlyPVControl("Side ion pump 6", "IOP1607-5-I22-03", this);
	iopSide7_ = new AMReadOnlyPVControl("Side ion pump 7", "IOP1607-5-I22-04", this);

	// Flow transducer controls
	flt1_ = new AMReadOnlyPVwStatusControl("Flow transducer 1", "FLT1407-I00-01", "FLT1407-I00-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt2_ = new AMReadOnlyPVwStatusControl("Flow transducer 2", "FLT1407-I00-02", "FLT1407-I00-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt3_ = new AMReadOnlyPVwStatusControl("Flow transducer 3", "FLT1407-I00-03", "FLT1407-I00-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt4_ = new AMReadOnlyPVwStatusControl("Flow transducer 4", "FLT1607-5-I00-01", "FLT1607-5-I00-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt5_ = new AMReadOnlyPVwStatusControl("Flow transducer 5", "FLT1607-5-I10-01", "FLT1607-5-I10-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt6_ = new AMReadOnlyPVwStatusControl("Flow transducer 6", "FLT1607-5-I10-02", "FLT1607-5-I10-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt7_ = new AMReadOnlyPVwStatusControl("Flow transducer 7", "FLT1607-5-I10-03", "FLT1607-5-I10-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt8_ = new AMReadOnlyPVwStatusControl("Flow transducer 8", "FLT1607-5-I10-04", "FLT1607-5-I10-04:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt9_ = new AMReadOnlyPVwStatusControl("Flow transducer 9", "FLT1607-5-I10-05", "FLT1607-5-I10-05:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt10_ = new AMReadOnlyPVwStatusControl("Flow transducer 10", "FLT1607-5-I10-06", "FLT1607-5-I10-06:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt11_ = new AMReadOnlyPVwStatusControl("Flow transducer 11", "FLT1607-5-I10-07", "FLT1607-5-I10-07:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt12_ = new AMReadOnlyPVwStatusControl("Flow transducer 12", "FLT1607-5-I20-01", "FLT1607-5-I20-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt13_ = new AMReadOnlyPVwStatusControl("Flow transducer 13", "FLT1607-5-I21-01", "FLT1607-5-I21-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt14_ = new AMReadOnlyPVwStatusControl("Flow transducer 14", "FLT1607-5-I21-02", "FLT1607-5-I21-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt15_ = new AMReadOnlyPVwStatusControl("Flow transducer 15", "FLT1607-5-I21-03", "FLT1607-5-I21-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt16_ = new AMReadOnlyPVwStatusControl("Flow transducer 16", "FLT1607-5-I21-04", "FLT1607-5-I21-04:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt17_ = new AMReadOnlyPVwStatusControl("Flow transducer 17", "FLT1607-5-I22-01", "FLT1607-5-I22-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt18_ = new AMReadOnlyPVwStatusControl("Flow transducer 18", "FLT1607-5-I22-02", "FLT1607-5-I22-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt19_ = new AMReadOnlyPVwStatusControl("Flow transducer 19", "FLT1607-5-I22-03", "FLT1607-5-I22-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt20_ = new AMReadOnlyPVwStatusControl("Flow transducer 20", "FLT1607-5-I22-04", "FLT1607-5-I22-04:lowflow", this, new AMControlStatusCheckerDefault(0));

	// Flow switch controls
	swf1_ = new AMReadOnlyPVControl("Flow switch 1", "SWF1407-I00-01", this);
	swf2_ = new AMReadOnlyPVControl("Flow switch 2", "SWF1407-I00-02", this);
	swf3_ = new AMReadOnlyPVControl("Flow switch 3", "SWF1407-I00-03", this);
	swf4_ = new AMReadOnlyPVControl("Flow switch 4", "SWF1607-5-I00-01", this);

	// Temperature controls
	tm1_ = new AMReadOnlyPVwStatusControl("Temperature 1", "TM1407-I00-01", "TM1407-I00-01:trip", this, new AMControlStatusCheckerDefault(0));
	tm2_ = new AMReadOnlyPVwStatusControl("Temperature 2", "TM1407-I00-02", "TM1407-I00-02:trip", this, new AMControlStatusCheckerDefault(0));
	tm3_ = new AMReadOnlyPVwStatusControl("Temperature 3", "TM1407-I00-03", "TM1407-I00-03:trip", this, new AMControlStatusCheckerDefault(0));
	tm4_ = new AMReadOnlyPVwStatusControl("Temperature 4", "TM1407-I00-04", "TM1407-I00-04:trip", this, new AMControlStatusCheckerDefault(0));
	tm5_ = new AMReadOnlyPVwStatusControl("Temperature 5", "TM1407-I00-05", "TM1407-I00-05:trip", this, new AMControlStatusCheckerDefault(0));
}

void BioXASSideBeamline::setupSampleStage()
{

}

void BioXASSideBeamline::setupMotorGroup()
{
	// BioXAS filter motors
	carbonFilterFarm1_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::FilterMotor, QString("SMTR1607-5-I00-01 Filter 1"), QString("SMTR1607-5-I00-01"), QString("SMTR1607-5-I00-01 Filter 1"), QString(":mm"),  true, 0.05, 2.0, this);
	carbonFilterFarm2_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::FilterMotor, QString("SMTR1607-5-I00-02 Filter 2"), QString("SMTR1607-5-I00-02"), QString("SMTR1607-5-I00-02 Filter 2"), QString(":mm"),  true, 0.05, 2.0, this);

	// BioXAS M1 motors
	m1VertUpStreamINB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I22-01 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I22-01"), QString("SMTR1607-5-I22-01 VERT INB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m1VertUpStreamOUTB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I22-02 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I22-02"), QString("SMTR1607-5-I22-02 VERT OUTB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m1VertDownStream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I22-03 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I22-03"), QString("SMTR1607-5-I22-03 VERT (DOWNSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m1StripeSelect_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I22-04 STRIPE SELECT"), QString("SMTR1607-5-I22-04"), QString("SMTR1607-5-I22-04 STRIPE SELECT"), QString(":mm"),  true, 0.05, 2.0, this);
	m1Yaw_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I22-05 YAW"), QString("SMTR1607-5-I22-05"), QString("SMTR1607-5-I22-05 YAW"), QString(":mm"),  true, 0.05, 2.0, this);
	m1BenderUpstream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I22-06 BENDER (UPSTREAM)"), QString("SMTR1607-5-I22-06"), QString("SMTR1607-5-I22-06 BENDER (UPSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);
	m1BenderDownStream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I22-07 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I22-07"), QString("SMTR1607-5-I22-07 BENDER (DOWNSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);
	m1UpperSlitBlade_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M1Motor,     QString("SMTR1607-5-I22-08 UPPER SLIT BLADE"), QString("SMTR1607-5-I22-08"), QString("SMTR1607-5-I22-08 UPPER SLIT BLADE"), QString(":mm"),  true, 0.05, 2.0, this);

	// BioXAS Variable Mask motors
	variableMaskVertUpperBlade_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MaskMotor,   QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), QString("SMTR1607-5-I22-09"), QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), QString(":mm"),  true, 0.05, 2.0, this);
	variableMaskVertLowerBlade_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MaskMotor,   QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), QString("SMTR1607-5-I22-10"), QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), QString(":mm"),  true, 0.05, 2.0, this);

	// BioXAS Mono motors
	monoPhosphorPaddle_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I22-11"), QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), QString(":mm"),  true, 0.05, 2.0, this);
	monoBragg_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), QString(":deg"), true, 0.05, 2.0, this);
	monoVertical_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I22-13 VERTICAL"), QString("SMTR1607-5-I22-13"), QString("SMTR1607-5-I22-13 VERTICAL"), QString(":mm"),  true, 0.05, 2.0, this);
	monoLateral_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I22-14 LATERAL"), QString("SMTR1607-5-I22-14"), QString("SMTR1607-5-I22-14 LATERAL"), QString(":mm"),  true, 0.05, 2.0, this);
	monoXtalXchage_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I22-22 XTAL XCHAGE"), QString("SMTR1607-5-I22-22"), QString("SMTR1607-5-I22-22 XTAL XCHAGE"), QString(":mm"),  true, 0.05, 2.0, this);
	monoXtal1Pitch_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), QString("SMTR1607-5-I22-23"), QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), QString(":V"),   true, 0.05, 2.0, this);
	monoXtal1Roll_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I22-24 XTAL 1 ROLL"), QString("SMTR1607-5-I22-24"), QString("SMTR1607-5-I22-24 XTAL 1 ROLL"), QString(":V"),   true, 0.05, 2.0, this);
	monoXtal2Pitch_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), QString("SMTR1607-5-I22-25"), QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), QString(":V"),   true, 0.05, 2.0, this);
	monoXtal2Roll_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor,   QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), QString("SMTR1607-5-I22-26"), QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), QString(":V"),   true, 0.05, 2.0, this);

	// BioXAS M2 motors
	m2VertUpstreamINB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I22-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I22-15"), QString("SMTR1607-5-I22-15 VERT INB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m2VertUpstreamOUTB_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I22-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I22-16"), QString("SMTR1607-5-I22-16 VERT OUTB (UPSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m2VertDownstream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I22-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I22-17"), QString("SMTR1607-5-I22-17 VERT (DOWNSTREAM)"), QString(":mm"),  true, 0.05, 2.0, this);
	m2StripeSelect_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I22-18 STRIPE SELECT"), QString("SMTR1607-5-I22-18"), QString("SMTR1607-5-I22-18 STRIPE SELECT"), QString(":mm"),  true, 0.05, 2.0, this);
	m2Yaw_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I22-19 YAW"), QString("SMTR1607-5-I22-19"), QString("SMTR1607-5-I22-19 YAW"), QString(":mm"),  true, 0.05, 2.0, this);
	m2BenderUpstream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I22-20"), QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);
	m2BenderDownStream_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::M2Motor,     QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I22-21"), QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), QString(":lbs"), true, 0.05, 2.0, this);

	// setup the M1 Upper slit Blade motor
	m1UpperSlitBlade_->setContextKnownDescription("Upper slit");
	connect( m1UpperSlitBlade_, SIGNAL(connected(bool)), this, SLOT(onM1UpperSlitConnectedChanged(bool)) );

}

void BioXASSideBeamline::setupDetectors()
{

}

void BioXASSideBeamline::setupControlSets()
{

}

void BioXASSideBeamline::setupMono()
{

}

void BioXASSideBeamline::setupSynchronizedDwellTime()
{

}

void BioXASSideBeamline::setupComponents()
{
	scaler_ = new CLSSIS3820Scaler("BL07ID-Side:mcs", this);

	i0Detector_ = new CLSBasicScalerChannelDetector("I0Detector", "I0 Detector", scaler_, 0, this);
	iTDetector_ = new CLSBasicScalerChannelDetector("ITDetector", "IT Detector", scaler_, 1, this);

	energyFeedbackControl_ = new AMReadOnlyPVControl("EnergyFeedback", "BL1607-5-I22:Energy:EV:fbk", this);
	energyFeedbackDetector_ = new AMBasicControlDetectorEmulator("EnergyFeedback", "Energy Feedback", energyFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

	connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnectedChanged(bool)) );

	i0Keithley_ = new CLSKeithley428("I0 Channel", "AMP1607-601:Gain");
	iTKeithley_ = new CLSKeithley428("IT Channel", "AMP1607-602:Gain");
	/*
	scaler_->channelAt(0)->setCurrentAmplifier(testChannel);
	scaler_->channelAt(0)->setVoltagRange(AMRange(1.0, 6.5));
	*/
	scaler_->channelAt(0)->setCustomChannelName("I0 Channel");
	scaler_->channelAt(1)->setCustomChannelName("IT Channel");

	m1UpperSlit_ = new CLSMAXvMotor("M1 Upper Slit", "SMTR1607-5-I22-08", "M1 Upper Slit Motor", true, 0.05, 2.0, this);
	m1UpperSlit_->setContextKnownDescription("Upper slit");

	//energy_ = new AMPVwStatusControl("Energy", "BL1607-5-I22:Energy:EV:fbk", "BL1607-5-I22:Energy:EV", "BL1607-5-I22:Energy:status", QString(), this, 0.75, 2.0, new AMControlStatusCheckerCLSMAXv());
	energy_ = new AMPVwStatusControl("Energy", "BL1607-5-I22:Energy:EV:fbk", "BL1607-5-I22:Energy:EV", "SMTR1607-5-I22-12:status", QString(), this, 1000, 2.0, new AMControlStatusCheckerCLSMAXv());

	connect( m1UpperSlit_, SIGNAL(connected(bool)), this, SLOT(onM1UpperSlitConnectedChanged(bool)) );
	connect( energy_, SIGNAL(connected(bool)), this, SLOT(onEnergyConnectedChanged(bool)) );
}

void BioXASSideBeamline::setupControlsAsDetectors()
{

}

void BioXASSideBeamline::setupExposedControls()
{
	addExposedControl(m1UpperSlitBlade_);
	addExposedControl(energy_);
	addExposedControl(m1UpperSlit_);
}

void BioXASSideBeamline::setupExposedDetectors()
{
	addExposedDetector(i0Detector_);
	addExposedDetector(iTDetector_);
	addExposedDetector(energyFeedbackDetector_);
}

BioXASSideBeamline::~BioXASSideBeamline()
{

}

CLSSIS3820Scaler* BioXASSideBeamline::scaler()
{
	return scaler_;
}

CLSMAXvMotor* BioXASSideBeamline::m1UpperSlit()
{
	return m1UpperSlitBlade_;
}

AMPVwStatusControl* BioXASSideBeamline::energy()
{
	return energy_;
}

CLSKeithley428* BioXASSideBeamline::i0Keithley()
{
	return i0Keithley_;
}

CLSKeithley428* BioXASSideBeamline::iTKeithley()
{
	return iTKeithley_;
}

bool BioXASSideBeamline::isConnected() const
{
	return scaler_->isConnected() && m1UpperSlit_->isConnected() && i0Keithley_->isConnected() && iTKeithley_->isConnected() && energy_->isConnected();
}

CLSBasicScalerChannelDetector* BioXASSideBeamline::i0Detector()
{
	return i0Detector_;
}

AMBasicControlDetectorEmulator* BioXASSideBeamline::energyFeedbackDetector()
{
	return energyFeedbackDetector_;
}

CLSBasicScalerChannelDetector* BioXASSideBeamline::iTDetector()
{
	return iTDetector_;
}

void BioXASSideBeamline::onScalerConnectedChanged(bool connectionState)
{
	Q_UNUSED(connectionState)

	if (wasConnected_ && !isConnected()) {
		emit connected(false);
		wasConnected_ = false;

	} else if (!wasConnected_ && isConnected()) {
		emit connected(true);
		wasConnected_ = true;
	}

}

void BioXASSideBeamline::onM1UpperSlitConnectedChanged(bool connectionState)
{
	Q_UNUSED(connectionState)

	if (wasConnected_ && !isConnected()) {
		emit connected(false);
		wasConnected_ = false;

	} else if (!wasConnected_ && isConnected()) {
		emit connected(true);
		wasConnected_ = true;
	}
}

QList<BioXASCLSMAXvMotor *> BioXASSideBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType category)
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

void BioXASSideBeamline::onEnergyConnectedChanged(bool connectionState)
{
	Q_UNUSED(connectionState)

	if (wasConnected_ && !isConnected()) {
		emit connected(false);
		wasConnected_ = false;

	} else if (!wasConnected_ && isConnected()) {
		emit connected(true);
		wasConnected_ = true;
	}
}
