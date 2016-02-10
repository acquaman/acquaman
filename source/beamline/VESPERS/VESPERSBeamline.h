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

#ifndef VESPERSBEAMLINE_H
#define VESPERSBEAMLINE_H

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/AMControlSet.h"
#include "beamline/VESPERS/VESPERSPIDLoopControl.h"
#include "beamline/VESPERS/VESPERSMonochromator.h"
#include "beamline/VESPERS/VESPERSIntermediateSlits.h"
#include "beamline/VESPERS/VESPERSEndstation.h"
#include "beamline/CLS/CLSVariableIntegrationTime.h"
#include "beamline/VESPERS/VESPERSRoperCCDDetector.h"
#include "beamline/VESPERS/VESPERSMarCCDDetector.h"
#include "beamline/VESPERS/VESPERSPilatusCCDDetector.h"
#include "beamline/VESPERS/VESPERS13ElementGeDetector.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "application/VESPERS/VESPERS.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/CLS/CLSPseudoMotorGroup.h"
#include "beamline/CLS/CLSPseudoMotorControl.h"

#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

#include "beamline/VESPERS/VESPERSSingleElementVortexDetector.h"
#include "beamline/VESPERS/VESPERSFourElementVortexDetector.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"

#define VESPERSBEAMLINE_PRESSURE_TOO_HIGH 67800
#define VESPERSBEAMLINE_VALVES_CLOSED 67801
#define VESPERSBEAMLINE_TEMPERATURE_TOO_HIGH 67802
#define VESPERSBEAMLINE_WATER_FLOW_SWITCH_TRIP 67803
#define VESPERSBEAMLINE_WATER_FLOW_TOO_LOW 67804
#define VESPERSBEAMLINE_ION_PUMP_TRIP 67805
#define VESPERSBEAMLINE_SINGLE_ELEMENT_NOT_CONNECTED 67806
#define VESPERSBEAMLINE_FOUR_ELEMENT_NOT_CONNECTED 67807
#define VESPERSBEAMLINE_SAMPLE_STAGE_NOT_CONNECTED 67808

#define VESPERSBEAMLINE_VALVE_OPEN 1
#define VESPERSBEAMLINE_VALVE_CLOSED 4

/// This class is the master class that holds EVERY control inside the VESPERS beamline.
class VESPERSBeamline : public CLSBeamline
{
	Q_OBJECT
public:

	/// Returns the instance of the beamline that has been created.

	static VESPERSBeamline* vespers()
	{
		if(!instance_){
			instance_ = new VESPERSBeamline();
			instance_->initializeBeamlineSupport();
		}
		return static_cast<VESPERSBeamline*>(instance_);
	}

	virtual ~VESPERSBeamline();

	/// Returns a string with a human readable text of what is important about this detector.
	virtual QString details() const;

	// Beam selection functions.
	/// Returns the current beam in use by the beamline.
	VESPERS::Beam currentBeam() const { return beam_; }

	// Accessing detectors.
	/// Returns a general AMDetector pointer of the Roper CCD.
	AMDetector *roperCCD() const { return roperCCD_; }
	/// Returns the specific pointer to the Roper CCD.
	VESPERSRoperCCDDetector *vespersRoperCCD() const { return (VESPERSRoperCCDDetector *)roperCCD_; }
	/// Returns a general AMDetector pointer of the Mar CCD.
	AMDetector *marCCD() const { return roperCCD_; }
	/// Returns the specific pointer to the Mar CCD.
	VESPERSMarCCDDetector *vespersMarCCD() const { return (VESPERSMarCCDDetector *)marCCD_; }
	/// Returns a general AMDetector pointer of the Pilatus CCD.
	AMDetector *pilatusAreaDetector() const { return pilatusAreaDetector_; }
	/// Returns the specific pointer to the Pilatus CCD.
	VESPERSPilatusCCDDetector *vespersPilatusAreaDetector() const { return (VESPERSPilatusCCDDetector *)pilatusAreaDetector_; }

