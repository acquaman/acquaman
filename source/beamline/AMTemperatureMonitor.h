#ifndef AMTEMPERATUREMONITOR_H
#define AMTEMPERATUREMONITOR_H

#include "beamline/AMPVControl.h"
#include "beamline/AMConnectedControl.h"

class AMTemperatureMonitor : public AMConnectedControl
{
	Q_OBJECT

public:
	/// Constructor.
	AMTemperatureMonitor(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTemperatureMonitor();

	/// Returns true if the status is good.
	virtual bool isGood() const;
	/// Returns true if the status is bad.
	virtual bool isBad() const;

	/// Returns the current status.
	virtual double status() const;
	/// Returns the current value.
	virtual double value() const;

	/// Returns the status control.
	AMReadOnlyPVControl* statusControl() const { return status_; }
	/// Returns the value control.
	AMReadOnlyPVControl* valueControl() const { return value_; }

signals:
	/// Notifier that the status has changed.
	void statusChanged(double status);
	/// Notifier that the status control has changed.
	void statusControlChanged(AMReadOnlyPVControl *control);
	/// Notifier that the value control has changed.
	void valueControlChanged(AMReadOnlyPVControl *control);

public slots:
	/// Sets the status control.
	void setStatusControl(AMReadOnlyPVControl *control, double goodValue, double badValue);
	/// Sets the value control.
	void setValueControl(AMReadOnlyPVControl *control);

protected:
	/// The good status value.
	double goodValue_;
	/// The bad status value.
	double badValue_;
	/// The status control.
	AMReadOnlyPVControl *status_;
	/// The value control.
	AMReadOnlyPVControl *value_;
};

#endif // AMTEMPERATUREMONITOR_H
