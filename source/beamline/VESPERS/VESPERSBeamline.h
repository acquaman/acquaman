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
#include "beamline/VESPERS/AMValveControl.h"
#include "beamline/VESPERS/XRFDetector.h"
#include "beamline/AMROI.h"
#include "beamline/VESPERS/SampleStageControl.h"
#include "beamline/VESPERS/VESPERSValveGroupControl.h"
#include "beamline/VESPERS/PIDLoopControl.h"
#include "beamline/VESPERS/VESPERSEndstation.h"

#include "util/AMErrorMonitor.h"

/// This class is the master class that holds EVERY control inside the VESPERS beamline.
class VESPERSBeamline : public AMBeamline
{
	Q_OBJECT
public:

	static VESPERSBeamline* vespers() {
		if(!instance_)
			instance_ = new VESPERSBeamline();
		return static_cast<VESPERSBeamline*>(instance_);
	}

	~VESPERSBeamline();

	// Accessing detectors.

	/// Returns a general AMDetector pointer of the single element XRF detector.
	AMDetector *vortexAM1E() const { return vortex1E_; }
	/// Returns the specific XRFDetector pointer of the single element XRF detector.
	XRFDetector *vortexXRF1E() const { return (XRFDetector *)vortex1E_; }
	/// Returns a general AMDetector pointer of the four element XRF detector.
	AMDetector *vortexAM4E() const { return vortex4E_; }
	/// Returns the specific XRFDetector pointer of the single element XRF detector.
	XRFDetector *vortexXRF4E() const { return (XRFDetector *)vortex4E_; }

	// Accessing control elements:

	// Pressure
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

	// Valves
	/// Returns the valve control for Front End section 1.
	AMControl *vvrFE1() const { return vvrFE1_; }
	/// Returns the valve control for Front End section 2a.
	AMControl *vvrFE2() const { return vvrFE2_; }
	/// Returns the valve control for M1.
	AMControl *vvrM1() const { return vvrM1_; }
	/// Returns the valve control for M2.
	AMControl *vvrM2() const { return vvrM2_; }
	/// Returns the valve control for BPM1.
	AMControl *vvrBPM1() const { return vvrBPM1_; }
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
	AMControl *vvrBeamTransfer() const { return vvrBeamTransfer_; }

	// The valve control.
	/// Returns the valve state modifying control for Front End section 1.
	AMValveControl *valveFE1() const { return valveFE1_; }
	/// Returns the valve state modifying control for Front End section 2a.
	AMValveControl *valveFE2() const { return valveFE2_; }
	/// Returns the valve state modifying control for M1.
	AMValveControl *valveM1() const { return valveM1_; }
	/// Returns the valve state modifying control for M2.
	AMValveControl *valveM2() const { return valveM2_; }
	/// Returns the valve state modifying control for BPM1.
	AMValveControl *valveBPM1() const { return valveBPM1_; }
	/// Returns the valve state modifying control for the Mono.
	AMValveControl *valveMono() const { return valveMono_; }
	/// Returns the valve state modifying control for the exit slits.
	AMValveControl *valveExitSlits() const { return valveExitSlits_; }
	/// Returns the valve state modifying control for the straight section.
	AMValveControl *valveStraightSection() const { return valveStraightSection_; }
	/// Returns the valve state modifying control for BPM3.
	AMValveControl *valveBPM3() const { return valveBPM3_; }
	/// Returns the valve state modifying control at the POE SSH.
	AMValveControl *valveSSH() const { return valveSSH_; }
	/// Returns the valve state modifying control for the beam transfer section (1).
	AMValveControl *valveBeamTransfer() const { return valveBeamTransfer_; }
	/// Returns the list of all the valves.
	QList<AMValveControl *> *valveList() const { return valveList_; }
	/// Returns the valve group control.
	VESPERSValveGroupControl *valves() const { return valves_; }

	// Ion pumps
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

	// Temperature
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

	// Water flow switches
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

	// Water flow transducers
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