	/// Returns the single element vortex detector.
	AMDetector *singleElementVortexDetector() const { return singleElementVortexDetector_; }
	/// Returns the single element vortex detector as its full type.
	VESPERSSingleElementVortexDetector *vespersSingleElementVortexDetector() const { return singleElementVortexDetector_; }
	/// Returns the four element vortex detector.
	AMDetector *fourElementVortexDetector() const { return fourElementVortexDetector_; }
	/// Returns the four element vortex detector as its full type.
	VESPERSFourElementVortexDetector *vespersFourElementVortexDetector() const { return fourElementVortexDetector_; }
	/// Returns the 13 element germanium detector.
	AMDetector *ge13ElementDetector() const { return ge13ElementDetector_; }
	/// Returns the 13 element germanium detector as its full type.
	VESPERS13ElementGeDetector *vespersGe13ElementDetector() const { return ge13ElementDetector_; }

	// Accessing control elements:
	/// Returns the monochromator abstraction for the VESPERS beamline.
	VESPERSMonochromator *mono() const { return mono_; }

	/// Returns the intermediate slits object.
	VESPERSIntermediateSlits *intermediateSlits() const { return intermediateSlits_; }

	/// Returns the scaler.
	CLSSIS3820Scaler *scaler() const { return scaler_; }

	// The photon and safety shutters.
	/// Returns the first photon shutter.
	CLSExclusiveStatesControl *photonShutter1() const { return photonShutter1_; }
	/// Returns the second photon shutter.
	CLSExclusiveStatesControl *photonShutter2() const { return photonShutter2_; }
	/// Returns the first safety shutter.
	CLSExclusiveStatesControl *safetyShutter1() const { return safetyShutter1_; }
	/// Returns the second safety shutter.
	CLSExclusiveStatesControl *safetyShutter2() const { return safetyShutter2_; }

	// Because there is some logic involved with opening and closing the shutters each shutter has it's own method for opening and closing.
	/// Opens the first photon shutter.  Returns whether the beamline was in the proper state before changing the shutter.
	bool openPhotonShutter1();
	/// Closes the first photon shutter.  Returns whether the beamline was in the proper state before changing the shutter.
	bool closePhotonShutter1();
	/// Opens the second photon shutter.  Returns whether the beamline was in the proper state before changing the shutter.
	bool openPhotonShutter2();
	/// Closes the second photon shutter.  Returns whether the beamline was in the proper state before changing the shutter.
	bool closePhotonShutter2();
	/// Opens the first safety shutter.  Returns whether the beamline was in the proper state before changing the shutter.
	bool openSafetyShutter1();
	/// Closes the second safety shutter.  Returns whether the beamline was in the proper state before changing the shutter.
	bool closeSafetyShutter1();
	/// Opens the first safety shutter.  Returns whether the beamline was in the proper state before changing the shutter.
	bool openSafetyShutter2();
	/// Closes the second safety shutter.  Returns whether the beamline was in the proper state before changing the shutter.
	bool closeSafetyShutter2();


	// The endstation.
	/// Returns the endstation model.
	VESPERSEndstation *endstation() const { return endstation_; }

	// End of endstation.

	// Beam selection motor.
	/// Returns the control for the beam selection motor.
	AMControl *beamSelectionMotor() const { return beamSelectionMotor_; }

	// POE status and enable.
	/// Returns the POE status control.
	AMControl *poeStatusControl() const { return poeBeamStatus_; }
	/// Returns the POE status enable control.
	AMControl *poeStatusEnableControl() const { return poeBeamStatusEnable_; }
	/// Sets the POE status enable to \param enabled.
	void setPOEStatusEnable(bool enabled);
	/// Returns the POE beam status enable.
	bool poeStatusEnable() const { return int(poeBeamStatusEnable_->value()) == 0; }
	/// Returns the POE beam status.
	bool poeStatus() const { return int(poeBeamStatus_->value()) == 1; }

