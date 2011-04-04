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
	: AMBeamline("VESPERSBeamline")
{
	setupDiagnostics();
	setupEndstation();
	setupSingleElementDetector();
	setupControlSets();
}

void VESPERSBeamline::setupDiagnostics()
{
	// Pressure controls.
	ccgFE1_ =  new AMReadOnlyPVwStatusControl("Pressure FE1", "CCG1408-B20-01:vac:p", "CCG1408-B20-01:vac", this, 0);
	ccgFE2a_ =  new AMReadOnlyPVwStatusControl("Pressure FE2a", "CCG1408-B20-02:vac:p", "CCG1408-B20-02:vac", this, 0);
	ccgFE2b_ =  new AMReadOnlyPVwStatusControl("Pressure FE2b", "CCG1408-B20-03:vac:p", "CCG1408-B20-03:vac", this, 0);
	ccgFE3a_ =  new AMReadOnlyPVwStatusControl("Pressure FE3a", "CCG1607-1-B20-02:vac:p", "CCG1607-1-B20-02:vac", this, 0);
	ccgFE3b_ =  new AMReadOnlyPVwStatusControl("Pressure FE3b", "CCG1607-1-B20-03:vac:p", "CCG1607-1-B20-03:vac", this, 0);
	ccgM1_ =  new AMReadOnlyPVwStatusControl("Pressure M1", "CCG1607-1-B20-04:vac:p", "CCG1607-1-B20-04:vac", this, 0);
	ccgM2_ =  new AMReadOnlyPVwStatusControl("Pressure M2", "CCG1607-1-B20-05:vac:p", "CCG1607-1-B20-05:vac", this, 0);
	ccgBPM1_ =  new AMReadOnlyPVwStatusControl("Pressure BPM1", "CCG1607-1-B20-06:vac:p", "CCG1607-1-B20-06:vac", this, 0);
	ccgBPM2_ =  new AMReadOnlyPVwStatusControl("Pressure BPM2", "CCG1607-1-B20-07:vac:p", "CCG1607-1-B20-07:vac", this, 0);
	ccgMono_ =  new AMReadOnlyPVwStatusControl("Pressure Mono", "CCG1607-1-B20-08:vac:p", "CCG1607-1-B20-08:vac", this, 0);
	ccgExitSlits_ =  new AMReadOnlyPVwStatusControl("Pressure Exit Slits", "CCG1607-1-B20-09:vac:p", "CCG1607-1-B20-09:vac", this, 0);
	ccgStraightSection_ =  new AMReadOnlyPVwStatusControl("Pressure Straight Section", "CCG1607-1-B20-10:vac:p", "CCG1607-1-B20-10:vac", this, 0);
	ccgBPM3_ =  new AMReadOnlyPVwStatusControl("Pressure BPM3", "CCG1607-1-B20-11:vac:p", "CCG1607-1-B20-11:vac", this, 0);
	ccgSSH_ =  new AMReadOnlyPVwStatusControl("Pressure SSH", "CCG1607-1-B20-012:vac:p", "CCG1607-1-B20-12:vac", this, 0);
	ccgBeamTransfer1_ =  new AMReadOnlyPVwStatusControl("Pressure Beam Transfer 1", "CCG1607-1-B21-01:vac:p", "CCG1607-1-B21-01:vac", this, 0);
	ccgBeamTransfer2_ =  new AMReadOnlyPVwStatusControl("Pressure Beam Transfer 2", "CCG1607-2-B21-01:vac:p", "CCG1607-2-B21-01:vac", this, 0);
	ccgPreWindow_ =  new AMReadOnlyPVwStatusControl("Pressure Pre-Window", "CCG1607-2-B21-02:vac:p", "CCG1607-2-B21-02:vac", this, 0);
	ccgPostWindow_ =  new AMReadOnlyPVwStatusControl("Pressure Post-Window", "CCG1607-2-B21-03:vac:p", "CCG1607-2-B21-03:vac", this, 0);

	// Valve controls.
	/// \todo This only reads the status right now.  Will need to change to a dual state control when I get around to making it.
	vvrFE1_ = new AMReadOnlyPVControl("Valve FE1", "VVR1408-B20-01:state", this);
	vvrFE2a_ = new AMReadOnlyPVControl("Valve FE2a", "VVR1408-B20-02:state", this);
	vvrFE2b_ = new AMReadOnlyPVControl("Valve FE2b", "VVR1408-B20-03:state", this);
	vvrFE3a_ = new AMReadOnlyPVControl("Valve FE3a", "VVR1607-1-B20-02:state", this);
	vvrFE3b_ = new AMReadOnlyPVControl("Valve FE3b", "VVR1607-1-B20-03:state", this);
	vvrM1_ = new AMReadOnlyPVControl("Valve M1", "VVR1607-1-B20-04:state", this);
	vvrM2_ = new AMReadOnlyPVControl("Valve M2", "VVR1607-1-B20-05:state", this);
	vvrBPM1_ = new AMReadOnlyPVControl("Valve BPM1", "VVR1607-1-B20-06:state", this);
	vvrBPM2_ = new AMReadOnlyPVControl("Valve BPM2", "VVR1607-1-B20-07:state", this);
	vvrMono_ = new AMReadOnlyPVControl("Valve Mono", "VVR1607-1-B20-08:state", this);
	vvrExitSlits_ = new AMReadOnlyPVControl("Valve Exit Slits", "VVR1607-1-B20-09:state", this);
	vvrStraightSection_ = new AMReadOnlyPVControl("Valve Straight Section", "VVR1607-1-B20-10:state", this);
	vvrBPM3_ = new AMReadOnlyPVControl("Valve BPM3", "VVR1607-1-B20-11:state", this);
	vvrSSH_ = new AMReadOnlyPVControl("Valve SSH", "VVR1607-1-B20-12:state", this);
	vvrBeamTransfer1_ = new AMReadOnlyPVControl("Valve Beam Transfer 1", "VVR1607-1-B21-01:state", this);
	vvrBeamTransfer2_ = new AMReadOnlyPVControl("Valve Beam Transfer 2", "VVR1607-2-B21-01:state", this);
	vvrPreWindow_ = new AMReadOnlyPVControl("Valve Pre-Window", "VVR1607-2-B21-02:state", this);
	vvrPostWindow_ = new AMReadOnlyPVControl("Valve Post-Window", "VVR1607-2-B21-03:state", this);

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
	iopBeamTransfer1_ = new AMReadOnlyPVControl("Ion Pump Beam Transfer 1", "IOP1607-1-B21-02", this);
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
	tmMono7_ = new AMReadOnlyPVControl("Temperature Mono 7", "TM1607-1-B20-19s:trip", this);

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
}

