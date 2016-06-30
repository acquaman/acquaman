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

#include "beamline/AMControlSet.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/AMSlits.h"
#include "beamline/CLS/CLSBeamline.h"
#include "beamline/CLS/CLSBeamlineStatus.h"
#include "beamline/CLS/CLSShutters.h"
#include "beamline/CLS/CLSValves.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSCrossHairGeneratorControl.h"
#include "beamline/CLS/CLSHVControl.h"

#include "application/SXRMB/SXRMB.h"
#include "beamline/SXRMB/SXRMBBrukerDetector.h"
#include "beamline/SXRMB/SXRMBCrystalChangeModel.h"
#include "beamline/SXRMB/SXRMBFourElementVortexDetector.h"

#include "util/AMBiHash.h"

class AMBasicControlDetectorEmulator;

//#define SXRMBBEAMLINE_ERR_xxx 30092xx

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

	/// returns the low end energy level
	double beamlineEnergyLowEnd() const { return beamlineEnergyLowEnd_; }
	/// returns the high end energy level
	double beamlineEnergyHighEnd() const { return beamlineEnergyHighEnd_; }

	/// Returns the slit for SXRMB
	AMSlits *jjSlits() const;

	/// Returns the scaler for SXRMB
	CLSSIS3820Scaler* scaler() const;

	/// ReturnsEnergy control for SXRMB
	AMPVwStatusControl* energy() const;

	/// Returns the current working endstation
	SXRMB::Endstation currentEndstation() const;

	/// Returns the X Stage for the microrobe sample stage
	AMPVwStatusControl* endstationSampleStageX(SXRMB::Endstation) const;
	/// Returns the Y Stage for the microrobe sample stage
	AMPVwStatusControl* endstationSampleStageY(SXRMB::Endstation) const;
	/// Returns the Z Stage for the microrobe sample stage
	AMPVwStatusControl* endstationSampleStageZ(SXRMB::Endstation) const;
	/// Returns the R Stage for the microrobe sample stage
	AMPVwStatusControl* endstationSampleStageR(SXRMB::Endstation) const;

	/// Returns the X Stage for the microrobe sample stage
	AMPVwStatusControl* microprobeSampleStageX() const;
	/// Returns the Y Stage for the microrobe sample stage
	AMPVwStatusControl* microprobeSampleStageY() const;
	/// Returns the Z Stage for the microrobe sample stage
	AMPVwStatusControl* microprobeSampleStageZ() const;

	/// Returns the height PV control for the ambiant endstation table.
	AMPVwStatusControl *ambiantTableHeight() const;
	/// Returns the X stage for the ambiant endstation.
	AMPVwStatusControl *ambiantSampleStageX() const;
	/// Returns the Z stage for the ambiant endstation.
	AMPVwStatusControl *ambiantSampleStageZ() const;
	/// Returns the Z motor from the ambiant sample manipulator.
	AMPVwStatusControl *ambiantSampleHolderZ() const;
	/// Returns the R motor from the ambiant sample manipulator.
	AMPVwStatusControl *ambiantSampleHolderR() const;

	/// Returns the X Stage for the SolidState End Station sample stage
	AMPVwStatusControl* solidStateSampleStageX() const;
	/// Returns the Y Stage for the SolidState End Station sample stage
	AMPVwStatusControl* solidStateSampleStageY() const;
	/// Returns the Z Stage for the SolidState End Station sample stage
	AMPVwStatusControl* solidStateSampleStageZ() const;
	/// Returns the R Stage for the SolidState End Station sample stage
	AMPVwStatusControl* solidStateSampleStageR() const;

	/// Returns the motor group for this beamline.
	AMMotorGroup *motorGroup() const;
	/// Returns the microprobe stage motor group object.
	AMMotorGroupObject *microprobeSampleStageMotorGroupObject() const;
	/// Returns the solid state stage motor group object.
	AMMotorGroupObject *solidStateSampleStageMotorGroupObject() const;
	/// Returns the ambiant with gas chamber stage motor group object.
	AMMotorGroupObject *ambiantWithGasChamberSampleStageMotorGroupObject() const;
	/// Returns the ambiant without gas chamber sample stage motor group object.
	AMMotorGroupObject *ambiantWithoutGasChamberSampleStageMotorGroupObject() const;
	/// Returns the motorGroup name of the current running endstation
	QString currentMotorGroupName() const;

	/// Returns the cross hair generator.
	CLSCrossHairGeneratorControl *crossHairGenerator() const;
	/// Returns the crystal selection model.
	SXRMBCrystalChangeModel *crystalSelection() const;

	/// Returns whether the PVs are connected or not
	virtual bool isConnected() const;

	/// Returns the beamline I0 Detector
	CLSBasicScalerChannelDetector* beamlineI0Detector() const;
	/// Returns the I0 Detector
	CLSBasicScalerChannelDetector* i0Detector() const;
	/// Returns the TEY Detector
	CLSBasicScalerChannelDetector* teyDetector() const;
	/// Returns the transmission Detector
	CLSBasicScalerChannelDetector* transmissionDetector() const;

	/// Returns the energy feedback detector (emulator)
	AMBasicControlDetectorEmulator* energyFeedbackDetector() const;

	/// Returns the bruker fluorescence detector.
	SXRMBBrukerDetector *brukerDetector() const;
	/// Returns the four element vortex detector.
	SXRMBFourElementVortexDetector *fourElementVortexDetector() const;
	/// Returns the XRF detector with given detector type.
	AMXRFDetector *xrfDetector(SXRMB::FluorescenceDetectors detectorType) const;

	/// Returns the control set of the HV controls
	AMControlSet *beamlineHVControlSet() const;
	/// Returns the control set of the HV controls
	AMControlSet *beamlinePersistentHVControlSet() const;
	/// Returns the I0 HV control
	CLSHVControl *i0HVControl() const;
	/// Returns the TEY HV control
	CLSHVControl *teyHVControl() const;
	/// Returns the microProbe TEY HV control
	CLSHVControl *microprobeTEYHVControl() const;
	/// Returns the ambiant IC0 HV control
	CLSHVControl *ambiantIC0HVControl() const;
	/// Returns the ambiant IC1 HV control
	CLSHVControl *ambiantIC1HVControl() const;