	// Attenuation filters
	/// Returns the first 250 um filter. Note that if you wish to toggle the filters you must move(1) then move(0).  You must make two moves for one action.
	AMControl *filter250umA() const { return filter250umA_; }
	/// Returns the second 250 um filter. Note that if you wish to toggle the filters you must move(1) then move(0).  You must make two moves for one action.
	AMControl *filter250umB() const { return filter250umB_; }
	/// Returns the first 100 um filter. Note that if you wish to toggle the filters you must move(1) then move(0).  You must make two moves for one action.
	AMControl *filter100umA() const { return filter100umA_; }
	/// Returns the second 100 um filter. Note that if you wish to toggle the filters you must move(1) then move(0).  You must make two moves for one action.
	AMControl *filter100umB() const { return filter100umB_; }
	/// Returns the first 50 um filter. Note that if you wish to toggle the filters you must move(1) then move(0).  You must make two moves for one action.
	AMControl *filter50umA() const { return filter50umA_; }
	/// Returns the second 50 um filter. Note that if you wish to toggle the filters you must move(1) then move(0).  You must make two moves for one action.
	AMControl *filter50umB() const { return filter50umB_; }
	/// Returns the upper shutter filter. Note that if you wish to toggle the filters you must move(1) then move(0).  You must make two moves for one action.
	AMControl *filterShutterUpper() const { return filterShutterUpper_; }
	/// Returns the lower shutter filter. Note that if you wish to toggle the filters you must move(1) then move(0).  You must make two moves for one action.
	AMControl *filterShutterLower() const { return filterShutterLower_; }

	// Endstation controls.

	/// Returns the CCD motor control.
	AMControl *ccdMotor() const { return ccdMotor_; }
	/// Returns the microscope motor contorl.
	AMControl *microscopeMotor() const { return microscopeMotor_; }
	/// Returns the four element detector motor control.
	AMControl *fourElMotor() const { return fourElMotor_; }
	/// Returns the single element detector motor control.
	AMControl *singleElMotor() const { return singleElMotor_; }

	/// Returns the CCD motor control feedback.
	AMControl *ccdMotorfbk() const { return ccdMotorfbk_; }
	/// Returns the four element detector motor control feedback.
	AMControl *fourElMotorfbk() const { return fourElMotorfbk_; }
	/// Returns the single element detector motor control feedback.
	AMControl *singleElMotorfbk() const { return singleElMotorfbk_; }
	/// Returns the beam focus motor control feedback.
	AMControl *focusMotorfbk() const { return focusMotorfbk_; }

	/// Returns the encapsulation class for the endstation.
	VESPERSEndstation *endstation() const { return endstation_; }

	// Sample stage motor controls.

	// Psedomotors.
	/// Returns the horizontal sample stage control.
	AMControl *sampleStageHorizontal() const { return sampleStageHorizontal_; }
	/// Returns the vertical sample stage control.
	AMControl *sampleStageVertical() const { return sampleStageVertical_; }
	/// Returns the normal motor control.
	AMControl *sampleStageNormal() const { return sampleStageNormal_; }

	// Real motors.
	/// Returns the sample stage motor in the x-direction.
	AMControl *sampleStageX() const { return sampleStageX_; }
	/// Returns the sample stage motor in the y-direction.
	AMControl *sampleStageY() const { return sampleStageY_; }
	/// Returns the sample stage motor in the z-direction.
	AMControl *sampleStageZ() const { return sampleStageZ_; }

	// Real motors steps.
	/// Returns the sample stage motor step feedback for the x-direction.
	AMControl *sampleStageStepX() const { return sampleStageStepX_; }
	/// Returns the sample stage motor step feedback for the y-direction.
	AMControl *sampleStageStepY() const { return sampleStageStepY_; }
	/// Returns the sample stage motor step feedback for the z-direction.
	AMControl *sampleStageStepZ() const { return sampleStageStepZ_; }

	// The sample stage.
	/// Returns the sample stage control.
	SampleStageControl *sampleStage() const { return sampleStage_; }

	// Sample stage PID controls.
	/// Returns the PID control for the x-direction of the sample stage.
	AMControl *sampleStagePidX() const { return sampleStagePidX_; }
	/// Returns the PID control for the y-direction of the sample stage.
	AMControl *sampleStagePidY() const { return sampleStagePidY_; }
	/// Returns the PID control for the z-direction of the sample stage.
	AMControl *sampleStagPidZ() const { return sampleStagePidZ_; }

	/// Returns the sample stage PID control.
	PIDLoopControl *sampleStagePID() const { return sampleStagePID_; }

	// These are the single element vortex controls.

