#ifndef BIOXASSSRLMONOCHROMATORMASKSTATE_H
#define BIOXASSSRLMONOCHROMATORMASKSTATE_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/AMPVControl.h"

class BioXASSSRLMonochromatorMaskState : public AMSingleEnumeratedControl
{
    Q_OBJECT

public:
	/// Enum describing the different mask control values.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASSSRLMonochromatorMaskState(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorMaskState();

	/// Returns true if this control can move, false otherwise. Reimplemented to make this control read-only, for now.
	virtual bool canMove() const { return false; }

	/// Returns the mask state control.
	AMReadOnlyPVControl* state() const { return qobject_cast<AMReadOnlyPVControl*>(control_); }

signals:
	/// Notifier that the mask status control has changed.
	void stateChanged(AMControl *newControl);

public slots:
	/// Sets the mask state PV control.
	void setState(AMReadOnlyPVControl *newControl);

protected:
	/// Creates and returns a move action to the given setpoint. This control does not presently support moving.
	AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }
};

#endif // BIOXASSSRLMONOCHROMATORMASKSTATE_H
