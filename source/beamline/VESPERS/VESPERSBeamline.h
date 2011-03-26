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

#ifndef VESPERSBEAMLINE_H
#define VESPERSBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/AMControlSet.h"

/// This class
class VESPERSBeamline : public AMBeamline
{
	Q_OBJECT
public:
	static VESPERSBeamline* vespers() {
		if(!instance_)
			instance_ = new VESPERSBeamline();
		return static_cast<VESPERSBeamline*>(instance_);
	}

	// Accessing control elements:

	/// \todo delete this
	AMControl *ccdDetector() const { return ccdDetector_; }

	/// Returns the pressure control for Front End section 1.
	AMControl *ccgFE1() const { return ccgFE1_; }
	/// Returns the pressure control for Front End section 2a.
	AMControl *ccgFE2a() const { return ccgFE2a_; }
	/// Returns the pressure control for Front End section 2b.
	AMControl *ccgFE2b() const { return ccgFE2b_; }
	/// Returns the pressure control for Front End section 3a.
	AMControl *ccgFE3a() const { return ccgFE3a_; }
	/// Returns the pressure control for Front End section 3b.
	AMControl *ccgFE3b() const { return ccgFE3b_; }
	/// Returns the pressure control for M1.
	AMControl *ccgM1() const { return ccgM1_; }
	/// Returns the pressure control for M2.
	AMControl *ccgM2() const { return ccgM2_; }
	/// Returns the pressure control for BPM1.
	AMControl *ccgBPM1() const { return ccgBPM1_; }
	/// Returns the pressure control for BPM2.
	AMControl *ccgBPM2() const { return ccgBPM2_; }
	/// Returns the pressure control for the Mono.
	AMControl *ccgMono() const { return ccgMono_; }
	/// Returns the pressure control for the exit slits.
	AMControl *ccgExitSlits() const { return ccgExitSlits_; }
	/// Returns the pressure control for the straight section.
	AMControl *ccgStraightSection() const { return ccgStraightSection_; }
	/// Returns the pressure control for BPM3.
	AMControl *ccgBPM3() const { return ccgBPM3_; }
	/// Returns the pressure control at the POE SSH.
	AMControl *ccgSSH() const { return ccgSSH_; }
	/// Returns the pressure control for the beam transfer section (1).
	AMControl *ccgBeamTransfer1() const { return ccgBeamTransfer1_; }
	/// Returns the pressure control for the beam transfer section (2).
	AMControl *ccgBeamTransfer2() const { return ccgBeamTransfer2_; }
	/// Returns the pressure control for the pre-window section.
	AMControl *ccgPreWindow() const { return ccgPreWindow_; }
	/// Returns the pressure control for the post-window section.
	AMControl *ccgPostWindow() const { return ccgPostWindow_; }

	/// Returns the valve control for Front End section 1.
	AMControl *vvrFE1() const { return vvrFE1_; }
	/// Returns the valve control for Front End section 2a.
	AMControl *vvrFE2a() const { return vvrFE2a_; }
	/// Returns the valve control for Front End section 2b.
	AMControl *vvrFE2b() const { return vvrFE2b_; }
	/// Returns the valve control for Front End section 3a.
	AMControl *vvrFE3a() const { return vvrFE3a_; }
	/// Returns the valve control for Front End section 3b.
	AMControl *vvrFE3b() const { return vvrFE3b_; }
	/// Returns the valve control for M1.
	AMControl *vvrM1() const { return vvrM1_; }
	/// Returns the valve control for M2.
	AMControl *vvrM2() const { return vvrM2_; }
	/// Returns the valve control for BPM1.
	AMControl *vvrBPM1() const { return vvrBPM1_; }
	/// Returns the valve control for BPM2.
	AMControl *vvrBPM2() const { return vvrBPM2_; }
	/// Returns the valve control for the Mono.
	AMControl *vvrMono() const { return vvrMono_; }
	/// Returns the valve control for the exit slits.
	AMControl *vvrExitSlits() const { return vvrExitSlits_; }
	/// Returns the valve control for the straight section.
	AMControl *vvrStraightSection() const { return vvrStraightSection_; }
	/// Returns the valve control for BPM3.
	AMControl *vvrBPM3() const { return vvrBPM3_; }
	/// Returns the valve control at the POE SSH.
	AMControl *vvrSSH() const { return vvrSSH_; }
	/// Returns the valve control for the beam transfer section (1).
	AMControl *vvrBeamTransfer1() const { return vvrBeamTransfer1_; }
	/// Returns the valve control for the beam transfer section (2).
	AMControl *vvrBeamTransfer2() const { return vvrBeamTransfer2_; }
	/// Returns the valve control for the pre-window section.
	AMControl *vvrPreWindow() const { return vvrPreWindow_; }
	/// Returns the valve control for the post-window section.
	AMControl *vvrPostWindow() const { return vvrPostWindow_; }