	/// Returns the status control for the single element vortex detector.
	AMControl *status1E() const { return status1E_; }
	/// Returns the elapsed time control for the single element vortex detector.
	AMControl *elapsedTime1E() const { return elapsedTime1E_; }
	/// Returns the integration time control for the single element vortex detector.
	AMControl *integrationTime1E() const { return integrationTime1E_; }
	/// Returns the live time control for the single element vortex detector.
	AMControl *liveTime1E() const { return liveTime1E_; }
	/// Returns the start control for the single element vortex detector.
	AMControl *start1E() const { return start1E_; }
	/// Returns the stop control for the single element vortex detector.
	AMControl *stop1E() const { return stop1E_; }
	/// Returns the dead time control for the single element vortex detector.
	AMControl *deadTime1E() const { return deadTime1E_; }
	/// Returns the maximum energy control for the single element vortex detector.
	AMControl *maxEnergy1E() const { return maxEnergy1E_; }
	/// Returns the spectrum refresh rate control for the single element vortex detector.
	AMControl *mcaUpdateRate1E() const { return mcaUpdateRate1E_; }
	/// Returns the peaking time control for the single element vortex detector.
	AMControl *peakingTime1E() const { return peakingTime1E_; }
	/// Returns the spectrum control for the single element vortex detector.
	AMControl *spectrum1E() const { return spectrum1E_; }

	// These are the four element vortex controls.

	/// Returns the status control for the four element vortex detector.
	AMControl *status4E() const { return status4E_; }
	/// Returns the elapsed time control for the four element vortex detector.
	AMControl *elapsedTime4E() const { return elapsedTime4E_; }
	/// Returns the integration time control for the four element vortex detector.
	AMControl *integrationTime4E() const { return integrationTime4E_; }
	/// Returns the live time control for the four element vortex detector.
	AMControl *liveTime4E() const { return liveTime4E_; }
	/// Returns the start control for the four element vortex detector.
	AMControl *start4E() const { return start4E_; }
	/// Returns the stop control for the four element vortex detector.
	AMControl *stop4E() const { return stop4E_; }
	/// Returns the maximum energy control for the four element vortex detector.
	AMControl *maxEnergy4E() const { return maxEnergy4E_; }
	/// Returns the spectrum refresh rate control for the four element vortex detector.
	AMControl *mcaUpdateRate4E() const { return mcaUpdateRate4E_; }
	/// Returns the peaking time control for the four element vortex detector.
	AMControl *peakingTime4E() const { return peakingTime4E_; }
	/// Returns the dead time of element 1 for the four element vortex detector.
	AMControl *deadTime14E() const { return deadTime14E_; }
	/// Returns the dead time of element 2 for the four element vortex detector.
	AMControl *deadTime24E() const { return deadTime24E_; }
	/// Returns the dead time of element 3 for the four element vortex detector.
	AMControl *deadTime34E() const { return deadTime34E_; }
	/// Returns the dead time of element 4 for the four element vortex detector.
	AMControl *deadTime44E() const { return deadTime44E_; }
	/// Returns dead time set with all four dead time sets.
	AMControlSet *deadTime4E() const { return deadTime4E_; }
	/// Returns raw spectrum 1 for the four element vortex detector.
	AMControl *rawSpectrum14E() const { return rawSpectrum14E_; }
	/// Returns raw spectrum 2 for the four element vortex detector.
	AMControl *rawSpectrum24E() const { return rawSpectrum24E_; }
	/// Returns raw spectrum 3 for the four element vortex detector.
	AMControl *rawSpectrum34E() const { return rawSpectrum34E_; }
	/// Returns raw spectrum 4 for the four element vortex detector.
	AMControl *rawSpectrum44E() const { return rawSpectrum44E_; }
	/// Returns the raw spectrum set with all four raw spectra in the four element detector.
	AMControlSet *spectra4E() const { return spectra4E_; }

	// These Control Sets are logical groups of controls that are commonly used by different Acquaman components

	/// Returns the pressure control set.
	AMControlSet *pressureSet() const { return pressureSet_; }
	/// Returns the valve control set.
	AMControlSet *valveSet() const { return valveSet_; }
	/// Returns the ion pump set.
	AMControlSet *ionPumpSet() const { return ionPumpSet_; }
	/// Returns the temperature control set.
	AMControlSet *temperatureSet() const { return temperatureSet_; }
	/// Returns the water flow switches control set.
	AMControlSet *flowSwitchSet() const { return flowSwitchSet_; }
	/// Returns the water flow transducer control set.
	AMControlSet *flowTransducerSet() const { return flowTransducerSet_; }
	/// Returns the endstation motor control set.
	AMControlSet *endstationMotorSet() const { return endstationMotorSet_; }
	/// Returns the sample stage motor control set.
	AMControlSet *sampleStageMotorSet() const { return sampleStageMotorSet_; }
	/// Returns the single element vortex control set.
	AMControlSet *vortex1EControls() const { return vortex1EControls_; }
	/// Returns the four element vortex control set.
	AMControlSet *vortex4EControls() const { return vortex4EControls_; }
	/// Returns the filter control set.
	AMControlSet *filterSet() const { return filterSet_; }

