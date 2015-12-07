#ifndef BIOXASBISTATECONTROL_H
#define BIOXASBISTATECONTROL_H

#include "beamline/AMEnumeratedControl.h"

class BioXASBiStateControl : public AMEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	BioXASBiStateControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBiStateControl();

	/// Returns true if the valves are open, false otherwise.
	virtual bool isOpen() const { return false; }
	/// Returns true if the valves are closed, false otherwise.
	virtual bool isClosed() const { return false; }

protected slots:
	/// Adds a new control to the valve set.
	virtual bool addChildControl(AMControl *newControl);
	/// Removes a control from the valve set.
	virtual bool removeChildControl(AMControl *control);

protected:
	/// Creates and returns a new move action.
	virtual AMAction3* createMoveAction(double setpoint);
};

#endif // BIOXASBISTATECONTROL_H