	// The helper controls for changing the dwell time for each region.
	/// Returns the control in charge of changing the dwell time trigger for changing the dwell time between regions.
	AMControl *dwellTimeTrigger() const { return dwellTimeTrigger_; }
	/// Returns the control holding the confirmed flag while setting the dwell time between regions.
	AMControl *dwellTimeConfirmed() const { return dwellTimeConfirmed_; }

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

	// Some methods to control the group of valves.
	/// Status: whether all the valves are open or not.
	bool allValvesOpen() const;
	/// Opens valve at a given index.
	void openValve(int index);
	/// Closes valve at a given index.
	void closeValve(int index);


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

	// Sample stage motor controls.

	// Psedomotors.
	/// Returns the horizontal sample stage control.
	AMControl *sampleStageHorizontal() const { return sampleStageHorizontal_; }
	/// Returns the vertical sample stage control.
	AMControl *sampleStageVertical() const { return sampleStageVertical_; }
	/// Returns the normal motor control.
	AMControl *sampleStageNormal() const { return sampleStageNormal_; }
	/// Returns the horizontal wire stage control.
	AMControl *wireStageHorizontal() const { return wireStageHorizontal_; }
	/// Returns the vertical wire stage control.
	AMControl *wireStageVertical() const { return wireStageVertical_; }
	/// Returns the normal wire motor control.
	AMControl *wireStageNormal() const { return wireStageNormal_; }

	// Real motors.
	/// Returns the sample stage motor in the x-direction.
	AMControl *sampleStageX() const { return sampleStageX_; }
	/// Returns the sample stage motor in the y-direction.
	AMControl *sampleStageY() const { return sampleStageY_; }
	/// Returns the sample stage motor in the z-direction.
	AMControl *sampleStageZ() const { return sampleStageZ_; }

	// Attocube motors.
	/// Returns the horizontal attocube control.
	AMControl *attoStageHorizontal() const { return attoStageHorizontal_; }
	/// Returns the vertical attocube control.
	AMControl *attoStageVertical() const { return attoStageVertical_; }
	/// Returns the normal attocube control.
	AMControl *attoStageNormal() const { return attoStageNormal_; }
	/// Returns the x-direction attocube control.
	AMControl *attoStageX() const { return attoStageX_; }
	/// Returns the z-direction attocube control.
	AMControl *attoStageZ() const { return attoStageZ_; }
	/// Returns the y-direction attocube control.
	AMControl *attoStageY() const { return attoStageY_; }
	/// Returns the phi rotation attocube control.
	AMControl *attoStageRz() const { return attoStageRz_; }
	/// Returns the theta tilt attocube control.
	AMControl *attoStageRy() const { return attoStageRy_; }
	/// Returns the psi tilt attocube control.
	AMControl *attoStageRx() const { return attoStageRx_; }
	/// Returns the big beam x control.
	AMControl *bigBeamX() const { return bigBeamX_; }
	/// Returns the big beam z control.
	AMControl *bigBeamZ() const { return bigBeamZ_; }

