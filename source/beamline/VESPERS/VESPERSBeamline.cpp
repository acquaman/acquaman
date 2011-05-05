/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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

#include "VESPERSBeamline.h"

VESPERSBeamline::VESPERSBeamline()
	: AMBeamline("VESPERS Beamline")
{
	setupDiagnostics();
	setupSampleStage();
	setupEndstation();
	setupSingleElementDetector();
	setupFourElementDetector();
	setupControlSets();
}

void VESPERSBeamline::setupDiagnostics()
{
	// Pressure controls.
	ccgFE1_ =  new AMReadOnlyPVwStatusControl("Pressure FE1", "CCG1408-B20-01:vac:p", "CCG1408-B20-01:vac", this, new AMControlStatusCheckerDefault(0));
	ccgFE2a_ =  new AMReadOnlyPVwStatusControl("Pressure FE2a", "CCG1408-B20-02:vac:p", "CCG1408-B20-02:vac", this, new AMControlStatusCheckerDefault(0));
	ccgFE2b_ =  new AMReadOnlyPVwStatusControl("Pressure FE2b", "CCG1408-B20-03:vac:p", "CCG1408-B20-03:vac", this, new AMControlStatusCheckerDefault(0));
	ccgFE3a_ =  new AMReadOnlyPVwStatusControl("Pressure FE3a", "CCG1607-1-B20-02:vac:p", "CCG1607-1-B20-02:vac", this, new AMControlStatusCheckerDefault(0));
	ccgFE3b_ =  new AMReadOnlyPVwStatusControl("Pressure FE3b", "CCG1607-1-B20-03:vac:p", "CCG1607-1-B20-03:vac", this, new AMControlStatusCheckerDefault(0));
	ccgM1_ =  new AMReadOnlyPVwStatusControl("Pressure M1", "CCG1607-1-B20-04:vac:p", "CCG1607-1-B20-04:vac", this, new AMControlStatusCheckerDefault(0));
	ccgM2_ =  new AMReadOnlyPVwStatusControl("Pressure M2", "CCG1607-1-B20-05:vac:p", "CCG1607-1-B20-05:vac", this, new AMControlStatusCheckerDefault(0));
	ccgBPM1_ =  new AMReadOnlyPVwStatusControl("Pressure BPM1", "CCG1607-1-B20-06:vac:p", "CCG1607-1-B20-06:vac", this, new AMControlStatusCheckerDefault(0));
	ccgBPM2_ =  new AMReadOnlyPVwStatusControl("Pressure BPM2", "CCG1607-1-B20-07:vac:p", "CCG1607-1-B20-07:vac", this, new AMControlStatusCheckerDefault(0));
	ccgMono_ =  new AMReadOnlyPVwStatusControl("Pressure Mono", "CCG1607-1-B20-08:vac:p", "CCG1607-1-B20-08:vac", this, new AMControlStatusCheckerDefault(0));
	ccgExitSlits_ =  new AMReadOnlyPVwStatusControl("Pressure Exit Slits", "CCG1607-1-B20-09:vac:p", "CCG1607-1-B20-09:vac", this, new AMControlStatusCheckerDefault(0));
	ccgStraightSection_ =  new AMReadOnlyPVwStatusControl("Pressure Straight Section", "CCG1607-1-B20-10:vac:p", "CCG1607-1-B20-10:vac", this, new AMControlStatusCheckerDefault(0));
	ccgBPM3_ =  new AMReadOnlyPVwStatusControl("Pressure BPM3", "CCG1607-1-B20-11:vac:p", "CCG1607-1-B20-11:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSSH_ =  new AMReadOnlyPVwStatusControl("Pressure SSH", "CCG1607-1-B20-12:vac:p", "CCG1607-1-B20-12:vac", this, new AMControlStatusCheckerDefault(0));
	ccgBeamTransfer1_ =  new AMReadOnlyPVwStatusControl("Pressure Beam Transfer 1", "CCG1607-1-B21-01:vac:p", "CCG1607-1-B21-01:vac", this, new AMControlStatusCheckerDefault(0));
	ccgBeamTransfer2_ =  new AMReadOnlyPVwStatusControl("Pressure Beam Transfer 2", "CCG1607-2-B21-01:vac:p", "CCG1607-2-B21-01:vac", this, new AMControlStatusCheckerDefault(0));
	ccgPreWindow_ =  new AMReadOnlyPVwStatusControl("Pressure Pre-Window", "CCG1607-2-B21-02:vac:p", "CCG1607-2-B21-02:vac", this, new AMControlStatusCheckerDefault(0));
	ccgPostWindow_ =  new AMReadOnlyPVwStatusControl("Pressure Post-Window", "CCG1607-2-B21-03:vac:p", "CCG1607-2-B21-03:vac", this, new AMControlStatusCheckerDefault(0));

	// Valve controls.
	vvrFE1_ = new AMReadOnlyPVControl("Valve FE1", "VVR1408-B20-01:state", this);
	vvrFE2_ = new AMReadOnlyPVControl("Valve FE2", "VVR1607-1-B20-01:state", this);
	vvrM1_ = new AMReadOnlyPVControl("Valve M1", "VVR1607-1-B20-02:state", this);
	vvrM2_ = new AMReadOnlyPVControl("Valve M2", "VVR1607-1-B20-03:state", this);
	vvrBPM1_ = new AMReadOnlyPVControl("Valve BPM1", "VVR1607-1-B20-04:state", this);
	vvrMono_ = new AMReadOnlyPVControl("Valve Mono", "VVR1607-1-B20-05:state", this);
	vvrExitSlits_ = new AMReadOnlyPVControl("Valve Exit Slits", "VVR1607-1-B20-06:state", this);
	vvrStraightSection_ = new AMReadOnlyPVControl("Valve Straight Section", "VVR1607-1-B20-07:state", this);
	vvrBPM3_ = new AMReadOnlyPVControl("Valve BPM3", "VVR1607-1-B20-08:state", this);
	vvrSSH_ = new AMReadOnlyPVControl("Valve SSH", "VVR1607-1-B21-01:state", this);
	vvrBeamTransfer_ = new AMReadOnlyPVControl("Valve Beam Transfer", "VVR1607-2-B21-01:state", this);

	// The actual valve control.  The reason for separating them is due to the fact that there currently does not exist an AMControl that handles setups like valves.
	valveFE1_ = new AMValveControl("Valve Control FE1", "VVR1408-B20-01:state", "VVR1408-B20-01:opr:open", "VVR1408-B20-01:opr:close", this);
	valveFE2_ = new AMValveControl("Valve Control FE2", "VVR1607-1-B20-01:state", "VVR1607-1-B20-01:opr:open", "VVR1607-1-B20-01:opr:close", this);
	valveM1_ = new AMValveControl("Valve Control M1", "VVR1607-1-B20-02:state", "VVR1607-1-B20-02:opr:open", "VVR1607-1-B20-02:opr:close", this);
	valveM2_ = new AMValveControl("Valve Control M2", "VVR1607-1-B20-03:state", "VVR1607-1-B20-03:opr:open", "VVR1607-1-B20-03:opr:close", this);
	valveBPM1_ = new AMValveControl("Valve Control BPM1", "VVR1607-1-B20-04:state", "VVR1607-1-B20-04:opr:open", "VVR1607-1-B20-04:opr:close", this);
	valveMono_ = new AMValveControl("Valve Control Mono", "VVR1607-1-B20-05:state", "VVR1607-1-B20-05:opr:open", "VVR1607-1-B20-05:opr:close", this);
	valveExitSlits_ = new AMValveControl("Valve Control Exit Slits", "VVR1607-1-B20-06:state", "VVR1607-1-B20-06:opr:open", "VVR1607-1-B20-06:opr:close", this);
	valveStraightSection_ = new AMValveControl("Valve Control Straight Section", "VVR1607-1-B20-07:state", "VVR1607-1-B20-07:opr:open", "VVR1607-1-B20-07:opr:close", this);
	valveBPM3_ = new AMValveControl("Valve Control BPM3", "VVR1607-1-B20-08:state", "VVR1607-1-B20-08:opr:open", "VVR1607-1-B20-08:opr:close", this);
	valveSSH_ = new AMValveControl("Valve Control SSH", "VVR1607-1-B21-01:state", "VVR1607-1-B21-01:opr:open", "VVR1607-1-B21-01:opr:close", this);
	valveBeamTransfer_ = new AMValveControl("Valve Control Beam Transfer", "VVR1607-2-B21-01:state", "VVR1607-2-B21-01:opr:open", "VVR1607-2-B21-01:opr:close", this);

	// Ion pump controls.
	iopFE1a_ = new AMReadOnlyPVControl("Ion Pump FE1 a", "IOP1408-B20-01", this);
	iopFE1b_ = new AMReadOnlyPVControl("Ion Pump FE1 b", "IOP1408-B20-02", this);
	iopFE2a_ = new AMReadOnlyPVControl("Ion Pump FE2 a", "IOP1408-B20-03", this);
	iopFE2b_ = new AMReadOnlyPVControl("Ion Pump FE2 b", "IOP1408-B20-04", this);
	iopFE2c_ = new AMReadOnlyPVControl("Ion Pump FE2 c", "IOP1408-B20-05", this);
	iopFE2d_ = new AMReadOnlyPVControl("Ion Pump FE2 d", "IOP1408-B20-06", this);
	iopFE3a_ = new AMReadOnlyPVControl("Ion Pump FE3 a", "IOP1607-1-B20-01", this);
	iopFE3b_ = new AMReadOnlyPVControl("Ion Pump FE3 b", "IOP1607-1-B20-02", this);
	iopM1_ = new AMReadOnlyPVControl("Ion Pump M1", "IOP1607-1-B20-03", this);
	iopM2_ = new AMReadOnlyPVControl("Ion Pump M2", "IOP1607-1-B20-04", this);
	iopBPM1_ = new AMReadOnlyPVControl("Ion Pump BPM1", "IOP1607-1-B20-05", this);
	iopBPM2_ = new AMReadOnlyPVControl("Ion Pump BPM2", "IOP1607-1-B20-06", this);
	iopMono_ = new AMReadOnlyPVControl("Ion Pump Mono", "IOP1607-1-B20-07", this);
	iopExitSlits_ = new AMReadOnlyPVControl("Ion Pump Exit Slits", "IOP1607-1-B20-08", this);
	iopStraightSection_ = new AMReadOnlyPVControl("Ion Pump Straight Section", "IOP1607-1-B20-09", this);
	iopBPM3_ = new AMReadOnlyPVControl("Ion Pump BPM3", "IOP1607-1-B20-10", this);
	iopSSH_ = new AMReadOnlyPVControl("Ion Pump SSH", "IOP1607-1-B20-11", this);
	iopBeamTransfer1_ = new AMReadOnlyPVControl("Ion Pump Beam Transfer 1", "IOP1607-1-B21-01", this);
	iopBeamTransfer2_ = new AMReadOnlyPVControl("Ion Pump Beam Transfer 2", "IOP1607-2-B21-01", this);
	iopPreWindow_ = new AMReadOnlyPVControl("Ion Pump Pre-Window", "IOP1607-2-B21-02", this);

	// Temperature controls.
	tmWaterSupply1_ = new AMReadOnlyPVControl("Temperature Water Supply 1", "TM1607-1-B20-02:trip", this);
	tmWaterSupply2_ = new AMReadOnlyPVControl("Temperature Water Supply 2", "TM1607-1-B20-03:trip", this);
	tmM1A1_ = new AMReadOnlyPVControl("Temperature M1A 1", "TM1607-1-B20-07:trip", this);
	tmM1A2_ = new AMReadOnlyPVControl("Temperature M1A 2", "TM1607-1-B20-08:trip", this);
	tmM1A3_ = new AMReadOnlyPVControl("Temperature M1A 3", "TM1607-1-B20-09:trip", this);
	tmM1B1_ = new AMReadOnlyPVControl("Temperature M1B 1", "TM1607-1-B20-10:trip", this);
	tmM1B2_ = new AMReadOnlyPVControl("Temperature M1B 2", "TM1607-1-B20-11:trip", this);
	tmM1B3_ = new AMReadOnlyPVControl("Temperature M1B 3", "TM1607-1-B20-12:trip", this);
	tmM2A1_ = new AMReadOnlyPVControl("Temperature M2A 1", "TM1607-1-B21-01:trip", this);
	tmM2A2_ = new AMReadOnlyPVControl("Temperature M2A 2", "TM1607-1-B21-02:trip", this);
	tmM2A3_ = new AMReadOnlyPVControl("Temperature M2A 3", "TM1607-1-B21-03:trip", this);
	tmM2B1_ = new AMReadOnlyPVControl("Temperature M2B 1", "TM1607-1-B22-01:trip", this);
	tmM2B2_ = new AMReadOnlyPVControl("Temperature M2B 2", "TM1607-1-B22-02:trip", this);
	tmM2B3_ = new AMReadOnlyPVControl("Temperature M2B 3", "TM1607-1-B22-03:trip", this);
	tmMono1_ = new AMReadOnlyPVControl("Temperature Mono 1", "TM1607-1-B20-13:trip", this);
	tmMono2_ = new AMReadOnlyPVControl("Temperature Mono 2", "TM1607-1-B20-14:trip", this);
	tmMono3_ = new AMReadOnlyPVControl("Temperature Mono 3", "TM1607-1-B20-15:trip", this);
	tmMono4_ = new AMReadOnlyPVControl("Temperature Mono 4", "TM1607-1-B20-16:trip", this);
	tmMono5_ = new AMReadOnlyPVControl("Temperature Mono 5", "TM1607-1-B20-17:trip", this);
	tmMono6_ = new AMReadOnlyPVControl("Temperature Mono 6", "TM1607-1-B20-18:trip", this);
	tmMono7_ = new AMReadOnlyPVControl("Temperature Mono 7", "TM1607-1-B20-19:trip", this);

	// Flow switch controls.
	swfM1A_ = new AMReadOnlyPVControl("Flow Switch M1A", "SWF1607-1-B20-02", this);
	swfM1B_ = new AMReadOnlyPVControl("Flow Switch M1B", "SWF1607-1-B20-03", this);
	swfM2A_ = new AMReadOnlyPVControl("Flow Switch M2A", "SWF1607-1-B21-01", this);
	swfM2B_ = new AMReadOnlyPVControl("Flow Switch M2B", "SWF1607-1-B22-01", this);
	swfMono_ = new AMReadOnlyPVControl("Flow Switch Mono", "SWF1607-1-B20-04", this);
	swfExitSlits_ = new AMReadOnlyPVControl("Flow Switch Exit Slits", "SWF1607-1-B20-05", this);
	swfInterimSlits1_ = new AMReadOnlyPVControl("Flow Switch Interim Slits 1", "SWF1607-1-B21-02", this);
	swfInterimSlits2_ = new AMReadOnlyPVControl("Flow Switch Interim Slits 2", "SWF1607-1-B21-03", this);
	swfPoeSsh1_ = new AMReadOnlyPVControl("Flow Switch POE SSH1", "SWF1607-1-B21-04", this);
	swfPoeSsh2_ = new AMReadOnlyPVControl("Flow Switch POE SSH2", "SWF1607-1-B22-02", this);

	// Flow transducer controls.
	fltM1A_ = new AMReadOnlyPVControl("Flow Transducer M1A", "FLT1607-1-B20-02:lowflow", this);
	fltM1B_ = new AMReadOnlyPVControl("Flow Transducer M1B", "FLT1607-1-B20-03:lowflow", this);
	fltM2A_ = new AMReadOnlyPVControl("Flow Transducer M2A", "FLT1607-1-B21-01:lowflow", this);
	fltM2B_ = new AMReadOnlyPVControl("Flow Transducer M2B", "FLT1607-1-B22-01:lowflow", this);
	fltMono_ = new AMReadOnlyPVControl("Flow Transducer Mono", "FLT1607-1-B20-04:lowflow", this);
	fltExitSlits_ = new AMReadOnlyPVControl("Flow Transducer Exit Slits", "FLT1607-1-B20-05:lowflow", this);
	fltInterimSlits1_ = new AMReadOnlyPVControl("Flow Transducer Interim Slits 1", "FLT1607-1-B21-02:lowflow", this);
	fltInterimSlits2_ = new AMReadOnlyPVControl("Flow Transducer Interim Slits 2", "FLT1607-1-B21-03:lowflow", this);
	fltPoeSsh1_ = new AMReadOnlyPVControl("Flow Transducer POE SSH1", "FLT1607-1-B21-04:lowflow", this);
	fltPoeSsh2_ = new AMReadOnlyPVControl("Flow Transducer POE SSH2", "FLT1607-1-B22-02:lowflow", this);

	// The beam attenuation filters.
	filter250umA_ = new AMPVControl("Filter 250um A", "07B2_PLC_PFIL_01_F1_Ctrl", "07B2_PLC_PFIL_01_F1_Toggle", QString(), this);
	filter250umB_ = new AMPVControl("Filter 250um B", "07B2_PLC_PFIL_01_F2_Ctrl", "07B2_PLC_PFIL_01_F2_Toggle", QString(), this);
	filter100umA_ = new AMPVControl("Filter 100um A", "07B2_PLC_PFIL_02_F3_Ctrl", "07B2_PLC_PFIL_02_F3_Toggle", QString(), this);
	filter100umB_ = new AMPVControl("Filter 100um B", "07B2_PLC_PFIL_02_F4_Ctrl", "07B2_PLC_PFIL_02_F4_Toggle", QString(), this);
	filter50umA_ = new AMPVControl("Filter 50um A", "07B2_PLC_PFIL_02_F1_Ctrl", "07B2_PLC_PFIL_02_F1_Toggle", QString(), this);
	filter50umB_ = new AMPVControl("Filter 50um B", "07B2_PLC_PFIL_02_F2_Ctrl", "07B2_PLC_PFIL_02_F2_Toggle", QString(), this);
	filterShutterUpper_ = new AMPVControl("Filter Shutter Upper", "07B2_PLC_PFIL_01_F3_Ctrl", "07B2_PLC_PFIL_01_F3_Toggle", QString(), this);
	filterShutterLower_ = new AMPVControl("Filter Shutter Lower", "07B2_PLC_PFIL_01_F4_Ctrl", "07B2_PLC_PFIL_01_F4_Toggle", QString(), this);
}

void VESPERSBeamline::setupSampleStage()
{
	sampleStageHorizontal_ = new AMPVwStatusControl("Horizontal Sample Stage", "TS1607-2-B21-01:H:user:mm:sp", "TS1607-2-B21-01:H:user:mm", "TS1607-2-B21-01:H:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.01, 10.0);
	sampleStageVertical_ = new AMPVwStatusControl("Vertical Sample Stage", "TS1607-2-B21-01:V:user:mm:sp", "TS1607-2-B21-01:V:user:mm", "TS1607-2-B21-01:V:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.01, 10.0);

	sampleStageMotorSet_ = new AMControlSet(this);
	sampleStageMotorSet_->addControl(sampleStageHorizontal_);
	sampleStageMotorSet_->addControl(sampleStageVertical_);

	connect(sampleStageMotorSet_, SIGNAL(controlSetTimedOut()), this, SLOT(sampleStageError()));
}

void VESPERSBeamline::setupEndstation()
{
	// The controls used for the control window.
	ccdMotor_ = new AMPVwStatusControl("CCD motor", "SMTR1607-2-B21-18:mm:sp", "SMTR1607-2-B21-18:mm", "SMTR1607-2-B21-18:status", "SMTR1607-2-B21-18:stop", this);
	microscopeMotor_ = new AMPVwStatusControl("Microscope motor", "SMTR1607-2-B21-17:mm:sp", "SMTR1607-2-B21-17:mm", "SMTR1607-2-B21-17:status", "SMTR1607-2-B21-17:stop", this);
	fourElMotor_ = new AMPVwStatusControl("4-Element Vortex motor", "SMTR1607-2-B21-27:mm:sp", "SMTR1607-2-B21-27:mm", "SMTR1607-2-B21-27:status", "SMTR1607-2-B21-27:stop", this);
	singleElMotor_ = new AMPVwStatusControl("1-Element Vortex motor", "SMTR1607-2-B21-15:mm:sp", "SMTR1607-2-B21-15:mm", "SMTR1607-2-B21-15:status", "SMTR1607-2-B21-15:stop", this);
	focusMotor_ = new AMPVwStatusControl("Focus motor", "TS1607-2-B21-01:N:user:mm:sp", "TS1607-2-B21-01:N:user:mm", "TS1607-2-B21-01:N:status", "TS1607-2-B21-01:HNV:stop.PROC", this);

	// The process variables that have the feedback value used for the button.  The microscope doesn't need one because it's encoder doesn't work.
	ccdMotorfbk_ = new AMReadOnlyPVControl("CCD motor feedback", "SMTR1607-2-B21-18:mm:fbk", this);
	fourElMotorfbk_ = new AMReadOnlyPVControl("4-Element Vortex motor feedback", "SMTR1607-2-B21-27:mm:fbk", this);
	singleElMotorfbk_ = new AMReadOnlyPVControl("1-Element Vortex motor feedback", "SMTR1607-2-B21-15:mm:fbk", this);
	focusMotorfbk_ = new AMReadOnlyPVControl("Focus motor feedback", "TS1607-2-B21-01:N:user:mm:fbk", this);

	// Microscope light PV.
	micLight_ = new AMProcessVariable("07B2_PLC_Mic_Light_Inten", true, this);
	micLight_->disablePutCallbackMode(true);

	// Laser on/off control.
	laserPower_ = new AMPVControl("Laser Power Control", "07B2_PLC_LaserDistON", "07B2_PLC_LaserDistON_Tog", QString(), this);

	// Various CCD file path PVs.
	ccdPath_ = new AMProcessVariable("IOC1607-003:det1:FilePath", true, this);
	ccdFile_ = new AMProcessVariable("IOC1607-003:det1:FileName", true, this);
	ccdNumber_ = new AMProcessVariable("IOC1607-003:det1:FileNumber", true, this);
}

void VESPERSBeamline::setupSingleElementDetector()
{
	status1E_ = new AMReadOnlyPVControl("1-el Status", "IOC1607-004:mca1.ACQG", this);
	elapsedTime1E_ = new AMReadOnlyPVControl("1-el Elapsed Time", "IOC1607-004:mca1.ERTM", this);
	integrationTime1E_ = new AMPVControl("1-el Integration Time", "IOC1607-004:mca1.PRTM", "IOC1607-004:mca1.PRTM", QString(), this);
	liveTime1E_ = new AMPVControl("1-el Live Time", "IOC1607-004:mca1.PLTM", "IOC1607-004:mca1.PLTM", QString(), this);
	start1E_ = new AMPVControl("1-el Start", "IOC1607-004:mca1.ERST", "IOC1607-004:mca1.ERST", QString(), this);
	stop1E_ = new AMPVControl("1-el Stop", "IOC1607-004:mca1.STOP", "IOC1607-004:mca1.STOP", QString(), this);
	deadTime1E_ = new AMReadOnlyPVControl("1-el Dead Time", "IOC1607-004:mca1.IDTIM", this);
	maxEnergy1E_ = new AMPVControl("1-el Maximum Energy", "IOC1607-004:dxp1.EMAX", "IOC1607-004:dxp1.EMAX", QString(), this);
	mcaUpdateRate1E_ = new AMPVControl("1-el MCA Update Rate", "IOC1607-004:mca1Read.SCAN", "IOC1607-004:mca1Read.SCAN", QString(), this);
	peakingTime1E_ = new AMPVControl("1-el Peaking Time", "IOC1607-004:dxp1.PKTIM", "IOC1607-004:dxp1.PKTIM", QString(), this);
	spectrum1E_ = new AMReadOnlyPVControl("1-el Spectrum", "IOC1607-004:mca1", this);

	// Putting the controls into their own control set.
	vortex1EControls_ = new AMControlSet(this);
	vortex1EControls_->addControl(status1E_);
	vortex1EControls_->addControl(elapsedTime1E_);
	vortex1EControls_->addControl(integrationTime1E_);
	vortex1EControls_->addControl(liveTime1E_);
	vortex1EControls_->addControl(start1E_);
	vortex1EControls_->addControl(stop1E_);
	vortex1EControls_->addControl(deadTime1E_);
	vortex1EControls_->addControl(maxEnergy1E_);
	vortex1EControls_->addControl(mcaUpdateRate1E_);
	vortex1EControls_->addControl(peakingTime1E_);
	vortex1EControls_->addControl(spectrum1E_);

	// MCA records behave incorrectly when using the ca_put_callback epics callback.  Need to use ca_put only.
	AMPVControl *temp;
	for (int i = 0; i < vortex1EControls_->count(); i++){

		temp = qobject_cast<AMPVControl *>(vortex1EControls_->at(i)); // Since AMReadOnlyPVControl is an ancestor of AMPVControl, this will return null if it's not AMPVControl.
		if (temp)
			temp->disableWritePVPutCallback(true);
	}

	vortex1E_ = new XRFDetector("Single Element XRF",
								status1E_,
								mcaUpdateRate1E_,
								peakingTime1E_,
								maxEnergy1E_,
								integrationTime1E_,
								liveTime1E_,
								elapsedTime1E_,
								start1E_,
								stop1E_,
								deadTime1E_,
								spectrum1E_,
								this);

	connect(vortexXRF1E(), SIGNAL(detectorConnected(bool)), this, SLOT(singleElVortexError(bool)));

	QList<AMROI *> rois;
	AMROI *roi;

	// Currently all detectors I'm aware of have 32 ROIs.
	for (int i = 0; i < 32; i++){

		roi = createROI(1, i, "IOC1607-004:mca");
		rois << roi;
	}

	vortexXRF1E()->setRoiList(rois);
}

void VESPERSBeamline::setupFourElementDetector()
{
	status4E_ = new AMReadOnlyPVControl("4-el Status", "dxp1607-B21-04:mca1.ACQG", this);
	elapsedTime4E_ = new AMReadOnlyPVControl("4-el Elapsed Time", "dxp1607-B21-04:ElapsedReal", this);
	integrationTime4E_ = new AMPVControl("4-el Integration Time", "dxp1607-B21-04:PresetReal", "dxp1607-B21-04:PresetReal", QString(), this);
	liveTime4E_ = new AMPVControl("4-el Live Time", "dxp1607-B21-04:PresetLive", "dxp1607-B21-04:PresetLive", QString(), this);
	start4E_ = new AMPVControl("4-el Start", "dxp1607-B21-04:EraseStart", "dxp1607-B21-04:EraseStart", QString(), this);
	stop4E_ = new AMPVControl("4-el Stop", "dxp1607-B21-04:StopAll", "dxp1607-B21-04:StopAll", QString(), this);
	maxEnergy4E_ = new AMPVControl("4-el Maximum Energy", "dxp1607-B21-04:mcaEMax", "dxp1607-B21-04:mcaEMax", QString(), this);
	mcaUpdateRate4E_ = new AMPVControl("4-el MCA Update Rate", "dxp1607-B21-04:ReadAll.SCAN", "dxp1607-B21-04:ReadAll.SCAN", QString(), this);
	peakingTime4E_ = new AMPVControl("4-el Peaking Time", "dxp1607-B21-04:EnergyPkTime", "dxp1607-B21-04:EnergyPkTime", QString(), this);
	deadTime14E_ = new AMReadOnlyPVControl("4-el Dead Time 1", "dxp1607-B21-04:dxp1:NetDTP", this);
	deadTime24E_ = new AMReadOnlyPVControl("4-el Dead Time 2", "dxp1607-B21-04:dxp2:NetDTP", this);
	deadTime34E_ = new AMReadOnlyPVControl("4-el Dead Time 3", "dxp1607-B21-04:dxp3:NetDTP", this);
	deadTime44E_ = new AMReadOnlyPVControl("4-el Dead Time 4", "dxp1607-B21-04:dxp4:NetDTP", this);
	rawSpectrum14E_ = new AMReadOnlyPVControl("4-el Raw Spectrum 1", "dxp1607-B21-04:mca1", this);
	rawSpectrum24E_ = new AMReadOnlyPVControl("4-el Raw Spectrum 2", "dxp1607-B21-04:mca2", this);
	rawSpectrum34E_ = new AMReadOnlyPVControl("4-el Raw Spectrum 3", "dxp1607-B21-04:mca3", this);
	rawSpectrum44E_ = new AMReadOnlyPVControl("4-el Raw Spectrum 4", "dxp1607-B21-04:mca4", this);

	deadTime4E_ = new AMControlSet(this);
	deadTime4E_->addControl(deadTime14E_);
	deadTime4E_->addControl(deadTime24E_);
	deadTime4E_->addControl(deadTime34E_);
	deadTime4E_->addControl(deadTime44E_);

	spectra4E_ = new AMControlSet(this);
	spectra4E_->addControl(rawSpectrum14E_);
	spectra4E_->addControl(rawSpectrum24E_);
	spectra4E_->addControl(rawSpectrum34E_);
	spectra4E_->addControl(rawSpectrum44E_);

	// This will hold all of the controls.  Unfortunately, the grouping for the dead time and spectra will be lost.  If those are what you'd actually be interested in then use their specific control sets.  That's why they were created.
	vortex4EControls_ = new AMControlSet(this);
	vortex4EControls_->addControl(status4E_);
	vortex4EControls_->addControl(elapsedTime4E_);
	vortex4EControls_->addControl(integrationTime4E_);
	vortex4EControls_->addControl(liveTime4E_);
	vortex4EControls_->addControl(start4E_);
	vortex4EControls_->addControl(stop4E_);
	vortex4EControls_->addControl(maxEnergy4E_);
	vortex4EControls_->addControl(mcaUpdateRate4E_);
	vortex4EControls_->addControl(peakingTime4E_);
	vortex4EControls_->addControl(deadTime14E_);
	vortex4EControls_->addControl(deadTime24E_);
	vortex4EControls_->addControl(deadTime34E_);
	vortex4EControls_->addControl(deadTime44E_);
	vortex4EControls_->addControl(rawSpectrum14E_);
	vortex4EControls_->addControl(rawSpectrum24E_);
	vortex4EControls_->addControl(rawSpectrum34E_);
	vortex4EControls_->addControl(rawSpectrum44E_);

	// MCA records behave incorrectly when using the ca_put_callback epics callback.  Need to use ca_put only.
	AMPVControl *temp;
	for (int i = 0; i < vortex4EControls_->count(); i++){

		temp = qobject_cast<AMPVControl *>(vortex4EControls_->at(i)); // Since AMReadOnlyPVControl is an ancestor of AMPVControl, this will return null if it's not AMPVControl.
		if (temp)
			temp->disableWritePVPutCallback(true);
	}

	vortex4E_ = new XRFDetector("Four Element XRF",
								4,
								status4E_,
								mcaUpdateRate4E_,
								peakingTime4E_,
								maxEnergy4E_,
								integrationTime4E_,
								liveTime4E_,
								elapsedTime4E_,
								start4E_,
								stop4E_,
								deadTime4E_,
								spectra4E_,
								this);

	connect(vortexXRF4E(), SIGNAL(detectorConnected(bool)), this, SLOT(fourElVortexError(bool)));

	QList<AMROI *> rois;
	AMROI *roi;

	// Currently all detectors I'm aware of have 32 ROIs.
	for (int i = 0; i < 32; i++){

		roi = createROI(4, i, "dxp1607-B21-04:mca");
		rois << roi;
	}

	vortexXRF4E()->setRoiList(rois);
}

AMROI *VESPERSBeamline::createROI(int numElements, int roiNum, QString baseName)
{
	AMProcessVariable *namePV;
	AMProcessVariable *lowPV;
	AMProcessVariable *highPV;
	AMProcessVariable *valPV;

	// Things are simpler with one element.
	if (numElements == 1){

		namePV = new AMProcessVariable(baseName+"1.R"+QString::number(roiNum)+"NM", true);
		lowPV = new AMProcessVariable(baseName+"1.R"+QString::number(roiNum)+"LO", true);
		highPV = new AMProcessVariable(baseName+"1.R"+QString::number(roiNum)+"HI", true);
		valPV = new AMProcessVariable(baseName+"1.R"+QString::number(roiNum), true);

		namePV->disablePutCallbackMode(true);
		lowPV->disablePutCallbackMode(true);
		highPV->disablePutCallbackMode(true);
		valPV->disablePutCallbackMode(true);

		// Giving null parameters because the detector might not be connected yet.  The detector can modify them as it wishes.
		return new AMROI(QString(), -1, -1, 10, namePV, lowPV, highPV, valPV);
	}

	else{

		QList<AMProcessVariable *> names;
		QList<AMProcessVariable *> lows;
		QList<AMProcessVariable *> highs;
		QList<AMProcessVariable *> vals;

		for (int i = 0; i < numElements; i++){

			namePV = new AMProcessVariable(baseName+QString::number(i+1)+".R"+QString::number(roiNum)+"NM", true);
			lowPV = new AMProcessVariable(baseName+QString::number(i+1)+".R"+QString::number(roiNum)+"LO", true);
			highPV = new AMProcessVariable(baseName+QString::number(i+1)+".R"+QString::number(roiNum)+"HI", true);
			valPV = new AMProcessVariable(baseName+QString::number(i+1)+".R"+QString::number(roiNum), true);

			namePV->disablePutCallbackMode(true);
			lowPV->disablePutCallbackMode(true);
			highPV->disablePutCallbackMode(true);
			valPV->disablePutCallbackMode(true);

			names << namePV;
			lows << lowPV;
			highs << highPV;
			vals << valPV;
		}

		return new AMROI(QString(), -1, -1, 10, names, lows, highs, vals);
	}
}

void VESPERSBeamline::setupControlSets()
{
	// Grouping the pressure controls together.
	pressureSet_ = new AMControlSet(this);
	pressureSet_->addControl(ccgFE1_);
	pressureSet_->addControl(ccgFE2a_);
	pressureSet_->addControl(ccgFE2b_);
	pressureSet_->addControl(ccgFE3a_);
	pressureSet_->addControl(ccgFE3b_);
	pressureSet_->addControl(ccgM1_);
	pressureSet_->addControl(ccgM2_);
	pressureSet_->addControl(ccgBPM1_);
	pressureSet_->addControl(ccgBPM2_);
	pressureSet_->addControl(ccgMono_);
	pressureSet_->addControl(ccgExitSlits_);
	pressureSet_->addControl(ccgStraightSection_);
	pressureSet_->addControl(ccgBPM3_);
	pressureSet_->addControl(ccgSSH_);
	pressureSet_->addControl(ccgBeamTransfer1_);
	pressureSet_->addControl(ccgBeamTransfer2_);
	pressureSet_->addControl(ccgPreWindow_);
	pressureSet_->addControl(ccgPostWindow_);

	connect(pressureSet_, SIGNAL(connected(bool)), this, SLOT(pressureConnected(bool)));

	// Grouping the valve controls together.
	valveSet_ = new AMControlSet(this);
	valveSet_->addControl(vvrFE1_);
	valveSet_->addControl(vvrFE2_);
	valveSet_->addControl(vvrM1_);
	valveSet_->addControl(vvrM2_);
	valveSet_->addControl(vvrBPM1_);
	valveSet_->addControl(vvrMono_);
	valveSet_->addControl(vvrExitSlits_);
	valveSet_->addControl(vvrStraightSection_);
	valveSet_->addControl(vvrBPM3_);
	valveSet_->addControl(vvrSSH_);
	valveSet_->addControl(vvrBeamTransfer_);

	connect(valveSet_, SIGNAL(connected(bool)), this, SLOT(valveConnected(bool)));

	// Grouping the valve state modifying controls together.
	valveList_ = new QList<AMValveControl *>;
	valveList_->append(valveFE1_);
	valveList_->append(valveFE2_);
	valveList_->append(valveM1_);
	valveList_->append(valveM2_);
	valveList_->append(valveBPM1_);
	valveList_->append(valveMono_);
	valveList_->append(valveExitSlits_);
	valveList_->append(valveStraightSection_);
	valveList_->append(valveBPM3_);
	valveList_->append(valveSSH_);
	valveList_->append(valveBeamTransfer_);

	// Grouping the ion pump controls together.
	ionPumpSet_ = new AMControlSet(this);
	ionPumpSet_->addControl(iopFE1a_);
	ionPumpSet_->addControl(iopFE1b_);
	ionPumpSet_->addControl(iopFE2a_);
	ionPumpSet_->addControl(iopFE2b_);
	ionPumpSet_->addControl(iopFE2c_);
	ionPumpSet_->addControl(iopFE2d_);
	ionPumpSet_->addControl(iopFE3a_);
	ionPumpSet_->addControl(iopFE3b_);
	ionPumpSet_->addControl(iopM1_);
	ionPumpSet_->addControl(iopM2_);
	ionPumpSet_->addControl(iopBPM1_);
	ionPumpSet_->addControl(iopBPM2_);
	ionPumpSet_->addControl(iopMono_);
	ionPumpSet_->addControl(iopExitSlits_);
	ionPumpSet_->addControl(iopStraightSection_);
	ionPumpSet_->addControl(iopBPM3_);
	ionPumpSet_->addControl(iopSSH_);
	ionPumpSet_->addControl(iopBeamTransfer1_);
	ionPumpSet_->addControl(iopBeamTransfer2_);
	ionPumpSet_->addControl(iopPreWindow_);

	connect(ionPumpSet_, SIGNAL(connected(bool)), this, SLOT(ionPumpConnected(bool)));

	// Grouping the temperature controls together.
	temperatureSet_ = new AMControlSet(this);
	temperatureSet_->addControl(tmWaterSupply1_);
	temperatureSet_->addControl(tmWaterSupply2_);
	temperatureSet_->addControl(tmM1A1_);
	temperatureSet_->addControl(tmM1A2_);
	temperatureSet_->addControl(tmM1A3_);
	temperatureSet_->addControl(tmM1B1_);
	temperatureSet_->addControl(tmM1B2_);
	temperatureSet_->addControl(tmM1B3_);
	temperatureSet_->addControl(tmM2A1_);
	temperatureSet_->addControl(tmM2A2_);
	temperatureSet_->addControl(tmM2A3_);
	temperatureSet_->addControl(tmM2B1_);
	temperatureSet_->addControl(tmM2B2_);
	temperatureSet_->addControl(tmM2B3_);
	temperatureSet_->addControl(tmMono1_);
	temperatureSet_->addControl(tmMono2_);
	temperatureSet_->addControl(tmMono3_);
	temperatureSet_->addControl(tmMono4_);
	temperatureSet_->addControl(tmMono5_);
	temperatureSet_->addControl(tmMono6_);
	temperatureSet_->addControl(tmMono7_);

	connect(temperatureSet_, SIGNAL(connected(bool)), this, SLOT(temperatureConnected(bool)));

	// Grouping the water flow switch controls together.
	flowSwitchSet_ = new AMControlSet(this);
	flowSwitchSet_->addControl(swfM1A_);
	flowSwitchSet_->addControl(swfM1B_);
	flowSwitchSet_->addControl(swfM2A_);
	flowSwitchSet_->addControl(swfM2B_);
	flowSwitchSet_->addControl(swfMono_);
	flowSwitchSet_->addControl(swfExitSlits_);
	flowSwitchSet_->addControl(swfInterimSlits1_);
	flowSwitchSet_->addControl(swfInterimSlits2_);
	flowSwitchSet_->addControl(swfPoeSsh1_);
	flowSwitchSet_->addControl(swfPoeSsh2_);

	connect(flowSwitchSet_, SIGNAL(connected(bool)), this, SLOT(flowSwitchConnected(bool)));

	// Grouping the water flow transducer controls together.
	flowTransducerSet_ = new AMControlSet(this);
	flowTransducerSet_->addControl(fltM1A_);
	flowTransducerSet_->addControl(fltM1B_);
	flowTransducerSet_->addControl(fltM2A_);
	flowTransducerSet_->addControl(fltM2B_);
	flowTransducerSet_->addControl(fltMono_);
	flowTransducerSet_->addControl(fltExitSlits_);
	flowTransducerSet_->addControl(fltInterimSlits1_);
	flowTransducerSet_->addControl(fltInterimSlits2_);
	flowTransducerSet_->addControl(fltPoeSsh1_);
	flowTransducerSet_->addControl(fltPoeSsh2_);

	connect(flowTransducerSet_, SIGNAL(connected(bool)), this, SLOT(flowTransducerConnected(bool)));

	// Grouping the enstation motors together.
	endstationMotorSet_ = new AMControlSet(this);
	endstationMotorSet_->addControl(ccdMotor_);
	endstationMotorSet_->addControl(ccdMotorfbk_);
	endstationMotorSet_->addControl(microscopeMotor_);
	endstationMotorSet_->addControl(fourElMotor_);
	endstationMotorSet_->addControl(fourElMotorfbk_);
	endstationMotorSet_->addControl(singleElMotor_);
	endstationMotorSet_->addControl(singleElMotorfbk_);
	endstationMotorSet_->addControl(focusMotor_);
	endstationMotorSet_->addControl(focusMotorfbk_);

	// Beam attenuation filters.  Only contains the filters of a certain size.  The upper and lower are used independently of these six.
	filterSet_ = new AMControlSet(this);
	filterSet_->addControl(filter250umA_);
	filterSet_->addControl(filter250umB_);
	filterSet_->addControl(filter100umA_);
	filterSet_->addControl(filter100umB_);
	filterSet_->addControl(filter50umA_);
	filterSet_->addControl(filter50umB_);
	filterSet_->addControl(filterShutterUpper_);
	filterSet_->addControl(filterShutterLower_);
}

void VESPERSBeamline::pressureConnected(bool connected)
{
	if (connected)
		for (int i = 0; i < pressureSet_->count(); i++)
			connect(qobject_cast<AMReadOnlyPVwStatusControl *>(pressureSet_->at(i)), SIGNAL(movingChanged(bool)), this, SLOT(pressureError()));
}

void VESPERSBeamline::valveConnected(bool connected)
{
	if (connected)
		connect(valveSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(valveError()));
}

void VESPERSBeamline::ionPumpConnected(bool connected)
{
	if (connected)
		connect(ionPumpSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(ionPumpError()));
}

void VESPERSBeamline::temperatureConnected(bool connected)
{
	if (connected)
		connect(temperatureSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(temperatureError()));
}

void VESPERSBeamline::flowSwitchConnected(bool connected)
{
	if (connected)
		connect(flowSwitchSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(flowSwitchError()));
}

void VESPERSBeamline::flowTransducerConnected(bool connected)
{
	if (connected)
		connect(flowTransducerSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(flowTransducerError()));
}

void VESPERSBeamline::pressureError()
{
	if (!pressureSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVwStatusControl *current = 0;

	for (int i = 0; i < pressureSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVwStatusControl *>(pressureSet_->at(i));

		if (current->isMoving())
			error += tr("%1 (%2) %3 %4\n").arg(current->name(), current->readPVName(), QString::number(current->value(), 'e', 3), current->units());
	}

	if (!error.isEmpty()){

		error.prepend("The following pressure readings are at a critical level:\n");
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 0, error));
	}
}

void VESPERSBeamline::valveError()
{
	if (!valveSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVControl *current = 0;

	for (int i = 0; i < valveSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(valveSet_->at(i));

		if (current->readPV()->getInt() == 4) // Closed is enum = 4.
			error += QString("%1 (%2)\n").arg(current->name(), current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following valves are closed:\n");
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 0, error));
	}
}

void VESPERSBeamline::ionPumpError()
{
	if (!ionPumpSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVControl *current = 0;

	for (int i = 0; i < ionPumpSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(ionPumpSet_->at(i));

		if (!current->readPV()->getInt())
			error += tr("%1 (%2)\n").arg(current->name(), current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following ion pumps are no longer operating correctly:\n");
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 0, error));
	}
}

void VESPERSBeamline::temperatureError()
{
	if (!temperatureSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVControl *current = 0;

	for (int i = 0; i < temperatureSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(temperatureSet_->at(i));

		if (!current->readPV()->getInt())
			error += tr("%1 (%2)\n").arg(current->name(), current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following temperature sensors are reading too high:\n");
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 0, error));
	}
}

void VESPERSBeamline::flowSwitchError()
{
	if (!flowSwitchSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVControl *current = 0;

	for (int i = 0; i < flowSwitchSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(flowSwitchSet_->at(i));

		if (!current->readPV()->getInt())
			error += tr("%1 (%2)\n").arg(current->name(), current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following flow switches have tripped:\n");
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 0, error));
	}
}

void VESPERSBeamline::flowTransducerError()
{
	if (!flowTransducerSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVControl *current = 0;

	for (int i = 0; i < flowTransducerSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(flowTransducerSet_->at(i));

		if (!current->readPV()->getInt())
			error += tr("%1 (%2)\n").arg(current->name(), current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following flow transducers are measuring too low:\n");
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 0, error));
	}
}

void VESPERSBeamline::singleElVortexError(bool isConnected)
{
	if (vortexXRF1E()->wasConnected() && !isConnected)
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 0, "The single element vortex detector is no longer connected."));
}

