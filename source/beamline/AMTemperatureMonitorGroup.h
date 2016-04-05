#ifndef AMTEMPERATUREMONITORGROUP_H
#define AMTEMPERATUREMONITORGROUP_H

#include "beamline/AMConnectedControl.h"
#include "beamline/AMTemperatureMonitorGroupStatus.h"

class AMTemperatureMonitorGroup : public AMConnectedControl
{
	Q_OBJECT

public:
	/// Constructor.
	AMTemperatureMonitorGroup(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTemperatureMonitorGroup();

	/// Returns true if this control is bad, false otherwise.
	virtual bool isBad() const;
	/// Returns true if this control is good, false otherwise.
	virtual bool isGood() const;

	/// Returns the status control.
	AMTemperatureMonitorGroupStatus* statusControl() const { return status_; }

	/// Returns the list of monitors in the 'bad' state.
	QList<AMTemperatureMonitor*> badMonitors() const;
	/// Returns the list of monitors in the 'good' state.
	QList<AMTemperatureMonitor*> goodMonitors() const;

public slots:
	/// Adds a monitor control, with the values for the given states. Overwrites any existing information for the given control. Returns true if successful, false otherwise.
	bool addMonitor(AMTemperatureMonitor *control);
	/// Removes a monitor control. Returns true if successful, false otherwise.
	bool removeMonitor(AMTemperatureMonitor *control);
	/// Clears the monitor controls. Returns true if successful, false otherwise.
	bool clearMonitors();

protected:
	/// The status control.
	AMTemperatureMonitorGroupStatus *status_;
};

#endif // AMTEMPERATUREMONITORGROUP_H
