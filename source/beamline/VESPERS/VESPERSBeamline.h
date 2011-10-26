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

#ifndef VESPERSBEAMLINE_H
#define VESPERSBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMDetectorSet.h"
#include "beamline/VESPERS/AMValveControl.h"
#include "beamline/VESPERS/XRFDetector.h"
#include "beamline/AMROI.h"
#include "beamline/VESPERS/SampleStageControl.h"
#include "beamline/VESPERS/VESPERSValveGroupControl.h"
#include "beamline/VESPERS/PIDLoopControl.h"
#include "beamline/VESPERS/VESPERSMonochromator.h"
#include "beamline/VESPERS/VESPERSIntermediateSlits.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "actions/AMBeamlineActionItem.h"
#include "beamline/VESPERS/VESPERSEndstation.h"
#include "beamline/VESPERS/VESPERSExperimentConfiguration.h"
#include "beamline/AMIonChamber.h"
#include "beamline/CLS/CLSIonChamber.h"
#include "beamline/CLS/CLSSplitIonChamber.h"

#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

/// This class is the master class that holds EVERY control inside the VESPERS beamline.
class VESPERSBeamline : public AMBeamline
{
	Q_OBJECT
public:

	/// Enum for the different beams.
	/*!
		- Pink is not monochromatized and contains all the energies from the bending magnet.
		- TenPercent (10%) is a broad band pass filter.
		- OnePointSixPercent (1.6%) is a narrow band pass filter.
		- Si is the monochromator with 0.01% band pass.
	  */
	enum Beam { None = 0, Pink, TenPercent, OnePointSixPercent, Si };

	/// Returns the instance of the beamline that has been created.
	static VESPERSBeamline* vespers() {
		if(!instance_)
			instance_ = new VESPERSBeamline();
		return static_cast<VESPERSBeamline*>(instance_);
	}

	~VESPERSBeamline();

	// Beam selection functions.
	/// Returns the current beam in use by the beamline.
	Beam currentBeam() const { return beam_; }

	// Helper functions.
	QString pvName(const QString &amName) const { return amNames2pvNames_.valueF(amName); }
	QString amName(const QString &pvName) const { return amNames2pvNames_.valueR(pvName); }

	// Accessing detectors.

	/// Returns a general AMDetector pointer of the single element XRF detector.
	AMDetector *vortexAM1E() const { return vortex1E_; }
	/// Returns the specific XRFDetector pointer of the single element XRF detector.
	XRFDetector *vortexXRF1E() const { return (XRFDetector *)vortex1E_; }
	/// Returns a general AMDetector pointer of the four element XRF detector.
	AMDetector *vortexAM4E() const { return vortex4E_; }
	/// Returns the specific XRFDetector pointer of the single element XRF detector.
	XRFDetector *vortexXRF4E() const { return (XRFDetector *)vortex4E_; }

	/// Returns a general AMDetector pointer to the split ion chamber.
	AMDetector *iSplitDetector() const { return iSplit_; }
	/// Returns a CLSIonChamber pointer to the split ion chamber.
	CLSSplitIonChamber *iSplit() const { return (CLSSplitIonChamber *)iSplit_; }
	/// Returns a general AMDetector pointer to the pre-KB ion chamber.
	AMDetector *iPreKBDetector() const { return iPreKB_; }
	/// Returns a CLSIonChamber pointer to the split ion chamber.
	CLSIonChamber *iPreKB() const { return (CLSIonChamber *)iPreKB_; }
	/// Returns a general AMDetector pointer to the mini ion chamber.
	AMDetector *iMiniDetector() const { return iMini_; }
	/// Returns a CLSIonChamber pointer to the split ion chamber.
	CLSIonChamber *iMini() const { return (CLSIonChamber *)iMini_; }
	/// Returns a general AMDetector pointer to the post sample ion chamber.
	AMDetector *iPostDetector() const { return iPost_; }
	/// Returns a CLSIonChamber pointer to the split ion chamber.
	CLSIonChamber *iPost() const { return (CLSIonChamber *)iPost_; }
	/// Returns the ion chamber detector set.
	AMDetectorSet *ionChambers() const { return ionChambers_; }

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

	// End of synchronized dwell time.