void VESPERSBeamline::setupEndstation()
{
	// The controls used for the control window.
	ccdMotor_ = new AMPVwStatusControl("CCD motor", "SMTR1607-2-B21-18:mm:sp", "SMTR1607-2-B21-18:mm", "SMTR1607-2-B21-18:status", "SMTR1607-2-B21-18:stop", this, 1);
	microscopeMotor_ = new AMPVwStatusControl("Microscope motor", "SMTR1607-2-B21-17:mm:sp", "SMTR1607-2-B21-17:mm", "SMTR1607-2-B21-17:status", "SMTR1607-2-B21-17:stop", this, 1);
	fourElMotor_ = new AMPVwStatusControl("4-Element Vortex motor", "SMTR1607-2-B21-27:mm:sp", "SMTR1607-2-B21-27:mm", "SMTR1607-2-B21-27:status", "SMTR1607-2-B21-27:stop", this, 1);
	singleElMotor_ = new AMPVwStatusControl("1-Element Vortex motor", "SMTR1607-2-B21-15:mm:sp", "SMTR1607-2-B21-15:mm", "SMTR1607-2-B21-15:status", "SMTR1607-2-B21-15:stop", this, 1);
	focusMotor_ = new AMPVwStatusControl("Focus motor", "TS1607-2-B21-01:N:user:mm:sp", "TS1607-2-B21-01:N:user:mm", "TS1607-2-B21-01:N:status", "TS1607-2-B21-01:HNV:stop.PROC", this, 1);

	// The process variables that have the feedback value used for the button.  The microscope doesn't need one because it's encoder doesn't work.
	ccdMotorfbk_ = new AMReadOnlyPVControl("CCD motor feedback", "SMTR1607-2-B21-18:mm:fbk", this);
	fourElMotorfbk_ = new AMReadOnlyPVControl("4-Element Vortex motor feedback", "SMTR1607-2-B21-27:mm:fbk", this);
	singleElMotorfbk_ = new AMReadOnlyPVControl("1-Element Vortex motor feedback", "SMTR1607-2-B21-15:mm:fbk", this);
	focusMotorfbk_ = new AMReadOnlyPVControl("Focus motor feedback", "TS1607-2-B21-01:N:user:mm:fbk", this);

	// Microscope light PV.
	micLight_ = new AMProcessVariable("07B2_PLC_Mic_Light_Inten", true, this);
	micLight_->disablePutCallbackMode(true);

	// Various CCD file path PVs.
	ccdPath_ = new AMProcessVariable("IOC1607-003:det1:FilePath", true, this);
	ccdFile_ = new AMProcessVariable("IOC1607-003:det1:FileName", true, this);
	ccdNumber_ = new AMProcessVariable("IOC1607-003:det1:FileNumber", true, this);
}

