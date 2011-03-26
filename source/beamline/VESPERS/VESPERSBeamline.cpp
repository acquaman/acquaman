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
	/// \todo delete this three
	ccdDetector_ = new AMPVwStatusControl("CCD", "SMTR1607-2-B21-18:mm:sp", "SMTR1607-2-B21-18:mm", "SMTR1607-2-B21-18:status", "SMTR1607-2-B21-18:stop", this, 1);

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
	vvrFE1_;
	vvrFE2a_;
	vvrFE2b_;
	vvrFE3a_;
	vvrFE3b_;
	vvrM1_;
	vvrM2_;
	vvrBPM1_;
	vvrBPM2_;
	vvrMono_;
	vvrExitSlits_;
	vvrStraightSection_;
	vvrBPM3_;
	vvrSSH_;
	vvrBeamTransfer1_;
	vvrBeamTransfer2_;
	vvrPreWindow_;
	vvrPostWindow_;

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
	swfSoeCcd_ = new AMReadOnlyPVControl("Flow Switch SOE CCD", "SWF1607-2-B21-01", this);

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
	fltSoeCcd_ = new AMReadOnlyPVControl("Flow Transducer SOE CCD", "FLT1607-2-B21-01:lowflow", this);
}