	// The endstation.
	/// Returns the endstation model.
	VESPERSEndstation *endstation() const { return endstation_; }

	// End of endstation.

	// Beam selection motor.
	/// Returns the control for the beam selection motor.
	AMControl *beamSelectionMotor() const { return beamSelectionMotor_; }

	// The experiment configuration.
	/// Returns the experiment configuration model.
	VESPERSExperimentConfiguration *experimentConfiguration() const { return experimentConfiguration_; }

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

	// The sample stage.
	/// Returns the sample stage control built with the pseudo-motors.
	SampleStageControl *pseudoSampleStage() const { return pseudoSampleStage_; }

	// Sample stage PID controls.
	/// Returns the PID control for the x-direction of the sample stage.
	AMControl *sampleStagePidX() const { return sampleStagePidX_; }
	/// Returns the PID control for the y-direction of the sample stage.
	AMControl *sampleStagePidY() const { return sampleStagePidY_; }
	/// Returns the PID control for the z-direction of the sample stage.
	AMControl *sampleStagPidZ() const { return sampleStagePidZ_; }

	/// Returns the sample stage PID control.
	PIDLoopControl *sampleStagePID() const { return sampleStagePID_; }

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
	/// Returns the relative energy control.
	AMControl *energyRelative() const { return energyRelative_; }
	/// Returns the master dwell time control.
	AMControl *masterDwellTime() const { return masterDwellTime_; }

	//////////////////////////////////////////////////////////////////////////////////////
	// Actions
	/// Creates an action that changes the beam.  Returns 0 if unable to create.
	AMBeamlineActionItem *createBeamChangeAction(Beam beam);

	// End of Actions
	//////////////////////////////////////////////////////////////////////////////////////

signals:
	/// Notifier that the beam has been changed.
	void currentBeamChanged(VESPERSBeamline::Beam);
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

public slots:

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
	/// Sets up the experiment status.
	void setupExperimentStatus();

	/// Constructor. This is a singleton class; access it through VESPERSBeamline::vespers().
	VESPERSBeamline();

	// Detectors.
	AMDetector *vortex1E_;
	AMDetector *vortex4E_;
	AMDetector *iSplit_;
	AMDetector *iPreKB_;
	AMDetector *iMini_;
	AMDetector *iPost_;

	// End detectors.

	// Detector sets.
	AMDetectorSet *ionChambers_;

	// End detector sets.

	// VESPERS monochromator.
	VESPERSMonochromator *mono_;

	// Intermediate slits.
	VESPERSIntermediateSlits *intermediateSlits_;

	// Synchronized Dwell time
	CLSSynchronizedDwellTime *synchronizedDwellTime_;

	// Endstation
	VESPERSEndstation *endstation_;

	// Experiment Configuration
	VESPERSExperimentConfiguration *experimentConfiguration_;

	// Beam selection members.
	// The current beam in use by the beamline.
	Beam beam_;
	// Pointer to the motor that controls which beam makes it down the beamline.
	AMControl *beamSelectionMotor_;
	// Look up table with the beam and its position.
	QHash<Beam, double> beamPositions_;

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
	AMControlSet *sampleStageMotorSet_;

	// End General Controls.

	// End ion chamber controls.

	// Sample stage controls.
	// CLS pseudo-motors.
	AMControl *sampleStageHorizontal_;
	AMControl *sampleStageVertical_;
	AMControl *sampleStageNormal_;

	// Physical motors.
	AMControl *sampleStageX_;
	AMControl *sampleStageY_;
	AMControl *sampleStageZ_;

	// The sample stage encapsulation.
	SampleStageControl *pseudoSampleStage_;
	SampleStageControl *realSampleStage_;

	// The PID loop controls.
	AMControl *sampleStagePidX_;
	AMControl *sampleStagePidY_;
	AMControl *sampleStagePidZ_;

	PIDLoopControl *sampleStagePID_;

	// End sample stage controls.

	// Scanning settings.
	AMControl *energyRelative_;
	AMControl *masterDwellTime_;

	// AM names bihash to/from PV names.
	AMBiHash<QString, QString> amNames2pvNames_;
};

#endif // VESPERSBEAMLINE_H