void VESPERSBeamline::setupSingleElementDetector()
{
	elapsedTime1E_ = new AMReadOnlyPVControl("1-el Elapsed Time", "IOC1607-004:mca1.ERTM", this);
	integrationTime1E_ = new AMPVControl("1-el Integration Time", "IOC1607-004:mca1.PRTM", "IOC1607-004::mca1.PRTM", QString(), this);
	liveTime1E_ = new AMPVControl("1-el Live Time", "IOC1607-004:mca1.PLTM", "IOC1607-004:mca1.PLTM", QString(), this);
	start1E_ = new AMPVControl("1-el Start", "IOC1607-004:mca1.ERST", "IOC1607-004:mca1.ERST", QString(), this);
	stop1E_ = new AMPVControl("1-el Stop", "IOC1607-004:mca1.STOP", "IOC1607-004:mca1.STOP", QString(), this);
	deadTime1E_ = new AMReadOnlyPVControl("1-el Dead Time", "IOC1607-004:mca1.IDTM", this);
	maxEnergy1E_ = new AMPVControl("1-el Maximum Energy", "IOC1607-004:dxp1.EMAX", "IOC1607-004:dxp1.EMAX", QString(), this);
	mcaUpdateRate1E_ = new AMPVControl("1-el MCA Update Rate", "IOC1607-004:mca1Read.SCAN", "IOC1607-004:mca1Read.SCAN", QString(), this);
	peakingTime1E_ = new AMPVControl("1-el Peaking Time", "IOC1607-004:dxp1.PKTIM", "IOC1607-004:dxp1.PKTIM", QString(), this);
	spectrum1E_ = new AMReadOnlyPVControl("1-el Spectrum", "IOC1607-004:mca1", this);

	// Putting the controls into their own control set.
	vortex1EControls_ = new AMControlSet(this);
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

	for (int i = 0; i < pressureSet_->count(); i++)
		connect(qobject_cast<AMReadOnlyPVwStatusControl *>(pressureSet_->at(i)), SIGNAL(movingChanged(bool)), this, SLOT(pressureError()));

	// Grouping the valve controls together.
	valveSet_ = new AMControlSet(this);
	valveSet_->addControl(vvrFE1_);
	valveSet_->addControl(vvrFE2a_);
	valveSet_->addControl(vvrFE2b_);
	valveSet_->addControl(vvrFE3a_);
	valveSet_->addControl(vvrFE3b_);
	valveSet_->addControl(vvrM1_);
	valveSet_->addControl(vvrM2_);
	valveSet_->addControl(vvrBPM1_);
	valveSet_->addControl(vvrBPM2_);
	valveSet_->addControl(vvrMono_);
	valveSet_->addControl(vvrExitSlits_);
	valveSet_->addControl(vvrStraightSection_);
	valveSet_->addControl(vvrBPM3_);
	valveSet_->addControl(vvrSSH_);
	valveSet_->addControl(vvrBeamTransfer1_);
	valveSet_->addControl(vvrBeamTransfer2_);
	valveSet_->addControl(vvrPreWindow_);
	valveSet_->addControl(vvrPostWindow_);

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

	connect(ionPumpSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(ionPumpError()));

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

	connect(temperatureSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(temperatureError()));

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

	connect(flowSwitchSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(flowSwitchError()));

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

	connect(flowTransducerSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(flowTransducerError()));

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
}

