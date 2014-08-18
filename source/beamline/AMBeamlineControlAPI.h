#ifndef AMBEAMLINECONTROLAPI_H
#define AMBEAMLINECONTROLAPI_H

#include <QString>

#include "dataman/info/AMControlInfo.h"

class AMControl;

class AMBeamlineControlAPI {
public:
	AMBeamlineControlAPI();

	/// Returns a control based on the name of the control.  Returns 0 if no control is found.
	virtual AMControl *exposedControlByName(const QString &name) { Q_UNUSED(name); return 0; }
	/// Returns a control based on the control info.  Returns 0 if no control is found.
	virtual AMControl *exposedControlByInfo(const AMControlInfo &info) { Q_UNUSED(info); return 0; }

	/// Adds a control to the exposed set.
	virtual bool addExposedControl(AMControl *control) { Q_UNUSED(control); return false; }
};

#endif // AMBEAMLINECONTROLAPI_H
