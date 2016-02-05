#ifndef BIOXASBEAMLINEUTILITIES_H
#define BIOXASBEAMLINEUTILITIES_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

#include "beamline/AMControlSet.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSBiStateControl.h"

#define BIOXASBEAMLINE_PRESSURE_TOO_HIGH 82210
#define BIOXASBEAMLINE_VALVES_CLOSED 82211
#define BIOXASBEAMLINE_TEMPERATURE_TOO_HIGH 82212
#define BIOXASBEAMLINE_WATER_FLOW_SWITCH_TRIP 82213
#define BIOXASBEAMLINE_WATER_FLOW_TOO_LOW 82214
#define BIOXASBEAMLINE_ION_PUMP_TRIP 82215

class BioXASBeamlineUtilities : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamlineUtilities(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineUtilities();

	/// Returns true if the utilities are connected, false otherwise.
	bool isConnected() const;

	/// Returns the upstream pressure monitor.
	AMControl* ccg1() const { return ccg1_; }
	/// Returns the middle pressure monitor.
	AMControl* ccg2() const { return ccg2_; }
	/// Returns the downstream pressure monitor.
	AMControl* ccg3() const { return ccg3_; }
	/// Returns the pressure set.
	AMControlSet* pressureSet() const { return pressureSet_; }

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
	/// Notifier that the pressure status has changed.
	void pressureStatusChanged(bool pressureOK);
	/// Notifier that the flow transducer status has changed.
	void flowTransducerStatusChanged(bool transducersOK);
	/// Notifier that the flow switch status has changed.
	void flowSwitchStatusChanged(bool switchesOK);
	/// Notifier that the temperature status has changed.
	void temperatureStatusChanged(bool temperatureOK);

protected slots:
	/// Handles setting pressure control connections once the whole pressure set reports as connected.
	void onPressureSetConnected();
	/// Handles pressure errors.
	void onPressureError();

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
	// Pressure controls.
	/// The upstream pressure monitor.
	AMReadOnlyPVwStatusControl *ccg1_;
	/// The middle pressure monitor.
	AMReadOnlyPVwStatusControl *ccg2_;
	/// The downstream pressure monitor.
	AMReadOnlyPVwStatusControl *ccg3_;
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

	// Flow transducer controls.
	/// The first front end flow transducer control.
	AMReadOnlyPVwStatusControl *flt1_;
	/// The second front end flow transducer control.
	AMReadOnlyPVwStatusControl *flt2_;
	/// The third front end flow transducer control.
	AMReadOnlyPVwStatusControl *flt3_;
	/// The fourth front end flow transducer control.
	AMReadOnlyPVwStatusControl *flt4_;
	/// The flow transducer set.
	AMControlSet *flowTransducerSet_;

	// Flow switch controls.
	/// The first front end flow switch control.
	AMReadOnlyPVControl *swf1_;
	/// The second front end flow switch control.
	AMReadOnlyPVControl *swf2_;
	/// The third front end flow switch control.
	AMReadOnlyPVControl *swf3_;
	/// The fourth front end flow switch control.
	AMReadOnlyPVControl *swf4_;
	/// The flow switch set.
	AMControlSet *flowSwitchSet_;

	// Temperature controls.
	/// The first temperature monitor control.
	AMReadOnlyPVwStatusControl *tm1_;
	/// The second temperature monitor control.
	AMReadOnlyPVwStatusControl *tm2_;
	/// The third temperature monitor control.
	AMReadOnlyPVwStatusControl *tm3_;
	/// The fourth temperature monitor control.
	AMReadOnlyPVwStatusControl *tm4_;
	/// The fifth temperature monitor control.
	AMReadOnlyPVwStatusControl *tm5_;
	/// The sixth temperature monitor control.
	AMReadOnlyPVwStatusControl *tm6_;
	/// The temperature set.
	AMControlSet *temperatureSet_;
};

#endif // BIOXASBEAMLINEUTILITIES_H