	// The motor group and specific motor group object getters.
	/// Helper method that returns a name of the motor group object given a VESPERS::Motor enum.
	QString motorGroupName(VESPERS::Motors motor) const;
	/// Returns the CLSPseudoMotorGroup pointer.
	CLSPseudoMotorGroup *motorGroup() const { return motorGroup_; }
	/// Returns the pseudo sample stage motor group object.
	AMMotorGroupObject *pseudoSampleStageMotorGroupObject() const { return motorGroup_->motorGroupObject("Sample Stage - H, V, N"); }
	/// Returns the real sample stage motor group object.
	AMMotorGroupObject *realSampleStageMotorGroupObject() const { return motorGroup_->motorGroupObject("Sample Stage - X, Z, Y"); }
	/// Returns the pseudo wire stage motor group object.
	AMMotorGroupObject *pseudoWireStageMotorGroupObject() const { return motorGroup_->motorGroupObject("Wire Stage - H, V, N"); }
	/// Returns the pseudo attocube stage motor group object.
	AMMotorGroupObject *pseudoAttocubeStageMotorGroupObject() const { return motorGroup_->motorGroupObject("Attocube Stage - H, V, N"); }
	/// Returns the real attocube stage motor group object.
	AMMotorGroupObject *realAttocubeStageMotorGroupObject() const { return motorGroup_->motorGroupObject("Attocube Stage - X, Z, Y"); }
	/// Returns the Rx rotation attocube motor group object.
	AMMotorGroupObject *attocubeRxMotorGroupObject() const { return motorGroup_->motorGroupObject("Attocube Stage - Rx"); }
	/// Returns the Ry rotation attocube motor group object.
	AMMotorGroupObject *attocubeRyMotorGroupObject() const { return motorGroup_->motorGroupObject("Attocube Stage - Ry"); }
	/// Returns the Rz rotation attocube motor group object.
	AMMotorGroupObject *attocubeRzMotorGroupObject() const { return motorGroup_->motorGroupObject("Attocube Stage - Rz"); }
	/// Returns the Big Beam motor group object.
	AMMotorGroupObject *bigBeamMotorGroupObject() const { return motorGroup_->motorGroupObject("Big Beam - X, Z"); }

	// The reset controls for the pseudo motors.
	/// Returns the pseudo sample stage reset control.
	AMControl *pseudoSampleStageResetControl() const { return pseudoSampleStageResetControl_; }
	/// Returns the real sample stage reset control.
	AMControl *realSampleStageResetControl() const { return realSampleStageResetControl_; }
	/// Returns the pseudo attocube stage reset control.
	AMControl *pseudoAttoStageResetControl() const { return pseudoAttoStageResetControl_; }
	/// Returns the real attocube stage reset control.
	AMControl *realAttoStageResetControl() const { return realAttoStageResetControl_; }
	/// Returns the pseudo wire stage reset control.
	AMControl *pseudoWireStageResetControl() const { return pseudoWireStageResetControl_; }

	// Sample stage PID controls.
	/// Returns the PID control for the x-direction of the sample stage.
	AMControl *sampleStagePidX() const { return sampleStagePidX_; }
	/// Returns the PID control for the y-direction of the sample stage.
	AMControl *sampleStagePidY() const { return sampleStagePidY_; }
	/// Returns the PID control for the z-direction of the sample stage.
	AMControl *sampleStagPidZ() const { return sampleStagePidZ_; }

	/// Returns the sample stage PID control.
	VESPERSPIDLoopControl *sampleStagePID() const { return sampleStagePID_; }

	// Wire stage PID controls.
	/// Returns the PID control for the x-direction of the wire stage.
	AMControl *wireStagePidX() const { return wireStagePidX_; }
	/// Returns the PID control for the y-direction of the wire stage.
	AMControl *wireStagePidY() const { return wireStagePidY_; }
	/// Returns the PID control for the z-direction of the wire stage.
	AMControl *wireStagPidZ() const { return wireStagePidZ_; }

	/// Returns the wire stage PID control.
	VESPERSPIDLoopControl *wireStagePID() const { return wireStagePID_; }

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
	/// Returns the sample stage motor control set.
	AMControlSet *sampleStageMotorSet() const { return sampleStageMotorSet_; }

	// This is where the controls and PVs for scanning are.  They are reproduced somewhat because my encapsulation classes don't return AMControls.
	/// Returns the energy control.
	AMControl *energy() const { return mono_->EaControl(); }
	/// Returns the relative energy control.
	AMControl *energyRelative() const { return mono_->delEControl(); }
	/// Returns the k control used for EXAFS scans.
	AMControl *kControl() const { return mono_->KControl(); }
	/// Returns the master dwell time control.
	AMControl *masterDwellTime() const { return masterDwellTime_; }

