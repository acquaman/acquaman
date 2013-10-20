/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/AMSingleControlDetector.h"
#include "actions/AMBeamlineControlMoveAction.h"
#include "actions/AMBeamlineParallelActionsList.h"
#include "actions/AMBeamlineListAction.h"
#include "beamline/CLS/CLSBiStateControl.h"

VESPERSBeamline::VESPERSBeamline()
	: AMBeamline("VESPERS Beamline")
{
	setupSynchronizedDwellTime();
	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupEndstation();
	setupDetectors();
	setupControlSets();
	setupMono();
	setupMotorGroup();
	setupExposedControls();
}

void VESPERSBeamline::setupDiagnostics()
{
	// The shutters.
	psh1_ = new CLSBiStateControl("PSH", "First Photon Shutter", "PSH1408-B20-01:state", "PSH1408-B20-01:opr:open", "PSH1408-B20-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	psh2_ = new CLSBiStateControl("Optic", "Second Photon Shutter", "PSH1408-B20-02:state", "PSH1408-B20-02:opr:open", "PSH1408-B20-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	ssh1_ = new CLSBiStateControl("SSH", "First Safety Shutter", "SSH1408-B20-01:state", "SSH1408-B20-01:opr:open", "SSH1408-B20-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	ssh2_ = new CLSBiStateControl("Exp.", "Second Safety Shutter", "SSH1607-1-B21-01:state", "SSH1607-1-B21-01:opr:open", "SSH1607-1-B21-01:opr:close", new AMControlStatusCheckerDefault(2), this);

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

	// The actual valve control.  The reason for separating them is due to the fact that there currently does not exist an AMControl that handles setups like valves.
	vvrFE1_ = new AMReadOnlyPVwStatusControl("Valve Control FE1", "VVR1408-B20-01:state", "VVR1408-B20-01:state", this, new AMControlStatusCheckerDefault(4));
	vvrFE2_ = new CLSBiStateControl("Valve Control FE2", "Valve Control FE2", "VVR1607-1-B20-01:state", "VVR1607-1-B20-01:opr:open", "VVR1607-1-B20-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrFE2_->setAllowsMovesWhileMoving(true);
	vvrM1_ = new CLSBiStateControl("Valve Control M1", "Valve Control M1", "VVR1607-1-B20-02:state", "VVR1607-1-B20-02:opr:open", "VVR1607-1-B20-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrM1_->setAllowsMovesWhileMoving(true);
	vvrM2_ = new CLSBiStateControl("Valve Control M2", "Valve Control M2", "VVR1607-1-B20-03:state", "VVR1607-1-B20-03:opr:open", "VVR1607-1-B20-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrM2_->setAllowsMovesWhileMoving(true);
	vvrBPM1_ = new CLSBiStateControl("Valve Control BPM1", "Valve Control BPM1", "VVR1607-1-B20-04:state", "VVR1607-1-B20-04:opr:open", "VVR1607-1-B20-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrBPM1_->setAllowsMovesWhileMoving(true);
	vvrMono_ = new CLSBiStateControl("Valve Control Mono", "Valve Control Mono", "VVR1607-1-B20-05:state", "VVR1607-1-B20-05:opr:open", "VVR1607-1-B20-05:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMono_->setAllowsMovesWhileMoving(true);
	vvrExitSlits_ = new CLSBiStateControl("Valve Control Exit Slits", "Valve Control Exit Slits", "VVR1607-1-B20-06:state", "VVR1607-1-B20-06:opr:open", "VVR1607-1-B20-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrExitSlits_->setAllowsMovesWhileMoving(true);
	vvrStraightSection_ = new CLSBiStateControl("Valve Control Straight Section", "Valve Control Straight Section", "VVR1607-1-B20-07:state", "VVR1607-1-B20-07:opr:open", "VVR1607-1-B20-07:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrStraightSection_->setAllowsMovesWhileMoving(true);
	vvrBPM3_ = new CLSBiStateControl("Valve Control BPM3", "Valve Control BPM3", "VVR1607-1-B20-08:state", "VVR1607-1-B20-08:opr:open", "VVR1607-1-B20-08:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrBPM3_->setAllowsMovesWhileMoving(true);
	vvrSSH_ = new CLSBiStateControl("Valve Control SSH", "Valve Control SSH", "VVR1607-1-B21-01:state", "VVR1607-1-B21-01:opr:open", "VVR1607-1-B21-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSSH_->setAllowsMovesWhileMoving(true);
	vvrBeamTransfer_ = new CLSBiStateControl("Valve Control Beam Transfer", "Valve Control Beam Transfer", "VVR1607-2-B21-01:state", "VVR1607-2-B21-01:opr:open", "VVR1607-2-B21-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrBeamTransfer_->setAllowsMovesWhileMoving(true);

	// Index used for opening and closing all the valves.
	valveIndex_ = -1;

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
}

void VESPERSBeamline::setupSampleStage()
{
	// Currently, the wire stage and sample stage have been flipped due to hardware problems with the sample stage.
	sampleStageHorizontal_ = new AMPVwStatusControl("Horizontal Sample Stage", "TS1607-2-B21-02:H:user:mm:sp", "TS1607-2-B21-02:H:user:mm", "TS1607-2-B21-02:H:status", "TS1607-2-B21-02:HNV:stop.PROC", this, 0.01, 10.0);
	sampleStageVertical_ = new AMPVwStatusControl("Vertical Sample Stage", "TS1607-2-B21-02:V:user:mm:sp", "TS1607-2-B21-02:V:user:mm", "TS1607-2-B21-02:V:status", "TS1607-2-B21-02:HNV:stop.PROC", this, 0.01, 10.0);
	sampleStageNormal_ = new AMPVwStatusControl("Normal Sample Stage", "TS1607-2-B21-02:N:user:mm:sp", "TS1607-2-B21-02:N:user:mm", "TS1607-2-B21-02:N:status", "TS1607-2-B21-02:HNV:stop.PROC", this, 0.01, 10.0);

	((AMPVwStatusControl *)sampleStageHorizontal_)->setMoveStartTolerance(0.0001);
	((AMPVwStatusControl *)sampleStageVertical_)->setMoveStartTolerance(0.0001);
	((AMPVwStatusControl *)sampleStageNormal_)->setMoveStartTolerance(0.0001);

	sampleStageX_ = new AMPVwStatusControl("X Motor Sample Stage", "TS1607-2-B21-02:X:user:mm:sp", "TS1607-2-B21-02:X:user:mm", "TS1607-2-B21-02:X:status", "TS1607-2-B21-02:XYZ:stop.PROC", this, 0.01, 10.0);
	sampleStageY_ = new AMPVwStatusControl("Y Motor Sample Stage", "TS1607-2-B21-02:Y:user:mm:sp", "TS1607-2-B21-02:Y:user:mm", "TS1607-2-B21-02:Y:status", "TS1607-2-B21-02:XYZ:stop.PROC", this, 0.01, 10.0);
	sampleStageZ_ = new AMPVwStatusControl("Z Motor Sample Stage", "TS1607-2-B21-02:Z:user:mm:sp", "TS1607-2-B21-02:Z:user:mm", "TS1607-2-B21-02:Z:status", "TS1607-2-B21-02:XYZ:stop.PROC", this, 0.01, 10.0);

	((AMPVwStatusControl *)sampleStageX_)->setMoveStartTolerance(0.0001);
	((AMPVwStatusControl *)sampleStageY_)->setMoveStartTolerance(0.0001);
	((AMPVwStatusControl *)sampleStageZ_)->setMoveStartTolerance(0.0001);

	sampleStageMotorSet_ = new AMControlSet(this);
	sampleStageMotorSet_->addControl(sampleStageHorizontal_);
	sampleStageMotorSet_->addControl(sampleStageVertical_);
	sampleStageMotorSet_->addControl(sampleStageNormal_);
	sampleStageMotorSet_->addControl(sampleStageX_);
	sampleStageMotorSet_->addControl(sampleStageY_);
	sampleStageMotorSet_->addControl(sampleStageZ_);

	connect(sampleStageMotorSet_, SIGNAL(controlSetTimedOut()), this, SLOT(sampleStageError()));

	sampleStagePidX_ = new AMPVControl("Sample Stage PID X", "SVM1607-2-B21-05:hold:sp", "SVM1607-2-B21-05:hold", QString(), this);
	sampleStagePidY_ = new AMPVControl("Sample Stage PID Y", "SVM1607-2-B21-06:hold:sp", "SVM1607-2-B21-06:hold", QString(), this);
	sampleStagePidZ_ = new AMPVControl("Sample Stage PID Z", "SVM1607-2-B21-04:hold:sp", "SVM1607-2-B21-04:hold", QString(), this);

	sampleStagePID_ = new VESPERSPIDLoopControl("PID - Sample Stage", sampleStagePidX_, sampleStagePidY_, sampleStagePidZ_, this);

	wireStagePidX_ = new AMPVControl("Wire Stage PID X", "SVM1607-2-B21-02:hold:sp", "SVM1607-2-B21-02:hold", QString(), this);
	wireStagePidY_ = new AMPVControl("Wire Stage PID Y", "SVM1607-2-B21-03:hold:sp", "SVM1607-2-B21-03:hold", QString(), this);
	wireStagePidZ_ = new AMPVControl("Wire Stage PID Z", "SVM1607-2-B21-01:hold:sp", "SVM1607-2-B21-01:hold", QString(), this);

	wireStagePID_ = new VESPERSPIDLoopControl("PID - Wire Stage", wireStagePidX_, wireStagePidY_, wireStagePidZ_, this);

	wireStageHorizontal_ = new AMPVwStatusControl("Horizontal Wire Stage", "TS1607-2-B21-01:H:user:mm:sp", "TS1607-2-B21-01:H:user:mm", "TS1607-2-B21-01:H:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.01, 10.0);
	wireStageVertical_ = new AMPVwStatusControl("Vertical Wire Stage", "TS1607-2-B21-01:V:user:mm:sp", "TS1607-2-B21-01:V:user:mm", "TS1607-2-B21-01:V:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.01, 10.0);
	wireStageNormal_ = new AMPVwStatusControl("Normal Wire Stage", "TS1607-2-B21-01:N:user:mm:sp", "TS1607-2-B21-01:N:user:mm", "TS1607-2-B21-01:N:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.01, 10.0);

	((AMPVwStatusControl *)sampleStageHorizontal_)->setMoveStartTolerance(0.0001);
	((AMPVwStatusControl *)sampleStageVertical_)->setMoveStartTolerance(0.0001);
	((AMPVwStatusControl *)sampleStageNormal_)->setMoveStartTolerance(0.0001);

	// attocube
	attoStageHorizontal_ = new AMPVwStatusControl("Horizontal Atto Stage", "TS1607-2-B21-07:H:user:mm:sp", "TS1607-2-B21-07:H:user:mm", "TS1607-2-B21-07:H:status", "TS1607-2-B21-07:HNV:stop.PROC", this, 0.01, 10.0);
	attoStageVertical_ = new AMPVwStatusControl("Vertical Atto Stage", "TS1607-2-B21-07:V:user:mm:sp", "TS1607-2-B21-07:V:user:mm", "TS1607-2-B21-07:V:status", "TS1607-2-B21-07:HNV:stop.PROC", this, 0.01, 10.0);
	attoStageNormal_ = new AMPVwStatusControl("Normal Atto Stage", "TS1607-2-B21-07:N:user:mm:sp", "TS1607-2-B21-07:N:user:mm", "TS1607-2-B21-07:N:status", "TS1607-2-B21-07:HNV:stop.PROC", this, 0.01, 10.0);

	((AMPVwStatusControl *)attoStageHorizontal_)->setMoveStartTolerance(0.00001);
	((AMPVwStatusControl *)attoStageVertical_)->setMoveStartTolerance(0.00001);
	((AMPVwStatusControl *)attoStageNormal_)->setMoveStartTolerance(0.00001);

	attoStageX_ = new AMPVwStatusControl("Atto X Stage", "TS1607-2-B21-07:X:user:mm:sp", "TS1607-2-B21-07:X:user:mm", "TS1607-2-B21-07:X:user:status", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.01, 10.0);
	attoStageZ_ = new AMPVwStatusControl("Atto Z Stage", "TS1607-2-B21-07:Z:user:mm:sp", "TS1607-2-B21-07:Z:user:mm", "TS1607-2-B21-07:Z:user:status", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.01, 10.0);
	attoStageY_ = new AMPVwStatusControl("Atto Y Stage", "TS1607-2-B21-07:Y:user:mm:sp", "TS1607-2-B21-07:Y:user:mm", "TS1607-2-B21-07:Y:user:status", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.01, 10.0);

	((AMPVwStatusControl *)attoStageX_)->setMoveStartTolerance(0.00001);
	((AMPVwStatusControl *)attoStageZ_)->setMoveStartTolerance(0.00001);
	((AMPVwStatusControl *)attoStageY_)->setMoveStartTolerance(0.00001);

	attoStageRz_ = new AMPVwStatusControl("Atto Phi Stage", "SVM1607-2-B21-09:deg:sp", "SVM1607-2-B21-09:deg", "SVM1607-2-B21-09:status", "SVM1607-2-B21-09:stop.PROC", this, 0.01, 10.0);
	attoStageRy_ = new AMPVwStatusControl("Atto Theta Stage", "SVM1607-2-B21-07:deg:sp", "SVM1607-2-B21-07:deg", "SVM1607-2-B21-07:status", "SVM1607-2-B21-07:stop.PROC", this, 0.01, 10.0);
	attoStageRx_ = new AMPVwStatusControl("Atto Psi Stage", "SVM1607-2-B21-08:deg:sp", "SVM1607-2-B21-08:deg", "SVM1607-2-B21-08:status", "SVM1607-2-B21-08:stop.PROC", this, 0.01, 10.0);

	((AMPVwStatusControl *)attoStageRz_)->setMoveStartTolerance(0.00001);
	((AMPVwStatusControl *)attoStageRy_)->setMoveStartTolerance(0.00001);
	((AMPVwStatusControl *)attoStageRx_)->setMoveStartTolerance(0.00001);

	// Reset signals.
	pseudoSampleStageResetControl_ = new AMSinglePVControl("Pseudo Sample Stage Reset Control", "TS1607-2-B21-02:HNV:loadOffsets.PROC", this, 0.1);
	realSampleStageResetControl_ = new AMSinglePVControl("Real Sample Stage Reset Control", "TS1607-2-B21-02:XYZ:loadOffsets.PROC", this, 0.1);
	pseudoAttoStageResetControl_ = new AMSinglePVControl("Pseudo Atto Stage Reset Control", "TS1607-2-B21-07:HNV:loadOffsets.PROC", this, 0.1);
	realAttoStageResetControl_ = new AMSinglePVControl("Real Atto Stage Reset Control", "TS1607-2-B21-07:XYZ:loadOffsets.PROC", this, 0.1);
}

void VESPERSBeamline::setupMotorGroup()
{
	// This is an AMMotorGroupObject because the CLSPseudoMotorGroupObject isn't used for all the motor group objects.
	AMMotorGroupObject *motorObject = 0;
	motorGroup_ = new CLSPseudoMotorGroup(this);

	motorObject = new CLSPseudoMotorGroupObject("Sample Stage - H, V, N",
										 QStringList() << "H" << "V" << "N",
										 QStringList() << "mm" << "mm" << "mm",
										 QList<AMControl *>() << sampleStageHorizontal_ << sampleStageVertical_ << sampleStageNormal_,
										 QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical << AMMotorGroupObject::Normal,
										 QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational,
										 pseudoSampleStageResetControl_,
										 this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);
	motorObject = new CLSPseudoMotorGroupObject("Sample Stage - X, Z, Y",
										 QStringList() << "X" << "Z" << "Y",
										 QStringList() << "mm" << "mm" << "mm",
										 QList<AMControl *>() << sampleStageX_ << sampleStageZ_ << sampleStageY_,
										 QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical << AMMotorGroupObject::Normal,
										 QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational,
										 realSampleStageResetControl_,
										 this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);
	motorObject = new CLSPseudoMotorGroupObject("Wire Stage - H, V, N",
										 QStringList() << "H" << "V" << "N",
										 QStringList() << "mm" << "mm" << "mm",
										 QList<AMControl *>() << wireStageHorizontal_ << wireStageVertical_ << wireStageNormal_,
										 QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical << AMMotorGroupObject::Normal,
										 QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational,
										 pseudoSampleStageResetControl_,
										 this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);
	motorObject = new CLSPseudoMotorGroupObject("Attocube Stage - H, V, N",
										 QStringList() << "H" << "V" << "N",
										 QStringList() << "mm" << "mm" << "mm",
										 QList<AMControl *>() << attoStageHorizontal_ << attoStageVertical_ << attoStageNormal_,
										 QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical << AMMotorGroupObject::Normal,
										 QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational,
										 pseudoAttoStageResetControl_,
										 this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);
	motorObject = new CLSPseudoMotorGroupObject("Attocube Stage - X, Z, Y",
										 QStringList() << "X" << "Z" << "Y",
										 QStringList() << "mm" << "mm" << "mm",
										 QList<AMControl *>() << attoStageX_ << attoStageZ_ << attoStageY_,
										 QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical << AMMotorGroupObject::Normal,
										 QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational,
										 realAttoStageResetControl_,
										 this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);
	motorObject = new AMMotorGroupObject("Attocube Stage - Rx", "Rx", "deg", attoStageRx_, AMMotorGroupObject::Horizontal, AMMotorGroupObject::Rotational, this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);
	motorObject = new AMMotorGroupObject("Attocube Stage - Ry", "Ry", "deg", attoStageRy_, AMMotorGroupObject::Horizontal, AMMotorGroupObject::Rotational, this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);
	motorObject = new AMMotorGroupObject("Attocube Stage - Rz", "Rz", "deg", attoStageRz_, AMMotorGroupObject::Horizontal, AMMotorGroupObject::Rotational, this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);
}

void VESPERSBeamline::setupEndstation()
{
	endstation_ = new VESPERSEndstation(this);
}

void VESPERSBeamline::setupDetectors()
{
	amNames2pvNames_.set("Isplit", "BL1607-B2-1:AddOns:Isplit");
	amNames2pvNames_.set("Iprekb", "BL1607-B2-1:mcs07:fbk");
	amNames2pvNames_.set("Imini", "BL1607-B2-1:mcs08:fbk");
	amNames2pvNames_.set("Ipost", "BL1607-B2-1:mcs09:fbk");

	ionChambers_ = new AMOldDetectorSet(this);

	CLSSplitIonChamber *tempSplit = new CLSSplitIonChamber("Isplit", "Split", "BL1607-B2-1:mcs05:fbk", "BL1607-B2-1:mcs06:fbk", "BL1607-B2-1:mcs05:userRate", "BL1607-B2-1:mcs06:userRate", "AMP1607-202:sens_num.VAL", "AMP1607-203:sens_num.VAL", "AMP1607-202:sens_unit.VAL", "AMP1607-203:sens_unit.VAL", this);
	tempSplit->setVoltagRange(1.0, 4.5);
	ionChambers_->addDetector(tempSplit);
	iSplit_ = tempSplit;

	CLSIonChamber *temp = new CLSIonChamber("Iprekb", "Pre-KB", "BL1607-B2-1:mcs07:fbk", "BL1607-B2-1:mcs07:userRate", "AMP1607-204:sens_num.VAL", "AMP1607-204:sens_unit.VAL", this);
	temp->setVoltagRange(1.0, 4.5);
	ionChambers_->addDetector(temp);
	iPreKB_ = temp;

	temp = new CLSIonChamber("Imini", "Mini", "BL1607-B2-1:mcs08:fbk", "BL1607-B2-1:mcs08:userRate", "AMP1607-205:sens_num.VAL", "AMP1607-205:sens_unit.VAL", this);
	temp->setVoltagRange(1.0, 4.5);
	ionChambers_->addDetector(temp);
	iMini_ = temp;

	temp = new CLSIonChamber("Ipost", "Post", "BL1607-B2-1:mcs09:fbk", "BL1607-B2-1:mcs09:userRate", "AMP1607-206:sens_num.VAL", "AMP1607-206:sens_unit.VAL", this);
	temp->setVoltagRange(1.0, 4.5);
	ionChambers_->addDetector(temp);
	iPost_ = temp;

	vortex1E_ = new XRFDetector("1-el Vortex", 1, "IOC1607-004", this);
	connect(vortexXRF1E(), SIGNAL(connected(bool)), this, SLOT(singleElVortexError(bool)));

	vortex4E_ = new XRFDetector("4-el Vortex", 4, "dxp1607-B21-04", this);
	connect(vortexXRF4E(), SIGNAL(connected(bool)), this, SLOT(fourElVortexError(bool)));

	roperCCD_ = new VESPERSRoperCCDDetector("Roper CCD", "Roper CCD Camera", this);
	marCCD_ = new VESPERSMarCCDDetector("Mar CCD", "Mar 165 CCD Camera", this);
	pilatusCCD_ = new VESPERSPilatusCCDDetector("Pilatus", "Pilatus 1M Pixel Array Detector", this);
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
}

void VESPERSBeamline::setupMono()
{
	energy_ = new AMPVwStatusControl("Energy", "07B2_Mono_SineB_Egec:eV", "07B2_Mono_SineB_Ea", "SMTR1607-1-B20-20:status", "SMTR1607-1-B20-20:stop", this, 20, 2.0, new AMControlStatusCheckerDefault(0), 1);
	energyRelative_ = new AMPVwStatusControl("Relative Energy Movement", "07B2_Mono_SineB_delE", "07B2_Mono_SineB_delE", "SMTR1607-1-B20-20:status", "SMTR1607-1-B20-20:stop", this, 0.1, 2.0, new AMControlStatusCheckerDefault(0), 1);
	masterDwellTime_ = new AMSinglePVControl("Master Dwell Time", "BL1607-B2-1:dwell:setTime", this, 0.1);
	kControl_ = new AMPVwStatusControl("K-space", "07B2_Mono_SineB_K:fbk", "07B2_Mono_SineB_K", "SMTR1607-1-B20-20:status", QString(), this, 0.01);

	mono_ = new VESPERSMonochromator(this);
	intermediateSlits_ = new VESPERSIntermediateSlits(this);
}

void VESPERSBeamline::setupSynchronizedDwellTime()
{
	synchronizedDwellTime_ = new CLSSynchronizedDwellTime("BL1607-B2-1:dwell", this);
	synchronizedDwellTime_->addElement(0);
	synchronizedDwellTime_->addElement(1);
	synchronizedDwellTime_->addElement(2);
	synchronizedDwellTime_->addElement(3);
	synchronizedDwellTime_->addElement(4);
	synchronizedDwellTime_->addElement(5);
	connect(synchronizedDwellTime_, SIGNAL(connected(bool)), this, SLOT(synchronizedDwellTimeConnected(bool)));

	// Helper functions for setting the dwell time between regions.
	dwellTimeTrigger_ = new AMSinglePVControl("Dwell Time Trigger", "BL1607-B2-1:AddOns:dwellTime:trigger", this, 0.1);
	dwellTimeConfirmed_ = new AMSinglePVControl("Dwell Time Confirmed", "BL1607-B2-1:AddOns:dwellTime:confirmed", this, 0.1);

	// Setting up all of the configurations for the synchronized dwell time.
	// The scaler.
	CLSSynchronizedDwellTimeConfigurationInfo *temp = new CLSSynchronizedDwellTimeConfigurationInfo(this);
	temp->setName("Scaler");
	temp->setDwellTimePV("BL1607-B2-1:mcs:delay PP NMS");
	temp->setScale("1000");
	temp->setOffset("0");
	temp->setUnits("ms");
	temp->setModePV("BL1607-B2-1:mcs:continuous PP NMS");
	temp->setSingleShot("0");
	temp->setContinuous("1");
	temp->setTriggerPV("BL1607-B2-1:mcs:startScan NPP NMS");
	temp->setTrigger(CLSSynchronizedDwellTimeConfigurationInfo::Normal);
	temp->setPreTrigger(0.0);
	temp->setDwellHold(0.0);
	temp->setStatusPV("BL1607-B2-1:mcs:startScan CP NMS");
	temp->setWaitFor(CLSSynchronizedDwellTimeConfigurationInfo::Nothing);
	temp->setDelay(5);
	temp->setWaitPV("");
	temp->setWaitValue("");

	synchronizedDwellTimeConfigurations_.append(temp);

	// The single element vortex detector.
	temp = new CLSSynchronizedDwellTimeConfigurationInfo(this);
	temp->setName("1-El Vortex");
	temp->setDwellTimePV("IOC1607-004:mca1.PRTM PP NMS");
	temp->setScale("1");
	temp->setOffset("0");
	temp->setUnits("s");
	temp->setModePV("IOC1607-004:mca1Read.SCAN PP NMS");
	temp->setSingleShot("1");
	temp->setContinuous("0");
	temp->setTriggerPV("IOC1607-004:mca1EraseStart NPP NMS");
	temp->setTrigger(CLSSynchronizedDwellTimeConfigurationInfo::Normal);
	temp->setPreTrigger(0.0);
	temp->setDwellHold(0.0);
	temp->setStatusPV("IOC1607-004:mca1.ACQG CP NMS");
	temp->setWaitFor(CLSSynchronizedDwellTimeConfigurationInfo::Nothing);
	temp->setDelay(0);
	temp->setWaitPV("");
	temp->setWaitValue("");

	synchronizedDwellTimeConfigurations_.append(temp);

	// The Roper CCD detector.
	temp = new CLSSynchronizedDwellTimeConfigurationInfo(this);
	temp->setName("Roper CCD");
	temp->setDwellTimePV("IOC1607-003:det1.AcquireTime PP NMS");
	temp->setScale("1");
	temp->setOffset("0");
	temp->setUnits("s");
	temp->setModePV("");
	temp->setSingleShot("1");
	temp->setContinuous("0");
	temp->setTriggerPV("DIO1607-01:CCD:ExtSync NPP NMS");
	temp->setTrigger(CLSSynchronizedDwellTimeConfigurationInfo::Normal);
	temp->setPreTrigger(0.0);
	temp->setDwellHold(0.0);
	temp->setStatusPV("CCD1607-001:extTrig:status CP NMS");
	temp->setWaitFor(CLSSynchronizedDwellTimeConfigurationInfo::Nothing);
	temp->setDelay(0);
	temp->setWaitPV("");
	temp->setWaitValue("");

	synchronizedDwellTimeConfigurations_.append(temp);

	// The picoammeters.
	temp = new CLSSynchronizedDwellTimeConfigurationInfo(this);
	temp->setName("Picoammeters");
	temp->setDwellTimePV("A2607:integ_interval PP NMS");
	temp->setScale("1");
	temp->setOffset("0");
	temp->setUnits("s");
	temp->setModePV("A2607:configure");
	temp->setSingleShot("1");
	temp->setContinuous("2");
	temp->setTriggerPV("A2607:start_read NPP NMS");
	temp->setTrigger(CLSSynchronizedDwellTimeConfigurationInfo::Normal);
	temp->setPreTrigger(0.0);
	temp->setDwellHold(0.0);
	temp->setStatusPV("A2607:start_read CP NMS");
	temp->setWaitFor(CLSSynchronizedDwellTimeConfigurationInfo::Nothing);
	temp->setDelay(5);
	temp->setWaitPV("");
	temp->setWaitValue("");

	synchronizedDwellTimeConfigurations_.append(temp);

	// The four element vortex detector.
	temp = new CLSSynchronizedDwellTimeConfigurationInfo(this);
	temp->setName("4-El Vortex");
	temp->setDwellTimePV("dxp1607-B21-04:PresetReal PP NMS");
	temp->setScale("1");
	temp->setOffset("0");
	temp->setUnits("s");
	temp->setModePV("");
	temp->setSingleShot("1");
	temp->setContinuous("0");
	temp->setTriggerPV("dxp1607-B21-04:EraseStart NPP NMS");
	temp->setTrigger(CLSSynchronizedDwellTimeConfigurationInfo::Normal);
	temp->setPreTrigger(0.0);
	temp->setDwellHold(0.0);
	temp->setStatusPV("dxp1607-B21-04:Acquiring CP NMS");
	temp->setWaitFor(CLSSynchronizedDwellTimeConfigurationInfo::Nothing);
	temp->setDelay(0);
	temp->setWaitPV("");
	temp->setWaitValue("");

	synchronizedDwellTimeConfigurations_.append(temp);

	// The Mar CCD detector.
	temp = new CLSSynchronizedDwellTimeConfigurationInfo(this);
	temp->setName("Mar CCD");
	temp->setDwellTimePV("ccd1607-002:cam1:AcquireTime PP NMS");
	temp->setScale("1");
	temp->setOffset("0");
	temp->setUnits("s");
	temp->setModePV("");
	temp->setSingleShot("1");
	temp->setContinuous("0");
	temp->setTriggerPV("ccd1607-002:cam1:Acquire NPP NMS");
	temp->setTrigger(CLSSynchronizedDwellTimeConfigurationInfo::Normal);
	temp->setPreTrigger(0.0);
	temp->setDwellHold(0.0);
	temp->setStatusPV("ccd1607-002:cam1:Acquire CP NMS");
	temp->setWaitFor(CLSSynchronizedDwellTimeConfigurationInfo::Nothing);
	temp->setDelay(0);
	temp->setWaitPV("");
	temp->setWaitValue("");

	synchronizedDwellTimeConfigurations_.append(temp);

	// The Pilatus pixel array detector.
	temp = new CLSSynchronizedDwellTimeConfigurationInfo(this);
	temp->setName("Pilatus CCD");
	temp->setDwellTimePV("PAD1607-B21-05:cam1:AcquireTime PP NMS");
	temp->setScale("1");
	temp->setOffset("0");
	temp->setUnits("s");
	temp->setModePV("");
	temp->setSingleShot("1");
	temp->setContinuous("0");
	temp->setTriggerPV("PAD1607-B21-05:cam1:Acquire NPP NMS");
	temp->setTrigger(CLSSynchronizedDwellTimeConfigurationInfo::Normal);
	temp->setPreTrigger(0.0);
	temp->setDwellHold(0.0);
	temp->setStatusPV("PAD1607-B21-05:cam1:Acquire CP NMS");
	temp->setWaitFor(CLSSynchronizedDwellTimeConfigurationInfo::Nothing);
	temp->setDelay(0);
	temp->setWaitPV("");
	temp->setWaitValue("");

	synchronizedDwellTimeConfigurations_.append(temp);
}

void VESPERSBeamline::synchronizedDwellTimeConnected(bool connected)
{
	if (connected){

		if (synchronizedDwellTime()->elementAt(0)->name() != "Scaler")
			synchronizedDwellTime()->elementAt(0)->configure(*synchronizedDwellTimeConfigurationByName("Scaler"));

		if (synchronizedDwellTime()->elementAt(0)->name() != "1-El Vortex")
			synchronizedDwellTime()->elementAt(1)->configure(*synchronizedDwellTimeConfigurationByName("1-El Vortex"));

		if (synchronizedDwellTime()->elementAt(0)->name() != "Roper CCD")
			synchronizedDwellTime()->elementAt(2)->configure(*synchronizedDwellTimeConfigurationByName("Roper CCD"));

		if (synchronizedDwellTime()->elementAt(0)->name() != "Pilatus CCD")
			synchronizedDwellTime()->elementAt(3)->configure(*synchronizedDwellTimeConfigurationByName("Pilatus CCD"));

		if (synchronizedDwellTime()->elementAt(0)->name() != "4-El Vortex")
			synchronizedDwellTime()->elementAt(4)->configure(*synchronizedDwellTimeConfigurationByName("4-El Vortex"));

		if (synchronizedDwellTime()->elementAt(0)->name() != "Mar CCD")
			synchronizedDwellTime()->elementAt(5)->configure(*synchronizedDwellTimeConfigurationByName("Mar CCD"));
	}
}

CLSSynchronizedDwellTimeConfigurationInfo *VESPERSBeamline::synchronizedDwellTimeConfigurationByName(const QString &name) const
{
	for (int i = 0, size = synchronizedDwellTimeConfigurations_.size(); i < size; i++)
		if (synchronizedDwellTimeConfigurations_.at(i)->name() == name)
			return synchronizedDwellTimeConfigurations_.at(i);

	return 0;
}

void VESPERSBeamline::setupComponents()
{
	beamPositions_.insert(VESPERS::Pink, 0);
	beamPositions_.insert(VESPERS::TenPercent, -12.5);
	beamPositions_.insert(VESPERS::Si, -17.5);
	beamPositions_.insert(VESPERS::OnePointSixPercent, -22.5);

	beamSelectionMotor_ = new CLSMAXvMotor("MonoBeamSelectionMotor", "SMTR1607-1-B20-21", "Motor that controls which beam makes it down the pipe.", false, 1, 2.0, this);
	connect(beamSelectionMotor_, SIGNAL(movingChanged(bool)), this, SLOT(determineBeam()));
	connect(beamSelectionMotor_, SIGNAL(valueChanged(double)), this, SLOT(onBeamSelectionMotorConnected()));

	variableIntegrationTime_ = new CLSVariableIntegrationTime("BL1607-B2-1:VarStep", this);

	scaler_ = new CLSSIS3820Scaler("BL1607-B2-1:mcs", this);

	poeBeamStatus_ = new AMReadOnlyPVControl("POE Beam Status", "07B2:POE_BeamStatus", this);
	poeBeamStatusEnable_ = new AMSinglePVControl("POE Beam Status Enable", "07B2:EnablePOEStat", this, 0.1);
	connect(poeBeamStatus_, SIGNAL(valueChanged(double)), this, SLOT(onPOEStatusChanged()));
	connect(poeBeamStatusEnable_, SIGNAL(valueChanged(double)), this, SIGNAL(poeStatusEnableChanged()));
}

void VESPERSBeamline::onPOEStatusChanged()
{
	bool beamStatus = poeStatus();
	emit poeStatusChanged(beamStatus);

	if (poeStatusEnable() && !beamStatus)
		emit beamDumped();
}

void VESPERSBeamline::setPOEStatusEnable(bool enabled)
{
	poeBeamStatusEnable_->move(enabled ? 0 : 1);
}

void VESPERSBeamline::setupExposedControls()
{
	addExposedControl(pseudoSampleStageMotorGroupObject()->horizontalControl());
	addExposedControl(pseudoSampleStageMotorGroupObject()->verticalControl());
	addExposedControl(pseudoSampleStageMotorGroupObject()->normalControl());
	addExposedControl(realSampleStageMotorGroupObject()->horizontalControl());
	addExposedControl(realSampleStageMotorGroupObject()->verticalControl());
	addExposedControl(realSampleStageMotorGroupObject()->normalControl());
}

AMBeamlineActionItem *VESPERSBeamline::createBeamChangeAction(VESPERS::Beam beam)
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

	if (beam != VESPERS::Pink){

		changeBeamActionsList->appendStage(new QList<AMBeamlineActionItem*>());
		changeBeamActionsList->appendAction(2, mono()->createAllowScanningAction(true));
	}

	return changeBeamAction;
}

void VESPERSBeamline::determineBeam()
{
	VESPERS::Beam temp;

	if (beamSelectionMotor_->withinTolerance(beamPositions_.value(VESPERS::Pink)))
		temp = VESPERS::Pink;
	else if (beamSelectionMotor_->withinTolerance(beamPositions_.value(VESPERS::TenPercent)))
		temp = VESPERS::TenPercent;
	else if (beamSelectionMotor_->withinTolerance(beamPositions_.value(VESPERS::OnePointSixPercent)))
		temp = VESPERS::OnePointSixPercent;
	else if (beamSelectionMotor_->withinTolerance(beamPositions_.value(VESPERS::Si)))
		temp = VESPERS::Si;
	else
		temp = VESPERS::NoBeam;

	if (temp != beam_){

		beam_ = temp;
		emit currentBeamChanged(beam_);
	}
}

void VESPERSBeamline::pressureConnected(bool connected)
{
	if (connected) {

		for (int i = 0; i < pressureSet_->count(); i++)
			connect(qobject_cast<AMReadOnlyPVwStatusControl *>(pressureSet_->at(i)), SIGNAL(movingChanged(bool)), this, SLOT(pressureError()));

		pressureError();
	}
}

void VESPERSBeamline::valveConnected(bool connected)
{
	if (connected){

		connect(valveSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(valveError()));
		valveError();
	}
}

void VESPERSBeamline::ionPumpConnected(bool connected)
{
	if (connected){

		connect(ionPumpSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(ionPumpError()));
		ionPumpError();
	}
}

void VESPERSBeamline::temperatureConnected(bool connected)
{
	if (connected) {

		for (int i = 0; i < temperatureSet_->count(); i++)
			connect(qobject_cast<AMReadOnlyPVwStatusControl *>(temperatureSet_->at(i)), SIGNAL(movingChanged(bool)), this, SLOT(temperatureError()));

		temperatureError();
	}
}

void VESPERSBeamline::flowSwitchConnected(bool connected)
{
	if (connected){

		connect(flowSwitchSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(flowSwitchError()));
		flowSwitchError();
	}
}

void VESPERSBeamline::flowTransducerConnected(bool connected)
{
	if (connected) {

		for (int i = 0; i < flowTransducerSet_->count(); i++)
			connect(qobject_cast<AMReadOnlyPVwStatusControl *>(flowTransducerSet_->at(i)), SIGNAL(movingChanged(bool)), this, SLOT(flowTransducerError()));

		flowTransducerError();
	}
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
			error += tr("%1 (%2) %3 %4\n").arg(current->name()).arg(current->readPVName()).arg(current->value(), 0, 'e', 3).arg(current->units());
	}

	if (!error.isEmpty()){

		error.prepend("The following pressure readings are at a critical level:\n");
		AMErrorMon::error(this, VESPERSBEAMLINE_PRESSURE_TOO_HIGH, error);
	}

	emit pressureStatus(error.isEmpty());
}

void VESPERSBeamline::valveError()
{
	if (!valveSet_->isConnected())
		return;

	QString error("");
	CLSBiStateControl *current = 0;

	for (int i = 0; i < valveSet_->count(); i++){

		if (i == 0){

			AMReadOnlyPVwStatusControl *first = qobject_cast<AMReadOnlyPVwStatusControl *>(valveSet_->at(i));

			if (first->isMoving()) // Closed is 0.
				error += QString("%1 (%2)\n").arg(first->name()).arg(first->movingPVName());
		}

		else {

			current = qobject_cast<CLSBiStateControl *>(valveSet_->at(i));

			if (current->state() == 0) // Closed is 0.
				error += QString("%1 (%2)\n").arg(current->name()).arg(current->statePVName());
		}
	}

	if (!error.isEmpty()){

		error.prepend("The following valves are closed:\n");
		AMErrorMon::error(this, VESPERSBEAMLINE_VALVES_CLOSED, error);
	}

	emit valveStatus(error.isEmpty());
}

void VESPERSBeamline::ionPumpError()
{
	if (!ionPumpSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVControl *current = 0;

	for (int i = 0; i < ionPumpSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(ionPumpSet_->at(i));

		if (!current->value())
			error += tr("%1 (%2)\n").arg(current->name()).arg(current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following ion pumps are no longer operating correctly:\n");
		AMErrorMon::error(this, VESPERSBEAMLINE_ION_PUMP_TRIP, error);
	}

	emit ionPumpStatus(error.isEmpty());
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
			error += tr("%1 (%2) %3 %4\n").arg(current->name()).arg(current->readPVName()).arg(current->value(), 0, 'e', 3).arg(current->units());
	}

	if (!error.isEmpty()){

		error.prepend("The following temperature sensors are reading too high:\n");
		AMErrorMon::error(this, VESPERSBEAMLINE_TEMPERATURE_TOO_HIGH, error);
	}

	emit temperatureStatus(error.isEmpty());
}

void VESPERSBeamline::flowSwitchError()
{
	if (!flowSwitchSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVControl *current = 0;

	for (int i = 0; i < flowSwitchSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(flowSwitchSet_->at(i));

		if (!current->value())
			error += tr("%1 (%2)\n").arg(current->name()).arg(current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following flow switches have tripped:\n");
		AMErrorMon::error(this, VESPERSBEAMLINE_WATER_FLOW_SWITCH_TRIP, error);
	}

	emit flowSwitchStatus(error.isEmpty());
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
			error += tr("%1 (%2) %3 %4\n").arg(current->name()).arg(current->readPVName()).arg(current->value(), 0, 'e', 3).arg(current->units());
	}

	if (!error.isEmpty()){

		error.prepend("The following flow transducers are measuring too low:\n");
		AMErrorMon::error(this, VESPERSBEAMLINE_WATER_FLOW_TOO_LOW, error);
	}

	emit flowTransducerStatus(error.isEmpty());
}

void VESPERSBeamline::singleElVortexError(bool isConnected)
{
	if (vortexXRF1E()->wasConnected() && !isConnected)
		AMErrorMon::error(this, VESPERSBEAMLINE_SINGLE_ELEMENT_NOT_CONNECTED, "The single element vortex detector is no longer connected.");
}

void VESPERSBeamline::fourElVortexError(bool isConnected)
{
	if (vortexXRF4E()->wasConnected() && !isConnected)
		AMErrorMon::error(this, VESPERSBEAMLINE_FOUR_ELEMENT_NOT_CONNECTED, "The four element vortex detector is no longer connected.");
}

void VESPERSBeamline::sampleStageError()
{
	if (!sampleStageMotorSet()->isConnected())
		AMErrorMon::alert(this, VESPERSBEAMLINE_SAMPLE_STAGE_NOT_CONNECTED, "The sample stage is no longer connected.");
}

VESPERSBeamline::~VESPERSBeamline()
{

}

bool VESPERSBeamline::allValvesOpen() const
{
	for (int i = 0; i < valveSet_->count(); i++)
		if (valveSet_->at(i)->value() == 0 || valveSet_->at(i)->value() == 2)
			return false;

	return true;
}

void VESPERSBeamline::openValve(int index)
{
	if (index >= 0 && index < valveSet_->count()){

		CLSBiStateControl *control = qobject_cast<CLSBiStateControl *>(valveSet_->at(index));

		if (control && control->isClosed())
			control->open();
	}
}

void VESPERSBeamline::closeValve(int index)
{
	if (index >= 0 && index < valveSet_->count()){

		CLSBiStateControl *control = qobject_cast<CLSBiStateControl *>(valveSet_->at(index));

		if (control && control->isOpen())
			control->close();
	}
}

void VESPERSBeamline::openAllValves()
{
	valveIndex_ = valveSet_->count()-1;
	openAllValvesHelper();
}

void VESPERSBeamline::closeAllValves()
{
	valveIndex_ = 0;
	closeAllValvesHelper();
}

void VESPERSBeamline::openAllValvesHelper()
{
	openValve(valveIndex_--);

	if (valveIndex_ >= 0)
		QTimer::singleShot(150, this, SLOT(openAllValvesHelper()));
}

void VESPERSBeamline::closeAllValvesHelper()
{
	closeValve(valveIndex_++);

	if (valveIndex_ < valveSet_->count())
		QTimer::singleShot(150, this, SLOT(closeAllValvesHelper()));
}

bool VESPERSBeamline::openPhotonShutter1()
{
	if (ssh1_->value() == 1 || (ssh1_->value() == 0 && psh2_->value() == 0)){

		psh1_->move(1);
		return true;
	}

	return false;
}

bool VESPERSBeamline::closePhotonShutter1()
{
	if (psh1_->value() == 1){

		psh1_->move(0);
		return true;
	}

	return false;
}

bool VESPERSBeamline::openPhotonShutter2()
{
	if (ssh1_->value() == 1 || (ssh1_->value() == 0 && psh1_->value() == 0)){

		psh2_->move(1);
		return true;
	}

	return false;
}

bool VESPERSBeamline::closePhotonShutter2()
{
	if (psh2_->value() == 1){

		psh2_->move(0);
		return true;
	}

	return false;
}

bool VESPERSBeamline::openSafetyShutter1()
{
	if (ssh1_->value() == 0){

		ssh1_->move(1);
		return true;
	}

	return false;
}

bool VESPERSBeamline::closeSafetyShutter1()
{
	if ((psh1_->value() == 1 && psh2_->value() == 0) || (psh1_->value() == 0 && psh2_->value() == 1)){

		ssh1_->move(0);
		return true;
	}

	return false;
}

bool VESPERSBeamline::openSafetyShutter2()
{
	if (ssh2_->value() == 0){

		ssh2_->move(1);
		return true;
	}

	return false;
}

bool VESPERSBeamline::closeSafetyShutter2()
{
	if (ssh2_->value() == 1){

		ssh2_->move(0);
		return false;
	}

	return false;
}
