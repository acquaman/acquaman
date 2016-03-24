#ifndef AMTEMPERATUREMONITORGROUPSTATUS_H
#define AMTEMPERATUREMONITORGROUPSTATUS_H

#include "beamline/AMEnumeratedControl.h"

class AMTemperatureMonitor;

class AMTemperatureMonitorGroupStatus : public AMEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Bad = 0, Good = 1 };

	/// Constructor.
	AMTemperatureMonitorGroupStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTemperatureMonitorGroupStatus();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise. Subclasses should reimplement if they want to consider only a subset of children.
	virtual bool canMove() const { return false; }
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const { return false; }

	/// Returns true if this control is bad, false otherwise.
	virtual bool isBad() const;
	/// Returns true if this control is good, false otherwise.
	virtual bool isGood() const { return !isBad(); }

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
	/// Creates and returns a move action. Always returns 0, as this control does not support moving.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }
	/// Returns the index for the current value.
	virtual int currentIndex() const;
};

#endif // AMTEMPERATUREMONITORGROUPSTATUS_H
