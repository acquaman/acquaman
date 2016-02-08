#ifndef BIOXASFLOWSWITCHES_H
#define BIOXASFLOWSWITCHES_H

#include "beamline/BioXAS/BioXASBiStateGroup.h"

class BioXASFlowSwitches : public BioXASBiStateGroup
{
	Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Bad = 0, Good = 1 };

	/// Constructor.
	BioXASFlowSwitches(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFlowSwitches();

	/// Returns true if this control is bad, false otherwise.
	virtual bool isBad() const;
	/// Returns true if this control is good, false otherwise.
	virtual bool isGood() const;

	/// Returns true if the given control is a child of the flow switches, false otherwise.
	bool hasFlowSwitch(AMControl *control) const;

	/// Returns the list of flow switch controls.
	QList<AMControl*> flowSwitchesList() const { return children_; }
	/// Returns the list of flow switch controls that are bad.
	QList<AMControl*> badFlowSwitchesList() const;
	/// Returns the list of flow switch controls that are good.
	QList<AMControl*> goodFlowSwitchesList() const;

signals:
	/// Notifier that the flow switches have changed.
	void flowSwitchesChanged();

public slots:
	/// Adds a flow switch control.
	bool addFlowSwitch(AMControl *newControl);
	/// Removes a flow switch control.
	bool removeFlowSwitch(AMControl *control);
	/// Clears all flow switch controls.
	bool clearFlowSwitches();

protected:
	/// Creates and returns a move action. Always returns 0, as this control does not support moving.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }

	/// Returns the index for the current value.
	virtual int currentIndex() const;
};

#endif // BIOXASFLOWSWITCHES_H
