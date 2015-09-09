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

#include "beamline/AMControlSet.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/AMPVControl.h"

#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSKeithley428.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSJJSlits.h"
#include "beamline/CLS/CLSJJSlitGapControl.h"
#include "beamline/CLS/CLSJJSlitCenterControl.h"
#include "beamline/CLS/CLSStandardsWheel.h"

#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "beamline/BioXAS/BioXASSideMonochromator.h"
#include "beamline/BioXAS/BioXASPseudoMotorControl.h"
#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"
#include "beamline/BioXAS/BioXASSideCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASSideXIAFilters.h"
#include "beamline/BioXAS/BioXASSideM1Mirror.h"
#include "beamline/BioXAS/BioXASSideM2Mirror.h"
#include "beamline/BioXAS/BioXASSideDBHRMirrors.h"
#include "beamline/BioXAS/BioXASEndstationTable.h"
#include "beamline/BioXAS/BioXASFourElementVortexDetector.h"

#define BIOXASSIDEBEAMLINE_PRESSURE_TOO_HIGH 54600
#define BIOXASSIDEBEAMLINE_VALVES_CLOSED 54601
#define BIOXASSIDEBEAMLINE_TEMPERATURE_TOO_HIGH 54602
#define BIOXASSIDEBEAMLINE_WATER_FLOW_SWITCH_TRIP 54603
#define BIOXASSIDEBEAMLINE_WATER_FLOW_TOO_LOW 54604
#define BIOXASSIDEBEAMLINE_ION_PUMP_TRIP 54605

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

	/// Returns the beamline JJ Slit.
	CLSJJSlits *jjSlits() const { return jjSlits_; }
	/// Returns the mono.
	virtual BioXASSSRLMonochromator* mono() const { return mono_; }
	/// Returns the scaler.
	virtual CLSSIS3820Scaler* scaler() const { return scaler_; }
	/// Returns the carbon filter farm.
	BioXASSideCarbonFilterFarm* carbonFilterFarm() const { return carbonFilterFarm_; }
	/// Returns the XIA filters.
	BioXASSideXIAFilters* xiaFilters() const { return xiaFilters_; }
	/// Returns the DBHR mirrors.
	BioXASSideDBHRMirrors* dbhrMirrors() const { return dbhrMirrors_; }
	/// Returns the standards wheel.
	CLSStandardsWheel *standardsWheel() const { return standardsWheel_; }

	// Pressure monitors.
	AMControl *ccg1() const { return ccg1_; }
	AMControl *ccg2() const { return ccg2_; }
	AMControl *ccg3() const { return ccg3_; }
	AMControl *ccgSide1() const { return ccgSide1_; }
	AMControl *ccgSide2() const { return ccgSide2_; }
	AMControl *ccgSide3() const { return ccgSide3_; }
	AMControl *ccgSide4() const { return ccgSide4_; }
	AMControl *ccgSide5() const { return ccgSide5_; }
	AMControl *ccgSide6() const { return ccgSide6_; }

	AMControlSet *pressureSet() const { return pressureSet_; }

	// Vaccum values.

	AMControl *vvr1() const { return vvr1_; }
	AMControl *vvr2() const { return vvr2_; }
	AMControl *vvr3() const { return vvr3_; }
	AMControl *vvr4() const { return vvr4_; }
	AMControl *vvr5() const { return vvr5_; }
	AMControl *vvrSide1() const { return vvrSide1_; }
	AMControl *vvrSide2() const { return vvrSide2_; }
	AMControl *vvrSide3() const { return vvrSide3_; }
	AMControl *vvrSide4() const { return vvrSide4_; }
	AMControl *vvrSide5() const { return vvrSide5_; }
	AMControl *vvrSide6() const { return vvrSide6_; }

	AMControlSet *valveSet() const { return valveSet_; }

	bool allValvesOpen() const;
	bool allValvesClosed() const;

	// Ion pumps.

	AMControl *iop1() const { return iop1_; }
	AMControl *iop2() const { return iop2_; }
	AMControl *iop3() const { return iop3_; }
	AMControl *iop4() const { return iop4_; }
	AMControl *iop5() const { return iop5_; }
	AMControl *iopSide1() const { return iopSide1_; }
	AMControl *iopSide2() const { return iopSide2_; }
	AMControl *iopSide3() const { return iopSide3_; }
	AMControl *iopSide4() const { return iopSide4_; }
	AMControl *iopSide5() const { return iopSide5_; }
	AMControl *iopSide6() const { return iopSide6_; }
	AMControl *iopSide7() const { return iopSide7_; }

	AMControlSet *ionPumpSet() const { return ionPumpSet_; }

	// Flow transducers.

	AMControl *flt1() const { return flt1_; }
	AMControl *flt2() const { return flt2_; }
	AMControl *flt3() const { return flt3_; }
	AMControl *flt4() const { return flt4_; }
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

	AMControlSet *flowTransducerSet() const { return flowTransducerSet_; }

	// Flow switches.

	AMControl *swf1() const { return swf1_; }
	AMControl *swf2() const { return swf2_; }
	AMControl *swf3() const { return swf3_; }
	AMControl *swf4() const { return swf4_; }

	AMControlSet *flowSwitchSet() const { return flowSwitchSet_; }

	// Temperature monitors.

	AMControl *tm1() const { return tm1_; }
	AMControl *tm2() const { return tm2_; }
	AMControl *tm3() const { return tm3_; }
	AMControl *tm4() const { return tm4_; }
	AMControl *tm5() const { return tm5_; }

	AMControlSet *temperatureSet() const { return temperatureSet_; }

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

	// Motor groups
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
	CLSBasicScalerChannelDetector* i0Detector() const { return i0Detector_; }
	/// Returns the I1 scaler channel detector.
	CLSBasicScalerChannelDetector* i1Detector() const { return i1Detector_; }
	/// Returns the I2 scaler channel detector.
	CLSBasicScalerChannelDetector* i2Detector() const { return i2Detector_; }
	/// Returns the bragg encoder-based energy setpoint detector.
	AMBasicControlDetectorEmulator* encoderEnergySetpointDetector() const { return encoderEnergySetpointDetector_; }
	/// Returns the bragg encoder-based energy feedback detector.
	AMBasicControlDetectorEmulator* encoderEnergyFeedbackDetector() const { return encoderEnergyFeedbackDetector_; }
	/// Returns the bragg step-based energy feedback detector.
	AMBasicControlDetectorEmulator* stepEnergyFeedbackDetector() const { return stepEnergyFeedbackDetector_; }
	/// Returns the scaler dwell time detector.
	AMBasicControlDetectorEmulator* dwellTimeDetector() const { return dwellTimeDetector_; }
	/// Returns the bragg motor detector.
	AMBasicControlDetectorEmulator* braggDetector() const { return braggDetector_; }
	/// Returns the bragg motor encoder feedback detector.
	AMBasicControlDetectorEmulator* braggEncoderFeedbackDetector() const { return braggEncoderFeedbackDetector_; }
	/// Returns the bragg move retries detector.
	AMBasicControlDetectorEmulator* braggMoveRetriesDetector() const { return braggMoveRetriesDetector_; }
	/// Returns the bragg step setpoint detector.
	AMBasicControlDetectorEmulator* braggStepSetpointDetector() const { return braggStepSetpointDetector_; }
	/// Returns the bragg motor feedback, step-based.
	AMBasicControlDetectorEmulator* braggStepMotorFeedbackDetector() const { return braggStepMotorFeedbackDetector_; }
	/// Returns the 32 element Ge detector.
	BioXAS32ElementGeDetector *ge32ElementDetector() const { return ge32ElementDetector_; }
	/// Returns the four element Vortex detector.