	// These are PVs that are needed for random small jobs around the beamline.

	/// Returns the process variable for the microscope light.
	AMProcessVariable *micLight() const { return micLight_; }
	/// Returns the laser on/off control.
	AMControl *laserPower() const { return laserPower_; }
	/// Returns the process variable for the CCD file path.  Needs special write function to get the info in or out.  See VESPERSEndstationView for example.
	AMProcessVariable *ccdPath() const { return ccdPath_; }
	/// Returns the process variable for the CCD file name.  Needs special write function to get the info in or out.  See VESPERSEndstationView for example.
	AMProcessVariable *ccdFile() const { return ccdFile_; }
	/// Returns the process variable for the CCD file number.
	AMProcessVariable *ccdNumber() const { return ccdNumber_; }
	/// Returns the process variable for the Pseudo-motor reset.
	AMProcessVariable *resetPseudoMotors() const { return resetPseudoMotors_; }

signals:

protected slots:
	/// Sets up any connections for the pressure controls once the whole set is connected.  Also checks if there are any errors with everything started up.
	void pressureConnected(bool connected);
	/// Slot used to deal with pressure errors.
	void pressureError();
	/// Sets up any connections for the valve controls once the whole set is connected.  Also checks if there are any errors with everything started up.
	void valveConnected(bool connected);
	/// Slot used to deal with valve errors.
	void valveError();
	/// Sets up any connections for the ion pump controls once the whole set is connected.  Also checks if there are any errors with everything started up.
	void ionPumpConnected(bool connected);
	/// Slot used to deal with ion pump errors.
	void ionPumpError();
	/// Sets up any connections for the temperature controls once the whole set is connected.  Also checks if there are any errors with everything started up.
	void temperatureConnected(bool connected);
	/// Slot used to deal with temperature errors.
	void temperatureError();
	/// Sets up any connections for the flow switch controls once the whole set is connected.  Also checks if there are any errors with everything started up.
	void flowSwitchConnected(bool connected);
	/// Slot used to deal with flow switch errors.
	void flowSwitchError();
	/// Sets up any connections for the flow transducers controls once the whole set is connected.  Also checks if there are any errors with everything started up.
	void flowTransducerConnected(bool connected);
	/// Slot used to deal with flow transducer errors.
	void flowTransducerError();
	/// Slot used to deal with single element detector errors.
	void singleElVortexError(bool isConnected);
	/// Slot used to deal with four element vortex detector errors.
	void fourElVortexError(bool isConnected);
	/// Slot used to dead with sample stage motor errors.
	void sampleStageError();

protected:
	/// Sets up the readings such as pressure, flow switches, temperature, etc.
	void setupDiagnostics();
	/// Sets up logical groupings of controls into sets.
	void setupControlSets();
	/// Sets up all the controls and PVs for the VESPERSEndstationView.
	void setupEndstation();
	/// Sets up all the controls for the single element detector.
	void setupSingleElementDetector();
	/// Sets up all the controls for the four element detector.
	void setupFourElementDetector();
	/// Sets up the sample stage motors.
	void setupSampleStage();
	/// Helper function.  Takes in a base name, an ROI number, and number of elements and returns a pointer to a new AMROI.  Creates PVs for the name, low limit, high limit, and current value.
	AMROI *createROI(int numElements, int roiNum, QString baseName);

	/// Constructor. This is a singleton class; access it through VESPERSBeamline::vespers().
	VESPERSBeamline();

	// Detectors.
	AMDetector *vortex1E_;
	AMDetector *vortex4E_;

	// End detectors.

	// Beamline General.
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
	AMControl *vvrFE2_;
	AMControl *vvrM1_;
	AMControl *vvrM2_;
	AMControl *vvrBPM1_;
	AMControl *vvrMono_;
	AMControl *vvrExitSlits_;
	AMControl *vvrStraightSection_;
	AMControl *vvrBPM3_;
	AMControl *vvrSSH_;
	AMControl *vvrBeamTransfer_;