	/// Returns the ion pump control for Front End section 1.a
	AMControl *iopFE1a() const { return iopFE1a_; }
	/// Returns the ion pump control for Front End section 1b.
	AMControl *iopFE1b() const { return iopFE1b_; }
	/// Returns the ion pump control for Front End section 2a.
	AMControl *iopFE2a() const { return iopFE2a_; }
	/// Returns the ion pump control for Front End section 2b.
	AMControl *iopFE2b() const { return iopFE2b_; }
	/// Returns the ion pump control for Front End section 2c.
	AMControl *iopFE2c() const { return iopFE2c_; }
	/// Returns the ion pump control for Front End section 2d.
	AMControl *iopFE2d() const { return iopFE2d_; }
	/// Returns the ion pump control for Front End section 3a.
	AMControl *iopFE3a() const { return iopFE3a_; }
	/// Returns the ion pump control for Front End section 3b.
	AMControl *iopFE3b() const { return iopFE3b_; }
	/// Returns the ion pump control for M1.
	AMControl *iopM1() const { return iopM1_; }
	/// Returns the ion pump control for M2.
	AMControl *iopM2() const { return iopM2_; }
	/// Returns the ion pump control for BPM1.
	AMControl *iopBPM1() const { return iopBPM1_; }
	/// Returns the ion pump control for BPM2.
	AMControl *iopBPM2() const { return iopBPM2_; }
	/// Returns the ion pump control for the Mono.
	AMControl *iopMono() const { return iopMono_; }
	/// Returns the ion pump control for the exit slits.
	AMControl *iopExitSlits() const { return iopExitSlits_; }
	/// Returns the ion pump control for the straight section.
	AMControl *iopStraightSection() const { return iopStraightSection_; }
	/// Returns the ion pump control for BPM3.
	AMControl *iopBPM3() const { return iopBPM3_; }
	/// Returns the ion pump control at the POE SSH.
	AMControl *iopSSH() const { return iopSSH_; }
	/// Returns the ion pump control for the beam transfer section (1).
	AMControl *iopBeamTransfer1() const { return iopBeamTransfer1_; }
	/// Returns the ion pump control for the beam transfer section (2).
	AMControl *iopBeamTransfer2() const { return iopBeamTransfer2_; }
	/// Returns the ion pump control for the pre-window section.
	AMControl *iopPreWindow() const { return iopPreWindow_; }

