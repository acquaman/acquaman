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

#ifndef VESPERSBEAMLINE_H
#define VESPERSBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMOldDetectorSet.h"
#include "beamline/VESPERS/XRFDetector.h"
#include "beamline/AMROI.h"
#include "beamline/VESPERS/VESPERSSampleStageControl.h"
#include "beamline/VESPERS/VESPERSPIDLoopControl.h"
#include "beamline/VESPERS/VESPERSMonochromator.h"
#include "beamline/VESPERS/VESPERSIntermediateSlits.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "actions/AMBeamlineActionItem.h"
#include "beamline/VESPERS/VESPERSEndstation.h"
#include "beamline/AMIonChamber.h"
#include "beamline/CLS/CLSIonChamber.h"
#include "beamline/CLS/CLSSplitIonChamber.h"
#include "beamline/CLS/CLSVariableIntegrationTime.h"
#include "beamline/VESPERS/VESPERSRoperCCDDetector.h"
#include "beamline/VESPERS/VESPERSMarCCDDetector.h"
#include "beamline/VESPERS/VESPERSPilatusCCDDetector.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "application/VESPERS/VESPERS.h"
#include "beamline/AMMotorGroup.h"

#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

#define VESPERSBEAMLINE_PRESSURE_TOO_HIGH 67800
#define VESPERSBEAMLINE_VALVES_CLOSED 67801
#define VESPERSBEAMLINE_TEMPERATURE_TOO_HIGH 67802
#define VESPERSBEAMLINE_WATER_FLOW_SWITCH_TRIP 67803
#define VESPERSBEAMLINE_WATER_FLOW_TOO_LOW 67804
#define VESPERSBEAMLINE_ION_PUMP_TRIP 67805
#define VESPERSBEAMLINE_SINGLE_ELEMENT_NOT_CONNECTED 67806
#define VESPERSBEAMLINE_FOUR_ELEMENT_NOT_CONNECTED 67807
#define VESPERSBEAMLINE_SAMPLE_STAGE_NOT_CONNECTED 67808

/// This class is the master class that holds EVERY control inside the VESPERS beamline.
class VESPERSBeamline : public AMBeamline
{
	Q_OBJECT
public:

	/// Returns the instance of the beamline that has been created.
	static VESPERSBeamline* vespers() {
		if(!instance_)
			instance_ = new VESPERSBeamline();
		return static_cast<VESPERSBeamline*>(instance_);
	}

	~VESPERSBeamline();

	// Beam selection functions.
	/// Returns the current beam in use by the beamline.
	VESPERS::Beam currentBeam() const { return beam_; }

	// Helper functions.
	QString pvName(const QString &amName) const { return amNames2pvNames_.valueF(amName); }
	QString amName(const QString &pvName) const { return amNames2pvNames_.valueR(pvName); }

	// Accessing detectors.

	/// Returns a general AMDetector pointer of the single element XRF detector.
	AMOldDetector *vortexAM1E() const { return vortex1E_; }
	/// Returns the specific XRFDetector pointer of the single element XRF detector.
	XRFDetector *vortexXRF1E() const { return (XRFDetector *)vortex1E_; }
	/// Returns a general AMDetector pointer of the four element XRF detector.
	AMOldDetector *vortexAM4E() const { return vortex4E_; }
	/// Returns the specific XRFDetector pointer of the single element XRF detector.
	XRFDetector *vortexXRF4E() const { return (XRFDetector *)vortex4E_; }

	/// Returns a general AMDetector pointer of the Roper CCD.
	AMOldDetector *roperCCDDetector() const { return roperCCD_; }
	/// Returns the specific pointer to the Roper CCD.
	VESPERSRoperCCDDetector *roperCCD() const { return (VESPERSRoperCCDDetector *)roperCCD_; }
	/// Returns a general AMDetector pointer of the Mar CCD.
	AMOldDetector *marCCDDetector() const { return marCCD_; }
	/// Returns the specific pointer to the Mar CCD.
	VESPERSMarCCDDetector *marCCD() const { return (VESPERSMarCCDDetector *)marCCD_; }
	/// Returns a general AMDetector pointer of the Pilatus CCD.
	AMOldDetector *pilatusCCDDetector() const { return pilatusCCD_; }
	/// Returns the specific pointer to the Pilatus CCD.
	VESPERSPilatusCCDDetector *pilatusCCD() const { return (VESPERSPilatusCCDDetector *)pilatusCCD_; }

