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

#ifndef IDEASBEAMLINE_H
#define IDEASBEAMLINE_H

#include "beamline/AMControlSet.h"
#include "beamline/AMMotorGroup.h"

#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSPseudoMotorGroup.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"

#include "application/IDEAS/IDEAS.h"
#include "beamline/IDEAS/IDEASMonochromatorControl.h"
#include "beamline/IDEAS/IDEASKETEKDetector.h"
#include "beamline/IDEAS/IDEAS13ElementGeDetector.h"



/// This class is the master class that holds EVERY control inside the VESPERS beamline.
class IDEASBeamline : public CLSBeamline
{
	Q_OBJECT

public:
	/// Returns the instance of the beamline that has been created.
	static IDEASBeamline* ideas()
	{
		if(!instance_){
			instance_ = new IDEASBeamline();
			instance_->initializeBeamlineSupport();
		}

		return static_cast<IDEASBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~IDEASBeamline();

	/// Returns the overall shutter status.  Returns true if both are open, false otherwise.
	bool shuttersOpen() const;

	/// Creates an action that opens the shutters to act like a Beam On.
	AMAction3 *createBeamOnAction() const;
	/// Creates an action that closes the shutters to act like a Beam Off.
	AMAction3 *createBeamOffAction() const;

	/// Create the scan initialization actions.
	virtual AMAction3* createScanInitializationAction(AMGenericStepScanConfiguration *configuration);
	/// Create the scan cleanup actions.
	virtual AMAction3* createScanCleanupAction(AMGenericStepScanConfiguration *configuration);


	/// Returns the monochromator control for the beamline.
	AMControl *monoEnergyControl() const { return monoEnergy_; }
	AMControl *monoDirectEnergyControl() const { return monoDirectEnergy_; }

	/// \todo THESE NEED TO BE COMMENTED!
	/// Returns the mono IOC Apps Crystal setting
	AMControl *monoCrystal() const { return monoCrystal_; }
	/// Returns the mono maximum energy
	AMControl *monoHighEV() const { return monoHighEV_; }
	/// Returns the mono minimum energy
	AMControl *monoLowEV() const { return monoLowEV_; }
	/// Returns the ring current
	AMControl *ringCurrent() const { return ringCurrent_; }
	/// Returns the mono returns the reading from the I_0 picoammeter
	AMControl *I0Current() const { return i0Current_; }
	/// Returns the reading from the PLC TC input
	AMControl *sampleTemp() const { return sampleTemp_; }

	/// Returns the mono Bragg angle
	AMControl *monoBraggAngle() const { return monoBraggAngle_; }
	/// Returns the mono IOC Apps lookup value for the current crystal 2d spacing
	AMControl *mono2d() const { return mono2d_; }
	/// returns the mono offset (calibration) angle
	AMControl *monoAngleOffset() const { return monoAngleOffset_; }

	/// Returns the KETEKs peaking time
	AMControl *ketekPeakingTime() const { return ketekPeakingTime_; }
	/// Returns the KETEKs trigger level
	AMControl *ketekTriggerLevel() const { return ketekTriggerLevel_; }
	/// Returns the KETEKs Baseline Threshold
	AMControl *ketekBaselineThreshold() const { return ketekBaselineThreshold_; }
	/// Returns the KETEKs preamp gain
	AMControl *ketekPreampGain() const { return ketekPreampGain_; }


	/// Returns JJ Slits horizontal gap
	AMControl *jjSlitHGap() const { return jjSlitHGap_ ; }
	/// Returns JJ Slits horizontal center position
	AMControl *jjSlitHCenter() const { return jjSlitHCenter_; }
	/// Returns JJ Slits vertical gap
	AMControl *jjSlitVGap() const { return jjSlitVGap_; }
	/// Returns JJ Slits vertical center position
	AMControl *jjSlitVCenter() const { return jjSlitVCenter_; }

	/// Returns Sample Platform Vertical Position
	AMControl *samplePlatformVertical() const { return samplePlatformVertical_ ; }
	/// Returns Sample Platform Horizontal Position
	AMControl *samplePlatformHorizontal() const { return samplePlatformHorizontal_ ; }
	/// Returns the sample platform motor group object.
	AMMotorGroupObject *samplePlatformMotorGroupObject() const { return motorGroup_->motorGroupObject("Sample Platform"); }
	/// Returns the vacuum stage motor group object.
	AMMotorGroupObject *vacuumStageMotorGroupObject() const { return motorGroup_->motorGroupObject("Vacuum Stage"); }

	/// Returns the sample platform motor group.
	AMMotorGroup *motorGroup() const { return motorGroup_;}
	/// Returns the vacuum stage motor control.
	AMControl *vacuumSampleStage() const { return vacuumSampleStage_ ; }

	/// Returns the XRF detector
	AMXRFDetector *xrfDetector(IDEAS::FluorescenceDetectors detectorType);

	/// Returns the KETEK detector pointer.
	IDEASKETEKDetector *ketek() const { return ketek_; }
	/// Returns the real time for the KETEK.
	AMDetector *ketekDwellTime() const {return ketekRealTime_; }

	/// Returns the 13-element Ge detector pointer.
	IDEAS13ElementGeDetector *ge13Element() const {return ge13Element_;}
	/// Returns the real time for the Ge detector.
	AMDetector *ge13ElementDwellTime() const {return ge13ElementRealTime_;}

	/// Returns the default I0 ion chamber.
	CLSBasicScalerChannelDetector *i0() const {return i0IonChamberScaler_;}
	/// Returns the sample ion chamber.
	CLSBasicScalerChannelDetector *sampleIonChamber() const {return sampleIonChamberScaler_;}
	/// Returns the ion chamber for reference samples.
	CLSBasicScalerChannelDetector *referenceIonChamber() const {return referenceIonChamberScaler_;}

	// The scaler.
	/// Returns the scaler.
	CLSSIS3820Scaler *scaler() const { return scaler_; }

	// End of scaler.

	/// Returns a newly created action to move sample platform vertical. Returns 0 if the control is not connected.
	AMAction3 *createSamplePlatformMoveVertical(double verticalPosition);
	/// Returns a newly created action to move sample platform horizontal. Returns 0 if the control is not connected.
	AMAction3 *createSamplePlatformMoveHorizontal(double verticalPosition);

	/// Returns a newly created action to move move vaccum sample stage. Returns 0 if the control is not connected.
	AMAction3 *createVaccumSampleStageMove(double vaccumStagePosition);

	/// Returns a newly created action to move jjSlitHorizontalGap. Returns 0 if the control is not connected.
	AMAction3 *createJJSlitHGapMove(double jjSlitHGapPosition);
	/// Returns a newly created action to move jjSlitHorizontalCenter. Returns 0 if the control is not connected.
	AMAction3 *createJJSlitHCenterMove(double jjSlitHCenterPosition);
	/// Returns a newly created action to move  jjSlitVerticalGap. Returns 0 if the control is not connected.
	AMAction3 *createJJSlitVGapMove(double jjSlitVGapPosition);
	/// Returns a newly created action to move  jjSlitVerticalCenter. Returns 0 if the control is not connected.
	AMAction3 *createJJSlitVCenterMove(double jjSlitVCenterPosition);



signals:

	/// Notifier that the status of the shutters has changed.
	void overallShutterStatus(bool);

public slots:
	void setMonoSettlingTime(double time) {monoEnergy_->setSettlingTime(time);}

protected slots:
	/// Helper slot that handles emitting the overall shutter status.
	void onShutterStatusChanged();

protected:
	/// Sets up the readings such as pressure, flow switches, temperature, etc.
	void setupDiagnostics();
	/// Sets up logical groupings of controls into sets.
	void setupControlSets();
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

	/// Constructor. This is a singleton class, access it through IDEASBeamline::ideas().
	IDEASBeamline();

	/// The safety shutter for the beamline.
	CLSExclusiveStatesControl *safetyShutter_;
	/// The second photon shutter.
	CLSExclusiveStatesControl *photonShutter2_;
	/// The safety shutter for the SOE.
	CLSExclusiveStatesControl *safetyShutter2_;


	/// Control for the mono
	IDEASMonochromatorControl *monoEnergy_;
	IDEASDirectMonochromatorControl *monoDirectEnergy_;

	/// Control for the mode of the IDEAS Ammeter Group
	AMPVControl *ammeterGroupMode_;

	CLSBasicScalerChannelDetector *i0IonChamberScaler_;
	CLSBasicScalerChannelDetector *sampleIonChamberScaler_;
	CLSBasicScalerChannelDetector *referenceIonChamberScaler_;

	// Scaler.
	CLSSIS3820Scaler *scaler_;

	IDEASKETEKDetector *ketek_;
	AMControl *ketekPeakingTime_;
	AMControl *ketekTriggerLevel_;
	AMControl *ketekBaselineThreshold_;
	AMControl *ketekPreampGain_;

	AMDetector *ketekRealTime_;
	AMReadOnlyPVControl *ketekRealTimeControl_;

	IDEAS13ElementGeDetector *ge13Element_;

	AMDetector *ge13ElementRealTime_;
	AMReadOnlyPVControl *ge13ElementRealTimeControl_;


	AMControl *monoCrystal_, *monoLowEV_, *monoHighEV_, *ringCurrent_, *i0Current_, *sampleTemp_, *monoBraggAngle_, *mono2d_, *monoAngleOffset_;

	//Sample Stage motors
	/////////////////////
	//J-J Slits
	AMControl *jjSlitHGap_, *jjSlitVGap_, *jjSlitHCenter_, *jjSlitVCenter_;
	//Sample Platform
	AMControl *samplePlatformVertical_, *samplePlatformHorizontal_;
	//Sample Stage motor Group
	AMMotorGroup *motorGroup_;

	//Vacuum Sample Stage
	AMControl *vacuumSampleStage_;

};

#endif // IDEASSBEAMLINE_H
