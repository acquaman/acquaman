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


#ifndef BIOXASIMAGINGBEAMLINE_H
#define BIOXASIMAGINGBEAMLINE_H

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
#include "beamline/BioXAS/BioXASCLSMAXvMotor.h"

class BioXASImagingBeamline : public AMBeamline
{
	Q_OBJECT

public:
	/// Returns the instance of the beamline that has been created.
	static BioXASImagingBeamline* bioXAS()
	{
		if(!imagingInstance_)
			imagingInstance_ = new BioXASImagingBeamline();

		return static_cast<BioXASImagingBeamline*>(imagingInstance_);
	}

	/// Destructor.
	virtual ~BioXASImagingBeamline();

	QList<BioXASCLSMAXvMotor *> getMotorsByType(BioXASBeamlineDef::BioXASMotorType category);

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

	/// Constructor. This is a singleton class, access it through BioXASImagingBeamline::bioXAS().
	BioXASImagingBeamline();

	/// BioXAS imaging beamline motors
	/// BioXAS filter motors
	BioXASCLSMAXvMotor *imagingCarbonFilterFarm1_;
	BioXASCLSMAXvMotor *imagingCarbonFilterFarm2_;

	/// BioXAS M1 motors
	BioXASCLSMAXvMotor *m1VertUpStreamINB_;
	BioXASCLSMAXvMotor *m1VertUpStreamOUTB_;
	BioXASCLSMAXvMotor *m1VertDownStream_;
	BioXASCLSMAXvMotor *m1StripeSelect_;
	BioXASCLSMAXvMotor *m1Yaw_;
	BioXASCLSMAXvMotor *m1BenderUpstream_;
	BioXASCLSMAXvMotor *m1BenderDownStream_;

	/// BioXAS Variable Mask motors
	BioXASCLSMAXvMotor *variableMaskVertUpperBlade_;
	BioXASCLSMAXvMotor *variableMaskVertLowerBlade_;

	/// BioXAS Mono motors
	BioXASCLSMAXvMotor *monoCrystal2Z_;
	BioXASCLSMAXvMotor *monoCrystal2Y_;
	BioXASCLSMAXvMotor *monoCrystal2Pitch_;
	BioXASCLSMAXvMotor *monoCrystal2Roll_;
	BioXASCLSMAXvMotor *monoBragg_;
	BioXASCLSMAXvMotor *monoVertical_;
	BioXASCLSMAXvMotor *monoLateral_;

	/// BioXAS M2 motors
	BioXASCLSMAXvMotor *m2VertUpstreamINB_;
	BioXASCLSMAXvMotor *m2VertUpstreamOUTB_;
	BioXASCLSMAXvMotor *m2VertDownstream_;
	BioXASCLSMAXvMotor *m2StripeSelect_;
	BioXASCLSMAXvMotor *m2Yaw_;
	BioXASCLSMAXvMotor *m2BenderUpstream_;
	BioXASCLSMAXvMotor *m2BenderDownStream_;

private:
	/// Instance variable
	static BioXASImagingBeamline* imagingInstance_;
};

#endif // BIOXASIMAGINGBEAMLINE_H