void VESPERSBeamline::fourElVortexError(bool isConnected)
{
	if (vortexXRF4E()->wasConnected() && !isConnected)
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 0, "The four element vortex detector is no longer connected."));
}

void VESPERSBeamline::sampleStageError()
{
	if (!sampleStageMotorSet()->isConnected())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "The sample stage is no longer connected."));
}

VESPERSBeamline::~VESPERSBeamline()
{
	delete ccgFE1_;
	delete ccgFE2a_;
	delete ccgFE2b_;
	delete ccgFE3a_;
	delete ccgFE3b_;
	delete ccgM1_;
	delete ccgM2_;
	delete ccgBPM1_;
	delete ccgBPM2_;
	delete ccgMono_;
	delete ccgExitSlits_;
	delete ccgStraightSection_;
	delete ccgBPM3_;
	delete ccgSSH_;
	delete ccgBeamTransfer1_;
	delete ccgBeamTransfer2_;
	delete ccgPreWindow_;
	delete ccgPostWindow_;
	delete vvrFE1_;
	delete vvrFE2_;
	delete vvrM1_;
	delete vvrM2_;
	delete vvrBPM1_;
	delete vvrMono_;
	delete vvrExitSlits_;
	delete vvrStraightSection_;
	delete vvrBPM3_;
	delete vvrSSH_;
	delete valveFE1_;
	delete valveFE2_;
	delete valveM1_;
	delete valveM2_;
	delete valveBPM1_;
	delete valveMono_;
	delete valveExitSlits_;
	delete valveStraightSection_;
	delete valveBPM3_;
	delete valveSSH_;
	delete valveBeamTransfer_;
	delete vvrBeamTransfer_;
	delete iopFE1a_;
	delete iopFE1b_;
	delete iopFE2a_;
	delete iopFE2b_;
	delete iopFE2c_;
	delete iopFE2d_;
	delete iopFE3a_;
	delete iopFE3b_;
	delete iopM1_;
	delete iopM2_;
	delete iopBPM1_;
	delete iopBPM2_;
	delete iopMono_;
	delete iopExitSlits_;
	delete iopStraightSection_;
	delete iopBPM3_;
	delete iopSSH_;
	delete iopBeamTransfer1_;
	delete iopBeamTransfer2_;
	delete iopPreWindow_;
	delete tmWaterSupply1_;
	delete tmWaterSupply2_;
	delete tmM1A1_;
	delete tmM1A2_;
	delete tmM1A3_;
	delete tmM1B1_;
	delete tmM1B2_;
	delete tmM1B3_;
	delete tmM2A1_;
	delete tmM2A2_;
	delete tmM2A3_;
	delete tmM2B1_;
	delete tmM2B2_;
	delete tmM2B3_;
	delete tmMono1_;
	delete tmMono2_;
	delete tmMono3_;
	delete tmMono4_;
	delete tmMono5_;
	delete tmMono6_;
	delete tmMono7_;
	delete swfM1A_;
	delete swfM1B_;
	delete swfM2A_;
	delete swfM2B_;
	delete swfMono_;
	delete swfExitSlits_;
	delete swfInterimSlits1_;
	delete swfInterimSlits2_;
	delete swfPoeSsh1_;
	delete swfPoeSsh2_;
	delete fltM1A_;
	delete fltM1B_;
	delete fltM2A_;
	delete fltM2B_;
	delete fltMono_;
	delete fltExitSlits_;
	delete fltInterimSlits1_;
	delete fltInterimSlits2_;
	delete fltPoeSsh1_;
	delete fltPoeSsh2_;
	delete pressureSet_;
	delete valveSet_;
	delete valveList_;
	delete ionPumpSet_;
	delete temperatureSet_;
	delete flowSwitchSet_;
	delete flowTransducerSet_;
	delete endstationMotorSet_;
	delete ccdMotor_;
	delete microscopeMotor_;
	delete fourElMotor_;
	delete singleElMotor_;
	delete focusMotor_;
	delete ccdMotorfbk_;
	delete fourElMotorfbk_;
	delete singleElMotorfbk_;
	delete focusMotorfbk_;
	delete micLight_;
	delete laserPower_;
	delete ccdPath_;
	delete ccdFile_;
	delete ccdNumber_;
	delete filter250umA_;
	delete filter250umB_;
	delete filter100umA_;
	delete filter100umB_;
	delete filter50umA_;
	delete filter50umB_;
	delete filterShutterUpper_;
	delete filterShutterLower_;
	delete elapsedTime1E_;
	delete integrationTime1E_;
	delete liveTime1E_;
	delete start1E_;
	delete stop1E_;
	delete deadTime1E_;
	delete maxEnergy1E_;
	delete mcaUpdateRate1E_;
	delete peakingTime1E_;
	delete spectrum1E_;
	delete vortex1EControls_;
	delete elapsedTime4E_;
	delete integrationTime4E_;
	delete liveTime4E_;
	delete start4E_;
	delete stop4E_;
	delete maxEnergy4E_;
	delete mcaUpdateRate4E_;
	delete peakingTime4E_;
	delete deadTime14E_;
	delete deadTime24E_;
	delete deadTime34E_;
	delete deadTime44E_;
	delete rawSpectrum14E_;
	delete rawSpectrum24E_;
	delete rawSpectrum34E_;
	delete rawSpectrum44E_;
	delete deadTime4E_;
	delete spectra4E_;
	delete vortex4EControls_;
	delete sampleStageHorizontal_;
	delete sampleStageVertical_;
	delete sampleStageMotorSet_;
	delete filterSet_;
}
