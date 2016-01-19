#ifndef BIOXASZEBRAPULSECONTROL_H
#define BIOXASZEBRAPULSECONTROL_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

#define BIOXASZEBRAPULSECONTROL_PULSE_WIDTH_MIN 0.0
#define BIOXASZEBRAPULSECONTROL_PULSE_WIDTH_MAX 6.5

/// Pulse control encapsulation for the Zebra.  Takes a base name and pulse group index.
class BioXASZebraPulseControl : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Takes a base name and group index (1, 2, 3, 4).
	explicit BioXASZebraPulseControl(const QString &baseName, int pulseIndex, QObject *parent = 0);
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
	/// Returns the time units as a string.
	QString timeUnitsValueString() const;
	/// Returns whether the trigger while active status is on.
	bool triggerWhileActiveValue() const;
	/// Returns the output status.
	bool outputValue() const;

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

	/// Returns true if the given pulse width falls within the range for acceptable pulse width values, false otherwise.
	static bool validPulseWidth(double pulseWidth);
	/// Returns the result of converting the given pulse width and time units to the desired time units.
	static double convertPulseWidth(double pulseWidth, double pulseWidthUnits, double desiredUnits);

signals:
	/// Notifier that the pulse control connectivity has changed.
	void connectedChanged(bool);
	/// Notifier that the input value changed.
	void inputValueChanged(int);
	/// Notifier that the input value changed as a string.
	void inputValueStringChanged(const QString &);
	/// Notifier that the input status changed.
	void inputValueStatusChanged(bool);
	/// Notifier that the edge trigger value changed.
	void edgeTriggerValueChanged(int);
	/// Notifier that the delay before value changed (in seconds).
	void delayBeforeValueChanged(double);
	/// Notifier that the pulse width value changed (in seconds).
	void pulseWidthValueChanged(double);
	/// Notifier that the time units value changed.
	void timeUnitsValueChanged(int);
	/// Notifier that the trigger while active value changed.
	void triggerWhileActiveValueChanged(bool);
	/// Notifier that the output value changed.
	void outputValueChanged(bool);

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

protected slots:
	/// On control set bool changed.
	void onControlSetConnectedChanged(bool connected);
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
};

#endif // BIOXASZEBRAPULSECONTROL_H