	//////////////////////////////////////////////////////////////////////////////////////
	// Actions

	/// Creates an action that changes the beam.  Returns 0 if unable to create.
	AMAction3 *createBeamChangeAction(VESPERS::Beam beam);
	/// Creates an action that opens the shutters to act like a Beam On.
	AMAction3 *createBeamOnAction();
	/// Creates an action that closes the shutters to act like a Beam Off.
	AMAction3 *createBeamOffAction();

	// End of Actions
	//////////////////////////////////////////////////////////////////////////////////////

signals:
	/// Notifier that the beam has been changed.
	void currentBeamChanged(VESPERS::Beam);
	/// Notifier that passes on that the beam has gone down.
	void beamDumped();
	/// Notifier of the current state of the pressures on the beamline.  Passes false if ANY of the pressures falls below its setpoint.
	void pressureStatus(bool);
	/// Notifier of the current state of the valves on the beamline.  Passes false if ANY of the valves are closed.
	void valveStatus(bool);
	/// Notifier of the current state of the ion pumps on the beamline.  Passes false if ANY of the ion pumps fail.
	void ionPumpStatus(bool);
	/// Notifier of the current state of the temperature on the beamline.  Passes false if ANY of the temperatures rises above its setpoint.
	void temperatureStatus(bool);
	/// Notifier of the current state of the flow switches on the beamline.  Passes false if ANY of the flow switches are disabled.
	void flowSwitchStatus(bool);
	/// Notifier of the current state of the flow transducers on the beamline.  Passes false if ANY of the flow rates fall below its setpoint.
	void flowTransducerStatus(bool);
	/// Notifier that the POE status has changed.  Passes the new state.
	void poeStatusChanged(bool);
	/// Notifier that the POE status enable has changed.
	void poeStatusEnableChanged();

public slots:
	/// Class that opens all the valves on the beamline in sequence.
	void openAllValves();
	/// Class that closes all the valves on the beamline in sequence.
	void closeAllValves();

protected slots:
	/// Determines is currently active on startup.  Also keeps track if the beam is changed outside of Acquaman.  Beam is set to None is if not inside any of the tolerances for the known beam positions.
	void determineBeam();
	/// Helper slot to determine what the beam is on startup.
	void onBeamSelectionMotorConnected();

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
	/// Slot used to dead with sample stage motor errors.
	void sampleStageError();
	/// Determines whether the beam has dumped or not.
	void onPOEStatusChanged();

	/// Helper slot that handles opening the next valve.
	void openAllValvesHelper();
	/// Helper slot that handles closing the next valve.
	void closeAllValvesHelper();


protected:
	/// Sets up the readings such as pressure, flow switches, temperature, etc.
	void setupDiagnostics();
	/// Sets up logical groupings of controls into sets.
	void setupControlSets();
	/// Sets up all the controls and PVs for the VESPERSEndstationView.
	void setupEndstation();
	/// Sets up all the detectors.
	void setupDetectors();
	/// Sets up the sample stage motors.
	void setupSampleStage();
	/// Sets up mono settings.
	void setupMono();
	/// Sets up various beamline components.
	void setupComponents();
	/// Sets up the exposed actions.
	void setupExposedControls();
	/// Sets up the exposed detectors.
	void setupExposedDetectors();
	/// Sets up the motor group for the various sample stages.
	void setupMotorGroup();
	/// Sets up all of the detectors that need to be added to scans that aren't a part of typical detectors.  This may just be temporary, not sure.
	void setupControlsAsDetectors();

	/// Constructor. This is a singleton class, access it through VESPERSBeamline::vespers().
	VESPERSBeamline();

