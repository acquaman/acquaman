#ifndef CLSEXCLUSIVESTATESCONTROL_H
#define CLSEXCLUSIVESTATESCONTROL_H

#include "beamline/AMExclusiveStatesEnumeratedControl.h"

class CLSExclusiveStatesControl : public AMExclusiveStatesEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumeration of the possible values.
	enum Value { Closed = 0, Open = 1, Between = 2 };

	/// Constructor.
	explicit CLSExclusiveStatesControl(const QString &name,
									   const QString &statusPV,
									   const QString &openPV,
									   const QString &closePV,
									   QObject *parent = 0);
	/// Destructor.
	virtual ~CLSExclusiveStatesControl();

	/// Returns the alarm status. Reimplemented to use the state control's alarm status.
	virtual int alarmStatus() const;
	/// Returns the alarm severity. Reimplemented to use the state control's alarm severity.
	virtual int alarmSeverity() const;

	/// Returns true if this control is open, false otherwise.
	bool isOpen() const;
	/// Returns true if this control is closed, false otherwise.
	bool isClosed() const;
	/// Returns true if this control is between states, false otherwise.
	bool isBetween() const;

protected slots:
	/// Sets the status control. Reimplemented to consider additional control signals.
	bool setStatusControl(AMControl *newControl);

	/// Updates the moving status. Reimplemented to use the Between state to indicate movement.
	virtual void updateMoving();

	/// Adds the open state.
	void addOpenState(AMControl *openTriggerControl, double openStatusValue, double triggerValue);
	/// Removes the open state.
	void removeOpenState();

	/// Adds the closed state.
	void addClosedState(AMControl *closeTriggerControl, double closedStatusValue, double triggerValue);
	/// Removes the closed state.
	void removeClosedState();

	/// Adds the between state.
	void addBetweenState(double betweenStatusValue);
	/// Removes the between state.
	void removeBetweenState();

protected:
	/// Enumeration of common status PV values.
	class Status { public: enum Values { Error0 = 0, Open = 1, Between = 2, Error3 = 3, Closed = 4 }; };
};

#endif // CLSEXCLUSIVESTATESCONTROL_H