//	BioXASFourElementVortexDetector *fourElementVortexDetector() const { return fourElementVortexDetector_; }

	/// Endstation table
	BioXASEndstationTable *endstationTable() const { return endstationTable_; }

signals:
	/// Notifier that the pressure status has changed. Argument is false if any of the pressures fall below its setpoint, true otherwise.
	void pressureStatusChanged(bool);
	/// Notifier that the valve status has changed. Argument is false if any of the valves are closed, true otherwise.
	void valveStatusChanged(bool);
	/// Notifier that the ion pump status has changed. Argument is false if any of the ion pumps fail, true otherwise.
	void ionPumpStatusChanged(bool);
	/// Notifier that the flow transducer status has changed. Argument is false if any of the flow rates fall below its setpoint.
	void flowTransducerStatusChanged(bool);
	/// Notifier that the flow switch status has changed. Argument is false if any of the flow switches are disabled.
	void flowSwitchStatusChanged(bool);
	/// Notifier that the temperature status has changed. Argument is false if any of the temperatures rise above their setpoint.
	void temperatureStatusChanged(bool);

protected slots:
	/// Sets up pressure control connections once the whole pressure set is connected.
	void onPressureSetConnected(bool connected);
	/// Handles pressure errors.
	void onPressureError();
	/// Sets up valve control connections once the whole valve set is connected.
	void onValveSetConnected(bool connected);
	/// Handles valve errors.
	void onValveError();
	/// Sets up ion pump control connections once the whole ion pump set is connected.
	void onIonPumpSetConnected(bool connected);
	/// Handles ion pump errors.
	void onIonPumpError();
	/// Sets up flow transducer control connections once the whole flow transducer set is connected.
	void onFlowTransducerSetConnected(bool connected);
	/// Handles flow transducer errors.
	void onFlowTransducerError();
	/// Sets up flow switch control connections once the whole flow switch set is connected.
	void onFlowSwitchSetConnected(bool connected);
	/// Handles flow switch errors.
	void onFlowSwitchError();
	/// Sets up temperature control connections once the whole temperature set is connected.
	void onTemperatureSetConnected(bool connected);
	/// Handles temperature errors.
	void onTemperatureError();

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
	AMBasicControlDetectorEmulator *braggStepMotorFeedbackDetector_;
	AMBasicControlDetectorEmulator *braggEncoderFeedbackDetector_;
	BioXAS32ElementGeDetector *ge32ElementDetector_;
//	BioXASFourElementVortexDetector *fourElementVortexDetector_;

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

	BioXASSideDBHRMirrors *dbhrMirrors_;

	// Misc controls

	AMControl *energySetpointControl_;

	// Extras

	CLSStandardsWheel *standardsWheel_;

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

	AMControlSet *pressureSet_;

	// Vacuum valve controls

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

	AMControlSet *valveSet_;

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

	AMControlSet *ionPumpSet_;

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

	AMControlSet *flowTransducerSet_;

	// Flow switch controls

	AMControl *swf1_;
	AMControl *swf2_;
	AMControl *swf3_;
	AMControl *swf4_;

	AMControlSet *flowSwitchSet_;

	// Temperature monitor controls

	AMControl *tm1_;
	AMControl *tm2_;
	AMControl *tm3_;
	AMControl *tm4_;
	AMControl *tm5_;

	AMControlSet *temperatureSet_;

	/// endstation table
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