	// Detectors.
	CLSBasicCompositeScalerChannelDetector *splitIonChamber_;
	CLSBasicScalerChannelDetector *preKBIonChamber_;
	CLSBasicScalerChannelDetector *miniIonChamber_;
	CLSBasicScalerChannelDetector *postIonChamber_;
	VESPERSSingleElementVortexDetector *singleElementVortexDetector_;
	VESPERSFourElementVortexDetector *fourElementVortexDetector_;
	VESPERS13ElementGeDetector *ge13ElementDetector_;
	VESPERSRoperCCDDetector *roperCCD_;
	VESPERSMarCCDDetector *marCCD_;
	VESPERSPilatusCCDDetector *pilatusAreaDetector_;

	// End detectors.

	// VESPERS monochromator.
	VESPERSMonochromator *mono_;

	// Intermediate slits.
	VESPERSIntermediateSlits *intermediateSlits_;

	// Scaler.
	CLSSIS3820Scaler *scaler_;

	// POE status and enable.
	// The POE beam status control.
	AMControl *poeBeamStatus_;
	// The POE beam status enable.
	AMControl *poeBeamStatusEnable_;

	// The shutters.
	CLSExclusiveStatesControl *photonShutter1_;
	CLSExclusiveStatesControl *photonShutter2_;
	CLSExclusiveStatesControl *safetyShutter1_;
	CLSExclusiveStatesControl *safetyShutter2_;

	// Endstation
	VESPERSEndstation *endstation_;

	// Dwell time control helper functions for the dwell time.
	AMControl *dwellTimeTrigger_;
	AMControl *dwellTimeConfirmed_;

	// Beam selection members.
	// The current beam in use by the beamline.
	VESPERS::Beam beam_;
	// Pointer to the motor that controls which beam makes it down the beamline.
	AMControl *beamSelectionMotor_;
	// Look up table with the beam and its position.
	QHash<VESPERS::Beam, double> beamPositions_;

	// End of Beam selection members.

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

	// Index used to keep track of which index we're on while opening or closing all the valves.  Always -1 except when opening or closing valves.
	int valveIndex_;

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
	AMControlSet *sampleStageMotorSet_;

	// End General Controls.


	// Sample stage controls.
	// CLS pseudo-motors.
	AMControl *sampleStageHorizontal_;
	AMControl *sampleStageVertical_;
	AMControl *sampleStageNormal_;

	AMControl *wireStageHorizontal_;
	AMControl *wireStageVertical_;
	AMControl *wireStageNormal_;

	// Physical motors.
	AMControl *sampleStageX_;
	AMControl *sampleStageY_;
	AMControl *sampleStageZ_;

	// Atto cube motors.
	AMControl *attoStageHorizontal_;
	AMControl *attoStageVertical_;
	AMControl *attoStageNormal_;
	AMControl *attoStageX_;
	AMControl *attoStageZ_;
	AMControl *attoStageY_;
	AMControl *attoStageRz_;
	AMControl *attoStageRy_;
	AMControl *attoStageRx_;

	// Big beam.
	AMControl *bigBeamX_;
	AMControl *bigBeamZ_;

	// The reset controls for each sample stage.
	AMControl *pseudoSampleStageResetControl_;
	AMControl *realSampleStageResetControl_;
	AMControl *pseudoAttoStageResetControl_;
	AMControl *realAttoStageResetControl_;
	AMControl *pseudoWireStageResetControl_;

	// Motor group.  Binds all the motors for scanning together.
	CLSPseudoMotorGroup *motorGroup_;

	// The PID loop controls.
	AMControl *sampleStagePidX_;
	AMControl *sampleStagePidY_;
	AMControl *sampleStagePidZ_;
	AMControl *wireStagePidX_;
	AMControl *wireStagePidY_;
	AMControl *wireStagePidZ_;

	VESPERSPIDLoopControl *sampleStagePID_;
	VESPERSPIDLoopControl *wireStagePID_;

	// End sample stage controls.

	// Scanning settings.
	AMControl *masterDwellTime_;

	// The energy setpoint control.
	AMControl *energySetpointControl_;

