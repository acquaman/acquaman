#ifndef BIOXASMIRRORMASKSTATE_H
#define BIOXASMIRRORMASKSTATE_H

#include "beamline/AMSingleEnumeratedControl.h"

#define BIOXASMIRRORMASKSTATE_UPPER_BLADE_CLOSED_POSITION_SETPOINT 0.0
#define BIOXASMIRRORMASKSTATE_UPPER_BLADE_CLOSED_POSITION_MINIMUM 0.0
#define BIOXASMIRRORMASKSTATE_UPPER_BLADE_CLOSED_POSITION_MAXIMUM 0.01

#define BIOXASMIRRORMASKSTATE_UPPER_BLADE_OPEN_POSITION_MINIMUM 0.01
#define BIOXASMIRRORMASKSTATE_UPPER_BLADE_OPEN_POSITION_MAXIMUM 10.0

class BioXASMirrorMaskState : public AMSingleEnumeratedControl
{
    Q_OBJECT

public:
	/// Enumeration of the possible values.
	enum Value { Closed = 0, Open = 1 };

	/// Constructor.
	explicit BioXASMirrorMaskState(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMirrorMaskState();

	/// Returns the upper blade control.
	AMControl* upperSlitBlade() const { return control_; }

signals:
	/// Notifier that the upper blade control has changed.
	void upperBladeChanged(AMControl *newControl);

public slots:
	/// Sets the upper blade control.
	void setUpperBlade(AMControl *newControl);

protected:
	/// Creates and returns a move action to the given setpoint. This control only supports closing, for now. I expect this functionality to get more complex, maybe this should be moved to another class someday?
	virtual AMAction3* createMoveAction(double setpoint);
	/// Creates and returna an action that closes the mask.
	AMAction3* createMoveToClosedAction();
};

#endif // BIOXASMIRRORMASKSTATE_H
