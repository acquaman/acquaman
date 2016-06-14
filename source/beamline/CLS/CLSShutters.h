#ifndef CLSSHUTTERS_H
#define CLSSHUTTERS_H

#include "beamline/CLS/CLSTriStateGroup.h"

#define CLSSHUTTERS_BEAM_ONOFF_LIST_CONFLICTION 3002001

class CLSShutters : public CLSTriStateGroup
{
	Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	explicit CLSShutters(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSShutters();

	/// helper function to create a beam on action list
	AMAction3* createBeamOnActionList();
	/// helper function to create a beam off action list
	AMAction3* createBeamOffActionList();

	/// Returns true if this control is open, false otherwise. Finds this out by investigating the states of all children.
	virtual bool isOpen() const;
	/// Returns true if this control is closed, false otherwise. Finds this out by investigating the states of all children.
	virtual bool isClosed() const;

	/// Returns the safety shutter control
	AMControl *safetyShutter() const { return safetyShutter_;}

	/// Returns true if the given control is one of the shutters, false otherwise.
	bool hasShutter(AMControl *control) const;

	/// Returns the list of shutters.
	QList<AMControl*> shuttersList() const { return children_; }

signals:
	/// Notifier that the shutters have changed.
	void shuttersChanged();

public slots:
	/// set the safety shutter
	bool setSafetyShutter(AMControl *safetyShutter);
	/// Adds a shutter control.
	/// @param beamOnOrder: the order to open a shutter when doing beam on/off. The smaller, the earlier. -1, if a shutter is not invovled in beam on/off
	bool addShutter(AMControl *newShutter, double openValue, double closedValue, int beamOnOrder=-1);
	/// Removes a shutter control.
	bool removeShutter(AMControl *newValve);
	/// Clears all shutter controls.
	bool clearShutters();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Creates and returns a new move action to Open.
	virtual AMAction3* createMoveToOpenAction();
	/// Creates and returns a new move action to Closed.
	virtual AMAction3* createMoveToClosedAction();

	/// Creates and returns a new action that opens the child control.
	AMAction3* createMoveChildToOpen(AMControl *child);
	/// Creates and returns a new action that closes the child control.
	AMAction3* createMoveChildToClosed(AMControl *child);

	/// Creates and returns a new action that checks that the child control is open, waiting the timeout amount.
	AMAction3* createCheckChildIsOpen(AMControl *child, double timeoutSec);
	/// Creates and returns a new action that checks that the child control is closed, waiting the timeout amount.
	AMAction3* createCheckChildIsClosed(AMControl *child, double timeoutSec);

	/// Returns the index for the current value.
	virtual int currentIndex() const;

protected:
	/// the safety shutter of a beamline
	AMControl *safetyShutter_;

	/// the order of the shutters in the beam on action. If a shutter is not invovled in the beam on/off action, the order is -1
	QMap<int, AMControl*> shuttersBeamOnOrderMap_;
};

#endif // CLSSHUTTERS_H
