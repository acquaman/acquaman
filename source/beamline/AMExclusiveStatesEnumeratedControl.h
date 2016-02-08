#ifndef AMEXCLUSIVESTATESENUMERATEDCONTROL_H
#define AMEXCLUSIVESTATESENUMERATEDCONTROL_H

#include "beamline/AMSingleEnumeratedControl.h"

class AMExclusiveStatesEnumeratedControl : public AMSingleEnumeratedControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMExclusiveStatesEnumeratedControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~AMExclusiveStatesEnumeratedControl();

	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns a list of indices with the given status value.
	QList<int> indicesWithStatus(double status) const { return indicesContaining(status); }
	/// Returns a list of indices with the given control.
	QList<int> indicesWithControl(AMControl *control) const { return indexControlMap_.keys(control); }
	/// Returns a list of indices with the given trigger value. Not sure this is terribly useful, but there you go.
	QList<int> indicesWithTrigger(double trigger) const { return indexTriggerMap_.keys(trigger); }

signals:
	/// Notifier that the status control has changed.
	void statusChanged(AMControl *newControl);

public slots:
	/// Sets the status control.
	void setStatusControl(AMControl *newControl);

	/// Adds a new state to the control.
	virtual bool addState(int index, const QString &stateName, double statusValue, AMControl *control, double controlTriggerValue);
	/// Adds a new read-only state to the control.
	virtual bool addReadOnlyState(int index, const QString &stateName, double statusValue);
	/// Removes a state.
	virtual bool removeState(int index);
	/// Clears all states.
	virtual bool clearStates();

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();

protected:
	/// Returns a new action that moves the control to the setpoint that corresponds to the given index setpoint.
	virtual AMAction3* createMoveAction(double optionIndex);

protected:
	/// The mapping between the option index and the operation control.
	QMap<int, AMControl*> indexControlMap_;
	/// The mapping between the option index and the operation control trigger value.
	QMap<int, double> indexTriggerMap_;

private:
	/// Sets the control for the given index. Handles removing any previous controls for this index before setting the new one. Adds new control as a child of this control.
	void setControl(int index, AMControl *control);
	/// Removes the control for the given index. Handles removing this control as a child.
	void removeControl(int index);
	/// Clears the controls.
	void clearControls();
};

#endif // AMEXCLUSIVESTATESENUMERATEDCONTROL_H
