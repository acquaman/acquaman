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

#ifndef IDEASBEAMLINE_H
#define IDEASBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/AMControlSet.h"
//#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/CLS/CLSPseudoMotorGroup.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"


#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"

#include "beamline/IDEAS/IDEASMonochromatorControl.h"
#include "beamline/IDEAS/IDEASKETEKDetector.h"

/// This class is the master class that holds EVERY control inside the VESPERS beamline.
class IDEASBeamline : public AMBeamline
{
	Q_OBJECT

public:

	/// Returns the instance of the beamline that has been created.
	static IDEASBeamline* ideas()
	{
		if(!instance_)
			instance_ = new IDEASBeamline();

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

//	/// Returns the beamline's synchronized dwell time object if one is available. Returns 0 (NULL) otherwise.
//	virtual AMSynchronizedDwellTime* synchronizedDwellTime() const { return synchronizedDwellTime_; }

        /// Returns the monochromator control for the beamline.
        AMControl *monoEnergyControl() const { return monoEnergy_; }
	AMControl *monoDirectEnergyControl() const { return monoDirectEnergy_; }

        /// \todo THESE NEED TO BE COMMENTED!
        AMControl *monoCrystal() const { return monoCrystal_; }
        AMControl *monoHighEV() const { return monoHighEV_; }
        AMControl *monoLowEV() const { return monoLowEV_; }
        AMControl *ringCurrent() const { return ringCurrent_; }
	AMControl *I0Current() const { return I0Current_; }
	AMControl *sampleTemp() const { return sampleTemp_; }


        AMControl *monoBraggAngle() const { return monoBraggAngle_; }
        AMControl *mono2d() const { return mono2d_; }
        AMControl *monoAngleOffset() const { return monoAngleOffset_; }

	AMControl *ketekPeakingTime() const { return ketekPeakingTime_; }
	AMControl *ketekTriggerLevel() const { return ketekTriggerLevel_; }
	AMControl *ketekBaselineThreshold() const { return ketekBaselineThreshold_; }
	AMControl *ketekPreampGain() const { return ketekPreampGain_; }



//        AMPVControl *masterDwellControl() const { return masterDwell_; }
//        AMPVControl *ammeterGroupModeControl() const {return ammeterGroupMode_; }


	/// Returns the KETEK detector pointer.
	IDEASKETEKDetector *ketek() const { return ketek_; }
	AMDetector *dwellTime() const {return ketekRealTime_; }

	CLSBasicScalerChannelDetector *I_0() const {return I0IonChamberScaler_;}
	CLSBasicScalerChannelDetector *Sample() const {return SampleIonChamberScaler_;}
	CLSBasicScalerChannelDetector *Reference() const {return ReferenceIonChamberScaler_;}

	// The scaler.
	/// Returns the scaler.
	CLSSIS3820Scaler *scaler() const { return scaler_; }

	// End of scaler.





signals:

		/// Notifier that the status of the shutters has changed.
		void overallShutterStatus(bool);

public slots:

protected slots:
		/// Helper slot that handles emitting the overall shutter status.
		void onShutterStatusChanged();

protected:
//	/// Sets up the synchronized dwell time.
//	void setupSynchronizedDwellTime();
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

	/// Constructor. This is a singleton class; access it through IDEASBeamline::ideas().
	IDEASBeamline();

		/// The safety shutter for the beamline.
		CLSBiStateControl *safetyShutter_;
		/// The second photon shutter.
		CLSBiStateControl *photonShutter2_;
		/// The safety shutter for the SOE.
		CLSBiStateControl *safetyShutter2_;


	/// Control for the mono
		IDEASMonochromatorControl *monoEnergy_;
		IDEASDirectMonochromatorControl *monoDirectEnergy_;
	/// Control for the master dwell time on the synchronized dwell time application
//	AMPVControl *masterDwell_;
        /// Control for the mode of the IDEAS Ammeter Group
        AMPVControl *ammeterGroupMode_;

//	/// The synchronized dwell time app for IDEAS
//	CLSSynchronizedDwellTime *synchronizedDwellTime_;

//	AMReadOnlyPVControl *oldIonChamberAmmeter_;
//	AMReadOnlyPVControl *oxfordI0IonChamberAmmeter_;
//	AMReadOnlyPVControl *oxfordSampleIonChamberAmmeter_;
//	AMReadOnlyPVControl *oxfordReferenceIonChamberAmmeter_;
	AMReadOnlyPVControl *ketekRealTimeControl_;
	AMControl *ketekPeakingTime_;
	AMControl *ketekTriggerLevel_;
	AMControl *ketekBaselineThreshold_;
	AMControl *ketekPreampGain_;

//	AMDetector *oldIonChamberDetector_;
//	AMDetector *oxfordI0IonChamberDetector_;
//	AMDetector *oxfordSampleIonChamberDetector_;
//	AMDetector *oxfordReferenceIonChamberDetector_;
	AMDetector *ketekRealTime_;
//	AMDetector *masterDwellTime_;

	CLSBasicScalerChannelDetector *I0IonChamberScaler_;
	CLSBasicScalerChannelDetector *SampleIonChamberScaler_;
	CLSBasicScalerChannelDetector *ReferenceIonChamberScaler_;

	// Scaler.
	CLSSIS3820Scaler *scaler_;



	IDEASKETEKDetector *ketek_;

	AMControl *monoCrystal_, *monoLowEV_, *monoHighEV_, *ringCurrent_, *I0Current_, *sampleTemp_, *monoBraggAngle_, *mono2d_, *monoAngleOffset_;

};

#endif // IDEASSBEAMLINE_H
