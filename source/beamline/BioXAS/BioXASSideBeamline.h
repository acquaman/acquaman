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
#include "beamline/CLS/CLSJJSlit.h"
#include "beamline/CLS/CLSJJSlitGapControl.h"
#include "beamline/CLS/CLSJJSlitCenterControl.h"

#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "beamline/BioXAS/BioXASSideMonochromator.h"
#include "beamline/BioXAS/BioXASPseudoMotorControl.h"
#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"
#include "beamline/BioXAS/BioXASSideCarbonFilterFarmControl.h"
#include "beamline/BioXAS/BioXASSideXIAFilters.h"
#include "beamline/BioXAS/BioXASSideM2Mirror.h"
#include "beamline/BioXAS/BioXASSideDBHRMirror.h"

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

	/// Returns the most recent connection state of the beamline.
	virtual bool isConnected() const { return isConnected_; }

	/// Returns the m2 mirrors.
	BioXASSideM2Mirror* m2Mirror() const { return m2Mirror_; }
	/// Returns the beamline monochromator.
	BioXASSideMonochromator *mono() const { return mono_; }
	/// Returns the beamline JJ Slit.
	CLSJJSlit *jjSlit() const { return jjSlit_; }
	/// Returns the JJ slits gap control.
	CLSJJSlitGapControl* jjSlitsGapControl() const { return jjSlitsGap_; }
	/// Returns the JJ slits center control.
	CLSJJSlitCenterControl* jjSlitsCenterControl() const { return jjSlitsCenter_; }
	/// Returns the scaler.
	virtual CLSSIS3820Scaler* scaler() const { return scaler_; }
	/// Returns the carbon filter farm.
	BioXASSideCarbonFilterFarmControl* carbonFilterFarm() const { return carbonFilterFarm_; }
	/// Returns the XIA filters.
	BioXASSideXIAFilters* xiaFilters() const { return xiaFilters_; }
	/// Returns the DBHR mirrors.
	BioXASSideDBHRMirror* dbhrMirror() const { return dbhrMirror_; }

	// Photon and safety shutters.
	/// Returns the first photon shutter.
	AMControl *photonShutter1() const { return psh1_; }
	/// Returns the second photon shutter.
	AMControl *photonShutter2() const { return psh2_; }
	/// Returns the first safety shutter.
	AMControl *safetyShutter1() const { return ssh1_; }
	/// Returns the second safety shutter.
	AMControl *safetyShutter2() const { return sshSide1_; }

	bool openPhotonShutter1();
	bool closePhotonShutter1();
	bool openPhotonShutter2();
	bool closePhotonShutter2();
	bool openSafetyShutter1();
	bool closeSafetyShutter1();
	bool openSafetyShutter2();
	bool closeSafetyShutter2();

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

	// Current amplifiers
	/// Returns the I0 Keithley428 amplifier.
	CLSKeithley428* i0Keithley() const { return i0Keithley_; }
	/// Returns the IT Keithley428 amplifier.
	CLSKeithley428* iTKeithley() const { return iTKeithley_; }
	/// Returns the I2 Keithley 428 amplifier.
	CLSKeithley428* i2Keithley() const { return i2Keithley_; }

	// Detectors
	/// Returns the I0 scaler channel detector.
	CLSBasicScalerChannelDetector* i0Detector() const { return i0Detector_; }
	/// Returns the IT scaler channel detector.
	CLSBasicScalerChannelDetector* iTDetector() const { return iTDetector_; }
	/// Returns the I2 scaler channel detector.
	CLSBasicScalerChannelDetector* i2Detector() const { return i2Detector_; }
	/// Returns the energy setpoint detector.
	AMBasicControlDetectorEmulator* energySetpointDetector() const { return energySetpointDetector_; }
	/// Returns the energy feedback detector.
	AMBasicControlDetectorEmulator* energyFeedbackDetector() const { return energyFeedbackDetector_; }
	/// Returns the scaler dwell time detector.
	AMBasicControlDetectorEmulator* dwellTimeDetector() const { return dwellTimeDetector_; }
	/// Returns the bragg motor detector.
	AMBasicControlDetectorEmulator* braggDetector() const { return braggDetector_; }
	/// Returns the bragg motor encoder feedback detector.
	AMBasicControlDetectorEmulator* braggEncoderFeedbackDetector() const { return braggEncoderFeedbackDetector_; }
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
	/// Returns the 32 element Ge detector.
	BioXAS32ElementGeDetector *ge32ElementDetector() const { return ge32ElementDetector_; }

