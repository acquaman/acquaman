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
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"


#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"
#include "beamline/BioXAS/BioXASToolSuite/BioXASMAXvMotor.h"
#include "beamline/BioXAS/BioXASToolSuite/BioXASBeamline.h"

class BioXASMainBeamline : public AMBeamline
{
	Q_OBJECT

public:
	/// Returns the instance of the beamline that has been created.
	static BioXASMainBeamline* bioXAS()
	{
		if(!instance_)
			instance_ = new BioXASMainBeamline();

		return static_cast<BioXASMainBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~BioXASMainBeamline();

	QList<BioXASMAXvMotor *> getMotorsByType(BioXASBeamlineDef::BioXASMotorType category);

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
	BioXASMAXvMotor *carbonFilterFarm1_;
	BioXASMAXvMotor *carbonFilterFarm2_;

	/// BioXAS M1 motors
	BioXASMAXvMotor *m1VertUpStreamINB_;
	BioXASMAXvMotor *m1VertUpStreamOUTB_;
	BioXASMAXvMotor *m1VertDownStream_;
	BioXASMAXvMotor *m1StripeSelect_;
	BioXASMAXvMotor *m1Yaw_;
	BioXASMAXvMotor *m1BenderUpstream_;
	BioXASMAXvMotor *m1BenderDownStream_;
	BioXASMAXvMotor *m1UpperSlitBlade_;

	/// BioXAS Variable Mask motors
	BioXASMAXvMotor *variableMaskVertUpperBlade_;
	BioXASMAXvMotor *variableMaskVertLowerBlade_;

	/// BioXAS Mono motors
	BioXASMAXvMotor *monoPhosphorPaddle_;
	BioXASMAXvMotor *monoBragg_;
	BioXASMAXvMotor *monoVertical_;
	BioXASMAXvMotor *monoLateral_;
	BioXASMAXvMotor *monoXtalXchage_;
	BioXASMAXvMotor *monoXtal1Pitch_;
	BioXASMAXvMotor *monoXtal1Roll_;
	BioXASMAXvMotor *monoXtal2Pitch_;
	BioXASMAXvMotor *monoXtal2Roll_;

	/// BioXAS M2 motors
	BioXASMAXvMotor *m2VertUpstreamINB_;
	BioXASMAXvMotor *m2VertUpstreamOUTB_;
	BioXASMAXvMotor *m2VertDownstream_;
	BioXASMAXvMotor *m2StripeSelect_;
	BioXASMAXvMotor *m2Yaw_;
	BioXASMAXvMotor *m2BenderUpstream_;
	BioXASMAXvMotor *m2BenderDownStream_;
};

#endif // BIOXASMAINBEAMLINE_H
