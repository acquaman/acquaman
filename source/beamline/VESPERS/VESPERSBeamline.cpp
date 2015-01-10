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

#include "VESPERSBeamline.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSR570.h"
#include "beamline/VESPERS/VESPERSMonochomatorControl.h"
#include "beamline/VESPERS/VESPERSCCDBasicDetectorEmulator.h"
#include "beamline/AM1DControlDetectorEmulator.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "beamline/AMScalerTimeControlDetector.h"

VESPERSBeamline::VESPERSBeamline()
	: AMBeamline("VESPERS Beamline")
{
	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupEndstation();
	setupDetectors();
	setupControlSets();
	setupMono();
	setupMotorGroup();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();
}

void VESPERSBeamline::setupDiagnostics()
{
	// The shutters.
	photonShutter1_ = new CLSBiStateControl("PSH", "First Photon Shutter", "PSH1408-B20-01:state", "PSH1408-B20-01:opr:open", "PSH1408-B20-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	photonShutter2_ = new CLSBiStateControl("Optic", "Second Photon Shutter", "PSH1408-B20-02:state", "PSH1408-B20-02:opr:open", "PSH1408-B20-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	safetyShutter1_ = new CLSBiStateControl("SSH", "First Safety Shutter", "SSH1408-B20-01:state", "SSH1408-B20-01:opr:open", "SSH1408-B20-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	safetyShutter2_ = new CLSBiStateControl("Exp.", "Second Safety Shutter", "SSH1607-1-B21-01:state", "SSH1607-1-B21-01:opr:open", "SSH1607-1-B21-01:opr:close", new AMControlStatusCheckerDefault(2), this);

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
//	sampleStageHorizontal_ = new AMPVwStatusControl("Horizontal Sample Stage", "BL1607-B2-1:AddOns:SampleStage:H:mm:fbk", "BL1607-B2-1:AddOns:SampleStage:H:mm", "BL1607-B2-1:AddOns:SampleStage:H:status", "TS1607-2-B21-02:HNV:stop.PROC", this, 0.005, 10.0);
//	sampleStageVertical_ = new AMPVwStatusControl("Vertical Sample Stage", "BL1607-B2-1:AddOns:SampleStage:V:mm:fbk", "BL1607-B2-1:AddOns:SampleStage:V:mm", "BL1607-B2-1:AddOns:SampleStage:V:status", "TS1607-2-B21-02:HNV:stop.PROC", this, 0.005, 10.0);
//	sampleStageNormal_ = new AMPVwStatusControl("Normal Sample Stage", "BL1607-B2-1:AddOns:SampleStage:N:mm:fbk", "BL1607-B2-1:AddOns:SampleStage:N:mm", "BL1607-B2-1:AddOns:SampleStage:N:status", "TS1607-2-B21-02:HNV:stop.PROC", this, 0.005, 10.0);
	sampleStageHorizontal_ = new AMPVwStatusControl("Horizontal Sample Stage", "BL1607-B2-1:AddOns:SampleStage:H:mm", "BL1607-B2-1:AddOns:SampleStage:H:mm", "BL1607-B2-1:AddOns:SampleStage:H:status", "TS1607-2-B21-02:HNV:stop.PROC", this, 0.005, 10.0);
	sampleStageVertical_ = new AMPVwStatusControl("Vertical Sample Stage", "BL1607-B2-1:AddOns:SampleStage:V:mm", "BL1607-B2-1:AddOns:SampleStage:V:mm", "BL1607-B2-1:AddOns:SampleStage:V:status", "TS1607-2-B21-02:HNV:stop.PROC", this, 0.005, 10.0);
	sampleStageNormal_ = new AMPVwStatusControl("Normal Sample Stage", "BL1607-B2-1:AddOns:SampleStage:N:mm", "BL1607-B2-1:AddOns:SampleStage:N:mm", "BL1607-B2-1:AddOns:SampleStage:N:status", "TS1607-2-B21-02:HNV:stop.PROC", this, 0.005, 10.0);

//	sampleStageX_ = new AMPVwStatusControl("X Motor Sample Stage", "BL1607-B2-1:AddOns:SampleStage:X:mm:fbk", "BL1607-B2-1:AddOns:SampleStage:X:mm", "BL1607-B2-1:AddOns:SampleStage:X:status", "TS1607-2-B21-02:XYZ:stop.PROC", this, 0.005, 10.0);
//	sampleStageY_ = new AMPVwStatusControl("Y Motor Sample Stage", "BL1607-B2-1:AddOns:SampleStage:Y:mm:fbk", "BL1607-B2-1:AddOns:SampleStage:Y:mm", "BL1607-B2-1:AddOns:SampleStage:Y:status", "TS1607-2-B21-02:XYZ:stop.PROC", this, 0.005, 10.0);
//	sampleStageZ_ = new AMPVwStatusControl("Z Motor Sample Stage", "BL1607-B2-1:AddOns:SampleStage:Z:mm:fbk", "BL1607-B2-1:AddOns:SampleStage:Z:mm", "BL1607-B2-1:AddOns:SampleStage:Z:status", "TS1607-2-B21-02:XYZ:stop.PROC", this, 0.005, 10.0);
	sampleStageX_ = new AMPVwStatusControl("X Motor Sample Stage", "BL1607-B2-1:AddOns:SampleStage:X:mm", "BL1607-B2-1:AddOns:SampleStage:X:mm", "BL1607-B2-1:AddOns:SampleStage:X:status", "TS1607-2-B21-02:XYZ:stop.PROC", this, 0.005, 10.0);
	sampleStageY_ = new AMPVwStatusControl("Y Motor Sample Stage", "BL1607-B2-1:AddOns:SampleStage:Y:mm", "BL1607-B2-1:AddOns:SampleStage:Y:mm", "BL1607-B2-1:AddOns:SampleStage:Y:status", "TS1607-2-B21-02:XYZ:stop.PROC", this, 0.005, 10.0);
	sampleStageZ_ = new AMPVwStatusControl("Z Motor Sample Stage", "BL1607-B2-1:AddOns:SampleStage:Z:mm", "BL1607-B2-1:AddOns:SampleStage:Z:mm", "BL1607-B2-1:AddOns:SampleStage:Z:status", "TS1607-2-B21-02:XYZ:stop.PROC", this, 0.005, 10.0);


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

//	wireStageHorizontal_ = new AMPVwStatusControl("Horizontal Wire Stage", "BL1607-B2-1:AddOns:WireStage:H:mm:fbk", "BL1607-B2-1:AddOns:WireStage:H:mm", "BL1607-B2-1:AddOns:WireStage:H:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.005, 10.0);
//	wireStageVertical_ = new AMPVwStatusControl("Vertical Wire Stage", "BL1607-B2-1:AddOns:WireStage:V:mm:fbk", "BL1607-B2-1:AddOns:WireStage:V:mm", "BL1607-B2-1:AddOns:WireStage:V:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.005, 10.0);
//	wireStageNormal_ = new AMPVwStatusControl("Normal Wire Stage", "BL1607-B2-1:AddOns:WireStage:N:mm:fbk", "BL1607-B2-1:AddOns:WireStage:N:mm", "BL1607-B2-1:AddOns:WireStage:N:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.005, 10.0);
	wireStageHorizontal_ = new AMPVwStatusControl("Horizontal Wire Stage", "BL1607-B2-1:AddOns:WireStage:H:mm", "BL1607-B2-1:AddOns:WireStage:H:mm", "BL1607-B2-1:AddOns:WireStage:H:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.005, 10.0);
	wireStageVertical_ = new AMPVwStatusControl("Vertical Wire Stage", "BL1607-B2-1:AddOns:WireStage:V:mm", "BL1607-B2-1:AddOns:WireStage:V:mm", "BL1607-B2-1:AddOns:WireStage:V:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.005, 10.0);
	wireStageNormal_ = new AMPVwStatusControl("Normal Wire Stage", "BL1607-B2-1:AddOns:WireStage:N:mm", "BL1607-B2-1:AddOns:WireStage:N:mm", "BL1607-B2-1:AddOns:WireStage:N:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 0.005, 10.0);

	// attocube
//	attoStageHorizontal_ = new AMPVwStatusControl("Horizontal Atto Stage", "BL1607-B2-1:AddOns:AttoStage:H:mm:fbk", "BL1607-B2-1:AddOns:AttoStage:H:mm", "BL1607-B2-1:AddOns:AttoStage:H:status", "TS1607-2-B21-07:HNV:stop.PROC", this, 0.005, 10.0);
//	attoStageVertical_ = new AMPVwStatusControl("Vertical Atto Stage", "BL1607-B2-1:AddOns:AttoStage:V:mm:fbk", "BL1607-B2-1:AddOns:AttoStage:V:mm", "BL1607-B2-1:AddOns:AttoStage:V:status", "TS1607-2-B21-07:HNV:stop.PROC", this, 0.005, 10.0);
//	attoStageNormal_ = new AMPVwStatusControl("Normal Atto Stage", "BL1607-B2-1:AddOns:AttoStage:N:mm:fbk", "BL1607-B2-1:AddOns:AttoStage:N:mm", "BL1607-B2-1:AddOns:AttoStage:N:status", "TS1607-2-B21-07:HNV:stop.PROC", this, 0.005, 10.0);
	attoStageHorizontal_ = new AMPVwStatusControl("Horizontal Atto Stage", "BL1607-B2-1:AddOns:AttoStage:H:mm", "BL1607-B2-1:AddOns:AttoStage:H:mm", "BL1607-B2-1:AddOns:AttoStage:H:status", "TS1607-2-B21-07:HNV:stop.PROC", this, 0.005, 10.0);
	attoStageVertical_ = new AMPVwStatusControl("Vertical Atto Stage", "BL1607-B2-1:AddOns:AttoStage:V:mm", "BL1607-B2-1:AddOns:AttoStage:V:mm", "BL1607-B2-1:AddOns:AttoStage:V:status", "TS1607-2-B21-07:HNV:stop.PROC", this, 0.005, 10.0);
	attoStageNormal_ = new AMPVwStatusControl("Normal Atto Stage", "BL1607-B2-1:AddOns:AttoStage:N:mm", "BL1607-B2-1:AddOns:AttoStage:N:mm", "BL1607-B2-1:AddOns:AttoStage:N:status", "TS1607-2-B21-07:HNV:stop.PROC", this, 0.005, 10.0);

//	attoStageX_ = new CLSPseudoMotorControl("Atto X Stage", "BL1607-B2-1:AddOns:AttoStage:X:mm:fbk", "BL1607-B2-1:AddOns:AttoStage:X:mm", "BL1607-B2-1:AddOns:AttoStage:X:status", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.01, 10.0);
//	attoStageZ_ = new CLSPseudoMotorControl("Atto Z Stage", "BL1607-B2-1:AddOns:AttoStage:Z:mm:fbk", "BL1607-B2-1:AddOns:AttoStage:Z:mm", "BL1607-B2-1:AddOns:AttoStage:Z:status", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.01, 10.0);
//	attoStageY_ = new CLSPseudoMotorControl("Atto Y Stage", "BL1607-B2-1:AddOns:AttoStage:Y:mm:fbk", "BL1607-B2-1:AddOns:AttoStage:Y:mm", "BL1607-B2-1:AddOns:AttoStage:Y:status", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.01, 10.0);
//	attoStageX_ = new AMPVwStatusControl("Atto X Stage", "BL1607-B2-1:AddOns:AttoStage:X:mm", "BL1607-B2-1:AddOns:AttoStage:X:mm", "BL1607-B2-1:AddOns:AttoStage:X:status", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.005, 10.0);
//	attoStageZ_ = new AMPVwStatusControl("Atto Z Stage", "BL1607-B2-1:AddOns:AttoStage:Z:mm", "BL1607-B2-1:AddOns:AttoStage:Z:mm", "BL1607-B2-1:AddOns:AttoStage:Z:status", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.005, 10.0);
//	attoStageY_ = new AMPVwStatusControl("Atto Y Stage", "BL1607-B2-1:AddOns:AttoStage:Y:mm", "BL1607-B2-1:AddOns:AttoStage:Y:mm", "BL1607-B2-1:AddOns:AttoStage:Y:status", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.005, 10.0);
	attoStageX_ = new AMPVControl("Atto X Stage", "BL1607-B2-1:AddOns:AttoStage:X:mm", "BL1607-B2-1:AddOns:AttoStage:X:mm", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.002, 30.0);
	attoStageZ_ = new AMPVControl("Atto Z Stage", "BL1607-B2-1:AddOns:AttoStage:Z:mm", "BL1607-B2-1:AddOns:AttoStage:Z:mm", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.002, 30.0);
	attoStageY_ = new AMPVControl("Atto Y Stage", "BL1607-B2-1:AddOns:AttoStage:Y:mm", "BL1607-B2-1:AddOns:AttoStage:Y:mm", "TS1607-2-B21-07:XYZ:stop.PROC", this, 0.002, 30.0);

//	attoStageRz_ = new AMPVwStatusControl("Atto Phi Stage (Rz)", "SVM1607-2-B21-09:deg:sp", "SVM1607-2-B21-09:deg", "SVM1607-2-B21-09:status", "SVM1607-2-B21-09:stop.PROC", this, 0.01, 10.0);
//	attoStageRy_ = new AMPVwStatusControl("Atto Theta Stage (Ry)", "SVM1607-2-B21-07:deg:sp", "SVM1607-2-B21-07:deg", "SVM1607-2-B21-07:status", "SVM1607-2-B21-07:stop.PROC", this, 0.01, 10.0);
//	attoStageRx_ = new AMPVwStatusControl("Atto Psi Stage (Rx)", "SVM1607-2-B21-08:deg:sp", "SVM1607-2-B21-08:deg", "SVM1607-2-B21-08:status", "SVM1607-2-B21-08:stop.PROC", this, 0.01, 10.0);
//	attoStageRz_ = new AMPVwStatusControl("Atto Phi Stage (Rz)", "BL1607-B2-1:AddOns:AttoStage:Rz:deg:fbk", "BL1607-B2-1:AddOns:AttoStage:Rz:deg", "BL1607-B2-1:AddOns:AttoStage:Rz:status", "SVM1607-2-B21-09:stop.PROC", this, 0.01, 10.0);
//	attoStageRy_ = new AMPVwStatusControl("Atto Theta Stage (Ry)", "BL1607-B2-1:AddOns:AttoStage:Ry:deg:fbk", "BL1607-B2-1:AddOns:AttoStage:Ry:deg", "BL1607-B2-1:AddOns:AttoStage:Ry:status", "SVM1607-2-B21-07:stop.PROC", this, 0.01, 10.0);
//	attoStageRx_ = new AMPVwStatusControl("Atto Psi Stage (Rx)", "BL1607-B2-1:AddOns:AttoStage:Rx:deg:fbk", "BL1607-B2-1:AddOns:AttoStage:Rx:deg", "BL1607-B2-1:AddOns:AttoStage:Rx:status", "SVM1607-2-B21-08:stop.PROC", this, 0.01, 10.0);
	attoStageRz_ = new AMPVControl("Atto Phi Stage (Rz)", "BL1607-B2-1:AddOns:AttoStage:Rz:deg:fbk", "BL1607-B2-1:AddOns:AttoStage:Rz:deg", "SVM1607-2-B21-09:stop.PROC", this, 0.001, 30.0);
	attoStageRy_ = new AMPVControl("Atto Theta Stage (Ry)", "BL1607-B2-1:AddOns:AttoStage:Ry:deg:fbk", "BL1607-B2-1:AddOns:AttoStage:Ry:deg", "SVM1607-2-B21-07:stop.PROC", this, 0.001, 30.0);
	attoStageRx_ = new AMPVControl("Atto Psi Stage (Rx)", "BL1607-B2-1:AddOns:AttoStage:Rx:deg:fbk", "BL1607-B2-1:AddOns:AttoStage:Rx:deg", "SVM1607-2-B21-08:stop.PROC", this, 0.001, 30.0);

	bigBeamX_ = new AMPVwStatusControl("Big Beam X", "SMTR1607-2-B21-36:mm:sp", "SMTR1607-2-B21-36:mm", "SMTR1607-2-B21-36:status", "SMTR1607-2-B21-36:stop.PROC", this, 0.01, 10.0);
	bigBeamZ_ = new AMPVwStatusControl("Big Beam Z", "SMTR1607-2-B21-37:mm:sp", "SMTR1607-2-B21-37:mm", "SMTR1607-2-B21-37:status", "SMTR1607-2-B21-37:stop.PROC", this, 0.01, 10.0);

	// Reset signals.
	pseudoSampleStageResetControl_ = new AMSinglePVControl("Pseudo Sample Stage Reset Control", "TS1607-2-B21-02:HNV:loadOffsets.PROC", this, 0.1);
	realSampleStageResetControl_ = new AMSinglePVControl("Real Sample Stage Reset Control", "TS1607-2-B21-02:XYZ:loadOffsets.PROC", this, 0.1);
	pseudoAttoStageResetControl_ = new AMSinglePVControl("Pseudo Atto Stage Reset Control", "TS1607-2-B21-07:HNV:loadOffsets.PROC", this, 0.1);
	realAttoStageResetControl_ = new AMSinglePVControl("Real Atto Stage Reset Control", "TS1607-2-B21-07:XYZ:loadOffsets.PROC", this, 0.1);
	pseudoWireStageResetControl_ = new AMSinglePVControl("Pseudo Wire Stage Reset Control", "TS1607-2-B21-01:HNV:loadOffsets.PROC", this, 0.1);
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
										 pseudoWireStageResetControl_,
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
	motorObject = new AMMotorGroupObject("Big Beam - X, Z",
										QStringList() << "X" << "Z",
										QStringList() << "mm" << "mm",
										QList<AMControl *>() << bigBeamX_ << bigBeamZ_,
										QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical,
										QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational,
										this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);
}

QString VESPERSBeamline::motorGroupName(VESPERS::Motors motor) const
{
	if (motor.testFlag(VESPERS::H) || motor.testFlag(VESPERS::V))
		return "Sample Stage - H, V, N";

	else if (motor.testFlag(VESPERS::X) || motor.testFlag(VESPERS::Z))
		return "Sample Stage - X, Z, Y";

	else if (motor.testFlag(VESPERS::AttoH) || motor.testFlag(VESPERS::AttoV))
		return "Attocube Stage - H, V, N";

	else if (motor.testFlag(VESPERS::AttoX) || motor.testFlag(VESPERS::AttoZ))
		return "Attocube Stage - X, Z, Y";

	else if (motor.testFlag(VESPERS::AttoRx))
		return "Attocube Stage - Rx";

	else if (motor.testFlag(VESPERS::AttoRy))
		return "Attocube Stage - Ry";

	else if (motor.testFlag(VESPERS::AttoRz))
		return "Attocube Stage - Rz";

	else if (motor.testFlag(VESPERS::BigBeamX) || motor.testFlag(VESPERS::BigBeamZ))
		return "Big Beam -X, Z";

	else if (motor.testFlag(VESPERS::WireH) || motor.testFlag(VESPERS::WireV) || motor.testFlag(VESPERS::WireN))
		return "Wire Stage - H, V, N";

	return "Sample Stage - H, V, N";
}

void VESPERSBeamline::setupEndstation()
{
	endstation_ = new VESPERSEndstation(this);
}

void VESPERSBeamline::setupDetectors()
{
	splitIonChamber_ = new CLSBasicCompositeScalerChannelDetector("SplitIonChamber", "Split Ion Chamber", scaler_, 5, 6, this);
	preKBIonChamber_ = new CLSBasicScalerChannelDetector("PreKBIonChamber", "Pre KB Ion Chamber", scaler_, 7, this);
	miniIonChamber_ = new CLSBasicScalerChannelDetector("MiniIonChamber", "Mini Ion Chamber", scaler_, 8, this);
	postIonChamber_  = new CLSBasicScalerChannelDetector("PostIonChamber", "Post Ion Chamber", scaler_, 9, this);

	singleElementVortexDetector_ = new VESPERSSingleElementVortexDetector("SingleElementVortex", "Single Element Vortex", this);
	fourElementVortexDetector_ = new VESPERSFourElementVortexDetector("FourElementVortex", "Four Element Vortex", this);

	roperCCD_ = new VESPERSRoperCCDDetector("RoperCCD", "Roper CCD Detector", this);
	marCCD_ = new VESPERSMarCCDDetector("MarCCD", "Mar 165 CCD Camera", this);
	pilatusAreaDetector_ = new VESPERSPilatusCCDDetector("PilatusPixelArrayDetector", "Pilatus Pixel Array Detector", this);

	addSynchronizedXRFDetector(singleElementVortexDetector_);
	addSynchronizedXRFDetector(fourElementVortexDetector_);
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
	mono_ = new VESPERSMonochromator(this);
	masterDwellTime_ = new AMReadOnlyPVControl("Master Dwell Time", "BL1607-B2-1:mcs:delay", this);
	intermediateSlits_ = new VESPERSIntermediateSlits(this);
	energySetpointControl_ = new AMReadOnlyPVControl("EnergySetpoint", "07B2_Mono_SineB_Ea", this);
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

	scaler_ = new CLSSIS3820Scaler("BL1607-B2-1:mcs", this);
	scaler_->channelAt(5)->setCustomChannelName("Split A");

	CLSSR570 *tempSR570 = new CLSSR570("Split bottom", "AMP1607-202", this);
	scaler_->channelAt(5)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(5)->setVoltagRange(AMRange(1.0, 4.5));
	scaler_->channelAt(6)->setCustomChannelName("Split B");
	tempSR570 = new CLSSR570("Split top", "AMP1607-203", this);
	scaler_->channelAt(6)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(6)->setVoltagRange(AMRange(1.0, 4.5));
	scaler_->channelAt(7)->setCustomChannelName("Pre-KB");
	tempSR570 = new CLSSR570("Pre-KB", "AMP1607-204", this);
	scaler_->channelAt(7)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(7)->setVoltagRange(AMRange(1.0, 4.5));
	scaler_->channelAt(8)->setCustomChannelName("Mini");
	tempSR570 = new CLSSR570("Mini", "AMP1607-205", this);
	scaler_->channelAt(8)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(8)->setVoltagRange(AMRange(1.0, 4.5));
	scaler_->channelAt(9)->setCustomChannelName("Post");
	tempSR570 = new CLSSR570("Post", "AMP1607-206", this);
	scaler_->channelAt(9)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(9)->setVoltagRange(AMRange(1.0, 4.5));

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

void VESPERSBeamline::setupControlsAsDetectors()
{
	energySetpointDetector_ = new AMBasicControlDetectorEmulator("EnergySetpoint", "Energy Setpoint", energySetpointControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	energySetpointDetector_->setHiddenFromUsers(true);
	energySetpointDetector_->setIsVisible(false);
	energyFeedbackDetector_ = new AMBasicControlDetectorEmulator("EnergyFeedback", "Energy Feedback", mono_->EaControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	energyFeedbackDetector_->setHiddenFromUsers(true);
	energyFeedbackDetector_->setIsVisible(false);
	masterDwellTimeDetector_ = new AMScalerTimeControlDetector("MasterDwellTime", "Master Dwell Time", masterDwellTime_, this);
	masterDwellTimeDetector_->setHiddenFromUsers(true);
	masterDwellTimeDetector_->setIsVisible(false);
	ringCurrentDetector_ = new AMBasicControlDetectorEmulator("RingCurrent", "Ring Current", CLSStorageRing::sr1()->ringCurrentControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	ringCurrentDetector_->setHiddenFromUsers(true);
	ringCurrentDetector_->setIsVisible(false);
	roperCCDFileNumberDetector_ = new VESPERSCCDBasicDetectorEmulator(roperCCD_, "RoperFileNumber", "Roper File Number", roperCCD_->ccdFileNumberControl(), roperCCD_->statusControl(), 1, 0, AMDetectorDefinitions::RequestRead, this);
	roperCCDFileNumberDetector_->setHiddenFromUsers(true);
	roperCCDFileNumberDetector_->setIsVisible(false);
	marCCDFileNumberDetector_ = new VESPERSCCDBasicDetectorEmulator(marCCD_, "MarFileNumber", "Mar File Number", marCCD_->ccdFileNumberControl(), marCCD_->statusControl(), 1, 0, AMDetectorDefinitions::RequestRead, this);
	marCCDFileNumberDetector_->setHiddenFromUsers(true);
	marCCDFileNumberDetector_->setIsVisible(false);
	pilatusCCDFileNumberDetector_ = new VESPERSCCDBasicDetectorEmulator(pilatusAreaDetector_, "PilatusFileNumber", "Pilatus File Number", pilatusAreaDetector_->ccdFileNumberControl(), pilatusAreaDetector_->statusControl(), 1, 0, AMDetectorDefinitions::RequestRead, this);
	pilatusCCDFileNumberDetector_->setHiddenFromUsers(true);
	pilatusCCDFileNumberDetector_->setIsVisible(false);

	// All the vortex controls and turning them into detectors.  Temporary solution.
	singleElementVortexDeadTimeControl_ = new AMReadOnlyPVControl("Single Element Vortex Dead Time", "IOC1607-004:mca1.DTIM", this);
	singleElementVortexRealTimeControl_ = new AMReadOnlyPVControl("Single Element Vortex Real Time", "IOC1607-004:mca1.ERTM", this);
	singleElementVortexLiveTimeControl_ = new AMReadOnlyPVControl("Single Element Vortex Live Time", "IOC1607-004:mca1.ELTM", this);
	singleElementVortexFastPeaksControl_ = new AMReadOnlyPVControl("Single Element Vortex Fast Peaks", "IOC1607-004:dxp1.FAST_PEAKS", this);
	singleElementVortexSlowPeaksControl_ = new AMReadOnlyPVControl("Single Element Vortex Slow Peaks", "IOC1607-004:dxp1.SLOW_PEAKS", this);
	singleElementVortexRawSpectrumControl_ = new AMReadOnlyPVControl("Single Element Vortex Raw Spectrum", "IOC1607-004:mca1", this);

	fourElementVortexDeadTime1Control_ = new AMReadOnlyPVControl("Four Element Vortex Dead Time 1", "dxp1607-B21-04:dxp1:NetDTP", this);
	fourElementVortexDeadTime2Control_ = new AMReadOnlyPVControl("Four Element Vortex Dead Time 2", "dxp1607-B21-04:dxp2:NetDTP", this);
	fourElementVortexDeadTime3Control_ = new AMReadOnlyPVControl("Four Element Vortex Dead Time 3", "dxp1607-B21-04:dxp3:NetDTP", this);
	fourElementVortexDeadTime4Control_ = new AMReadOnlyPVControl("Four Element Vortex Dead Time 4", "dxp1607-B21-04:dxp4:NetDTP", this);
	fourElementVortexRealTime1Control_ = new AMReadOnlyPVControl("Four Element Vortex Real Time 1", "dxp1607-B21-04:dxp1:ElapsedRealTime", this);
	fourElementVortexRealTime2Control_ = new AMReadOnlyPVControl("Four Element Vortex Real Time 2", "dxp1607-B21-04:dxp2:ElapsedRealTime", this);
	fourElementVortexRealTime3Control_ = new AMReadOnlyPVControl("Four Element Vortex Real Time 3", "dxp1607-B21-04:dxp3:ElapsedRealTime", this);
	fourElementVortexRealTime4Control_ = new AMReadOnlyPVControl("Four Element Vortex Real Time 4", "dxp1607-B21-04:dxp4:ElapsedRealTime", this);
	fourElementVortexLiveTime1Control_ = new AMReadOnlyPVControl("Four Element Vortex Live Time 1", "dxp1607-B21-04:dxp1:ElapsedTriggerLiveTime", this);
	fourElementVortexLiveTime2Control_ = new AMReadOnlyPVControl("Four Element Vortex Live Time 2", "dxp1607-B21-04:dxp2:ElapsedTriggerLiveTime", this);
	fourElementVortexLiveTime3Control_ = new AMReadOnlyPVControl("Four Element Vortex Live Time 3", "dxp1607-B21-04:dxp3:ElapsedTriggerLiveTime", this);
	fourElementVortexLiveTime4Control_ = new AMReadOnlyPVControl("Four Element Vortex Live Time 4", "dxp1607-B21-04:dxp4:ElapsedTriggerLiveTime", this);
	fourElementVortexFastPeaks1Control_ = new AMReadOnlyPVControl("Four Element Vortex Fast Peaks 1", "dxp1607-B21-04:dxp1:Triggers", this);
	fourElementVortexFastPeaks2Control_ = new AMReadOnlyPVControl("Four Element Vortex Fast Peaks 2", "dxp1607-B21-04:dxp2:Triggers", this);
	fourElementVortexFastPeaks3Control_ = new AMReadOnlyPVControl("Four Element Vortex Fast Peaks 3", "dxp1607-B21-04:dxp3:Triggers", this);
	fourElementVortexFastPeaks4Control_ = new AMReadOnlyPVControl("Four Element Vortex Fast Peaks 4", "dxp1607-B21-04:dxp4:Triggers", this);
	fourElementVortexSlowPeaks1Control_ = new AMReadOnlyPVControl("Four Element Vortex Slow Peaks 1", "dxp1607-B21-04:dxp1:Events", this);
	fourElementVortexSlowPeaks2Control_ = new AMReadOnlyPVControl("Four Element Vortex Slow Peaks 2", "dxp1607-B21-04:dxp2:Events", this);
	fourElementVortexSlowPeaks3Control_ = new AMReadOnlyPVControl("Four Element Vortex Slow Peaks 3", "dxp1607-B21-04:dxp3:Events", this);
	fourElementVortexSlowPeaks4Control_ = new AMReadOnlyPVControl("Four Element Vortex Slow Peaks 4", "dxp1607-B21-04:dxp4:Events", this);
	fourElementVortexRawSpectrumControl1_ = new AMReadOnlyPVControl("Four Element Vortex Raw Spectrum 1", "dxp1607-B21-04:mca1", this);
	fourElementVortexRawSpectrumControl2_ = new AMReadOnlyPVControl("Four Element Vortex Raw Spectrum 2", "dxp1607-B21-04:mca2", this);
	fourElementVortexRawSpectrumControl3_ = new AMReadOnlyPVControl("Four Element Vortex Raw Spectrum 3", "dxp1607-B21-04:mca3", this);
	fourElementVortexRawSpectrumControl4_ = new AMReadOnlyPVControl("Four Element Vortex Raw Spectrum 4", "dxp1607-B21-04:mca4", this);

	singleElementVortexDeadTime_ = new AMBasicControlDetectorEmulator("SingleElementVortexDeadTime", "Single Element Vortex Dead Time", singleElementVortexDeadTimeControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	singleElementVortexDeadTime_->setHiddenFromUsers(true);
	singleElementVortexDeadTime_->setIsVisible(false);
	singleElementVortexRealTime_ = new AMBasicControlDetectorEmulator("SingleElementVortexRealTime", "Single Element Vortex Real Time", singleElementVortexRealTimeControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	singleElementVortexRealTime_->setHiddenFromUsers(true);
	singleElementVortexRealTime_->setIsVisible(false);
	singleElementVortexLiveTime_ = new AMBasicControlDetectorEmulator("SingleElementVortexLiveTime", "Single Element Vortex Live Time", singleElementVortexLiveTimeControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	singleElementVortexLiveTime_->setHiddenFromUsers(true);
	singleElementVortexLiveTime_->setIsVisible(false);
	singleElementVortexFastPeaks_ = new AMBasicControlDetectorEmulator("SingleElementVortexFastPeaks", "Single Element Vortex Fast Peaks", singleElementVortexFastPeaksControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	singleElementVortexFastPeaks_->setHiddenFromUsers(true);
	singleElementVortexFastPeaks_->setIsVisible(false);
	singleElementVortexSlowPeaks_ = new AMBasicControlDetectorEmulator("SingleElementVortexSlowPeaks", "Single Element Vortex Slow Peaks", singleElementVortexSlowPeaksControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	singleElementVortexSlowPeaks_->setHiddenFromUsers(true);
	singleElementVortexSlowPeaks_->setIsVisible(false);
	singleElementVortexRawSpectrum_ = new AM1DControlDetectorEmulator("SingleElementVortexRawSpectrum", "Single Element Vortex Raw Spectrum", 2048, singleElementVortexRawSpectrumControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	singleElementVortexRawSpectrum_->setHiddenFromUsers(true);
	singleElementVortexRawSpectrum_->setIsVisible(false);

	fourElementVortexDeadTime1_ = new AMBasicControlDetectorEmulator("FourElementVortexDeadTime1", "Four Element Vortex Dead Time 1", fourElementVortexDeadTime1Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexDeadTime1_->setHiddenFromUsers(true);
	fourElementVortexDeadTime1_->setIsVisible(false);
	fourElementVortexDeadTime2_ = new AMBasicControlDetectorEmulator("FourElementVortexDeadTime2", "Four Element Vortex Dead Time 2", fourElementVortexDeadTime2Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexDeadTime2_->setHiddenFromUsers(true);
	fourElementVortexDeadTime2_->setIsVisible(false);
	fourElementVortexDeadTime3_ = new AMBasicControlDetectorEmulator("FourElementVortexDeadTime3", "Four Element Vortex Dead Time 3", fourElementVortexDeadTime3Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexDeadTime3_->setHiddenFromUsers(true);
	fourElementVortexDeadTime3_->setIsVisible(false);
	fourElementVortexDeadTime4_ = new AMBasicControlDetectorEmulator("FourElementVortexDeadTime4", "Four Element Vortex Dead Time 4", fourElementVortexDeadTime4Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexDeadTime4_->setHiddenFromUsers(true);
	fourElementVortexDeadTime4_->setIsVisible(false);
	fourElementVortexRealTime1_ = new AMBasicControlDetectorEmulator("FourElementVortexRealTime1", "Four Element Vortex Real Time 1", fourElementVortexRealTime1Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexRealTime1_->setHiddenFromUsers(true);
	fourElementVortexRealTime1_->setIsVisible(false);
	fourElementVortexRealTime2_ = new AMBasicControlDetectorEmulator("FourElementVortexRealTime2", "Four Element Vortex Real Time 2", fourElementVortexRealTime2Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexRealTime2_->setHiddenFromUsers(true);
	fourElementVortexRealTime2_->setIsVisible(false);
	fourElementVortexRealTime3_ = new AMBasicControlDetectorEmulator("FourElementVortexRealTime3", "Four Element Vortex Real Time 3", fourElementVortexRealTime3Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexRealTime3_->setHiddenFromUsers(true);
	fourElementVortexRealTime3_->setIsVisible(false);
	fourElementVortexRealTime4_ = new AMBasicControlDetectorEmulator("FourElementVortexRealTime4", "Four Element Vortex Real Time 4", fourElementVortexRealTime4Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexRealTime4_->setHiddenFromUsers(true);
	fourElementVortexRealTime4_->setIsVisible(false);
	fourElementVortexLiveTime1_ = new AMBasicControlDetectorEmulator("FourElementVortexLiveTime1", "Four Element Vortex Live Time 1", fourElementVortexLiveTime1Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexLiveTime1_->setHiddenFromUsers(true);
	fourElementVortexLiveTime1_->setIsVisible(false);
	fourElementVortexLiveTime2_ = new AMBasicControlDetectorEmulator("FourElementVortexLiveTime2", "Four Element Vortex Live Time 2", fourElementVortexLiveTime2Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexLiveTime2_->setHiddenFromUsers(true);
	fourElementVortexLiveTime2_->setIsVisible(false);
	fourElementVortexLiveTime3_ = new AMBasicControlDetectorEmulator("FourElementVortexLiveTime3", "Four Element Vortex Live Time 3", fourElementVortexLiveTime3Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexLiveTime3_->setHiddenFromUsers(true);
	fourElementVortexLiveTime3_->setIsVisible(false);
	fourElementVortexLiveTime4_ = new AMBasicControlDetectorEmulator("FourElementVortexLiveTime4", "Four Element Vortex Live Time 4", fourElementVortexLiveTime4Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexLiveTime4_->setHiddenFromUsers(true);
	fourElementVortexLiveTime4_->setIsVisible(false);
	fourElementVortexFastPeaks1_ = new AMBasicControlDetectorEmulator("FourElementVortexFastPeaks1", "Four Element Vortex Fast Peaks 1", fourElementVortexFastPeaks1Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexFastPeaks1_->setHiddenFromUsers(true);
	fourElementVortexFastPeaks1_->setIsVisible(false);
	fourElementVortexFastPeaks2_ = new AMBasicControlDetectorEmulator("FourElementVortexFastPeaks2", "Four Element Vortex Fast Peaks 2", fourElementVortexFastPeaks2Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexFastPeaks2_->setHiddenFromUsers(true);
	fourElementVortexFastPeaks2_->setIsVisible(false);
	fourElementVortexFastPeaks3_ = new AMBasicControlDetectorEmulator("FourElementVortexFastPeaks3", "Four Element Vortex Fast Peaks 3", fourElementVortexFastPeaks3Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexFastPeaks3_->setHiddenFromUsers(true);
	fourElementVortexFastPeaks3_->setIsVisible(false);
	fourElementVortexFastPeaks4_ = new AMBasicControlDetectorEmulator("FourElementVortexFastPeaks4", "Four Element Vortex Fast Peaks 4", fourElementVortexFastPeaks4Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexFastPeaks4_->setHiddenFromUsers(true);
	fourElementVortexFastPeaks4_->setIsVisible(false);
	fourElementVortexSlowPeaks1_ = new AMBasicControlDetectorEmulator("FourElementVortexSlowPeaks1", "Four Element Vortex Slow Peaks 1", fourElementVortexSlowPeaks1Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexSlowPeaks1_->setHiddenFromUsers(true);
	fourElementVortexSlowPeaks1_->setIsVisible(false);
	fourElementVortexSlowPeaks2_ = new AMBasicControlDetectorEmulator("FourElementVortexSlowPeaks2", "Four Element Vortex Slow Peaks 2", fourElementVortexSlowPeaks1Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexSlowPeaks2_->setHiddenFromUsers(true);
	fourElementVortexSlowPeaks2_->setIsVisible(false);
	fourElementVortexSlowPeaks3_ = new AMBasicControlDetectorEmulator("FourElementVortexSlowPeaks3", "Four Element Vortex Slow Peaks 3", fourElementVortexSlowPeaks1Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexSlowPeaks3_->setHiddenFromUsers(true);
	fourElementVortexSlowPeaks3_->setIsVisible(false);
	fourElementVortexSlowPeaks4_ = new AMBasicControlDetectorEmulator("FourElementVortexSlowPeaks4", "Four Element Vortex Slow Peaks 4", fourElementVortexSlowPeaks1Control_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexSlowPeaks4_->setHiddenFromUsers(true);
	fourElementVortexSlowPeaks4_->setIsVisible(false);
	fourElementVortexRawSpectrum1_ = new AM1DControlDetectorEmulator("FourElementVortexRawSpectrum1", "Four Element Vortex Raw Spectrum 1", 2048, fourElementVortexRawSpectrumControl1_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexRawSpectrum1_->setHiddenFromUsers(true);
	fourElementVortexRawSpectrum1_->setIsVisible(false);
	fourElementVortexRawSpectrum2_ = new AM1DControlDetectorEmulator("FourElementVortexRawSpectrum2", "Four Element Vortex Raw Spectrum 2", 2048, fourElementVortexRawSpectrumControl2_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexRawSpectrum2_->setHiddenFromUsers(true);
	fourElementVortexRawSpectrum2_->setIsVisible(false);
	fourElementVortexRawSpectrum3_ = new AM1DControlDetectorEmulator("FourElementVortexRawSpectrum3", "Four Element Vortex Raw Spectrum 3", 2048, fourElementVortexRawSpectrumControl3_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexRawSpectrum3_->setHiddenFromUsers(true);
	fourElementVortexRawSpectrum3_->setIsVisible(false);
	fourElementVortexRawSpectrum4_ = new AM1DControlDetectorEmulator("FourElementVortexRawSpectrum4", "Four Element Vortex Raw Spectrum 4", 2048, fourElementVortexRawSpectrumControl4_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	fourElementVortexRawSpectrum4_->setHiddenFromUsers(true);
	fourElementVortexRawSpectrum4_->setIsVisible(false);

	// Motors
	sampleStageHorizontalFeedbackControl_ = new AMReadOnlyPVControl("SampleHFeedback", "BL1607-B2-1:AddOns:SampleStage:H:mm:fbk", this);
	sampleStageVerticalFeedbackControl_ = new AMReadOnlyPVControl("SampleVFeedback", "BL1607-B2-1:AddOns:SampleStage:V:mm:fbk", this);
	sampleStageNormalFeedbackControl_ = new AMReadOnlyPVControl("SampleNFeedback", "BL1607-B2-1:AddOns:SampleStage:N:mm:fbk", this);
	wireStageHorizontalFeedbackControl_ = new AMReadOnlyPVControl("WireHFeedback", "BL1607-B2-1:AddOns:WireStage:H:mm:fbk", this);
	wireStageVerticalFeedbackControl_ = new AMReadOnlyPVControl("WireVFeedback", "BL1607-B2-1:AddOns:WireStage:V:mm:fbk", this);
	wireStageNormalFeedbackControl_ = new AMReadOnlyPVControl("WireNFeedback", "BL1607-B2-1:AddOns:WireStage:N:mm:fbk", this);
	sampleStageXFeedbackControl_ = new AMReadOnlyPVControl("SampleXFeedback", "BL1607-B2-1:AddOns:SampleStage:X:mm:fbk", this);
	sampleStageYFeedbackControl_ = new AMReadOnlyPVControl("SampleYFeedback", "BL1607-B2-1:AddOns:SampleStage:Y:mm:fbk", this);
	sampleStageZFeedbackControl_ = new AMReadOnlyPVControl("SampleZFeedback", "BL1607-B2-1:AddOns:SampleStage:Z:mm:fbk", this);
	attoStageHorizontalFeedbackControl_ = new AMReadOnlyPVControl("AttoHFeedback", "BL1607-B2-1:AddOns:AttoStage:H:mm:fbk", this);
	attoStageVerticalFeedbackControl_ = new AMReadOnlyPVControl("AttoVFeedback", "BL1607-B2-1:AddOns:AttoStage:V:mm:fbk", this);
	attoStageNormalFeedbackControl_ = new AMReadOnlyPVControl("AttoNFeedback", "BL1607-B2-1:AddOns:AttoStage:N:mm:fbk", this);
	attoStageXFeedbackControl_ = new AMReadOnlyPVControl("AttoXFeedback", "BL1607-B2-1:AddOns:AttoStage:X:mm:fbk", this);
	attoStageZFeedbackControl_ = new AMReadOnlyPVControl("AttoZFeedback", "BL1607-B2-1:AddOns:AttoStage:Z:mm:fbk", this);
	attoStageYFeedbackControl_ = new AMReadOnlyPVControl("AttoYFeedback", "BL1607-B2-1:AddOns:AttoStage:Y:mm:fbk", this);
	attoStageRxFeedbackControl_ = new AMReadOnlyPVControl("AttoRxFeedback", "SVM1607-2-B21-08:deg:fbk", this);
	attoStageRzFeedbackControl_ = new AMReadOnlyPVControl("AttoRzFeedback", "SVM1607-2-B21-09:deg:sp", this);
	attoStageRyFeedbackControl_ = new AMReadOnlyPVControl("AttoRyFeedback", "SVM1607-2-B21-07:deg:sp", this);
	bigBeamXFeedbackControl_ = new AMReadOnlyPVControl("BigBeamXFeedback", "SMTR1607-2-B21-36:mm:sp", this);
	bigBeamZFeedbackControl_ = new AMReadOnlyPVControl("BigBeamZFeedback", "SMTR1607-2-B21-37:mm:sp", this);

	sampleStageHorizontalFeedback_ = new AMBasicControlDetectorEmulator("SampleHFeedback", "Sample H Feedback", sampleStageHorizontalFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	sampleStageHorizontalFeedback_->setHiddenFromUsers(true);
	sampleStageHorizontalFeedback_->setIsVisible(false);
	sampleStageVerticalFeedback_ = new AMBasicControlDetectorEmulator("SampleVFeedback", "Sample V Feedback", sampleStageVerticalFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	sampleStageVerticalFeedback_->setHiddenFromUsers(true);
	sampleStageVerticalFeedback_->setIsVisible(false);
	sampleStageNormalFeedback_ = new AMBasicControlDetectorEmulator("SampleNFeedback", "Sample N Feedback", sampleStageNormalFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	sampleStageNormalFeedback_->setHiddenFromUsers(true);
	sampleStageNormalFeedback_->setIsVisible(false);
	wireStageHorizontalFeedback_ = new AMBasicControlDetectorEmulator("WireHFeedback", "Wire H Feedback", wireStageHorizontalFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	wireStageHorizontalFeedback_->setHiddenFromUsers(true);
	wireStageHorizontalFeedback_->setIsVisible(false);
	wireStageVerticalFeedback_ = new AMBasicControlDetectorEmulator("WireVFeedback", "Wire V Feedback", wireStageVerticalFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	wireStageVerticalFeedback_->setHiddenFromUsers(true);
	wireStageVerticalFeedback_->setIsVisible(false);
	wireStageNormalFeedback_ = new AMBasicControlDetectorEmulator("WireNFeedback", "Wire N Feedback", wireStageNormalFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	wireStageNormalFeedback_->setHiddenFromUsers(true);
	wireStageNormalFeedback_->setIsVisible(false);
	sampleStageXFeedback_ = new AMBasicControlDetectorEmulator("SampleXFeedback", "Sample X Feedback", sampleStageXFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	sampleStageXFeedback_->setHiddenFromUsers(true);
	sampleStageXFeedback_->setIsVisible(false);
	sampleStageYFeedback_ = new AMBasicControlDetectorEmulator("SampleYFeedback", "Sample Y Feedback", sampleStageYFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	sampleStageYFeedback_->setHiddenFromUsers(true);
	sampleStageYFeedback_->setIsVisible(false);
	sampleStageZFeedback_ = new AMBasicControlDetectorEmulator("SampleZFeedback", "Sample Z Feedback", sampleStageZFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	sampleStageZFeedback_->setHiddenFromUsers(true);
	sampleStageZFeedback_->setIsVisible(false);
	attoStageHorizontalFeedback_ = new AMBasicControlDetectorEmulator("AttoHFeedback", "Atto H Feedback", attoStageHorizontalFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	attoStageHorizontalFeedback_->setHiddenFromUsers(true);
	attoStageHorizontalFeedback_->setIsVisible(false);
	attoStageVerticalFeedback_ = new AMBasicControlDetectorEmulator("AttoVFeedback", "Atto V Feedback", attoStageVerticalFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	attoStageVerticalFeedback_->setHiddenFromUsers(true);
	attoStageVerticalFeedback_->setIsVisible(false);
	attoStageNormalFeedback_ = new AMBasicControlDetectorEmulator("AttoNFeedback", "Atto N Feedback", attoStageNormalFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	attoStageNormalFeedback_->setHiddenFromUsers(true);
	attoStageNormalFeedback_->setIsVisible(false);
	attoStageXFeedback_ = new AMBasicControlDetectorEmulator("AttoXFeedback", "Atto X Feedback", attoStageXFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	attoStageXFeedback_->setHiddenFromUsers(true);
	attoStageXFeedback_->setIsVisible(false);
	attoStageZFeedback_ = new AMBasicControlDetectorEmulator("AttoZFeedback", "Atto Z Feedback", attoStageZFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	attoStageZFeedback_->setHiddenFromUsers(true);
	attoStageZFeedback_->setIsVisible(false);
	attoStageYFeedback_ = new AMBasicControlDetectorEmulator("AttoYFeedback", "Atto Y Feedback", attoStageYFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	attoStageYFeedback_->setHiddenFromUsers(true);
	attoStageYFeedback_->setIsVisible(false);
	attoStageRxFeedback_ = new AMBasicControlDetectorEmulator("AttoRxFeedback", "Atto Rx Feedback", attoStageRxFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	attoStageRxFeedback_->setHiddenFromUsers(true);
	attoStageRxFeedback_->setIsVisible(false);
	attoStageRzFeedback_ = new AMBasicControlDetectorEmulator("AttoRzFeedback", "Atto Rz Feedback", attoStageRzFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	attoStageRzFeedback_->setHiddenFromUsers(true);
	attoStageRzFeedback_->setIsVisible(false);
	attoStageRyFeedback_ = new AMBasicControlDetectorEmulator("AttoRyFeedback", "Atto Ry Feedback", attoStageRyFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	attoStageRyFeedback_->setHiddenFromUsers(true);
	attoStageRyFeedback_->setIsVisible(false);
	bigBeamXFeedback_ = new AMBasicControlDetectorEmulator("BigBeamXFeedback", "Big Beam X Feedback", bigBeamXFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	bigBeamXFeedback_->setHiddenFromUsers(true);
	bigBeamXFeedback_->setIsVisible(false);
	bigBeamZFeedback_ = new AMBasicControlDetectorEmulator("BigBeamZFeedback", "Big Beam Z Feedback", bigBeamXFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	bigBeamZFeedback_->setHiddenFromUsers(true);
	bigBeamZFeedback_->setIsVisible(false);
}

void VESPERSBeamline::setupExposedControls()
{
	addExposedControl(pseudoSampleStageMotorGroupObject()->horizontalControl());
	addExposedControl(pseudoSampleStageMotorGroupObject()->verticalControl());
	addExposedControl(pseudoSampleStageMotorGroupObject()->normalControl());
	addExposedControl(realSampleStageMotorGroupObject()->horizontalControl());
	addExposedControl(realSampleStageMotorGroupObject()->verticalControl());
	addExposedControl(realSampleStageMotorGroupObject()->normalControl());
	addExposedControl(pseudoWireStageMotorGroupObject()->horizontalControl());
	addExposedControl(pseudoWireStageMotorGroupObject()->verticalControl());
	addExposedControl(pseudoWireStageMotorGroupObject()->normalControl());
	addExposedControl(bigBeamMotorGroupObject()->horizontalControl());
	addExposedControl(bigBeamMotorGroupObject()->verticalControl());
	addExposedControl(pseudoAttocubeStageMotorGroupObject()->horizontalControl());
	addExposedControl(pseudoAttocubeStageMotorGroupObject()->verticalControl());
	addExposedControl(pseudoAttocubeStageMotorGroupObject()->normalControl());
	addExposedControl(realAttocubeStageMotorGroupObject()->horizontalControl());
	addExposedControl(realAttocubeStageMotorGroupObject()->verticalControl());
	addExposedControl(realAttocubeStageMotorGroupObject()->normalControl());
	addExposedControl(attocubeRxMotorGroupObject()->horizontalControl());
	addExposedControl(attocubeRyMotorGroupObject()->horizontalControl());
	addExposedControl(attocubeRzMotorGroupObject()->horizontalControl());
	addExposedControl(mono_->delEControl());
	addExposedControl(mono_->EaControl());
}

void VESPERSBeamline::setupExposedDetectors()
{
	addExposedDetector(singleElementVortexDetector_);
	addExposedDetector(fourElementVortexDetector_);
	addExposedDetector(splitIonChamber_);
	addExposedDetector(preKBIonChamber_);
	addExposedDetector(miniIonChamber_);
	addExposedDetector(postIonChamber_);
	addExposedDetector(energySetpointDetector_);
	addExposedDetector(energyFeedbackDetector_);
	addExposedDetector(masterDwellTimeDetector_);
	addExposedDetector(ringCurrentDetector_);
	addExposedDetector(roperCCD_);
	addExposedDetector(roperCCDFileNumberDetector_);
	addExposedDetector(marCCD_);
	addExposedDetector(marCCDFileNumberDetector_);
	addExposedDetector(pilatusCCDFileNumberDetector_);
	addExposedDetector(pilatusAreaDetector_);

	// All the extras for vortex detectors.
	addExposedDetector(singleElementVortexDeadTime_);
	addExposedDetector(singleElementVortexRealTime_);
	addExposedDetector(singleElementVortexLiveTime_);
	addExposedDetector(singleElementVortexFastPeaks_);
	addExposedDetector(singleElementVortexSlowPeaks_);
	addExposedDetector(singleElementVortexRawSpectrum_);
	addExposedDetector(fourElementVortexDeadTime1_);
	addExposedDetector(fourElementVortexDeadTime2_);
	addExposedDetector(fourElementVortexDeadTime3_);
	addExposedDetector(fourElementVortexDeadTime4_);
	addExposedDetector(fourElementVortexRealTime1_);
	addExposedDetector(fourElementVortexRealTime2_);
	addExposedDetector(fourElementVortexRealTime3_);
	addExposedDetector(fourElementVortexRealTime4_);
	addExposedDetector(fourElementVortexLiveTime1_);
	addExposedDetector(fourElementVortexLiveTime2_);
	addExposedDetector(fourElementVortexLiveTime3_);
	addExposedDetector(fourElementVortexLiveTime4_);
	addExposedDetector(fourElementVortexFastPeaks1_);
	addExposedDetector(fourElementVortexFastPeaks2_);
	addExposedDetector(fourElementVortexFastPeaks3_);
	addExposedDetector(fourElementVortexFastPeaks4_);
	addExposedDetector(fourElementVortexSlowPeaks1_);
	addExposedDetector(fourElementVortexSlowPeaks2_);
	addExposedDetector(fourElementVortexSlowPeaks3_);
	addExposedDetector(fourElementVortexSlowPeaks4_);
	addExposedDetector(fourElementVortexRawSpectrum1_);
	addExposedDetector(fourElementVortexRawSpectrum2_);
	addExposedDetector(fourElementVortexRawSpectrum3_);
	addExposedDetector(fourElementVortexRawSpectrum4_);

	// All the extra motors
	addExposedDetector(sampleStageHorizontalFeedback_);
	addExposedDetector(sampleStageVerticalFeedback_);
	addExposedDetector(sampleStageNormalFeedback_);
	addExposedDetector(wireStageHorizontalFeedback_);
	addExposedDetector(wireStageVerticalFeedback_);
	addExposedDetector(wireStageNormalFeedback_);
	addExposedDetector(sampleStageXFeedback_);
	addExposedDetector(sampleStageYFeedback_);
	addExposedDetector(sampleStageZFeedback_);
	addExposedDetector(attoStageHorizontalFeedback_);
	addExposedDetector(attoStageVerticalFeedback_);
	addExposedDetector(attoStageNormalFeedback_);
	addExposedDetector(attoStageXFeedback_);
	addExposedDetector(attoStageZFeedback_);
	addExposedDetector(attoStageYFeedback_);
	addExposedDetector(attoStageRxFeedback_);
	addExposedDetector(attoStageRyFeedback_);
	addExposedDetector(attoStageYFeedback_);
	addExposedDetector(bigBeamXFeedback_);
	addExposedDetector(bigBeamZFeedback_);
}

AMAction3 *VESPERSBeamline::createBeamChangeAction(VESPERS::Beam beam)
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

	AMListAction3 *changeBeamAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("Change Beam Action", "Does all the necessary work to switch beams by ensuring all steps are done correctly."));
	changeBeamAction->addSubAction(mono()->createAllowScanningAction(false));

	AMControlInfo setpoint = beamSelectionMotor_->toInfo();
	setpoint.setValue(beamPositions_.value(beam));
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, beamSelectionMotor_);
	changeBeamAction->addSubAction(action);

	if (beam != VESPERS::Pink)
		changeBeamAction->addSubAction(mono()->createAllowScanningAction(true));

	return changeBeamAction;
}

AMAction3 *VESPERSBeamline::createBeamOnAction()
{
	// The correct order for turning the beam on is turning on the safety shutter and then the second photon shutter.
	AMSequentialListAction3 *beamOnAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("The beam on action.", "The beam on action."));

	AMControlInfo setpoint = safetyShutter1_->toInfo();
	setpoint.setValue(1);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, safetyShutter1_);
	beamOnAction->addSubAction(action);

	setpoint = photonShutter2_->toInfo();
	setpoint.setValue(1);
	actionInfo = new AMControlMoveActionInfo3(setpoint);
	action = new AMControlMoveAction3(actionInfo, photonShutter2_);
	beamOnAction->addSubAction(action);

	setpoint = safetyShutter2_->toInfo();
	setpoint.setValue(1);
	actionInfo = new AMControlMoveActionInfo3(setpoint);
	action = new AMControlMoveAction3(actionInfo, safetyShutter2_);
	beamOnAction->addSubAction(action);

	return beamOnAction;
}

AMAction3 *VESPERSBeamline::createBeamOffAction()
{
	// The correct order for turning the beam off is turning off the second photon shutter and then the safety shutter.
	AMControlInfo setpoint = safetyShutter2_->toInfo();
	setpoint.setValue(0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMControlMoveAction3 *beamOffAction = new AMControlMoveAction3(actionInfo, safetyShutter2_);

	return beamOffAction;
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
	if (safetyShutter1_->isOpen() || (safetyShutter1_->isClosed() && photonShutter2_->isClosed())){

		photonShutter1_->open();
		return true;
	}

	return false;
}

bool VESPERSBeamline::closePhotonShutter1()
{
	if (photonShutter1_->isOpen()){

		photonShutter1_->close();
		return true;
	}

	return false;
}

bool VESPERSBeamline::openPhotonShutter2()
{
	if (safetyShutter1_->isOpen() || (safetyShutter1_->isClosed() && photonShutter1_->isClosed())){

		photonShutter2_->open();
		return true;
	}

	return false;
}

bool VESPERSBeamline::closePhotonShutter2()
{
	if (photonShutter2_->isOpen()){

		photonShutter2_->close();
		return true;
	}

	return false;
}

bool VESPERSBeamline::openSafetyShutter1()
{
	if (safetyShutter1_->isClosed()){

		safetyShutter1_->open();
		return true;
	}

	return false;
}

bool VESPERSBeamline::closeSafetyShutter1()
{
	if ((photonShutter1_->isOpen() && photonShutter2_->isClosed()) || (photonShutter1_->isClosed() && photonShutter2_->isOpen())){

		safetyShutter1_->close();
		return true;
	}

	return false;
}

bool VESPERSBeamline::openSafetyShutter2()
{
	if (safetyShutter2_->isClosed()){

		safetyShutter2_->open();
		return true;
	}

	return false;
}

bool VESPERSBeamline::closeSafetyShutter2()
{
	if (safetyShutter2_->isOpen()){

		safetyShutter2_->close();
		return false;
	}

	return false;
}
