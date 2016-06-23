#ifndef CLSMIRRORMASKSTATE_H
#define CLSMIRRORMASKSTATE_H

#include "beamline/AMSingleEnumeratedControl.h"

class CLSMirrorMaskState : public AMSingleEnumeratedControl
{
    Q_OBJECT

public:
	/// Enumeration of the possible values.
	enum Value { Closed = 0, Open = 1 };

	/// Constructor.
	explicit CLSMirrorMaskState(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSMirrorMaskState();

	/// Returns the upper slit blade control.
	AMControl* upperSlitBlade() const { return control_; }

signals:
	/// Notifier that the upper slit blade control has changed.
	void upperSlitBladeChanged(AMControl *newControl);

public slots:
	/// Sets the upper slit blade control.
	void setUpperSlitBlade(AMControl *newControl);

protected:
	/// Creates and returns a move action to the given setpoint. This control only supports closing, for now. I expect this functionality to get more complex, maybe this should be moved to another class someday?
	virtual AMAction3* createMoveAction(double setpoint);
	/// Creates and returna an action that closes the mask.
	AMAction3* createMoveToClosedAction();
};

#endif // CLSMIRRORMASKSTATE_H
