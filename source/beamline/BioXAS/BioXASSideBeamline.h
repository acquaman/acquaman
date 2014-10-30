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


#ifndef BIOXASSIDEBEAMLINE_H
#define BIOXASSIDEBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSKeithley428.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"
#include "beamline/BioXAS/BioXASCLSMAXvMotor.h"


#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

class CLSMAXvMotor;
class AMBasicControlDetectorEmulator;

class BioXASSideBeamline : public AMBeamline
{
	Q_OBJECT

public:
	/// Returns the instance of the beamline that has been created.
	static BioXASSideBeamline* bioXAS()
	{
		if(!instance_){
			instance_ = new BioXASSideBeamline();
			instance_->initializeBeamlineSupport();
		}

		return static_cast<BioXASSideBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~BioXASSideBeamline();

	/// Returns the scaler.
	CLSSIS3820Scaler* scaler();

	/// Returns the m1 upper slit blade motor control.
	CLSMAXvMotor* m1UpperSlit();

	/// Returns an instance of the keithley428 amplifier.
	CLSKeithley428* keithley();

	virtual bool isConnected() const;

	CLSBasicScalerChannelDetector* testDetector();

	QList<BioXASCLSMAXvMotor *> getMotorsByType(BioXASBeamlineDef::BioXASMotorType category);
	AMPVwStatusControl* energy();

	/// Returns an instance of the keithley428 amplifier.
	CLSKeithley428* i0Keithley();
	CLSKeithley428* iTKeithley();

	CLSBasicScalerChannelDetector* i0Detector();
	CLSBasicScalerChannelDetector* iTDetector();
	AMBasicControlDetectorEmulator* energyFeedbackDetector();

protected slots:
	void onScalerConnectedChanged(bool connectionState);
	void onM1UpperSlitConnectedChanged(bool connectionState);
	void onEnergyConnectedChanged(bool connectionState);

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

	/// Constructor. This is a singleton class, access it through BioXASSideBeamline::bioXAS().
	BioXASSideBeamline();

protected:
	CLSSIS3820Scaler *scaler_;

	bool wasConnected_;

	CLSBasicScalerChannelDetector *testDetector_;
	CLSKeithley428 *keithley_;

	CLSMAXvMotor *m1UpperSlit_;
	AMPVwStatusControl *energy_;

	CLSBasicScalerChannelDetector *i0Detector_;
	CLSKeithley428 *i0Keithley_;

	CLSBasicScalerChannelDetector *iTDetector_;
	CLSKeithley428 *iTKeithley_;

	AMControl *energyFeedbackControl_;
	AMBasicControlDetectorEmulator *energyFeedbackDetector_;

	// Shutters
	CLSBiStateControl *psh1_;
	CLSBiStateControl *psh2_;
	CLSBiStateControl *ssh1_;

	CLSBiStateControl *sshSide1_;

	// Pressure controls
	AMControl *ccg1_;
	AMControl *ccg2_;
	AMControl *ccg3_;

	AMControl *ccgSide1_;
	AMControl *ccgSide2_;
	AMControl *ccgSide3_;
	AMControl *ccgSide4_;
	AMControl *ccgSide5_;
	AMControl *ccgSide6_;

	// vacuum valve controls
	CLSBiStateControl *vvr1_;
	CLSBiStateControl *vvr2_;
	CLSBiStateControl *vvr3_;
	CLSBiStateControl *vvr4_;
	CLSBiStateControl *vvr5_;

	CLSBiStateControl *vvrSide1_;
	CLSBiStateControl *vvrSide2_;
	CLSBiStateControl *vvrSide3_;
	CLSBiStateControl *vvrSide4_;
	CLSBiStateControl *vvrSide5_;
	CLSBiStateControl *vvrSide6_;

	// Ion pump controls
	AMControl *iop1_;
	AMControl *iop2_;
	AMControl *iop3_;
	AMControl *iop4_;
	AMControl *iop5_;

	AMControl *iopSide1_;
	AMControl *iopSide2_;
	AMControl *iopSide3_;
	AMControl *iopSide4_;
	AMControl *iopSide5_;
	AMControl *iopSide6_;
	AMControl *iopSide7_;

	// Flow transducer controls
	AMControl *flt1_;
	AMControl *flt2_;
	AMControl *flt3_;
	AMControl *flt4_;
	AMControl *flt5_;
	AMControl *flt6_;
	AMControl *flt7_;
	AMControl *flt8_;
	AMControl *flt9_;
	AMControl *flt10_;
	AMControl *flt11_;
	AMControl *flt12_;
	AMControl *flt13_;
	AMControl *flt14_;
	AMControl *flt15_;
	AMControl *flt16_;
	AMControl *flt17_;
	AMControl *flt18_;
	AMControl *flt19_;
	AMControl *flt20_;

	// Flow switch controls
	AMControl *swf1_;
	AMControl *swf2_;
	AMControl *swf3_;
	AMControl *swf4_;

	// Temperature monitor controls
	AMControl *tm1_;
	AMControl *tm2_;
	AMControl *tm3_;
	AMControl *tm4_;
	AMControl *tm5_;

	/// BioXAS side beamline motors
	/// BioXAS filter motors
	BioXASCLSMAXvMotor *carbonFilterFarm1_;
	BioXASCLSMAXvMotor *carbonFilterFarm2_;

	/// BioXAS M1 motors
	BioXASCLSMAXvMotor *m1VertUpStreamINB_;
	BioXASCLSMAXvMotor *m1VertUpStreamOUTB_;
	BioXASCLSMAXvMotor *m1VertDownStream_;
	BioXASCLSMAXvMotor *m1StripeSelect_;
	BioXASCLSMAXvMotor *m1Yaw_;
	BioXASCLSMAXvMotor *m1BenderUpstream_;
	BioXASCLSMAXvMotor *m1BenderDownStream_;
	BioXASCLSMAXvMotor *m1UpperSlitBlade_;

	/// BioXAS Variable Mask motors
	BioXASCLSMAXvMotor *variableMaskVertUpperBlade_;
	BioXASCLSMAXvMotor *variableMaskVertLowerBlade_;

	/// BioXAS Mono motors
	BioXASCLSMAXvMotor *monoPhosphorPaddle_;
	BioXASCLSMAXvMotor *monoBragg_;
	BioXASCLSMAXvMotor *monoVertical_;
	BioXASCLSMAXvMotor *monoLateral_;
	BioXASCLSMAXvMotor *monoXtalXchage_;
	BioXASCLSMAXvMotor *monoXtal1Pitch_;
	BioXASCLSMAXvMotor *monoXtal1Roll_;
	BioXASCLSMAXvMotor *monoXtal2Pitch_;
	BioXASCLSMAXvMotor *monoXtal2Roll_;

	/// BioXAS M2 motors
	BioXASCLSMAXvMotor *m2VertUpstreamINB_;
	BioXASCLSMAXvMotor *m2VertUpstreamOUTB_;
	BioXASCLSMAXvMotor *m2VertDownstream_;
	BioXASCLSMAXvMotor *m2StripeSelect_;
	BioXASCLSMAXvMotor *m2Yaw_;
	BioXASCLSMAXvMotor *m2BenderUpstream_;
	BioXASCLSMAXvMotor *m2BenderDownStream_;

};

#endif // BIOXASSIDEBEAMLINE_H
