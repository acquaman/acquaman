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
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

//	/// Returns true if this control is open, false otherwise. Finds this out by investigating the states of all children.
//	virtual bool isOpen() const;
//	/// Returns true if this control is closed, false otherwise. Finds this out by investigating the states of all children.
//	virtual bool isClosed() const;

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the moving state.
	virtual void updateMoving();

	/// Adds a child control control, with the values for the given states. Overwrites any existing information for the given control. Returns true if successful, false otherwise.
	bool addBiStateControl(AMControl *control, double valueState1, double valueState2);
	/// Removes a control. Returns true if successful, false otherwise.
	bool removeBiStateControl(AMControl *control);
	/// Clears the controls. Returns true if successful, false otherwise.
	bool clearBiStateControls();

protected:
	/// Returns true if the given child control is in state 1, false otherwise.
	bool isChildState1(AMControl *child) const;
	/// Returns true if the given child control is in state 2, false otherwise.
	bool isChildState2(AMControl *child) const;

//	/// Creates and returns a new move action.
//	virtual AMAction3* createMoveAction(double setpoint);
//	/// Creates and returns a new move action to Open.
//	virtual AMAction3* createMoveToOpenAction();
//	/// Creates and returns a new move action to Closed.
//	virtual AMAction3* createMoveToClosedAction();

	/// Creates and returns a new action that opens the given child.
	AMAction3* createMoveChildToState1Action(AMControl *child);
	/// Creates and returns a new action that closes the given child.
	AMAction3* createMoveChildToState2Action(AMControl *child);

//	/// Returns the current index. Subclasses must reimplement for their specific behavior/interaction.
//	virtual int currentIndex() const;

protected:
	/// The map between control and open value.
	QMap<AMControl*, double> controlOpenValueMap_;
	/// The map between control and close value.
	QMap<AMControl*, double> controlCloseValueMap_;
};

#endif // BIOXASBISTATEGROUP_H
