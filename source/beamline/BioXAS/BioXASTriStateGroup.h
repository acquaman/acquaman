#ifndef BIOXASTRISTATEGROUP_H
#define BIOXASTRISTATEGROUP_H

#include "beamline/BioXAS/BioXASBiStateGroup.h"

class BioXASTriStateGroup : public BioXASBiStateGroup
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASTriStateGroup(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASTriStateGroup();

protected slots:
	/// Adds a child control control, with the values for the given states. Overwrites any existing information for the given control. Returns true if successful, false otherwise.
	bool addTriStateControl(AMControl *control, double state1Value, double state2Value);
	/// Removes a control. Returns true if successful, false otherwise.
	bool removeTriStateControl(AMControl *control);
	/// Clears the controls. Returns true if successful, false otherwise.
	bool clearTriStateControls();

protected:
	/// Returns the list of children in state 2.
	QList<AMControl*> childrenInState2() const;
	/// Returns the list of children not in state 2.
	QList<AMControl*> childrenNotInState2() const;

	/// Returns true if any children are in state 2, false otherwise.
	bool areAnyChildrenState2() const;

	/// Returns true if all children are in state 2, false otherwise.
	bool areAllChildrenState2() const;

	/// Returns true if the given child control is in state 2, false otherwise.
	bool isChildState2(AMControl *child) const;

	/// Creates and returns a new action that moves all valid children to state 2.
	AMAction3* createMoveChildrenToState2Action();
	/// Creates and returns a new action that moves the given child control to state 2.
	AMAction3* createMoveChildToState2Action(AMControl *child);

	/// Creates and returns a new action that waits for the given child control to reach state 2.
	AMAction3* createCheckChildAtState2Action(AMControl *child, double timeoutSec = 10);

protected:
	/// The map between control and state 2 value.
	QMap<AMControl*, double> controlState2ValueMap_;
};

#endif // BIOXASTRISTATEGROUP_H
