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

#include "beamline/BioXAS/BioXASPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "beamline/BioXAS/BioXASMainMonochromator.h"
#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"
#include "beamline/BioXAS/BioXASMainM2Mirror.h"
#include "beamline/BioXAS/BioXASMainM1Mirror.h"
#include "beamline/BioXAS/BioXASMainXIAFilters.h"
#include "beamline/BioXAS/BioXASMainDBHRMirrors.h"
#include "beamline/BioXAS/BioXASMainCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASMainStandardsWheel.h"

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

	/// Returns the current connected state.
	virtual bool isConnected() const;

	/// Returns the endstation safety shutter.
	virtual CLSBiStateControl* safetyShutterES() const { return safetyShutterES_; }

	/// Returns the M1 mirror.
	virtual BioXASM1Mirror* m1Mirror() const { return m1Mirror_; }
	/// Returns the beamline monochromator.
	virtual BioXASMainMonochromator *mono() const { return mono_; }
	/// Returns the beamline M2 mirror.
	virtual BioXASM2Mirror *m2Mirror() const { return m2Mirror_; }
	/// Returns the carbon filter farm.
	virtual BioXASMainCarbonFilterFarm* carbonFilterFarm() const { return carbonFilterFarm_; }
	/// Returns the JJ slits.
	virtual CLSJJSlits* jjSlits() const { return jjSlits_; }
	/// Returns the XIA filters.
	virtual BioXASMainXIAFilters* xiaFilters() const { return xiaFilters_; }
	/// Returns the DBHR mirrors.
	virtual BioXASMainDBHRMirrors* dbhrMirrors() const { return dbhrMirrors_; }
	/// Returns the standards wheel.
	virtual BioXASMainStandardsWheel* standardsWheel() const { return standardsWheel_; }
	/// Returns the endstation table.
	virtual BioXASEndstationTable *endstationTable() const { return endstationTable_; }
	/// Returns the scaler.
	virtual CLSSIS3820Scaler* scaler() const { return scaler_; }
	/// Returns the I0 amplifier.
	CLSKeithley428* i0Keithley() const { return i0Keithley_; }
	/// Returns the IT amplifier.
	CLSKeithley428* i1Keithley() const { return i1Keithley_; }
	/// Returns the I2 amplifier.
	CLSKeithley428* i2Keithley() const { return i2Keithley_; }

	/// Returns the beamline utilities.
	virtual BioXASBeamlineUtilities* utilities() const { return utilities_; }

	/// Returns the I0 scaler channel detector.
	virtual CLSBasicScalerChannelDetector* i0Detector() const { return i0Detector_; }
	/// Returns the I1 scaler channel detector.
	virtual CLSBasicScalerChannelDetector* i1Detector() const { return i1Detector_; }
	/// Returns the I2 scaler channel detector.
	virtual CLSBasicScalerChannelDetector* i2Detector() const { return i2Detector_; }
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

	/// Return the set of BioXAS Motors by given motor category.
	QList<AMControl *> getMotorsByType(BioXASBeamlineDef::BioXASMotorType category);

protected:
	/// Sets up all the detectors.
	void setupDetectors();
	/// Sets up various beamline components.
	virtual void setupComponents();
	/// Sets up the exposed controls.
	void setupExposedControls();
	/// Sets up the exposed detectors.
	void setupExposedDetectors();
	/// Sets up all of the detectors that need to be added to scans that aren't a part of typical detectors.  This may just be temporary, not sure.
	void setupControlsAsDetectors();

	/// Constructor. This is a singleton class, access it through BioXASMainBeamline::bioXAS().
	BioXASMainBeamline();

protected:
	/// The endstation safety shutter.
	CLSBiStateControl *safetyShutterES_;

	/// The M1 mirror.
	BioXASMainM1Mirror *m1Mirror_;
	// Monochromator
	BioXASMainMonochromator *mono_;
	/// The M2 mirror.
	BioXASMainM2Mirror *m2Mirror_;
	/// The carbon filter farm.
	BioXASMainCarbonFilterFarm *carbonFilterFarm_;
	/// JJ slits
	CLSJJSlits *jjSlits_;
	/// XIA filters
	BioXASMainXIAFilters *xiaFilters_;
	/// DBHR mirrors
	BioXASMainDBHRMirrors *dbhrMirrors_;
	/// Standards wheel
	BioXASMainStandardsWheel *standardsWheel_;
	/// Endstation table
	BioXASEndstationTable *endstationTable_;

	/// Utilities
	BioXASBeamlineUtilities *utilities_;

	// Scaler controls
	/// Scaler
	CLSSIS3820Scaler *scaler_;
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
	/// The scaler dwell time detector
	AMBasicControlDetectorEmulator *dwellTimeDetector_;
	/// The bragg encoder-based energy feedback detector
	AMBasicControlDetectorEmulator *encoderEnergyFeedbackDetector_;
	/// The bragg step-based energy feedback detector
	AMBasicControlDetectorEmulator *stepEnergyFeedbackDetector_;
	/// The bragg angle detector
	AMBasicControlDetectorEmulator *braggDetector_;
	/// The step-based bragg setpoint detector
	AMBasicControlDetectorEmulator *braggStepSetpointDetector_;
};

#endif // BIOXASMAINBEAMLINE_H