	// Extra controls.
	AMControl *singleElementVortexDeadTimeControl_;
	AMControl *singleElementVortexRealTimeControl_;
	AMControl *singleElementVortexLiveTimeControl_;
	AMControl *singleElementVortexFastPeaksControl_;
	AMControl *singleElementVortexSlowPeaksControl_;
	AMControl *singleElementVortexRawSpectrumControl_;

	AMControl *fourElementVortexDeadTime1Control_;
	AMControl *fourElementVortexDeadTime2Control_;
	AMControl *fourElementVortexDeadTime3Control_;
	AMControl *fourElementVortexDeadTime4Control_;
	AMControl *fourElementVortexRealTime1Control_;
	AMControl *fourElementVortexRealTime2Control_;
	AMControl *fourElementVortexRealTime3Control_;
	AMControl *fourElementVortexRealTime4Control_;
	AMControl *fourElementVortexLiveTime1Control_;
	AMControl *fourElementVortexLiveTime2Control_;
	AMControl *fourElementVortexLiveTime3Control_;
	AMControl *fourElementVortexLiveTime4Control_;
	AMControl *fourElementVortexFastPeaks1Control_;
	AMControl *fourElementVortexFastPeaks2Control_;
	AMControl *fourElementVortexFastPeaks3Control_;
	AMControl *fourElementVortexFastPeaks4Control_;
	AMControl *fourElementVortexSlowPeaks1Control_;
	AMControl *fourElementVortexSlowPeaks2Control_;
	AMControl *fourElementVortexSlowPeaks3Control_;
	AMControl *fourElementVortexSlowPeaks4Control_;
	AMControl *fourElementVortexRawSpectrumControl1_;
	AMControl *fourElementVortexRawSpectrumControl2_;
	AMControl *fourElementVortexRawSpectrumControl3_;
	AMControl *fourElementVortexRawSpectrumControl4_;

	AMControl *ge13ElementRawSpectrumControl1_;
	AMControl *ge13ElementRawSpectrumControl2_;
	AMControl *ge13ElementRawSpectrumControl3_;
	AMControl *ge13ElementRawSpectrumControl4_;
	AMControl *ge13ElementRawSpectrumControl5_;
	AMControl *ge13ElementRawSpectrumControl6_;
	AMControl *ge13ElementRawSpectrumControl7_;
	AMControl *ge13ElementRawSpectrumControl8_;
	AMControl *ge13ElementRawSpectrumControl9_;
	AMControl *ge13ElementRawSpectrumControl10_;
	AMControl *ge13ElementRawSpectrumControl11_;
	AMControl *ge13ElementRawSpectrumControl12_;

	// Extra AMDetectors for the various single controls added to scans.
	AMDetector *energySetpointDetector_;
	AMDetector *energyFeedbackDetector_;
	AMDetector *masterDwellTimeDetector_;
	AMDetector *ringCurrentDetector_;
	AMDetector *roperCCDFileNumberDetector_;
	AMDetector *marCCDFileNumberDetector_;
	AMDetector *pilatusCCDFileNumberDetector_;

	AMDetector *singleElementVortexDeadTime_;
	AMDetector *singleElementVortexRealTime_;
	AMDetector *singleElementVortexLiveTime_;
	AMDetector *singleElementVortexFastPeaks_;
	AMDetector *singleElementVortexSlowPeaks_;
	AMDetector *singleElementVortexRawSpectrum_;

