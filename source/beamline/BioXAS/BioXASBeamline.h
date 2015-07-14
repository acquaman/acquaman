#ifndef BIOXASBEAMLINE_H
#define BIOXASBEAMLINE_H

#include "beamline/AMDetector.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMMotorGroup.h"

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSStandardsWheel.h"
#include "beamline/CLS/CLSJJSlits.h"

#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASXIAFilters.h"
#include "beamline/BioXAS/BioXASM1Mirror.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASM2Mirror.h"
#include "beamline/BioXAS/BioXASDBHRMirror.h"
#include "beamline/BioXAS/BioXASEndstationTable.h"
#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"
#include "beamline/BioXAS/BioXASFourElementVortexDetector.h"

#define BIOXASBEAMLINE_PRESSURE_TOO_HIGH 82210
#define BIOXASBEAMLINE_VALVES_CLOSED 82211
#define BIOXASBEAMLINE_TEMPERATURE_TOO_HIGH 82212
#define BIOXASBEAMLINE_WATER_FLOW_SWITCH_TRIP 82213
#define BIOXASBEAMLINE_WATER_FLOW_TOO_LOW 82214
#define BIOXASBEAMLINE_ION_PUMP_TRIP 82215

class BioXASBeamline : public CLSBeamline
{
    Q_OBJECT

public:
	/// Enum indicating different shutter states.
	class Shutters { public: enum State { Open = 1, Between = 2, Closed = 4 }; };
	/// Enum indicating different beam on/off states.
	class Beam { public: enum State { Off = 0, On = 1 }; };

