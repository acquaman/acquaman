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

#include "beamline/BioXAS/BioXASBeamline.h"

#include "beamline/BioXAS/BioXASSideMonochromator.h"
#include "beamline/BioXAS/BioXASSideCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASSideXIAFilters.h"
#include "beamline/BioXAS/BioXASSideM1Mirror.h"
#include "beamline/BioXAS/BioXASSideM2Mirror.h"
#include "beamline/BioXAS/BioXASSideDBHRMirrors.h"
#include "beamline/BioXAS/BioXASSideBeamlineUtilities.h"

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

	/// Returns the endstation safety shutter.
	virtual CLSBiStateControl* safetyShutterES() const { return safetyShutterES_; }
	/// Returns the m1 mirror.
	virtual BioXASM1Mirror* m1Mirror() const { return m1Mirror_; }
	/// Returns the mono.
	virtual BioXASSSRLMonochromator* mono() const { return mono_; }
	/// Returns the m2 mirror.
	virtual BioXASM2Mirror* m2Mirror() const { return m2Mirror_; }
	/// Returns the carbon filter farm.
	virtual BioXASSideCarbonFilterFarm* carbonFilterFarm() const { return carbonFilterFarm_; }
	/// Returns the JJ slits.
	virtual CLSJJSlits* jjSlits() const { return jjSlits_; }
	/// Returns the XIA filters.
	virtual BioXASSideXIAFilters* xiaFilters() const { return xiaFilters_; }
	/// Returns the DBHR mirrors.
	virtual BioXASSideDBHRMirrors* dbhrMirrors() const { return dbhrMirrors_; }
	/// Returns the standards wheel.
	virtual CLSStandardsWheel* standardsWheel() const { return standardsWheel_; }
	/// Returns the endstation table.
	virtual BioXASEndstationTable *endstationTable() const { return endstationTable_; }

	/// Returns the cryostat stage motors group.
	virtual AMMotorGroup* cryostatStageMotors() const { return cryostatStageMotors_; }

	/// Returns the scaler.
	virtual CLSSIS3820Scaler* scaler() const { return scaler_; }
	/// Returns the I0 Keithley428 amplifier.
	CLSKeithley428* i0Keithley() const { return i0Keithley_; }
	/// Returns the IT Keithley428 amplifier.
	CLSKeithley428* i1Keithley() const { return i1Keithley_; }
	/// Returns the I2 Keithley 428 amplifier.
	CLSKeithley428* i2Keithley() const { return i2Keithley_; }

	/// Returns the lateral detector stage motor.
	CLSMAXvMotor* detectorStageLateral() const { return detectorStageLateral_; }
	/// Returns the cryostat x motor.
	CLSMAXvMotor* cryostatX() const { return cryostatX_; }
	/// Returns the cryostat y motor.
	CLSMAXvMotor* cryostatY() const { return cryostatY_; }
	/// Returns the cryostat z motor.
	CLSMAXvMotor* cryostatZ() const { return cryostatZ_; }

	/// Return the set of BioXAS Motors by given motor category
	QList<AMControl *> getMotorsByType(BioXASBeamlineDef::BioXASMotorType category) const;

	/// Returns the beamline utilities.
	virtual BioXASBeamlineUtilities *utilities() const { return utilities_; }

	/// Returns the I0 scaler channel detector.
	virtual AMDetector* i0Detector() const { return i0Detector_; }
	/// Returns the I1 scaler channel detector.
	virtual AMDetector* i1Detector() const { return i1Detector_; }
	/// Returns the I2 scaler channel detector.
	virtual AMDetector* i2Detector() const { return i2Detector_; }
	/// Returns the 32 element Ge detector.
	virtual BioXAS32ElementGeDetector *ge32ElementDetector() const { return ge32ElementDetector_; }
	/// Returns the four element Vortex detector.
	virtual BioXASFourElementVortexDetector *fourElementVortexDetector() const { return 0; }

	/// Returns the scaler dwell time detector.
	virtual AMBasicControlDetectorEmulator* scalerDwellTimeDetector() const { return dwellTimeDetector_; }
	/// Returns the bragg encoder-based energy feedback detector.
	AMBasicControlDetectorEmulator* encoderEnergyFeedbackDetector() const { return encoderEnergyFeedbackDetector_; }
	/// Returns the bragg step-based energy feedback detector.
	AMBasicControlDetectorEmulator* stepEnergyFeedbackDetector() const { return stepEnergyFeedbackDetector_; }
	/// Returns the bragg motor detector.
	AMBasicControlDetectorEmulator* braggDetector() const { return braggDetector_; }
	/// Returns the bragg step setpoint detector.
	AMBasicControlDetectorEmulator* braggStepSetpointDetector() const { return braggStepSetpointDetector_; }

