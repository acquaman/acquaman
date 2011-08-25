/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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
#include "beamline/CLS/CLSVMEMotor.h"
#include "beamline/AMSingleControlDetector.h"
#include "beamline/AMBeamlineControlMoveAction.h"
#include "beamline/AMBeamlineParallelActionsList.h"
#include "beamline/AMBeamlineListAction.h"


VESPERSBeamline::VESPERSBeamline()
	: AMBeamline("VESPERS Beamline")
{
	setupDiagnostics();
	setupSampleStage();
	setupEndstation();
	setupDetectors();
	setupControlSets();
	setupMono();
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
	tmWaterSupply1_ = new AMReadOnlyPVwStatusControl("Temperature Water Supply 1", "TM1607-1-B20-02", "TM1607-1-B20-02:trip", this, new AMControlStatusCheckerDefault(0));
	tmWaterSupply2_ = new AMReadOnlyPVwStatusControl("Temperature Water Supply 2", "TM1607-1-B20-03", "TM1607-1-B20-03:trip", this, new AMControlStatusCheckerDefault(0));
	tmM1A1_ = new AMReadOnlyPVwStatusControl("Temperature M1A 1", "TM1607-1-B20-07", "TM1607-1-B20-07:trip", this, new AMControlStatusCheckerDefault(0));
	tmM1A2_ = new AMReadOnlyPVwStatusControl("Temperature M1A 2", "TM1607-1-B20-08", "TM1607-1-B20-08:trip", this, new AMControlStatusCheckerDefault(0));
	tmM1A3_ = new AMReadOnlyPVwStatusControl("Temperature M1A 3", "TM1607-1-B20-09", "TM1607-1-B20-09:trip", this, new AMControlStatusCheckerDefault(0));
	tmM1B1_ = new AMReadOnlyPVwStatusControl("Temperature M1B 1", "TM1607-1-B20-10", "TM1607-1-B20-10:trip", this, new AMControlStatusCheckerDefault(0));
	tmM1B2_ = new AMReadOnlyPVwStatusControl("Temperature M1B 2", "TM1607-1-B20-11", "TM1607-1-B20-11:trip", this, new AMControlStatusCheckerDefault(0));
	tmM1B3_ = new AMReadOnlyPVwStatusControl("Temperature M1B 3", "TM1607-1-B20-12", "TM1607-1-B20-12:trip", this, new AMControlStatusCheckerDefault(0));
	tmM2A1_ = new AMReadOnlyPVwStatusControl("Temperature M2A 1", "TM1607-1-B21-01", "TM1607-1-B21-01:trip", this, new AMControlStatusCheckerDefault(0));
	tmM2A2_ = new AMReadOnlyPVwStatusControl("Temperature M2A 2", "TM1607-1-B21-02", "TM1607-1-B21-02:trip", this, new AMControlStatusCheckerDefault(0));
	tmM2A3_ = new AMReadOnlyPVwStatusControl("Temperature M2A 3", "TM1607-1-B21-03", "TM1607-1-B21-03:trip", this, new AMControlStatusCheckerDefault(0));
	tmM2B1_ = new AMReadOnlyPVwStatusControl("Temperature M2B 1", "TM1607-1-B22-01", "TM1607-1-B22-01:trip", this, new AMControlStatusCheckerDefault(0));
	tmM2B2_ = new AMReadOnlyPVwStatusControl("Temperature M2B 2", "TM1607-1-B22-02", "TM1607-1-B22-02:trip", this, new AMControlStatusCheckerDefault(0));
	tmM2B3_ = new AMReadOnlyPVwStatusControl("Temperature M2B 3", "TM1607-1-B22-03", "TM1607-1-B22-03:trip", this, new AMControlStatusCheckerDefault(0));
	tmMono1_ = new AMReadOnlyPVwStatusControl("Temperature Mono 1", "TM1607-1-B20-13", "TM1607-1-B20-13:trip", this, new AMControlStatusCheckerDefault(0));
	tmMono2_ = new AMReadOnlyPVwStatusControl("Temperature Mono 2", "TM1607-1-B20-14", "TM1607-1-B20-14:trip", this, new AMControlStatusCheckerDefault(0));
	tmMono3_ = new AMReadOnlyPVwStatusControl("Temperature Mono 3", "TM1607-1-B20-15", "TM1607-1-B20-15:trip", this, new AMControlStatusCheckerDefault(0));
	tmMono4_ = new AMReadOnlyPVwStatusControl("Temperature Mono 4", "TM1607-1-B20-16", "TM1607-1-B20-16:trip", this, new AMControlStatusCheckerDefault(0));
	tmMono5_ = new AMReadOnlyPVwStatusControl("Temperature Mono 5", "TM1607-1-B20-17", "TM1607-1-B20-17:trip", this, new AMControlStatusCheckerDefault(0));
	tmMono6_ = new AMReadOnlyPVwStatusControl("Temperature Mono 6", "TM1607-1-B20-18", "TM1607-1-B20-18:trip", this, new AMControlStatusCheckerDefault(0));
	tmMono7_ = new AMReadOnlyPVwStatusControl("Temperature Mono 7", "TM1607-1-B20-19", "TM1607-1-B20-19:trip", this, new AMControlStatusCheckerDefault(0));

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
	fltM1A_ = new AMReadOnlyPVwStatusControl("Flow Transducer M1A", "FLT1607-1-B20-02", "FLT1607-1-B20-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	fltM1B_ = new AMReadOnlyPVwStatusControl("Flow Transducer M1B", "FLT1607-1-B20-03", "FLT1607-1-B20-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	fltM2A_ = new AMReadOnlyPVwStatusControl("Flow Transducer M2A", "FLT1607-1-B21-01", "FLT1607-1-B21-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	fltM2B_ = new AMReadOnlyPVwStatusControl("Flow Transducer M2B", "FLT1607-1-B22-01", "FLT1607-1-B22-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	fltMono_ = new AMReadOnlyPVwStatusControl("Flow Transducer Mono", "FLT1607-1-B20-04", "FLT1607-1-B20-04:lowflow", this, new AMControlStatusCheckerDefault(0));
	fltExitSlits_ = new AMReadOnlyPVwStatusControl("Flow Transducer Exit Slits", "FLT1607-1-B20-05", "FLT1607-1-B20-05:lowflow", this, new AMControlStatusCheckerDefault(0));
	fltInterimSlits1_ = new AMReadOnlyPVwStatusControl("Flow Transducer Interim Slits 1", "FLT1607-1-B21-02", "FLT1607-1-B21-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	fltInterimSlits2_ = new AMReadOnlyPVwStatusControl("Flow Transducer Interim Slits 2", "FLT1607-1-B21-03", "FLT1607-1-B21-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	fltPoeSsh1_ = new AMReadOnlyPVwStatusControl("Flow Transducer POE SSH1", "FLT1607-1-B21-04", "FLT1607-1-B21-04:lowflow", this, new AMControlStatusCheckerDefault(0));
	fltPoeSsh2_ = new AMReadOnlyPVwStatusControl("Flow Transducer POE SSH2", "FLT1607-1-B22-02", "FLT1607-1-B22-02:lowflow", this, new AMControlStatusCheckerDefault(0));

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
	sampleStageNormal_ = new AMPVwStatusControl("Normal Sample Stage", "TS1607-2-B21-01:N:user:mm:sp", "TS1607-2-B21-01:N:user:mm", "TS1607-2-B21-01:N:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.01, 10.0);

	sampleStageX_ = new CLSVMEMotor("xMotorSampleStage", "SVM1607-2-B21-02", "X Motor Sample Stage", true, 0.01, 10.0, this);
	sampleStageY_ = new CLSVMEMotor("yMotorSampleStage", "SVM1607-2-B21-03", "Y Motor Sample Stage", true, 0.01, 10.0, this);
	sampleStageZ_ = new CLSVMEMotor("zMotorSampleStage", "SVM1607-2-B21-01", "Z Motor Sample Stage", true, 0.01, 10.0, this);

	pseudoSampleStage_ = new SampleStageControl(sampleStageHorizontal_, sampleStageVertical_, sampleStageNormal_, this);
	pseudoSampleStage_->setXRange(-700000, 700000);
	pseudoSampleStage_->setYRange(-200000, 200000);
	pseudoSampleStage_->setZRange(-200000, 200000);

	realSampleStage_ = new SampleStageControl(sampleStageX_, sampleStageY_, sampleStageZ_, this);
	realSampleStage_->setXRange(-700000, 700000);
	realSampleStage_->setYRange(-200000, 200000);
	realSampleStage_->setZRange(-200000, 200000);

	sampleStageMotorSet_ = new AMControlSet(this);
	sampleStageMotorSet_->addControl(sampleStageHorizontal_);
	sampleStageMotorSet_->addControl(sampleStageVertical_);
	sampleStageMotorSet_->addControl(sampleStageNormal_);
	sampleStageMotorSet_->addControl(sampleStageX_);
	sampleStageMotorSet_->addControl(sampleStageY_);
	sampleStageMotorSet_->addControl(sampleStageZ_);

	connect(sampleStageMotorSet_, SIGNAL(controlSetTimedOut()), this, SLOT(sampleStageError()));

	sampleStagePidX_ = new AMPVControl("Sample Stage PID X", "SVM1607-2-B21-02:hold:sp", "SVM1607-2-B21-02:hold", QString(), this);
	sampleStagePidY_ = new AMPVControl("Sample Stage PID Y", "SVM1607-2-B21-03:hold:sp", "SVM1607-2-B21-03:hold", QString(), this);
	sampleStagePidZ_ = new AMPVControl("Sample Stage PID Z", "SVM1607-2-B21-01:hold:sp", "SVM1607-2-B21-01:hold", QString(), this);

	sampleStagePID_ = new PIDLoopControl("PID - Sample Stage", sampleStagePidX_, sampleStagePidY_, sampleStagePidZ_, this);

	resetPseudoMotors_ = new AMProcessVariable("TS1607-2-B21-01:HNV:loadOffsets.PROC", false, this);
}

void VESPERSBeamline::setupEndstation()
{
	// The controls used for the control window.
	ccdMotor_ = new CLSVMEMotor("CCD motor", "SMTR1607-2-B21-18", "CCD motor", false, 1.0, 2.0, this);
	microscopeMotor_ = new CLSVMEMotor("Microscope motor", "SMTR1607-2-B21-17", "Microscope motor", false, 1.0, 2.0, this);
	fourElMotor_ = new CLSVMEMotor("4-Element Vortex motor", "SMTR1607-2-B21-27", "4-Element Vortex motor", false, 1.0, 2.0, this);
	singleElMotor_ = new CLSVMEMotor("1-Element Vortex motor", "SMTR1607-2-B21-15", "1-Element Vortex motor", false, 1.0, 2.0, this);

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

void VESPERSBeamline::setupDetectors()
{
	amNames2pvNames_.set("Isplit", "BL1607-B2-1:AddOns:Isplit");
	amNames2pvNames_.set("Iprekb", "BL1607-B2-1:mcs07:fbk");
	amNames2pvNames_.set("Imini", "BL1607-B2-1:mcs08:fbk");
	amNames2pvNames_.set("Ipost", "BL1607-B2-1:mcs09:fbk");

	iSplitControl_ = new AMReadOnlyPVControl("Isplit", amNames2pvNames_.valueF("Isplit"), this, "Split Ion Chamber");
	iPreKBControl_ = new AMReadOnlyPVControl("Iprekb", amNames2pvNames_.valueF("Iprekb"), this, "Pre-KB Ion Chamber");
	iMiniControl_ = new AMReadOnlyPVControl("Imini", amNames2pvNames_.valueF("Imini"), this, "Mini Ion Chamber");
	iPostControl_ = new AMReadOnlyPVControl("Ipost", amNames2pvNames_.valueF("Ipost"), this, "Post Sample Ion Chamber");

	iSplit_ = new AMSingleControlDetector(iSplitControl_->name(), iSplitControl_, AMDetector::RequestRead, this);
	iPreKB_ = new AMSingleControlDetector(iPreKBControl_->name(), iPreKBControl_, AMDetector::RequestRead, this);
	iMini_ = new AMSingleControlDetector(iMiniControl_->name(), iMiniControl_, AMDetector::RequestRead, this);
	iPost_ = new AMSingleControlDetector(iPostControl_->name(), iPostControl_, AMDetector::RequestRead, this);

	ionChambers_ = new AMDetectorSet(this);
	ionChambers_->addDetector(iSplit_);
	ionChambers_->addDetector(iPreKB_);
	ionChambers_->addDetector(iMini_);
	ionChambers_->addDetector(iPost_);

	ionChamberCalibration_ = new VESPERSIonChamberCalibration(this);
	ionChamberCalibration_->addSplitIonChamber(new VESPERSSplitIonChamber("Split", "PS1607-201:c2:Voltage", "AMP1607-202", "AMP1607-203", "BL1607-B2-1:mcs05:userRate", "BL1607-B2-1:mcs06:userRate", "BL1607-B2-1:mcs05:fbk", "BL1607-B2-1:mcs06:fbk", this));
	ionChamberCalibration_->addIonChamber(new VESPERSIonChamber("Pre-KB", "PS1607-202:c1:Voltage", "AMP1607-204", "BL1607-B2-1:mcs07:userRate", "BL1607-B2-1:mcs07:fbk", this));
	ionChamberCalibration_->addIonChamber(new VESPERSIonChamber("Mini", "PS1607-202:c2:Voltage", "AMP1607-205", "BL1607-B2-1:mcs08:userRate", "BL1607-B2-1:mcs08:fbk", this));
	ionChamberCalibration_->addIonChamber(new VESPERSIonChamber("Post", "PS1607-203:c1:Voltage", "AMP1607-206", "BL1607-B2-1:mcs09:userRate", "BL1607-B2-1:mcs09:fbk", this));

	vortex1E_ = new XRFDetector("1-el Vortex", 1, "IOC1607-004", this);
	connect(vortexXRF1E(), SIGNAL(detectorConnected(bool)), this, SLOT(singleElVortexError(bool)));

	vortex4E_ = new XRFDetector("4-el Vortex", 4, "dxp1607-B21-04", this);
	connect(vortexXRF4E(), SIGNAL(detectorConnected(bool)), this, SLOT(fourElVortexError(bool)));
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

	valves_ = new VESPERSValveGroupControl(valveList_);

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
	endstationMotorSet_->addControl(microscopeMotor_);
	endstationMotorSet_->addControl(fourElMotor_);
	endstationMotorSet_->addControl(singleElMotor_);
	endstationMotorSet_->addControl(sampleStageNormal_);

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

void VESPERSBeamline::setupMono()
{
	energyRelative_ = new AMPVwStatusControl("Relative Energy Movement", "07B2_Mono_SineB_delE", "07B2_Mono_SineB_delE", "SMTR1607-1-B20-20:status", "SMTR1607-1-B20-20:stop", this, 0.1, 2.0, new AMControlStatusCheckerDefault(0), 1);
	mono_ = new VESPERSMonochromator(this);
	intermediateSlits_ = new VESPERSIntermediateSlits(this);

	synchronizedDwellTime_ = new CLSSynchronizedDwellTime("BL1607-B2-1:dwell", this);
	synchronizedDwellTime_->addElement(0);
	synchronizedDwellTime_->addElement(1);
	synchronizedDwellTime_->addElement(2);
	synchronizedDwellTime_->addElement(3);
	synchronizedDwellTime_->addElement(4);

	beamPositions_.insert(Pink, 0);
	beamPositions_.insert(TenPercent, -12.5);
	beamPositions_.insert(Si, -17.5);
	beamPositions_.insert(OnePointSixPercent, -22.5);

	beamSelectionMotor_ = new CLSVMEMotor("MonoBeamSelectionMotor", "SMTR1607-1-B20-21", "Motor that controls which beam makes it down the pipe.", false, 0.1, 2.0, this);
	connect(beamSelectionMotor_, SIGNAL(movingChanged(bool)), this, SLOT(determineBeam()));
	connect(beamSelectionMotor_, SIGNAL(valueChanged(double)), this, SLOT(onBeamSelectionMotorConnected()));
}

AMBeamlineActionItem *VESPERSBeamline::createBeamChangeAction(Beam beam)
{
	// If we are already at the new beam position and the internal state of the beam is the same, then don't do anything.
	if (beam_ == beam && beamSelectionMotor_->withinTolerance(beamPositions_.value(beam)))
		return 0;

	// To change beams, it is either a two or three stage process.
	/*
		First: Turn off the ability to scan.  This ensures that the mono motor doesn't swing wildly around while switching between beams.
		Second: Move to the chosen beam.
		Third (if applicable): If the new beam is a monochromatic beam, turn on the ability to scan the energy.
	 */
	AMBeamlineParallelActionsList *changeBeamActionsList = new AMBeamlineParallelActionsList;
	AMBeamlineListAction *changeBeamAction = new AMBeamlineListAction(changeBeamActionsList);

	changeBeamActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	changeBeamActionsList->appendAction(0, mono()->createAllowScanningAction(false));

	changeBeamActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	AMBeamlineControlMoveAction *moveBeamAction = new AMBeamlineControlMoveAction(beamSelectionMotor());
	moveBeamAction->setSetpoint(beamPositions_.value(beam));
	changeBeamActionsList->appendAction(1, moveBeamAction);

	if (beam != Pink){

		changeBeamActionsList->appendStage(new QList<AMBeamlineActionItem*>());
		changeBeamActionsList->appendAction(2, mono()->createAllowScanningAction(true));
	}

	return changeBeamAction;
}

void VESPERSBeamline::determineBeam()
{
	Beam temp;

	if (beamSelectionMotor_->withinTolerance(beamPositions_.value(Pink)))
		temp = Pink;
	else if (beamSelectionMotor_->withinTolerance(beamPositions_.value(TenPercent)))
		temp = TenPercent;
	else if (beamSelectionMotor_->withinTolerance(beamPositions_.value(OnePointSixPercent)))
		temp = OnePointSixPercent;
	else if (beamSelectionMotor_->withinTolerance(beamPositions_.value(Si)))
		temp = Si;
	else
		temp = None;

	if (temp != beam_){

		beam_ = temp;
		emit currentBeamChanged(beam_);
	}
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
		for (int i = 0; i < temperatureSet_->count(); i++)
			connect(qobject_cast<AMReadOnlyPVwStatusControl *>(temperatureSet_->at(i)), SIGNAL(movingChanged(bool)), this, SLOT(temperatureError()));
}

void VESPERSBeamline::flowSwitchConnected(bool connected)
{
	if (connected)
		connect(flowSwitchSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(flowSwitchError()));
}

void VESPERSBeamline::flowTransducerConnected(bool connected)
{
	if (connected)
		for (int i = 0; i < flowTransducerSet_->count(); i++)
			connect(qobject_cast<AMReadOnlyPVwStatusControl *>(flowTransducerSet_->at(i)), SIGNAL(movingChanged(bool)), this, SLOT(flowTransducerError()));
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
	AMReadOnlyPVwStatusControl *current = 0;

	for (int i = 0; i < temperatureSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVwStatusControl *>(temperatureSet_->at(i));

		if (current->isMoving())
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
	AMReadOnlyPVwStatusControl *current = 0;

	for (int i = 0; i < flowTransducerSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVwStatusControl *>(flowTransducerSet_->at(i));

		if (current->isMoving())
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

}