	/// Singleton accessor.
	static BioXASBeamline *bioXAS()
	{
		if (!instance_) {
			instance_ = new BioXASBeamline("BioXAS Beamline");
			instance_->initializeBeamlineSupport();
		}

		return static_cast<BioXASBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~BioXASBeamline();

	/// Returns the current connected state.
	virtual bool isConnected() const;
	/// Returns the (cached) current connected state.
	virtual bool connected() const { return connected_; }

	/// Returns the current 'beam off' state, true if all front end photon and safety shutters are closed. False otherwise.
	virtual bool beamOff() const;
	/// Returns the current 'beam on' state, true if the front end photon and safety shutters are open. False otherwise.
	virtual bool beamOn() const;

	/// Returns the front end upstream photon shutter.
	AMControl* photonShutterFEUpstream() const { return photonShutterFEUpstream_; }
	/// Returns the front end downstream photon shutter.
	AMControl* photonShutterFEDownstream() const { return photonShutterFEDownstream_; }
	/// Returns the front end safety shutter.
	AMControl* safetyShutterFE() const { return safetyShutterFE_; }
	/// Returns the endstation safety shutter.
	AMControl* safetyShutterES() const { return 0; }

	/// Returns the carbon filter farm.
	virtual BioXASCarbonFilterFarm* carbonFilterFarm() const { return 0; }
	/// Returns the XIA filters.
	virtual BioXASXIAFilters* xiaFilters() const { return 0; }
	/// Returns the m1 mirror.
	virtual BioXASM1Mirror* m1Mirror() const { return 0; }
	/// Returns the monochromator.
	virtual BioXASSSRLMonochromator* mono() const { return 0; }
	/// Returns the m2 mirror.
	virtual BioXASM2Mirror* m2Mirror() const { return 0; }
	/// Returns the DBHR mirrors.
	virtual BioXASDBHRMirror* dbhrMirrors() const { return 0; }
	/// Returns the standards wheel.
	virtual CLSStandardsWheel* standardsWheel() const { return 0; }
	/// Returns the JJ slits.
	virtual CLSJJSlits* jjSlits() const { return 0; }
	/// Returns the endstation table.
	virtual BioXASEndstationTable* endstationTable() const { return 0; }
	/// Returns the scaler.
	virtual CLSSIS3820Scaler* scaler() const { return 0; }
	/// Returns the cryostat stage motor group.
	virtual AMMotorGroup* cryostatStageMotors() const { return 0; }

	/// Returns the I0 scaler channel detector.
	virtual AMDetector* i0Detector() const { return 0; }
	/// Returns the I1 scaler channel detector.
	virtual AMDetector* i1Detector() const { return 0; }
	/// Returns the I2 scaler channel detector.
	virtual AMDetector* i2Detector() const { return 0; }
	/// Returns the 32-element Germanium detector.
	virtual BioXAS32ElementGeDetector* ge32ElementDetector() const { return 0; }
	/// Returns the four-element Vortex detector.
	virtual BioXASFourElementVortexDetector* fourElementVortexDetector() const { return 0; }
	/// Returns the scaler dwell time detector.
	virtual AMBasicControlDetectorEmulator* scalerDwellTimeDetector() const { return 0; }

	/// Returns the upstream pressure monitor.
	AMControl* ccg1() const { return ccg1_; }
	/// Returns the middle pressure monitor.
	AMControl* ccg2() const { return ccg2_; }
	/// Returns the downstream pressure monitor.
	AMControl* ccg3() const { return ccg3_; }
	/// Returns the pressure set.
	AMControlSet* pressureSet() const { return pressureSet_; }

	/// Returns the first front end vacuum valve control.
	AMControl *vvr1() const { return vvr1_; }
	/// Returns the second front end vacuum valve control.
	AMControl *vvr2() const { return vvr2_; }
	/// Returns the third front end vacuum valve control.
	AMControl *vvr3() const { return vvr3_; }
	/// Returns the fourth front end vacuum valve control.
	AMControl *vvr4() const { return vvr4_; }
	/// Returns the fifth front end vacuum valve control.
	AMControl *vvr5() const { return vvr5_; }
	/// Returns the valve set.
	AMControlSet *valveSet() const { return valveSet_; }

	/// Returns the first front end ion pump control.
	AMControl *iop1() const { return iop1_; }
	/// Returns the second front end ion pump control.
	AMControl *iop2() const { return iop2_; }
	/// Returns the third front end ion pump control.
	AMControl *iop3() const { return iop3_; }
	/// Returns the fourth front end ion pump control.
	AMControl *iop4() const { return iop4_; }
	/// Returns the fifth front end ion pump control.
	AMControl *iop5() const { return iop5_; }
	/// Returns the ion pump set.
	AMControlSet *ionPumpSet() const { return ionPumpSet_; }

	/// Returns the first front end flow transducer control.
	AMControl *flt1() const { return flt1_; }
	/// Returns the second front end flow transducer control.
	AMControl *flt2() const { return flt2_; }
	/// Returns the third front end flow transducer control.
	AMControl *flt3() const { return flt3_; }
	/// Returns the fourth front end flow transducer control.
	AMControl *flt4() const { return flt4_; }
	/// Returns the flow transducer set.
	AMControlSet *flowTransducerSet() const { return flowTransducerSet_; }

	/// Returns the first front end flow switch control.
	AMControl *swf1() const { return swf1_; }
	/// Returns the second front end flow switch control.
	AMControl *swf2() const { return swf2_; }
	/// Returns the third front end flow switch control.
	AMControl *swf3() const { return swf3_; }
	/// Returns the fourth front end flow switch control.
	AMControl *swf4() const { return swf4_; }
	/// Returns the flow switch set.
	AMControlSet *flowSwitchSet() const { return flowSwitchSet_; }

	/// Returns the first front end temperature monitor control.
	AMControl *tm1() const { return tm1_; }
	/// Returns the second front end temperature monitor control.
	AMControl *tm2() const { return tm2_; }
	/// Returns the third front end temperature monitor control.
	AMControl *tm3() const { return tm3_; }
	/// Returns the fourth front end temperature monitor control.
	AMControl *tm4() const { return tm4_; }
	/// Returns the fifth front end temperature monitor control.
	AMControl *tm5() const { return tm5_; }
	/// Returns the sixth front end temperature monitor control.
	AMControl* tm6() const { return tm6_; }
	/// Returns the temperature monitor set.
	AMControlSet *temperatureSet() const { return temperatureSet_; }

signals:
	/// Notifier that the current connected state has changed.
	void connectedChanged(bool isConnected);
	/// Notifier that the pressure status has changed.
	void pressureStatusChanged(bool pressureOK);
	/// Notifier that the value status has changed.
	void valveStatusChanged(bool valvesOK);
	/// Notifier that the ion pump status has changed.
	void ionPumpStatusChanged(bool ionPumpsOK);
	/// Notifier that the flow transducer status has changed.
	void flowTransducerStatusChanged(bool transducersOK);
	/// Notifier that the flow switch status has changed.
	void flowSwitchStatusChanged(bool switchesOK);
	/// Notifier that the temperature status has changed.
	void temperatureStatusChanged(bool temperatureOK);

protected slots:
	/// Sets the cached connected state.
	void setConnected(bool isConnected);
	/// Updates the cached connected state.
	void updateConnected();

	/// Handles setting pressure control connections once the whole pressure set reports as connected.
	void onPressureSetConnected();
	/// Handles pressure errors.
	void onPressureError();

	/// Sets up valve control connections once the whole valve set is connected.
	void onValveSetConnected();
	/// Handles valve errors.
	void onValveError();

	/// Sets up ion pump control connections once the whole ion pump set is connected.
	void onIonPumpSetConnected();
	/// Handles ion pump errors.
	void onIonPumpError();

	/// Sets up flow transducer control connections once the whole flow transducer set is connected.
	void onFlowTransducerSetConnected();
	/// Handles flow transducer errors.
	void onFlowTransducerError();

	/// Sets up flow switch control connections once the whole flow switch set is connected.
	void onFlowSwitchSetConnected();
	/// Handles flow switch errors.
	void onFlowSwitchError();

	/// Sets up temperature control connections once the whole temperature set is connected.
	void onTemperatureSetConnected();
	/// Handles temperature errors.
	void onTemperatureError();

protected:
	/// Sets up controls for front end beamline components.
	virtual void setupComponents();
	/// Sets up controls for front end diagnostics.
	virtual void setupDiagnostics();

	/// Protected constructor.
	BioXASBeamline(const QString &controlName);

protected:
	/// The current connected state.
	bool connected_;

	// Shutters.
	/// The front end upstream photon shutter.
	CLSBiStateControl *photonShutterFEUpstream_;
	/// The front end downstream photon shutter.
	CLSBiStateControl *photonShutterFEDownstream_;
	/// The front end safety shutter.
	CLSBiStateControl *safetyShutterFE_;

	// Pressure controls.
	/// The upstream pressure monitor.
	AMControl *ccg1_;
	/// The middle pressure monitor.
	AMControl *ccg2_;
	/// The downstream pressure monitor.
	AMControl *ccg3_;
	/// The pressure set.
	AMControlSet *pressureSet_;

	// Vacuum valve controls.
	/// The first front end vacuum valve control.
	CLSBiStateControl *vvr1_;
	/// The second front end vacuum valve control.
	CLSBiStateControl *vvr2_;
	/// The third front end vacuum valve control.
	CLSBiStateControl *vvr3_;
	/// The fourth front end vacuum valve control.
	CLSBiStateControl *vvr4_;
	/// The fifth front end vacuum valve control.
	CLSBiStateControl *vvr5_;
	/// The vacuum valve set.
	AMControlSet *valveSet_;

	// Ion pump controls.
	/// The first front end ion pump control.
	AMControl *iop1_;
	/// The second front end ion pump control.
	AMControl *iop2_;
	/// The third front end ion pump control.
	AMControl *iop3_;
	/// The fourth front end ion pump control.
	AMControl *iop4_;
	/// The fifth front end ion pump control.
	AMControl *iop5_;
	/// The ion pump set.
	AMControlSet *ionPumpSet_;

	// Flow transducer controls.
	/// The first front end flow transducer control.
	AMControl *flt1_;
	/// The second front end flow transducer control.
	AMControl *flt2_;
	/// The third front end flow transducer control.
	AMControl *flt3_;
	/// The fourth front end flow transducer control.
	AMControl *flt4_;
	/// The flow transducer set.
	AMControlSet *flowTransducerSet_;

	// Flow switch controls.
	/// The first front end flow switch control.
	AMControl *swf1_;
	/// The second front end flow switch control.
	AMControl *swf2_;
	/// The third front end flow switch control.
	AMControl *swf3_;
	/// The fourth front end flow switch control.
	AMControl *swf4_;
	/// The flow switch set.
	AMControlSet *flowSwitchSet_;

	// Temperature controls.
	/// The first temperature monitor control.
	AMControl *tm1_;
	/// The second temperature monitor control.
	AMControl *tm2_;
	/// The third temperature monitor control.
	AMControl *tm3_;
	/// The fourth temperature monitor control.
	AMControl *tm4_;
	/// The fifth temperature monitor control.
	AMControl *tm5_;
	/// The sixth temperature monitor control.
	AMControl *tm6_;
	/// The temperature set.
	AMControlSet *temperatureSet_;
};

#endif // BIOXASBEAMLINE_H