void VESPERSBeamline::pressureError()
{
	QString error("");
	AMReadOnlyPVwStatusControl *current;

	for (int i = 0; i < pressureSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVwStatusControl *>(pressureSet_->at(i));

		if (current->isMoving())
			error += tr("%1 (%2) %3 %4\n").arg(current->name(), current->readPVName(), QString::number(current->value(), 'e', 3), current->units());
	}

	if (!error.isEmpty()){

		error.prepend("The following pressure readings are at a critical level:\n");
		AMErrorMon::report(AMErrorReport(pressureSet_, AMErrorReport::Serious, 0, error));
	}
}

void VESPERSBeamline::ionPumpError()
{
	QString error("");
	AMReadOnlyPVControl *current;

	for (int i = 0; i < ionPumpSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(ionPumpSet_->at(i));

		if (!current->readPV()->getInt())
			error += tr("%1 (%2)\n").arg(current->name(), current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following ion pumps are no longer operating correctly:\n");
		AMErrorMon::report(AMErrorReport(ionPumpSet_, AMErrorReport::Serious, 0, error));
	}
}

void VESPERSBeamline::temperatureError()
{
	QString error("");
	AMReadOnlyPVControl *current;

	for (int i = 0; i < temperatureSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(temperatureSet_->at(i));

		if (!current->readPV()->getInt())
			error += tr("%1 (%2)\n").arg(current->name(), current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following temperature sensors are reading too high:\n");
		AMErrorMon::report(AMErrorReport(temperatureSet_, AMErrorReport::Serious, 0, error));
	}
}

void VESPERSBeamline::flowSwitchError()
{
	QString error("");
	AMReadOnlyPVControl *current;

	for (int i = 0; i < flowSwitchSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(flowSwitchSet_->at(i));

		if (!current->readPV()->getInt())
			error += tr("%1 (%2)\n").arg(current->name(), current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following flow switches have tripped:\n");
		AMErrorMon::report(AMErrorReport(flowSwitchSet_, AMErrorReport::Serious, 0, error));
	}
}

void VESPERSBeamline::flowTransducerError()
{
	QString error("");
	AMReadOnlyPVControl *current;

	for (int i = 0; i < flowTransducerSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(flowTransducerSet_->at(i));

		if (!current->readPV()->getInt())
			error += tr("%1 (%2)\n").arg(current->name(), current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following flow transducers are measuring too low:\n");
		AMErrorMon::report(AMErrorReport(flowTransducerSet_, AMErrorReport::Serious, 0, error));
	}
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
	delete vvrFE2a_;
	delete vvrFE2b_;
	delete vvrFE3a_;
	delete vvrFE3b_;
	delete vvrM1_;
	delete vvrM2_;
	delete vvrBPM1_;
	delete vvrBPM2_;
	delete vvrMono_;
	delete vvrExitSlits_;
	delete vvrStraightSection_;
	delete vvrBPM3_;
	delete vvrSSH_;
	delete vvrBeamTransfer1_;
	delete vvrBeamTransfer2_;
	delete vvrPreWindow_;
	delete vvrPostWindow_;
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
	delete ccdPath_;
	delete ccdFile_;
	delete ccdNumber_;
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
}
