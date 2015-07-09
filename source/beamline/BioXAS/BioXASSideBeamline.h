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

#include "beamline/AMMotorGroup.h"
#include "beamline/AMPVControl.h"

#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSKeithley428.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSJJSlits.h"
#include "beamline/CLS/CLSJJSlitGapControl.h"
#include "beamline/CLS/CLSJJSlitCenterControl.h"
#include "beamline/CLS/CLSStandardsWheel.h"

#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "beamline/BioXAS/BioXASSideMonochromator.h"
#include "beamline/BioXAS/BioXASPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASSideCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASSideXIAFilters.h"
#include "beamline/BioXAS/BioXASSideM1Mirror.h"
#include "beamline/BioXAS/BioXASSideM2Mirror.h"
#include "beamline/BioXAS/BioXASSideDBHRMirror.h"
#include "beamline/BioXAS/BioXASEndstationTable.h"

#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

class AMBasicControlDetectorEmulator;

class BioXASSideBeamline : public BioXASBeamline
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

	/// Returns the current connected state.
	virtual bool isConnected() const;

	/// Returns the current 'beam off' state, true if the endstation safety shutter is closed. False otherwise.
	virtual bool beamOff() const;
	/// Returns the current 'beam on' state, true if the endstation safety shutter is open. False otherwise.
	virtual bool beamOn() const;

	/// Returns a newly created action that turns off beam by closing the endstation (downstream) safety shutter. Returns 0 if not connected.
	virtual AMAction3* createTurnOffBeamActions();
	/// Returns a newly created action that turns on beam by opening the endstation (upstream) safety shutter. Returns 0 if not connected.
	virtual AMAction3* createTurnOnBeamActions();

	/// Returns the m1 mirror.
	virtual BioXASM1Mirror* m1Mirror() const { return m1Mirror_; }
	/// Returns the carbon filter farm.
	BioXASSideCarbonFilterFarm* carbonFilterFarm() const { return carbonFilterFarm_; }
	/// Returns the XIA filters.
	BioXASSideXIAFilters* xiaFilters() const { return xiaFilters_; }
	/// Returns the DBHR mirrors.
	BioXASSideDBHRMirror* dbhrMirror() const { return dbhrMirror_; }
	/// Returns the mono.
	virtual BioXASSSRLMonochromator* mono() const { return mono_; }
	/// Returns the m2 mirror.
	virtual BioXASM2Mirror* m2Mirror() const { return m2Mirror_; }
	/// Returns the standards wheel.
	CLSStandardsWheel *standardsWheel() const { return standardsWheel_; }
	/// Returns the beamline JJ Slit.
	CLSJJSlits *jjSlits() const { return jjSlits_; }
	/// Returns the scaler.
	virtual CLSSIS3820Scaler* scaler() const { return scaler_; }

	// Pressure monitors.

	AMControl *ccgSide1() const { return ccgSide1_; }
	AMControl *ccgSide2() const { return ccgSide2_; }
	AMControl *ccgSide3() const { return ccgSide3_; }
	AMControl *ccgSide4() const { return ccgSide4_; }
	AMControl *ccgSide5() const { return ccgSide5_; }
	AMControl *ccgSide6() const { return ccgSide6_; }

	// Vaccum values.


	AMControl *vvrSide1() const { return vvrSide1_; }
	AMControl *vvrSide2() const { return vvrSide2_; }
	AMControl *vvrSide3() const { return vvrSide3_; }
	AMControl *vvrSide4() const { return vvrSide4_; }
	AMControl *vvrSide5() const { return vvrSide5_; }
	AMControl *vvrSide6() const { return vvrSide6_; }

	// Ion pumps.


	AMControl *iopSide1() const { return iopSide1_; }
	AMControl *iopSide2() const { return iopSide2_; }
	AMControl *iopSide3() const { return iopSide3_; }
	AMControl *iopSide4() const { return iopSide4_; }
	AMControl *iopSide5() const { return iopSide5_; }
	AMControl *iopSide6() const { return iopSide6_; }
	AMControl *iopSide7() const { return iopSide7_; }

	// Flow transducers.

	AMControl *flt5() const { return flt5_; }
	AMControl *flt6() const { return flt6_; }
	AMControl *flt7() const { return flt7_; }
	AMControl *flt8() const { return flt8_; }
	AMControl *flt9() const { return flt9_; }
	AMControl *flt10() const { return flt10_; }
	AMControl *flt11() const { return flt11_; }
	AMControl *flt12() const { return flt12_; }
	AMControl *flt13() const { return flt13_; }
	AMControl *flt14() const { return flt14_; }
	AMControl *flt15() const { return flt15_; }
	AMControl *flt16() const { return flt16_; }
	AMControl *flt17() const { return flt17_; }
	AMControl *flt18() const { return flt18_; }
	AMControl *flt19() const { return flt19_; }
	AMControl *flt20() const { return flt20_; }

	// Flow switches.



	// Temperature monitors.



	/// return the set of BioXAS Motors by given motor category
	QList<AMControl *> getMotorsByType(BioXASBeamlineDef::BioXASMotorType category) const;

	// Motor controls.
	/// Returns the lateral detector stage motor.
	CLSMAXvMotor* detectorStageLateral() const { return detectorStageLateral_; }
	/// Returns the cryostat x motor.
	CLSMAXvMotor* cryostatX() const { return cryostatX_; }
	/// Returns the cryostat y motor.
	CLSMAXvMotor* cryostatY() const { return cryostatY_; }
	/// Returns the cryostat z motor.
	CLSMAXvMotor* cryostatZ() const { return cryostatZ_; }

	/// Returns the motor group.
	AMMotorGroup* motorGroup() const { return motorGroup_; }

	// Current amplifiers
	/// Returns the I0 Keithley428 amplifier.
	CLSKeithley428* i0Keithley() const { return i0Keithley_; }
	/// Returns the IT Keithley428 amplifier.
	CLSKeithley428* i1Keithley() const { return i1Keithley_; }
	/// Returns the I2 Keithley 428 amplifier.
	CLSKeithley428* i2Keithley() const { return i2Keithley_; }

	// Detectors
	/// Returns the I0 scaler channel detector.
	virtual AMDetector* i0Detector() const { return i0Detector_; }
	/// Returns the I1 scaler channel detector.
	virtual AMDetector* i1Detector() const { return i1Detector_; }
	/// Returns the I2 scaler channel detector.
	virtual AMDetector* i2Detector() const { return i2Detector_; }
	/// Returns the bragg encoder-based energy setpoint detector.
	AMBasicControlDetectorEmulator* encoderEnergySetpointDetector() const { return encoderEnergySetpointDetector_; }
	/// Returns the bragg encoder-based energy feedback detector.
	AMBasicControlDetectorEmulator* encoderEnergyFeedbackDetector() const { return encoderEnergyFeedbackDetector_; }
	/// Returns the bragg step-based energy feedback detector.
	AMBasicControlDetectorEmulator* stepEnergyFeedbackDetector() const { return stepEnergyFeedbackDetector_; }
	/// Returns the scaler dwell time detector.
	virtual AMBasicControlDetectorEmulator* scalerDwellTimeDetector() const { return dwellTimeDetector_; }
	/// Returns the bragg motor detector.
	AMBasicControlDetectorEmulator* braggDetector() const { return braggDetector_; }
	/// Returns the bragg motor encoder feedback detector.
	AMBasicControlDetectorEmulator* braggEncoderFeedbackDetector() const { return braggEncoderFeedbackDetector_; }
	/// Returns the bragg move retries detector.
	AMBasicControlDetectorEmulator* braggMoveRetriesDetector() const { return braggMoveRetriesDetector_; }
	/// Returns the bragg step setpoint detector.
	AMBasicControlDetectorEmulator* braggStepSetpointDetector() const { return braggStepSetpointDetector_; }

	/// Returns the 32 element Ge detector.
	virtual BioXAS32ElementGeDetector *ge32ElementDetector() const { return ge32ElementDetector_; }
	/// Returns the four element Vortex detector.
	virtual BioXASFourElementVortexDetector *fourElementVortexDetector() const { return fourElementVortexDetector_; }

	// Endstation table
	BioXASEndstationTable *endstationTable() const { return endstationTable_; }