signals:
	void beamAvaliability(bool beamOn);

	void endstationChanged(SXRMB::Endstation fromEndstation, SXRMB::Endstation toEndstation);

public slots:
	/// switch the running endstation
	void switchEndstation(SXRMB::Endstation endstation);

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
	/// Sets up the High Voltage Controls.
	void setupHVControls();
	/// Sets up the exposed actions.
	void setupExposedControls();
	/// Sets up the exposed detectors.
	void setupExposedDetectors();
	/// Sets up the SIGNAL and SLOT connections.
	void setupConnections();

	/// Returns the list of actions to turn the beam on
	virtual AMListAction3* createBeamOnActions() const;
	/// Returns the list of actions to turn the beam off (just photon shutter 2 right now)
	virtual AMListAction3* createBeamOffActions() const;

protected slots:

	/// Handles PhotonShutter State changed signal, turn off TEY HV Control
	void onPhotonShutterStateChanged();
	/// Handles connected status of the SXRMB Endstation PV
	void onEndstationPVConnected(bool);
	/// Handles value change of the SXRMB Endstation PV
	void onEndstationPVValueChanged(double);
	/// Handles connected status of all of the sample stage controls
	void onSampleStagePVsConnected(bool);

protected:
	virtual void createBeamlineStatus();

protected:
	/// the lowest energy range for bruker detector
	double beamlineEnergyLowEnd_;
	/// the highest energy range for bruker detector
	double beamlineEnergyHighEnd_;
	/// the Endstation using right now
	SXRMB::Endstation currentEndstation_;

	/// The shutters control.
	CLSShutters *beamlineShutters_;
	/// The valves control.
	CLSValves *beamlineValves_;

	/// Scaler for SXRMB
	CLSSIS3820Scaler *scaler_;

	/// Endstation control for SXRMB
	AMPVControl *endstationControl_;

	/// The JJ slits
	AMSlits *jjSlits_;

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
	/// Control set for microprobe sample stage
	AMControlSet *microprobeSampleStageControlSet_;

	/// X Stage for the Solid State sample stage
	AMPVwStatusControl *solidStateSampleStageX_;
	/// Y Stage for the Solid State sample stage
	AMPVwStatusControl *solidStateSampleStageY_;
	/// Z Stage for the Solid State sample stage
	AMPVwStatusControl *solidStateSampleStageZ_;
	/// R Stage for the Solid State sample stage
	AMPVwStatusControl *solidStateSampleStageR_;
	/// Control set for Solid State sample stage
	AMControlSet *solidStateSampleStageControlSet_;

	/// Height PV control for the Ambiant endstation table
	AMPVwStatusControl *ambiantTableHeight_;
	/// X Stage for the Ambiant sample stage
	AMPVwStatusControl *ambiantSampleStageX_;
	/// Z Stage for the Solid State sample stage
	AMPVwStatusControl *ambiantSampleStageZ_;
	/// Z Stage for the Ambiant sample holder
	AMPVwStatusControl *ambiantSampleHolderZ_;
	/// R Stage for the Solid State sample holder
	AMPVwStatusControl *ambiantSampleHolderR_;

	/// Control set for Ambiant with gas chamber sample stage
	AMControlSet *ambiantWithGasChamberSampleStageControlSet_;

	/// Control set for Ambiant w/o gas chamber sample stage
	AMControlSet *ambiantWithoutGasChamberSampleStageControlSet_;

	/// Detector for the beamline I0 detector channel on the scaler.
	CLSBasicScalerChannelDetector *beamlineI0Detector_;
	/// Detector for the I0Detector channel on the scaler
	CLSBasicScalerChannelDetector *i0Detector_;
	/// Detector for the TEYDetctor channel on the scaler
	CLSBasicScalerChannelDetector *teyDetector_;
	/// Detector for the transmission ion chamber on the scaler.
	CLSBasicScalerChannelDetector *transmissionDetector_;


	/// Motor group.  Holds sets of motors that are used together.
	AMMotorGroup *motorGroup_;

	/// The cross hair generator.
	CLSCrossHairGeneratorControl *crossHairGenerator_;
	/// The crystal selection model.
	SXRMBCrystalChangeModel *crystalSelection_;

	/// The bruker detector.
	SXRMBBrukerDetector *brukerDetector_;
	/// The four element vortex detector.
	SXRMBFourElementVortexDetector *fourElementVortexDetector_;

	/// The control set of the HV controls
	AMControlSet *beamlineHVControlSet_;
	/// The control set of the HV controls
	AMControlSet *beamlinePersistentHVControlSet_;
	/// The IO HV control
	CLSHVControl *i0HVControl_;
	/// The TEY HV control
	CLSHVControl *teyHVControl_;
	/// The microProb TEY HV control
	CLSHVControl *microprobeTEYHVControl_;
	/// The ambiant IC0 HV control
	CLSHVControl *ambiantIC0HVControl_;
	/// The ambiant IC1 HV control
	CLSHVControl *ambiantIC1HVControl_;

	CLSExclusiveStatesControl *PSH1406B1002Shutter_;
};

#endif // SXRMBBEAMLINE_H
