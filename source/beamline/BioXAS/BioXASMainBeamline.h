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


#ifndef BIOXASMAINBEAMLINE_H
#define BIOXASMAINBEAMLINE_H

#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSKeithley428.h"

#include "beamline/BioXAS/BioXASPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "beamline/BioXAS/BioXASMainMonochromator.h"
#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"
#include "beamline/BioXAS/BioXASMainM2Mirror.h"

#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

class BioXASMainBeamline : public BioXASBeamline
{
	Q_OBJECT

public:
	/// Returns the instance of the beamline that has been created.
	static BioXASMainBeamline* bioXAS()
	{
		if(!instance_){
			instance_ = new BioXASMainBeamline();
			instance_->initializeBeamlineSupport();
		}

		return static_cast<BioXASMainBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~BioXASMainBeamline();

    /// Returns true if all beamline components are connected, false otherwise.
    virtual bool isConnected() const { return connected_; }

	/// Returns the beamline m2 mirror.
	virtual BioXASM2Mirror *m2Mirror() const { return m2Mirror_; }
	/// Returns the beamline monochromator.
	virtual BioXASMainMonochromator *mono() const { return mono_; }
    /// Returns the scaler.
    virtual CLSSIS3820Scaler* scaler() const { return scaler_; }

    /// Returns the I0 amplifier.
    CLSKeithley428* i0Keithley() const { return i0Keithley_; }
    /// Returns the IT amplifier.
    CLSKeithley428* iTKeithley() const { return iTKeithley_; }
    /// Returns the I2 amplifier.
    CLSKeithley428* i2Keithley() const { return i2Keithley_; }

    /// Returns the I0 detector.
    CLSBasicScalerChannelDetector* i0Detector() const { return i0Detector_; }
    /// Returns the IT detector.
    CLSBasicScalerChannelDetector* iTDetector() const { return iTDetector_; }
    /// Returns the I2 detector.
    CLSBasicScalerChannelDetector* i2Detector() const { return i2Detector_; }
    /// Returns the energy feedback detector.
    AMBasicControlDetectorEmulator* energyFeedbackDetector() const { return energyFeedbackDetector_; }
    /// Returns the scaler dwell time detector.
	AMBasicControlDetectorEmulator* dwellTimeDetector() const { return dwellTimeDetector_; }
	/// Returns the bragg motor detector.
	AMBasicControlDetectorEmulator* braggDetector() const { return braggDetector_; }
    /// Returns the bragg move retries detector.
    AMBasicControlDetectorEmulator* braggMoveRetriesDetector() const { return braggMoveRetriesDetector_; }
    /// Returns the bragg move max retries detector.
    AMBasicControlDetectorEmulator* braggMoveRetriesMaxDetector() const { return braggMoveRetriesMaxDetector_; }
    /// Returns the bragg step setpoint detector.
    AMBasicControlDetectorEmulator* braggStepSetpointDetector() const { return braggStepSetpointDetector_; }
    /// Returns the bragg degree setpoint detector.
    AMBasicControlDetectorEmulator* braggDegreeSetpointDetector() const { return braggDegreeSetpointDetector_; }
    /// Returns the physical bragg angle detector.
    AMBasicControlDetectorEmulator* braggAngleDetector() const { return braggAngleDetector_; }

    /// Return the set of BioXAS Motors by given motor category.
	QList<AMControl *> getMotorsByType(BioXASBeamlineDef::BioXASMotorType category);

protected slots:
	/// Updates the beamline's reported connection state.
	void onConnectedChanged();

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

	/// Constructor. This is a singleton class, access it through BioXASMainBeamline::bioXAS().
	BioXASMainBeamline();

	/// BioXAS main beamline motors
	/// BioXAS filter motors
	CLSMAXvMotor *carbonFilterFarm1_;
	CLSMAXvMotor *carbonFilterFarm2_;

	/// BioXAS M1 motors
	CLSMAXvMotor *m1VertUpStreamINB_;
	CLSMAXvMotor *m1VertUpStreamOUTB_;
	CLSMAXvMotor *m1VertDownStream_;
	CLSMAXvMotor *m1StripeSelect_;
	CLSMAXvMotor *m1Yaw_;
	CLSMAXvMotor *m1BenderUpstream_;
	CLSMAXvMotor *m1BenderDownStream_;
	CLSMAXvMotor *m1UpperSlitBlade_;

	/// BioXAS Variable Mask motors
	CLSMAXvMotor *variableMaskVertUpperBlade_;
	CLSMAXvMotor *variableMaskVertLowerBlade_;

    // Monochromator

    BioXASMainMonochromator *mono_;

	// M2 mirror

	BioXASMainM2Mirror *m2Mirror_;

	/// BioXAS Pseudo motors
	BioXASPseudoMotorControl *m1PseudoRoll_;
	BioXASPseudoMotorControl *m1PseudoPitch_;
	BioXASPseudoMotorControl *m1PseudoHeight_;
	BioXASPseudoMotorControl *m1PseudoYaw_;
	BioXASPseudoMotorControl *m1PseudoLateral_;

	BioXASPseudoMotorControl *m2PseudoRoll_;
	BioXASPseudoMotorControl *m2PseudoPitch_;
	BioXASPseudoMotorControl *m2PseudoHeight_;
	BioXASPseudoMotorControl *m2PseudoYaw_;
	BioXASPseudoMotorControl *m2PseudoLateral_;

	BioXASPseudoMotorControl *monoPseudoEnergy_;
	AMPVwStatusControl *monoBraggAngle_;

protected:
    // Connected
    bool connected_;

    // Detectors
    CLSBasicScalerChannelDetector *i0Detector_;
    CLSBasicScalerChannelDetector *iTDetector_;
    CLSBasicScalerChannelDetector *i2Detector_;
    AMBasicControlDetectorEmulator *energyFeedbackDetector_;
    AMBasicControlDetectorEmulator *dwellTimeDetector_;
	AMBasicControlDetectorEmulator *braggDetector_;
    AMBasicControlDetectorEmulator *braggMoveRetriesDetector_;
    AMBasicControlDetectorEmulator *braggMoveRetriesMaxDetector_;
    AMBasicControlDetectorEmulator *braggStepSetpointDetector_;
    AMBasicControlDetectorEmulator *braggDegreeSetpointDetector_;
    AMBasicControlDetectorEmulator *braggAngleDetector_;

    // Scaler
    CLSSIS3820Scaler *scaler_;
    AMReadOnlyPVControl *scalerDwellTime_;

    // Amplifiers
    CLSKeithley428 *i0Keithley_;
    CLSKeithley428 *iTKeithley_;
    CLSKeithley428 *i2Keithley_;
};

#endif // BIOXASMAINBEAMLINE_H