	/// Returns a general AMDetector pointer to the split ion chamber.
	AMOldDetector *iSplitDetector() const { return iSplit_; }
	/// Returns a CLSIonChamber pointer to the split ion chamber.
	CLSSplitIonChamber *iSplit() const { return (CLSSplitIonChamber *)iSplit_; }
	/// Returns a general AMDetector pointer to the pre-KB ion chamber.
	AMOldDetector *iPreKBDetector() const { return iPreKB_; }
	/// Returns a CLSIonChamber pointer to the split ion chamber.
	CLSIonChamber *iPreKB() const { return (CLSIonChamber *)iPreKB_; }
	/// Returns a general AMDetector pointer to the mini ion chamber.
	AMOldDetector *iMiniDetector() const { return iMini_; }
	/// Returns a CLSIonChamber pointer to the split ion chamber.
	CLSIonChamber *iMini() const { return (CLSIonChamber *)iMini_; }
	/// Returns a general AMDetector pointer to the post sample ion chamber.
	AMOldDetector *iPostDetector() const { return iPost_; }
	/// Returns a CLSIonChamber pointer to the split ion chamber.
	CLSIonChamber *iPost() const { return (CLSIonChamber *)iPost_; }
	/// Returns the ion chamber detector set.
	AMOldDetectorSet *ionChambers() const { return ionChambers_; }

	// Accessing control elements:

	// The monochromator abstraction.
	/// Returns the monochromator abstraction for the VESPERS beamline.
	VESPERSMonochromator *mono() const { return mono_; }

	// End of monochromator abstraction.

	// The intermediate slits.
	/// Returns the intermediate slits object.
	VESPERSIntermediateSlits *intermediateSlits() const { return intermediateSlits_; }

	// End of intermediate slits.

	// The synchronized dwell time.
	/// Returns the synchronized dwell time.
	CLSSynchronizedDwellTime *synchronizedDwellTime() const { return synchronizedDwellTime_; }
	/// Returns the synchronized dwell time configuration info's list.
	QList<CLSSynchronizedDwellTimeConfigurationInfo *> synchronizedDwellTimeConfigurations() const { return synchronizedDwellTimeConfigurations_; }
	/// Returns a synchronized dwell time configuration info from the index provided.
	CLSSynchronizedDwellTimeConfigurationInfo *synchronizedDwellTimeConfigurationAt(int index) const { return synchronizedDwellTimeConfigurations_.at(index); }
	/// Returns the synchronized dwell time configuration info based on the name provided.  Returns 0 if not found.
	CLSSynchronizedDwellTimeConfigurationInfo *synchronizedDwellTimeConfigurationByName(const QString &name) const;

	// End of synchronized dwell time.

	// The variable integration time.
	/// Returns the variable integration time.
	CLSVariableIntegrationTime *variableIntegrationTime() const { return variableIntegrationTime_; }

	// End of variable integration time.

	// The scaler.
	/// Returns the scaler.
	CLSSIS3820Scaler *scaler() const { return scaler_; }

	// End of scaler.

	// The photon and safety shutters.
	/// Returns the first photon shutter.
	AMControl *photonShutter1() const { return psh1_; }
	/// Returns the second photon shutter.
	AMControl *photonShutter2() const { return psh2_; }
	/// Returns the first safety shutter.
	AMControl *safetyShutter1() const { return ssh1_; }
	/// Returns the second safety shutter.
	AMControl *safetyShutter2() const { return ssh2_; }

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

	// The sample stages.
	/// Returns the sample stage control built with the pseudo-motors.
	VESPERSSampleStageControl *pseudoSampleStage() const { return pseudoSampleStage_; }
	/// Returns the real sample stage control (real as in, there are no pseudo motor levels in between).
	VESPERSSampleStageControl *realSampleStage() const { return realSampleStage_; }
	/// Returns the wire stage control built with the pseudo-motors.
	VESPERSSampleStageControl *pseudoWireStage() const { return pseudoWireStage_; }
	/// Returns the attocube pseudo-motor stage.
	VESPERSSampleStageControl *pseudoAttoStage() const { return pseudoAttoStage_; }
	/// Returns the attocube real-motor stage.
	VESPERSSampleStageControl *realAttoStage() const { return realAttoStage_; }

	// The motor group.
	AMMotorGroup *motorGroup() const { return motorGroup_; }