protected:
	/// Sets up the readings such as pressure, flow switches, temperature, etc.
	void setupDiagnostics();
	/// Sets up logical groupings of controls into sets.
	void setupControlSets();
	/// Sets up all the detectors.
	void setupDetectors();
	/// Sets up the cryostat stage motors.
	void setupCryostatStage();
	/// Sets up the sample stage motors.
	void setupSampleStage();
	/// Sets up the detector stage motors.
	void setupDetectorStage();
	/// Sets up various beamline components.
	virtual void setupComponents();
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
	// Detectors
	CLSBasicScalerChannelDetector *i0Detector_;
	CLSBasicScalerChannelDetector *i1Detector_;
	CLSBasicScalerChannelDetector *i2Detector_;
	AMBasicControlDetectorEmulator *encoderEnergySetpointDetector_;
	AMBasicControlDetectorEmulator *encoderEnergyFeedbackDetector_;
	AMBasicControlDetectorEmulator *stepEnergySetpointDetector_;
	AMBasicControlDetectorEmulator *stepEnergyFeedbackDetector_;
	AMBasicControlDetectorEmulator *dwellTimeDetector_;
	AMBasicControlDetectorEmulator *braggDetector_;
	AMBasicControlDetectorEmulator *braggMoveRetriesDetector_;
	AMBasicControlDetectorEmulator *braggStepSetpointDetector_;
	AMBasicControlDetectorEmulator *braggEncoderFeedbackDetector_;
	BioXAS32ElementGeDetector *ge32ElementDetector_;
	BioXASFourElementVortexDetector *fourElementVortexDetector_;

	/// The endstation safety shutter.
	CLSBiStateControl *safetyShutterES_;

	/// The M1 mirror.
	BioXASSideM1Mirror *m1Mirror_;
	/// The M2 mirror.
	BioXASSideM2Mirror *m2Mirror_;

	/// The JJ slits
	CLSJJSlits *jjSlits_;

	// The mono.
	BioXASSSRLMonochromator *mono_;

	// Scaler

	CLSSIS3820Scaler *scaler_;
	AMControl *scalerDwellTime_;

	// Amplifiers

	CLSKeithley428 *i0Keithley_;
	CLSKeithley428 *i1Keithley_;
	CLSKeithley428 *i2Keithley_;

	// Filters.

	BioXASSideCarbonFilterFarm *carbonFilterFarm_;
	BioXASSideXIAFilters *xiaFilters_;

	// DBHR mirror.

	BioXASSideDBHRMirror *dbhrMirror_;

	// Misc controls

	AMControl *energySetpointControl_;

	// Extras

	CLSStandardsWheel *standardsWheel_;

	// Pressure controls

	AMControl *ccgSide1_;
	AMControl *ccgSide2_;
	AMControl *ccgSide3_;
	AMControl *ccgSide4_;
	AMControl *ccgSide5_;
	AMControl *ccgSide6_;

	// Vacuum valve controls

	CLSBiStateControl *vvrSide1_;
	CLSBiStateControl *vvrSide2_;
	CLSBiStateControl *vvrSide3_;
	CLSBiStateControl *vvrSide4_;
	CLSBiStateControl *vvrSide5_;
	CLSBiStateControl *vvrSide6_;

	// Ion pump controls

	AMControl *iopSide1_;
	AMControl *iopSide2_;
	AMControl *iopSide3_;
	AMControl *iopSide4_;
	AMControl *iopSide5_;
	AMControl *iopSide6_;
	AMControl *iopSide7_;

	// Flow transducer controls

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


	// Temperature monitor controls

	// endstation table
	BioXASEndstationTable *endstationTable_;

	/// Cryostat motors.

	AMMotorGroup *motorGroup_;
	CLSMAXvMotor *cryostatX_;
	CLSMAXvMotor *cryostatY_;
	CLSMAXvMotor *cryostatZ_;

	/// Detector motors.

	CLSMAXvMotor *detectorStageLateral_;
};

#endif // BIOXASSIDEBEAMLINE_H
