#ifndef BIOXASBEAMSTATUS_H
#define BIOXASBEAMSTATUS_H

#include "beamline/CLS/CLSBiStateGroup.h"

class BioXASBeamStatus : public CLSBiStateGroup
{
    Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { On = 0, Off = 1 };

	/// Constructor.
	explicit BioXASBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatus();

	/// Returns true if this control can move, false otherwise. Reimplemented to make all beam status controls read-only by default (for now).
	virtual bool canMove() const { return false; }

	/// Returns true if the beam is on, false otherwise.
	virtual bool isOn() const;
	/// Returns true if the beam is off, false otherwise.
	virtual bool isOff() const;

	/// Returns true if the given component is in the beam on state, false otherwise.
	virtual bool componentInBeamOnState(AMControl *control) const { return isChildState1(control); }
	/// Returns true if the given component is not in the beam on state, false otherwise.
	virtual bool componentNotInBeamOnState(AMControl *control) const { return !componentInBeamOnState(control); }

	/// Returns the beam on value for the given control.
	double componentBeamOnValue(AMControl *control) const { return controlState1ValueMap_.value(control, -1); }

	/// Returns the list of components.
	QList<AMControl*> components() const { return children_; }
	/// Returns the list of components that are in the 'beam on' state.
	QList<AMControl*> componentsInBeamOnState() const;
	/// Returns the list of components that are not in the 'beam on' state.
	QList<AMControl*> componentsNotInBeamOnState() const;

signals:
	/// Notifier that the components have changed.
	void componentsChanged();

public slots:
	/// Adds a component to the beam status. Returns true if successful, false otherwise.
	bool addComponent(AMControl *newControl, double beamOnValue);
	/// Removes a component from the beam status. Returns true if successful, false otherwise.
	bool removeComponent(AMControl *control);
	/// Clears all components from the beam status. Returns true if successful, false otherwise.
	bool clearComponents();

protected:
	/// Creates and returns a new move action.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }

	/// Returns the index for the current value.
	virtual int currentIndex() const;
};

#endif // BIOXASBEAMSTATUS_H
