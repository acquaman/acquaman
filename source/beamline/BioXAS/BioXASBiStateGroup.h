#ifndef BIOXASBISTATEGROUP_H
#define BIOXASBISTATEGROUP_H

#include "beamline/AMEnumeratedControl.h"

class BioXASBiStateGroup : public AMEnumeratedControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBiStateGroup(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBiStateGroup();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise. Subclasses should reimplement if they want to consider only a subset of children.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

protected slots:
//	/// Updates the connected state.
//	virtual void updateConnected();
	/// Updates the moving state.
	virtual void updateMoving();

	/// Adds a child control control, with the values for the given states. Overwrites any existing information for the given control. Returns true if successful, false otherwise.
	bool addBiStateControl(AMControl *control, double state1Value);
	/// Removes a control. Returns true if successful, false otherwise.
	bool removeBiStateControl(AMControl *control);
	/// Clears the controls. Returns true if successful, false otherwise.
	bool clearBiStateControls();

protected:
	/// Returns the list of children in state 1.
	QList<AMControl*> childrenInState1() const;
	/// Returns the list of children not in state 1.
	QList<AMControl*> childrenNotInState1() const;

	/// Returns true if any children are in state 1, false otherwise.
	bool areAnyChildrenState1() const;

	/// Returns true if all children are in state 1, false otherwise.
	bool areAllChildrenState1() const;

	/// Returns true if the given child control is in state 1, false otherwise.
	bool isChildState1(AMControl *child) const;

	/// Creates and returns a new action that moves all valid children to state 1.
	AMAction3* createMoveChildrenToState1Action();
	/// Creates and returns a new action that moves the given child control to state 1.
	AMAction3* createMoveChildToState1Action(AMControl *child);

	/// Creates and returns a new action that waits for the given child control to reach state 1.
	AMAction3* createCheckChildAtState1Action(AMControl *child, double timeoutSec = 10);

protected:
	/// The map between control and state 1 value.
	QMap<AMControl*, double> controlState1ValueMap_;
};

#endif // BIOXASBISTATEGROUP_H
