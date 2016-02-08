#ifndef BIOXASSSRLMONOCHROMATORMASKSTATE_H
#define BIOXASSSRLMONOCHROMATORMASKSTATE_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/AMPVControl.h"

class BioXASSSRLMonochromatorMaskState : public AMSingleEnumeratedControl
{
    Q_OBJECT

public:
	/// Enum describing the different mask control values.
	enum Value { Closed = 0, Open = 1 };

	/// Constructor.
	explicit BioXASSSRLMonochromatorMaskState(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorMaskState();

	/// Returns true if this control can move, false otherwise. Reimplemented to consider the upper and lower blades.
	virtual bool canMove() const;

	/// Returns true if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the blades state control.
	AMReadOnlyPVControl* bladesState() const { return qobject_cast<AMReadOnlyPVControl*>(control_); }
	/// Returns the upper blade control.
	AMControl* upperBlade() const { return upperBlade_; }
	/// Returns the lower blade control.
	AMControl* lowerBlade() const { return lowerBlade_; }

signals:
	/// Notifier that the blades state control has changed.
	void bladesStateChanged(AMControl *newControl);
	/// Notifier that the upper blade control has changed.
	void upperBladeChanged(AMControl *newControl);
	/// Notifier that the lower blade control has changed.
	void lowerBladeChanged(AMControl *newControl);

public slots:
	/// Sets the blades state control.
	void setBladesState(AMControl *newControl);
	/// Sets the upper blade control.
	void setUpperBlade(AMControl *newControl);
	/// Sets the lower blade control.
	void setLowerBlade(AMControl *newControl);

protected:
	/// Creates and returns a move action to the given setpoint. This control only supports closing, for now. I expect this functionality to get more complex, maybe this should be moved to another class someday?
	virtual AMAction3* createMoveAction(double setpoint);
	/// Creates and returna an action that closes the mask.
	AMAction3* createMoveToClosedAction();

protected:
	/// The upper blade control.
	AMControl *upperBlade_;
	/// The lower blade control.
	AMControl *lowerBlade_;
};

#endif // BIOXASSSRLMONOCHROMATORMASKSTATE_H