	/// Returns the temperature control for the first water supply sensor.
	AMControl *tmWaterSupply1() const { return tmWaterSupply1_; }
	/// Returns the temperature control for the second water supply sensor.
	AMControl *tmWaterSupply2() const { return tmWaterSupply2_; }
	/// Returns the temperature control for the first M1A sensor.
	AMControl *tmM1A1() const { return tmM1A1_; }
	/// Returns the temperature control for the second M1A sensor.
	AMControl *tmM1A2() const { return tmM1A2_; }
	/// Returns the temperature control for the third M1A sensor.
	AMControl *tmM1A3() const { return tmM1A3_; }
	/// Returns the temperature control for the first M1B sensor.
	AMControl *tmM1B1() const { return tmM1B1_; }
	/// Returns the temperature control for the second M1B sensor.
	AMControl *tmM1B2() const { return tmM1B2_; }
	/// Returns the temperature control for the third M1B sensor.
	AMControl *tmM1B3() const { return tmM1B3_; }
	/// Returns the temperature control for the first M2A sensor.
	AMControl *tmM2A1() const { return tmM2A1_; }
	/// Returns the temperature control for the second M2A sensor.
	AMControl *tmM2A2() const { return tmM2A2_; }
	/// Returns the temperature control for the third M2A sensor.
	AMControl *tmM2A3() const { return tmM2A3_; }
	/// Returns the temperature control for the first M2B sensor.
	AMControl *tmM2B1() const { return tmM2B1_; }
	/// Returns the temperature control for the second M2B sensor.
	AMControl *tmM2B2() const { return tmM2B2_; }
	/// Returns the temperature control for the third M2B sensor.
	AMControl *tmM2B3() const { return tmM2B3_; }
	/// Returns the temperature control for the first mono sensor.
	AMControl *tmMono1() const { return tmMono1_; }
	/// Returns the temperature control for the second mono sensor.
	AMControl *tmMono2() const { return tmMono2_; }
	/// Returns the temperature control for the third mono sensor.
	AMControl *tmMono3() const { return tmMono3_; }
	/// Returns the temperature control for the fourth mono sensor.
	AMControl *tmMono4() const { return tmMono4_; }
	/// Returns the temperature control for the fifth mono sensor.
	AMControl *tmMono5() const { return tmMono5_; }
	/// Returns the temperature control for the sixth mono sensor.
	AMControl *tmMono6() const { return tmMono6_; }
	/// Returns the temperature control for the seventh mono sensor.
	AMControl *tmMono7() const { return tmMono7_; }

	/// Returns the water flow switch control for M1A.
	AMControl *swfM1A() const { return swfM1A_; }
	/// Returns the water flow switch control for M1B.
	AMControl *swfM1B() const { return swfM1B_; }
	/// Returns the water flow switch control for M2A.
	AMControl *swfM2A() const { return swfM2A_; }
	/// Returns the water flow switch control for M2B.
	AMControl *swfM2B() const { return swfM2B_; }
	/// Returns the water flow switch control for the Mono.
	AMControl *swfMono() const { return swfMono_; }
	/// Returns the water flow switch control for the exit slits.
	AMControl *swfExitSlits() const { return swfExitSlits_; }
	/// Returns the water flow switch control for the first set of interim slits.
	AMControl *swfInterimSlits1() const { return swfInterimSlits1_; }
	/// Returns the water flow switch control for the second set of interim slits.
	AMControl *swfInterimSlits2() const { return swfInterimSlits2_; }
	/// Returns the water flow switch control for the POE SSH1.
	AMControl *swfPoeSsh1() const { return swfPoeSsh1_; }
	/// Returns the water flow switch control for the POE SSH2.
	AMControl *swfPoeSsh2() const { return swfPoeSsh2_; }
	/// Returns the water flow switch control for the SOE Roper CCD.
	AMControl *swfSoeCcd() const { return swfSoeCcd_; }

	/// Returns the water flow transducer control for M1A.
	AMControl *fltM1A() const { return fltM1A_; }
	/// Returns the water flow transducer control for M1B.
	AMControl *fltM1B() const { return fltM1B_; }
	/// Returns the water flow transducer control for M2A.
	AMControl *fltM2A() const { return fltM2A_; }
	/// Returns the water flow transducer control for M2B.
	AMControl *fltM2B() const { return fltM2B_; }
	/// Returns the water flow transducer control for the Mono.
	AMControl *fltMono() const { return fltMono_; }
	/// Returns the water flow transducer control for the exit slits.
	AMControl *fltExitSlits() const { return fltExitSlits_; }
	/// Returns the water flow transducer control for the first set of interim slits.
	AMControl *fltInterimSlits1() const { return fltInterimSlits1_; }
	/// Returns the water flow transducer control for the second set of interim slits.
	AMControl *fltInterimSlits2() const { return fltInterimSlits2_; }
	/// Returns the water flow transducer control for the POE SSH1.
	AMControl *fltPoeSsh1() const { return fltPoeSsh1_; }
	/// Returns the water flow transducer control for the POE SSH2.
	AMControl *fltPoeSsh2() const { return fltPoeSsh2_; }
	/// Returns the water flow transducer control for the SOE Roper CCD.
	AMControl *fltSoeCcd() const { return fltSoeCcd_; }

	// These Control Sets are logical groups of controls, that are commonly used by different Acquaman components

signals:

public slots:

protected:
	/// Constructor. This is a singleton class; access it through VESPERSBeamline::vespers().
	VESPERSBeamline();

