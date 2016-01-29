#ifndef BIOXASZEBRAPULSECONTROL_H
#define BIOXASZEBRAPULSECONTROL_H

#include <QObject>

#include "actions3/AMAction3.h"
#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"
#include "beamline/BioXAS/BioXASZebraTimeSeconds.h"

/// Pulse control encapsulation for the Zebra.  Takes a base name and pulse group index.
class BioXASZebraPulseControl : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.  Takes a base name and group index (1, 2, 3, 4).
	explicit BioXASZebraPulseControl(const QString &name, const QString &baseName, int pulseIndex, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraPulseControl();

	/// Returns the name of the pulse control.
	QString name() const;
	/// Returns the connected status of the pulse control.
	bool isConnected() const;

	/// Returns the current input value.
	int inputValue() const;
	/// Returns the current input value as a string.
	QString inputValueString() const;
	/// Returns the input status value.
	bool inputStatusValue() const;
	/// Returns the edge trigger value.
	int edgeTriggerValue() const;
	/// Returns the delay before value.
	double delayBeforeValue() const;
	/// Returns the pulse width value.
	double pulseWidthValue() const;
	/// Returns the time units value.
	int timeUnitsValue() const;
	/// Returns whether the trigger while active status is on.
	bool triggerWhileActiveValue() const;
	/// Returns the output status.
	bool outputValue() const;
	/// Returns the delay before value, in seconds.
	double delayBeforeValueSeconds() const;
	/// Returns the pulse width value, in seconds.
	double pulseWidthValueSeconds() const;

	/// Returns the edge trigger value preference.
	int edgeTriggerPreference() const;

	/// Returns the input control.
	AMPVControl *inputControl() const { return inputControl_; }
	/// Returns the input status control.
	AMReadOnlyPVControl *inputStatusControl() const { return inputStatusControl_; }
	/// Returns the edge trigger control.
	AMPVControl *edgeTriggerControl() const { return edgeTriggerControl_; }
	/// Returns the delay before trigger control.
	AMPVControl *delayBeforeTriggerControl() const { return delayBeforeControl_; }
	/// Returns the pulse width control.
	AMPVControl *pulseWidthControl() const { return pulseWidthControl_; }
	/// Returns the time units control.
	AMPVControl *timeUnitsControl() const { return timeUnitsControl_; }
	/// Returns the trigger while active control.
	AMReadOnlyPVControl *triggerWhileActiveControl() const { return triggerWhileActiveControl_; }
	/// Returns the output pulse control.
	AMReadOnlyPVControl *outputPulseControl() const { return outputPulseControl_; }
	/// Returns the delay before control (seconds).
	BioXASZebraTimeSeconds* delayBeforeSecondsControl() const { return delayBeforeSecondsControl_; }
	/// Returns the pulse width control (seconds).
	BioXASZebraTimeSeconds* pulseWidthSecondsControl() const { return pulseWidthSecondsControl_; }

	/// Creates and returns a new action that sets the input value.
	AMAction3* createSetInputValueAction(double newValue);

signals:
	/// Notifier that the input value changed.
	void inputValueChanged(int);
	/// Notifier that the input value changed as a string.
	void inputValueStringChanged(const QString &);
	/// Notifier that the input status changed.
	void inputValueStatusChanged(bool);
	/// Notifier that the edge trigger value changed.
	void edgeTriggerValueChanged(int);
	/// Notifier that the delay before value changed.
	void delayBeforeValueChanged(double);
	/// Notifier that the pulse width value changed.
	void pulseWidthValueChanged(double);
	/// Notifier that the time units value changed.
	void timeUnitsValueChanged(int);
	/// Notifier that the trigger while active value changed.
	void triggerWhileActiveValueChanged(bool);
	/// Notifier that the output value changed.
	void outputValueChanged(bool);
	/// Notifier that the delay before value changed, in seconds.
	void delayBeforeValueSecondsChanged(double);
	/// Notifier that the pulse width value changed, in seconds.
	void pulseWidthValueSecondsChanged(double);

	/// Notifier that the edge trigger value preference has changed.
	void edgeTriggerPreferenceChanged(int);

public slots:
	/// Sets the input value.
	void setInputValue(int value);
	/// Sets the edge trigger value.
	void setEdgeTriggerValue(int value);
	/// Sets the delay before value in seconds (does appropriate conversion).
	void setDelayBeforeValue(double value);
	/// Sets the pulse width value in seconds (does the appropriate conversion).
	void setPulseWidthValue(double value);
	/// Sets the time units value.
	void setTimeUnitsValue(int value);
	/// Sets the delay in seconds.
	void setDelayBeforeValueSeconds(double delayValue);
	/// Sets the pulse width in seconds.
	void setPulseWidthValueSeconds(double pulseWidth);

	/// Sets the edge trigger value preference, to be applied once the edge trigger control connects.
	void setEdgeTriggerPreference(int value);

protected slots:
	/// On control set bool changed.
	void onControlSetConnectedChanged(bool newState);
	/// Handles emitting the input value signal.
	void onInputValueChanged();
	/// Handles emitting the input status value changed signal.
	void onInputValueStatusChanged();
	/// Handles emitting the edge trigger signal.
	void onEdgeTriggerValueChanged();
	/// Handles emitting the delay before value changed signal.
	void onDelayBeforeValueChanged();
	/// Handles emitting the pulse width value changed signal.
	void onPulseWidthValueChanged();
	/// Handles emitting the time units value changed signal.
	void onTimeUnitsValueChanged();
	/// Handles emitting the trigger while active signal.
	void onTriggerWhileActiveValueChanged();
	/// Handles emitting the output value status.
	void onOutputValueStatusChanged();
	/// Handles emitting the delay time value changed signal.
	void onDelayBeforeValueSecondsChanged();
	/// Handles emitting the pulse width time value changed signal.
	void onPulseWidthValueSecondsChanged();

	/// Updates the edge trigger control with the edge trigger value preference.
	void updateEdgeTriggerControl();

protected:
	/// Helper method that returns the appropriate "letter" for the pulse index.
	QString letterFromPulseIndex(int index) const;

	/// Name of the zebra pulse control.
	QString name_;
	/// Flag for previous connectivity state.
	bool connected_;

	/// The control set for this pulse control.
	AMControlSet *allControls_;
	/// The input control.
	AMPVControl *inputControl_;
	/// The input control status.
	AMReadOnlyPVControl *inputStatusControl_;
	/// The trigger on rising or falling edge control.
	AMPVControl *edgeTriggerControl_;
	/// The pulse delay control.  Value is proportional different based on the time units PV.
	AMPVControl *delayBeforeControl_;
	/// The pulse width control.
	AMPVControl *pulseWidthControl_;
	/// The time units control.
	AMPVControl *timeUnitsControl_;
	/// The trigger while active status display.
	AMReadOnlyPVControl *triggerWhileActiveControl_;
	/// The output pulse status.
	AMReadOnlyPVControl *outputPulseControl_;
	/// The pulse delay control, resolves the delay control value and time units to value in seconds.
	BioXASZebraTimeSeconds *delayBeforeSecondsControl_;
	/// The pulse width control, resolve the pulse width value and time units into value in seconds.
	BioXASZebraTimeSeconds *pulseWidthSecondsControl_;

	/// The trigger edge control value preference, to be applied when the control connects.
	int edgeTriggerPreference_;
};

#endif // BIOXASZEBRAPULSECONTROL_H
