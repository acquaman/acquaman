#ifndef BIOXASZEBRASOFTINPUTCONTROL_H
#define BIOXASZEBRASOFTINPUTCONTROL_H

#include "beamline/AMPVControl.h"

class BioXASZebraSoftInputControl : public AMSinglePVControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraSoftInputControl(const QString& name,
										 const QString& PVname,
										 QObject* parent = 0,
										 double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
										 double completionTimeoutSeconds = 10.0,
										 const QString &description = "");
	/// Destructor.
	virtual ~BioXASZebraSoftInputControl();

	/// Returns true if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the length of time, in seconds, the control value should stay at a non-zero value before resetting to zero.
	double timeBeforeReset() const;
	/// Returns the preference for the time before reset value, applied to the control once it is connected.
	double timeBeforeResetPreference() const { return timeBeforeResetPreference_; }
	/// Returns the control for the length of time, in seconds, the value should stay at a non-zero value before resetting to zero.
	AMSinglePVControl* timeBeforeResetControl() const { return timeBeforeResetControl_; }

signals:
	/// Notifier that the length of time the value should stay at a non-zero value has changed.
	void timeBeforeResetChanged(double newValue);
	/// Notifier that the preference for the time before reset value has changed.
	void timeBeforeResetPreferenceChanged(double newValue);

public slots:
	/// Sets the length of time, in seconds, the value should stay at a non-zero value before resetting to zero.
	void setTimeBeforeReset(double newValue);
	/// Sets the preference for the time before reset value.
	void setTimeBeforeResetPreference(double newValue);

protected slots:
	/// Updates the time before reset control with the time before reset value preference.
	void updateTimeBeforeResetControl();

protected:
	/// The time before reset value preference.
	double timeBeforeResetPreference_;
	/// The time before reset control.
	AMSinglePVControl *timeBeforeResetControl_;
};

#endif // BIOXASZEBRASOFTINPUTCONTROL_H