	/// \todo delete this too
	AMControl *ccdDetector_;

	// Pressure controls.
	AMControl *ccgFE1_;
	AMControl *ccgFE2a_;
	AMControl *ccgFE2b_;
	AMControl *ccgFE3a_;
	AMControl *ccgFE3b_;
	AMControl *ccgM1_;
	AMControl *ccgM2_;
	AMControl *ccgBPM1_;
	AMControl *ccgBPM2_;
	AMControl *ccgMono_;
	AMControl *ccgExitSlits_;
	AMControl *ccgStraightSection_;
	AMControl *ccgBPM3_;
	AMControl *ccgSSH_;
	AMControl *ccgBeamTransfer1_;
	AMControl *ccgBeamTransfer2_;
	AMControl *ccgPreWindow_;
	AMControl *ccgPostWindow_;

	// Valve controls.
	AMControl *vvrFE1_;
	AMControl *vvrFE2a_;
	AMControl *vvrFE2b_;
	AMControl *vvrFE3a_;
	AMControl *vvrFE3b_;
	AMControl *vvrM1_;
	AMControl *vvrM2_;
	AMControl *vvrBPM1_;
	AMControl *vvrBPM2_;
	AMControl *vvrMono_;
	AMControl *vvrExitSlits_;
	AMControl *vvrStraightSection_;
	AMControl *vvrBPM3_;
	AMControl *vvrSSH_;
	AMControl *vvrBeamTransfer1_;
	AMControl *vvrBeamTransfer2_;
	AMControl *vvrPreWindow_;
	AMControl *vvrPostWindow_;

	// Ion pump controls.
	AMControl *iopFE1a_;
	AMControl *iopFE1b_;
	AMControl *iopFE2a_;
	AMControl *iopFE2b_;
	AMControl *iopFE2c_;
	AMControl *iopFE2d_;
	AMControl *iopFE3a_;
	AMControl *iopFE3b_;
	AMControl *iopM1_;
	AMControl *iopM2_;
	AMControl *iopBPM1_;
	AMControl *iopBPM2_;
	AMControl *iopMono_;
	AMControl *iopExitSlits_;
	AMControl *iopStraightSection_;
	AMControl *iopBPM3_;
	AMControl *iopSSH_;
	AMControl *iopBeamTransfer1_;
	AMControl *iopBeamTransfer2_;
	AMControl *iopPreWindow_;

	// Temperature controls.
	AMControl *tmWaterSupply1_;
	AMControl *tmWaterSupply2_;
	AMControl *tmM1A1_;
	AMControl *tmM1A2_;
	AMControl *tmM1A3_;
	AMControl *tmM1B1_;
	AMControl *tmM1B2_;
	AMControl *tmM1B3_;
	AMControl *tmM2A1_;
	AMControl *tmM2A2_;
	AMControl *tmM2A3_;
	AMControl *tmM2B1_;
	AMControl *tmM2B2_;
	AMControl *tmM2B3_;
	AMControl *tmMono1_;
	AMControl *tmMono2_;
	AMControl *tmMono3_;
	AMControl *tmMono4_;
	AMControl *tmMono5_;
	AMControl *tmMono6_;
	AMControl *tmMono7_;

	// Flow switch controls.
	AMControl *swfM1A_;
	AMControl *swfM1B_;
	AMControl *swfM2A_;
	AMControl *swfM2B_;
	AMControl *swfMono_;
	AMControl *swfExitSlits_;
	AMControl *swfInterimSlits1_;
	AMControl *swfInterimSlits2_;
	AMControl *swfPoeSsh1_;
	AMControl *swfPoeSsh2_;
	AMControl *swfSoeCcd_;

	// Flow transducer controls.
	AMControl *fltM1A_;
	AMControl *fltM1B_;
	AMControl *fltM2A_;
	AMControl *fltM2B_;
	AMControl *fltMono_;
	AMControl *fltExitSlits_;
	AMControl *fltInterimSlits1_;
	AMControl *fltInterimSlits2_;
	AMControl *fltPoeSsh1_;
	AMControl *fltPoeSsh2_;
	AMControl *fltSoeCcd_;
};

#endif // VESPERSBEAMLINE_H
