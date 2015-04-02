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


#ifndef SXRMBBEAMLINE_H
#define SXRMBBEAMLINE_H

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/SXRMB/SXRMBBrukerDetector.h"
#include "beamline/AMMotorGroup.h"

#include "util/AMBiHash.h"

class AMBasicControlDetectorEmulator;

class SXRMBBeamline : public CLSBeamline
{
	Q_OBJECT

public:
	/// Returns the instance of the beamline that has been created.
	static SXRMBBeamline* sxrmb()
	{
		if(!instance_){
			instance_ = new SXRMBBeamline();
			instance_->initializeBeamlineSupport();
		}

		return static_cast<SXRMBBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~SXRMBBeamline();

	/// Returns the scaler for SXRMB
	CLSSIS3820Scaler* scaler() const;

	/// ReturnsEnergy control for SXRMB
	AMPVwStatusControl* energy() const;

	/// Returns the X Stage for the microrobe sample stage
	AMPVwStatusControl* microprobeSampleStageX() const;
	/// Returns the Y Stage for the microrobe sample stage
	AMPVwStatusControl* microprobeSampleStageY() const;
	/// Returns the Z Stage for the microrobe sample stage
	AMPVwStatusControl* microprobeSampleStageZ() const;

	/// Returns the motor group for this beamline.
	AMMotorGroup *motorGroup() const;
	/// Returns the microprobe stage motor group object.
	AMMotorGroupObject *microprobeSampleStageMotorGroupObject() const;

	/// Returns the SXRMB overall status control
	AMReadOnlyPVControl* beamlineStatus() const;

	/// Returns whether the PVs are connected or not
	virtual bool isConnected() const;

	/// Returns the I0 Detector
	CLSBasicScalerChannelDetector* i0Detector() const;
	/// Returns the TEY Detector
	CLSBasicScalerChannelDetector* teyDetector() const;

	/// Returns the energy feedback detector (emulator)
	AMBasicControlDetectorEmulator* energyFeedbackDetector() const;

	/// Returns the bruker fluorescence detector.
	SXRMBBrukerDetector *brukerDetector() const;

	/// Returns the list of actions to turn the beam on
	AMAction3* createBeamOnActions() const;
	/// Returns the list of actions to turn the beam off (just photon shutter 2 right now)
	AMAction3* createBeamOffActions() const;

signals:
	void beamAvaliability(bool beamOn);
	void beamlineControlShuttersTimeout();

protected:
	/// Constructor. This is a singleton class, access it through SXRMBBeamline::sxrmb().
	SXRMBBeamline();

	/// Sets up the synchronized dwell time.
	void setupSynchronizedDwellTime();
	/// Sets up various beamline components.
	void setupComponents();
	/// Sets up the readings such as pressure, flow switches, temperature, etc.
	void setupDiagnostics();
	/// Sets up the sample stage motors.
	void setupSampleStage();
	/// Sets up all the detectors.
	void setupDetectors();
	/// Sets up logical groupings of controls into sets.
	void setupControlSets();
	/// Sets up mono settings.
	void setupMono();
	/// Sets up the motor group for the various sample stages.
	void setupMotorGroup();
	/// Sets up all of the detectors that need to be added to scans that aren't a part of typical detectors.  This may just be temporary, not sure.
	void setupControlsAsDetectors();
	/// Sets up the exposed actions.
	void setupExposedControls();
	/// Sets up the exposed detectors.
	void setupExposedDetectors();
	/// Sets up the SIGNAL and SLOT connections.
	void setupConnections();

	/// Helper function to check for the beam availability
	void beamAvailabilityHelper();
	/// Helper function to check for changes in the connected state
	void connectedHelper();

protected slots:
	/// Handles the beamAvailability signal of the Storage ring
	void onStorageRingBeamAvailabilityChanged(bool value);
	/// Handles value changed signal of the beamline status
	void onBeamlineStatusPVValueChanged(double value);
	/// Handles connected status of the beamline status
	void onBeamlineStatusPVConnected(bool);
	/// Handles connected status of the energy
	void onEnergyPVConnected(bool);
	/// Handles connected status of all of the microprobe sample stage controls
	void onMicroprobeSampleStagePVsConnected(bool);
	/// Handles connected status of all of the beam on/off control shutters
	void onBeamlineControlShuttersConnected(bool);

protected:
	/// Scaler for SXRMB
	CLSSIS3820Scaler *scaler_;

	/// Energy control for SXRMB
	AMPVwStatusControl *energy_;

	/// Energy feedback control for SXRMB
	AMControl *energyFeedbackControl_;
	/// Detector for getting the energy feedback
	AMBasicControlDetectorEmulator *energyFeedbackDetector_;

	/// X Stage for the microrobe sample stage
	AMPVwStatusControl *microprobeSampleStageX_;
	/// Y Stage for the microrobe sample stage
	AMPVwStatusControl *microprobeSampleStageY_;
	/// Z Stage for the microrobe sample stage
	AMPVwStatusControl *microprobeSampleStageZ_;

	/// Detector for the I0Detector channel on the scaler
	CLSBasicScalerChannelDetector *i0Detector_;
	/// Detector for the TEYDetctor channel on the scaler
	CLSBasicScalerChannelDetector *teyDetector_;

	/// Control set for microprobe sample stage
	AMControlSet *microprobeSampleStageControlSet_;

	/// Motor group.  Holds sets of motors that are used together.
	AMMotorGroup *motorGroup_;

	/// SXRMB overall status control
	AMReadOnlyPVControl *beamlineStatus_;

	/// Previous connected state for the whole SXRMB beamline
	bool wasConnected_;

	/// The bruker detector.
	SXRMBBrukerDetector *brukerDetector_;

	/// Beamline valves, the valves involved in the Beam on/off action
	AMControlSet * beamlineControlShutterSet_;
	CLSBiStateControl *PSH1406B1002Shutter_;
	CLSBiStateControl *VVR16064B1003Valve_;
	CLSBiStateControl *VVR16064B1004Valve_;
	// NOT THIS ONE! It's connected to the pump on the mono
	//CLSBiStateControl *VVR16064B1005Valve_;
	CLSBiStateControl *VVR16064B1006Valve_;
	CLSBiStateControl *VVR16064B1007Valve_;
	CLSBiStateControl *VVR16065B1001Valve_;
};

#endif // SXRMBBEAMLINE_H