	AMDetector *fourElementVortexDeadTime1_;
	AMDetector *fourElementVortexDeadTime2_;
	AMDetector *fourElementVortexDeadTime3_;
	AMDetector *fourElementVortexDeadTime4_;
	AMDetector *fourElementVortexRealTime1_;
	AMDetector *fourElementVortexRealTime2_;
	AMDetector *fourElementVortexRealTime3_;
	AMDetector *fourElementVortexRealTime4_;
	AMDetector *fourElementVortexLiveTime1_;
	AMDetector *fourElementVortexLiveTime2_;
	AMDetector *fourElementVortexLiveTime3_;
	AMDetector *fourElementVortexLiveTime4_;
	AMDetector *fourElementVortexFastPeaks1_;
	AMDetector *fourElementVortexFastPeaks2_;
	AMDetector *fourElementVortexFastPeaks3_;
	AMDetector *fourElementVortexFastPeaks4_;
	AMDetector *fourElementVortexSlowPeaks1_;
	AMDetector *fourElementVortexSlowPeaks2_;
	AMDetector *fourElementVortexSlowPeaks3_;
	AMDetector *fourElementVortexSlowPeaks4_;
	AMDetector *fourElementVortexRawSpectrum1_;
	AMDetector *fourElementVortexRawSpectrum2_;
	AMDetector *fourElementVortexRawSpectrum3_;
	AMDetector *fourElementVortexRawSpectrum4_;

	AMDetector *ge13ElementRawSpectrum1_;
	AMDetector *ge13ElementRawSpectrum2_;
	AMDetector *ge13ElementRawSpectrum3_;
	AMDetector *ge13ElementRawSpectrum4_;
	AMDetector *ge13ElementRawSpectrum5_;
	AMDetector *ge13ElementRawSpectrum6_;
	AMDetector *ge13ElementRawSpectrum7_;
	AMDetector *ge13ElementRawSpectrum8_;
	AMDetector *ge13ElementRawSpectrum9_;
	AMDetector *ge13ElementRawSpectrum10_;
	AMDetector *ge13ElementRawSpectrum11_;
	AMDetector *ge13ElementRawSpectrum12_;

	// Motors
	AMControl *sampleStageHorizontalFeedbackControl_;
	AMControl *sampleStageVerticalFeedbackControl_;
	AMControl *sampleStageNormalFeedbackControl_;
	AMControl *wireStageHorizontalFeedbackControl_;
	AMControl *wireStageVerticalFeedbackControl_;
	AMControl *wireStageNormalFeedbackControl_;
	AMControl *sampleStageXFeedbackControl_;
	AMControl *sampleStageYFeedbackControl_;
	AMControl *sampleStageZFeedbackControl_;
	AMControl *attoStageHorizontalFeedbackControl_;
	AMControl *attoStageVerticalFeedbackControl_;
	AMControl *attoStageNormalFeedbackControl_;
	AMControl *attoStageXFeedbackControl_;
	AMControl *attoStageZFeedbackControl_;
	AMControl *attoStageYFeedbackControl_;
	AMControl *attoStageRxFeedbackControl_;
	AMControl *attoStageRzFeedbackControl_;
	AMControl *attoStageRyFeedbackControl_;
	AMControl *bigBeamXFeedbackControl_;
	AMControl *bigBeamZFeedbackControl_;

	AMDetector *sampleStageHorizontalFeedback_;
	AMDetector *sampleStageVerticalFeedback_;
	AMDetector *sampleStageNormalFeedback_;
	AMDetector *wireStageHorizontalFeedback_;
	AMDetector *wireStageVerticalFeedback_;
	AMDetector *wireStageNormalFeedback_;
	AMDetector *sampleStageXFeedback_;
	AMDetector *sampleStageYFeedback_;
	AMDetector *sampleStageZFeedback_;
	AMDetector *attoStageHorizontalFeedback_;
	AMDetector *attoStageVerticalFeedback_;
	AMDetector *attoStageNormalFeedback_;
	AMDetector *attoStageXFeedback_;
	AMDetector *attoStageZFeedback_;
	AMDetector *attoStageYFeedback_;
	AMDetector *attoStageRxFeedback_;
	AMDetector *attoStageRzFeedback_;
	AMDetector *attoStageRyFeedback_;
	AMDetector *bigBeamXFeedback_;
	AMDetector *bigBeamZFeedback_;
};

#endif // VESPERSBEAMLINE_H