signals:
	/// Notifier that the beamline's global connection state has changed.
	void connected(bool);
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
	/// Updates the beamline's reported connection state.
	void onConnectionChanged();
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
	/// Sets up the sample stage motors.
	void setupSampleStage();
	/// Sets up the detector stage motors.
	void setupDetectorStage();
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
	/// The beamline connection state.
	bool isConnected_;

	// Detectors

	CLSBasicScalerChannelDetector *i0Detector_;
	CLSBasicScalerChannelDetector *iTDetector_;
	CLSBasicScalerChannelDetector *i2Detector_;
	AMBasicControlDetectorEmulator *energySetpointDetector_;
	AMBasicControlDetectorEmulator *energyFeedbackDetector_;
	AMBasicControlDetectorEmulator *dwellTimeDetector_;
	AMBasicControlDetectorEmulator *braggDetector_;
	AMBasicControlDetectorEmulator *braggMoveRetriesDetector_;
	AMBasicControlDetectorEmulator *braggMoveRetriesMaxDetector_;
	AMBasicControlDetectorEmulator *braggStepSetpointDetector_;
	AMBasicControlDetectorEmulator *braggDegreeSetpointDetector_;
	AMBasicControlDetectorEmulator *braggAngleDetector_;
	AMBasicControlDetectorEmulator *braggEncoderFeedbackDetector_;
	BioXAS32ElementGeDetector *ge32ElementDetector_;

	// M2 mirror

	BioXASSideM2Mirror *m2Mirror_;

	// Monochromator

	BioXASSideMonochromator *mono_;

	/// The JJ slit
	CLSJJSlit *jjSlit_;

	CLSJJSlitGapControl *jjSlitsGap_;
	CLSJJSlitCenterControl *jjSlitsCenter_;

	// Scaler

	CLSSIS3820Scaler *scaler_;
	AMControl *scalerDwellTime_;

	// Amplifiers

	CLSKeithley428 *i0Keithley_;
	CLSKeithley428 *iTKeithley_;
	CLSKeithley428 *i2Keithley_;

	// Filters.

	BioXASSideCarbonFilterFarmControl *carbonFilterFarm_;
	BioXASSideXIAFilters *xiaFilters_;

	// DBHR mirror.

	BioXASSideDBHRMirror *dbhrMirror_;

	// Misc controls

	AMControl *energySetpointControl_;

	// Shutter controls

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

	// Filter motors

	CLSMAXvMotor *carbonFilterFarm1_;
	CLSMAXvMotor *carbonFilterFarm2_;

	// M1 motors

	CLSMAXvMotor *m1VertUpStreamINB_;
	CLSMAXvMotor *m1VertUpStreamOUTB_;
	CLSMAXvMotor *m1VertDownStream_;
	CLSMAXvMotor *m1StripeSelect_;
	CLSMAXvMotor *m1Yaw_;
	CLSMAXvMotor *m1BenderUpstream_;
	CLSMAXvMotor *m1BenderDownStream_;
	CLSMAXvMotor *m1UpperSlitBlade_;

	// Variable Mask motors

	CLSMAXvMotor *variableMaskVertUpperBlade_;
	CLSMAXvMotor *variableMaskVertLowerBlade_;

	// M2 motors

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
	AMPVwStatusControl *monoBraggAngle_;

	/// Detector motors.

	CLSMAXvMotor *detectorStageLateral_;
};

#endif // BIOXASSIDEBEAMLINE_H
