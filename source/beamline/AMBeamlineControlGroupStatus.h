#ifndef AMBEAMLINECONTROLGROUPSTATUS_H
#define AMBEAMLINECONTROLGROUPSTATUS_H

#include "beamline/AMEnumeratedControl.h"

class AMBeamlineControl;

class AMBeamlineControlGroupStatus : public AMEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Bad = 0, Good = 1 };

	/// Constructor.
	AMBeamlineControlGroupStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMBeamlineControlGroupStatus();

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

	/// Returns the list of controls in the 'bad' state.
	QList<AMBeamlineControl*> badControls() const;
	/// Returns the list of controls in the 'good' state.
	QList<AMBeamlineControl*> goodControls() const;

public slots:
	/// Adds a monitor control, with the values for the given states. Overwrites any existing information for the given control. Returns true if successful, false otherwise.
	bool addControl(AMBeamlineControl *control);
	/// Removes a monitor control. Returns true if successful, false otherwise.
	bool removeControl(AMBeamlineControl *control);
	/// Clears the monitor controls. Returns true if successful, false otherwise.
	bool clearControls();

protected:
	/// Creates and returns a move action. Always returns 0, as this control does not support moving.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }
	/// Returns the index for the current value.
	virtual int currentIndex() const;
};

#endif // AMBEAMLINECONTROLGROUPSTATUS_H
