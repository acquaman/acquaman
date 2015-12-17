#ifndef BIOXASM1MIRRORMASKSTATE_H
#define BIOXASM1MIRRORMASKSTATE_H

#include "beamline/AMSingleEnumeratedControl.h"

class BioXASM1MirrorMaskState : public AMSingleEnumeratedControl
{
    Q_OBJECT

public:
	/// Enumeration of the possible values.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASM1MirrorMaskState(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASM1MirrorMaskState();

	/// Returns true if this control can move, false otherwise. Reimplemented to make this control read-only, for now.
	virtual bool canMove() const { return false; }

	/// Returns the upper slit blade control.
	AMControl* upperSlitBlade() const { return control_; }

signals:
	/// Notifier that the upper slit blade control has changed.
	void upperSlitBladeChanged(AMControl *newControl);

public slots:
	/// Sets the upper slit blade control.
	void setUpperSlitBlade(AMControl *newControl);

protected:
	/// Creates and returns a new move action. This control does not presently support moving.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }
};

#endif // BIOXASM1MIRRORMASKSTATE_H
