#ifndef AMINTERNALCONTROLMOVEACTION_H
#define AMINTERNALCONTROLMOVEACTION_H

#include "actions2/actions/AMControlMoveAction.h"

/// This AMControlMoveAction subclass is an example of a standalone AMAction that directly takes care of moving an AMControl into position.
/*! Most AMAction subclasses should be used in conjunction with an associated AMActionInfo subclass, which allows them to register views/editors, and re-run completed actions.  This class is an example of an internal, standalone action without this capability.  It doesn't need an associated AMControlMoveActionInfo subclass, and you can just specify the AMControl directly. A consequence of this is that it cannot be registered with AMActionRegistry, and completed actions cannot be re-loaded out of the user's database.

It is best suited for internal use to implement other actions/processes (for ex: inside AMListAction), rather than running in a user-visible way via AMActionRunner.*/
class AMInternalControlMoveAction : public AMControlMoveAction
{
    Q_OBJECT
public:
	/// Construct an action to move \c control into a position \c setpoint.  You can change the setpoint later with setSetpoint().
	/*! This constructor is not Q_INVOKABLE and does not accept an AMActionInfo*, so it will not be possible to register the class with AMActionRegistry::registerInfoAndAction().*/
	AMInternalControlMoveAction(AMControl* control, double setpoint, QObject *parent = 0);
	/// Copy constructor
	AMInternalControlMoveAction(const AMInternalControlMoveAction& other) : AMControlMoveAction(other) { internalControl_ = other.internalControl_; }
	/// Virtual copy constructor
	virtual AMInternalControlMoveAction* createCopy() const { return new AMInternalControlMoveAction(*this); }

	/// Re-implemented from AMControlMoveAction to provide our explicitly-specified control as the control to run the action.
	virtual AMControl* getControlForAction(const AMControlInfo &) const { return internalControl_; }

	/// You can change the move destination position \c setpoint before running the action.
	bool setSetpoint(double setpoint);

protected:
	AMControl* internalControl_;

};

#endif // AMINTERNALCONTROLMOVEACTION_H
