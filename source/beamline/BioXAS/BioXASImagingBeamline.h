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

#include "beamline/CLS/CLSMAXvMotor.h"

#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "beamline/BioXAS/BioXASPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASImagingCarbonFilterFarm.h"

class BioXASImagingBeamline : public BioXASBeamline
{
	Q_OBJECT

public:
	/// Returns the instance of the beamline that has been created.
	static BioXASImagingBeamline* bioXAS()
	{
		if(!instance_){
			instance_ = new BioXASImagingBeamline();
			instance_->initializeBeamline();
		}

		return static_cast<BioXASImagingBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~BioXASImagingBeamline();

	/// Returns the current connected state.
	virtual bool isConnected() const;

	/// Returns the carbon filter farm.
	virtual BioXASImagingCarbonFilterFarm* carbonFilterFarm() const { return carbonFilterFarm_; }

	QList<AMControl *> getMotorsByType(BioXASBeamlineDef::BioXASMotorType category);

protected:
	/// Sets up various beamline components.
	virtual void setupComponents();
	/// Sets up all of the detectors that need to be added to scans that aren't a part of typical detectors.  This may just be temporary, not sure.
	void setupControlsAsDetectors();
	/// Sets up the exposed controls.
	void setupExposedControls();
	/// Sets up the exposed detectors.
	void setupExposedDetectors();

	/// helper function to check whether the beam of a beamline is available or not --- this usually is combined with beamline status PV and/or beamline shutters/valves stauts
	virtual bool isBeamlineBeamAvailable();

	/// Constructor. This is a singleton class, access it through BioXASImagingBeamline::bioXAS().
	BioXASImagingBeamline();

protected:
	/// The carbon filter farm.
	BioXASImagingCarbonFilterFarm *carbonFilterFarm_;

	/// BioXAS filter motors
	CLSMAXvMotor *imagingCarbonFilterFarm1_;
	CLSMAXvMotor *imagingCarbonFilterFarm2_;

	/// BioXAS M1 motors
	CLSMAXvMotor *m1VertUpStreamINB_;
	CLSMAXvMotor *m1VertUpStreamOUTB_;
	CLSMAXvMotor *m1VertDownStream_;
	CLSMAXvMotor *m1StripeSelect_;
	CLSMAXvMotor *m1Yaw_;
	CLSMAXvMotor *m1BenderUpstream_;
	CLSMAXvMotor *m1BenderDownStream_;

	/// BioXAS Variable Mask motors
	CLSMAXvMotor *variableMaskVertUpperBlade_;
	CLSMAXvMotor *variableMaskVertLowerBlade_;

	/// BioXAS Mono motors
	CLSMAXvMotor *monoCrystal2Z_;
	CLSMAXvMotor *monoCrystal2Y_;
	CLSMAXvMotor *monoCrystal2Pitch_;
	CLSMAXvMotor *monoCrystal2Roll_;
	CLSMAXvMotor *monoBragg_;
	CLSMAXvMotor *monoVertical_;
	CLSMAXvMotor *monoLateral_;

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
};

#endif // BIOXASIMAGINGBEAMLINE_H