	// Sample stage PID controls.
	/// Returns the PID control for the x-direction of the sample stage.
	AMControl *sampleStagePidX() const { return sampleStagePidX_; }
	/// Returns the PID control for the y-direction of the sample stage.
	AMControl *sampleStagePidY() const { return sampleStagePidY_; }
	/// Returns the PID control for the z-direction of the sample stage.
	AMControl *sampleStagPidZ() const { return sampleStagePidZ_; }

	/// Returns the sample stage PID control.
	VESPERSPIDLoopControl *sampleStagePID() const { return sampleStagePID_; }

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
	AMControl *energy() const { return energy_; }
	/// Returns the relative energy control.
	AMControl *energyRelative() const { return energyRelative_; }
	/// Returns the master dwell time control.
	AMControl *masterDwellTime() const { return masterDwellTime_; }
	/// Returns the k control used for EXAFS scans.
	AMControl *kControl() const { return kControl_; }

	//////////////////////////////////////////////////////////////////////////////////////
	// Actions
	/// Creates an action that changes the beam.  Returns 0 if unable to create.
	AMBeamlineActionItem *createBeamChangeAction(VESPERS::Beam beam);

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
	void onBeamSelectionMotorConnected() { disconnect(beamSelectionMotor_, SIGNAL(valueChanged(double)), this, SLOT(onBeamSelectionMotorConnected())); determineBeam(); }

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
	/// Slot that is used for making sure the synchronized dwell time is configured properly once it is connected.
	void synchronizedDwellTimeConnected(bool connected);
	/// Determines whether the beam has dumped or not.
	void onPOEStatusChanged();

	/// Helper slot that handles opening the next valve.
	void openAllValvesHelper();
	/// Helper slot that handles closing the next valve.
	void closeAllValvesHelper();


protected:
	/// Sets up the synchronized dwell time.
	void setupSynchronizedDwellTime();
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
	/// Sets up the motor group for the various sample stages.
	void setupMotorGroup();

	/// Constructor. This is a singleton class; access it through VESPERSBeamline::vespers().
	VESPERSBeamline();

	// Detectors.
	AMOldDetector *vortex1E_;
	AMOldDetector *vortex4E_;
	AMOldDetector *roperCCD_;
	AMOldDetector *marCCD_;
	AMOldDetector *pilatusCCD_;
	AMOldDetector *iSplit_;
	AMOldDetector *iPreKB_;
	AMOldDetector *iMini_;
	AMOldDetector *iPost_;

	// End detectors.

	// Detector sets.
	AMOldDetectorSet *ionChambers_;

	// End detector sets.

	// VESPERS monochromator.
	VESPERSMonochromator *mono_;

	// Intermediate slits.
	VESPERSIntermediateSlits *intermediateSlits_;

	// Synchronized Dwell time
	CLSSynchronizedDwellTime *synchronizedDwellTime_;
	// List of all the various synchronized dwell time configurations.
	QList<CLSSynchronizedDwellTimeConfigurationInfo *> synchronizedDwellTimeConfigurations_;

	// Variable integration time.
	CLSVariableIntegrationTime *variableIntegrationTime_;

	// Scaler.
	CLSSIS3820Scaler *scaler_;

	// POE status and enable.
	// The POE beam status control.
	AMControl *poeBeamStatus_;
	// The POE beam status enable.
	AMControl *poeBeamStatusEnable_;

	// The shutters.
	AMControl *psh1_;
	AMControl *psh2_;
	AMControl *ssh1_;
	AMControl *ssh2_;

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

	// End ion chamber controls.

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

	// The sample stage encapsulation.
	VESPERSSampleStageControl *pseudoSampleStage_;
	VESPERSSampleStageControl *realSampleStage_;
	VESPERSSampleStageControl *pseudoWireStage_;
	VESPERSSampleStageControl *pseudoAttoStage_;
	VESPERSSampleStageControl *realAttoStage_;

	// Motor group.  Binds all the motors for scanning together.
	AMMotorGroup *motorGroup_;

	// The PID loop controls.
	AMControl *sampleStagePidX_;
	AMControl *sampleStagePidY_;
	AMControl *sampleStagePidZ_;

	VESPERSPIDLoopControl *sampleStagePID_;

	// End sample stage controls.

	// Scanning settings.
	AMControl *energy_;
	AMControl *energyRelative_;
	AMControl *masterDwellTime_;
	AMControl *kControl_;

	// AM names bihash to/from PV names.
	AMBiHash<QString, QString> amNames2pvNames_;
};

#endif // VESPERSBEAMLINE_H