protected:
	/// Sets up all the detectors.
	void setupDetectors();
	/// Sets up the cryostat stage motors.
	void setupCryostatStage();
	/// Sets up the detector stage motors.
	void setupDetectorStage();
	/// Sets up various beamline components.
	virtual void setupComponents();
	/// Sets up the exposed actions.
	void setupExposedControls();
	/// Sets up the exposed detectors.
	void setupExposedDetectors();
	/// Sets up all of the detectors that need to be added to scans that aren't a part of typical detectors.  This may just be temporary, not sure.
	void setupControlsAsDetectors();

	/// Constructor. This is a singleton class, access it through BioXASSideBeamline::bioXAS().
	BioXASSideBeamline();

protected:
	/// The endstation safety shutter.
	CLSBiStateControl *safetyShutterES_;

	/// The M1 mirror.
	BioXASSideM1Mirror *m1Mirror_;
	// The mono.
	BioXASSSRLMonochromator *mono_;
	/// The M2 mirror.
	BioXASSideM2Mirror *m2Mirror_;
	/// The carbon filter farm
	BioXASSideCarbonFilterFarm *carbonFilterFarm_;
	/// The JJ slits
	CLSJJSlits *jjSlits_;
	/// The XIA filters
	BioXASSideXIAFilters *xiaFilters_;
	// DBHR mirrors
	BioXASSideDBHRMirrors *dbhrMirrors_;
	/// Standards wheel
	CLSStandardsWheel *standardsWheel_;
	/// Endstation table
	BioXASEndstationTable *endstationTable_;

	// Cryostat stage motors
	/// Cryostat stage motor group.
	AMMotorGroup *cryostatStageMotors_;
	/// Cryostat stage x motor.
	CLSMAXvMotor *cryostatX_;
	/// Cryostat stage y motor.
	CLSMAXvMotor *cryostatY_;
	/// Cryostat stage z motor.
	CLSMAXvMotor *cryostatZ_;

	/// Detector stage lateral motor.
	CLSMAXvMotor *detectorStageLateral_;

	/// Utilities
	BioXASSideBeamlineUtilities *utilities_;

	// Scaler controls
	/// Scaler
	CLSSIS3820Scaler *scaler_;
	/// Scaler dwell time control
	AMControl *scalerDwellTime_;
	/// I0 Keithley amplifier
	CLSKeithley428 *i0Keithley_;
	/// I1 Keithley amplifier
	CLSKeithley428 *i1Keithley_;
	/// I2 Keithley amplifier
	CLSKeithley428 *i2Keithley_;

	// Detectors
	/// I0 detector
	CLSBasicScalerChannelDetector *i0Detector_;
	/// I1 detector
	CLSBasicScalerChannelDetector *i1Detector_;
	/// I2 detector
	CLSBasicScalerChannelDetector *i2Detector_;
	/// Ge 32-el detector
	BioXAS32ElementGeDetector *ge32ElementDetector_;
	/// The bragg encoder-based energy feedback detector
	AMBasicControlDetectorEmulator *encoderEnergyFeedbackDetector_;
	/// The bragg step-based energy setpoint detector
	AMBasicControlDetectorEmulator *stepEnergySetpointDetector_;
	/// The bragg step-based energy feedback detector
	AMBasicControlDetectorEmulator *stepEnergyFeedbackDetector_;
	/// The scaler dwell time detector
	AMBasicControlDetectorEmulator *dwellTimeDetector_;
	/// The bragg angle detector
	AMBasicControlDetectorEmulator *braggDetector_;
	/// The step-based bragg setpoint detector
	AMBasicControlDetectorEmulator *braggStepSetpointDetector_;
};

#endif // BIOXASSIDEBEAMLINE_H
