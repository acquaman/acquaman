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

#include "beamline/AMBeamline.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"
#include "beamline/CLS/CLSMAXvMotor.h"

#include "beamline/BioXAS/BioXASPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "beamline/CLS/CLSKeithley428.h"

#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

class BioXASMainBeamline : public AMBeamline
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

    /// Returns the I0 amplifier.
    CLSKeithley428* i0Amplifier() const { return i0Keithley_; }
    /// Returns the IT amplifier.
    CLSKeithley428* iTAmplifier() const { return iTKeithley_; }
    /// Returns the I2 amplifier.
    CLSKeithley428* i2Amplifier() const { return i2Keithley_; }

    /// Return the set of BioXAS Motors by given motor category.
	QList<AMControl *> getMotorsByType(BioXASBeamlineDef::BioXASMotorType category);

protected:
	/// Sets up the synchronized dwell time.
	void setupSynchronizedDwellTime();
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

	/// BioXAS Mono motors
	CLSMAXvMotor *monoPhosphorPaddle_;
	CLSMAXvMotor *monoBragg_;
	CLSMAXvMotor *monoVertical_;
	CLSMAXvMotor *monoLateral_;
	CLSMAXvMotor *monoXtalXchage_;
	CLSMAXvMotor *monoXtal1Pitch_;
	CLSMAXvMotor *monoXtal1Roll_;
	CLSMAXvMotor *monoXtal2Pitch_;
	CLSMAXvMotor *monoXtal2Roll_;

	/// BioXAS M2 motors
	CLSMAXvMotor *m2VertUpstreamINB_;
	CLSMAXvMotor *m2VertUpstreamOUTB_;
	CLSMAXvMotor *m2VertDownstream_;
	CLSMAXvMotor *m2StripeSelect_;
	CLSMAXvMotor *m2Yaw_;
	CLSMAXvMotor *m2BenderUpstream_;
	CLSMAXvMotor *m2BenderDownStream_;

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

protected:
    /// Amplifiers
    CLSKeithley428 *i0Keithley_;
    CLSKeithley428 *iTKeithley_;
    CLSKeithley428 *i2Keithley_;
};

#endif // BIOXASMAINBEAMLINE_H