	// The Valve controls that change the state of the valve.
	AMValveControl *valveFE1_;
	AMValveControl *valveFE2_;
	AMValveControl *valveM1_;
	AMValveControl *valveM2_;
	AMValveControl *valveBPM1_;
	AMValveControl *valveMono_;
	AMValveControl *valveExitSlits_;
	AMValveControl *valveStraightSection_;
	AMValveControl *valveBPM3_;
	AMValveControl *valveSSH_;
	AMValveControl *valveBeamTransfer_;
	QList<AMValveControl *> *valveList_;
	VESPERSValveGroupControl *valves_;

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

	// Control sets.
	AMControlSet *pressureSet_;
	AMControlSet *valveSet_;
	AMControlSet *ionPumpSet_;
	AMControlSet *temperatureSet_;
	AMControlSet *flowSwitchSet_;
	AMControlSet *flowTransducerSet_;
	AMControlSet *endstationMotorSet_;
	AMControlSet *sampleStageMotorSet_;
	AMControlSet *filterSet_;

	// Beam attenuation filters.
	AMControl *filter250umA_;
	AMControl *filter250umB_;
	AMControl *filter100umA_;
	AMControl *filter100umB_;
	AMControl *filter50umA_;
	AMControl *filter50umB_;
	AMControl *filterShutterUpper_;
	AMControl *filterShutterLower_;

	// End General Controls.

	// Endstation controls
	// The controls used for the control window.
	AMControl *ccdMotor_;
	AMControl *microscopeMotor_;
	AMControl *fourElMotor_;
	AMControl *singleElMotor_;

	// The process variables that have the feedback value used for the button.  The microscope doesn't need one because it's encoder doesn't work.
	AMControl *ccdMotorfbk_;
	AMControl *fourElMotorfbk_;
	AMControl *singleElMotorfbk_;
	AMControl *focusMotorfbk_;

	// Microscope light PV.
	AMProcessVariable *micLight_;

	// Laser on/off PV.
	AMControl *laserPower_;

	// Various CCD file path PVs.
	AMProcessVariable *ccdPath_;
	AMProcessVariable *ccdFile_;
	AMProcessVariable *ccdNumber_;

	// Pseudo-motor reset PV.
	AMProcessVariable *resetPseudoMotors_;

	// Endstation encapulation.
	VESPERSEndstation *endstation_;

	// End Endstation controls.

	// Sample stage controls.
	// Pseudo-motors
	AMControl *sampleStageHorizontal_;
	AMControl *sampleStageVertical_;
	AMControl *sampleStageNormal_;

	// Physical motors.
	AMControl *sampleStageX_;
	AMControl *sampleStageY_;
	AMControl *sampleStageZ_;

	// Step feedback.
	AMControl *sampleStageStepX_;
	AMControl *sampleStageStepY_;
	AMControl *sampleStageStepZ_;

	SampleStageControl *sampleStage_;

	AMControl *sampleStagePidX_;
	AMControl *sampleStagePidY_;
	AMControl *sampleStagePidZ_;

	PIDLoopControl *sampleStagePID_;

	// End sample stage controls.

	// Single element vortex detector.

	AMControl *status1E_;
	AMControl *elapsedTime1E_;
	AMControl *integrationTime1E_;
	AMControl *liveTime1E_;
	AMControl *start1E_;
	AMControl *stop1E_;
	AMControl *deadTime1E_;
	AMControl *maxEnergy1E_;
	AMControl *mcaUpdateRate1E_;
	AMControl *peakingTime1E_;
	AMControl *spectrum1E_;

	AMControlSet *vortex1EControls_;
	// End single element vortex detector.

	// Four element vortex detector.

	AMControl *status4E_;
	AMControl *elapsedTime4E_;
	AMControl *integrationTime4E_;
	AMControl *liveTime4E_;
	AMControl *start4E_;
	AMControl *stop4E_;
	AMControl *maxEnergy4E_;
	AMControl *mcaUpdateRate4E_;
	AMControl *peakingTime4E_;
	AMControl *deadTime14E_;
	AMControl *deadTime24E_;
	AMControl *deadTime34E_;
	AMControl *deadTime44E_;
	AMControl *rawSpectrum14E_;
	AMControl *rawSpectrum24E_;
	AMControl *rawSpectrum34E_;
	AMControl *rawSpectrum44E_;

	AMControlSet *deadTime4E_;
	AMControlSet *spectra4E_;
	AMControlSet *vortex4EControls_;
	// End four element vortex detector.
};

#endif // VESPERSBEAMLINE_H
