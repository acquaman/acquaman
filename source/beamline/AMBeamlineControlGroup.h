#ifndef AMBEAMLINECONTROLGROUP_H
#define AMBEAMLINECONTROLGROUP_H

#include "beamline/AMConnectedControl.h"
#include "beamline/AMBeamlineControlGroupStatus.h"

class AMBeamlineControlGroup : public AMConnectedControl
{
	Q_OBJECT

public:
	/// Constructor.
	AMBeamlineControlGroup(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMBeamlineControlGroup();

	/// Returns true if this control is bad, false otherwise.
	virtual bool isBad() const;
	/// Returns true if this control is good, false otherwise.
	virtual bool isGood() const;

	/// Returns true if the group has the given beamline control.
	virtual bool hasControl(AMControl *control) const;

	/// Returns the status control.
	AMBeamlineControlGroupStatus* statusControl() const { return status_; }

public slots:
	/// Adds a monitor control, with the values for the given states. Overwrites any existing information for the given control. Returns true if successful, false otherwise.
	bool addControl(AMBeamlineControl *control);
	/// Removes a monitor control. Returns true if successful, false otherwise.
	bool removeControl(AMBeamlineControl *control);
	/// Clears the monitor controls. Returns true if successful, false otherwise.
	bool clearControls();

protected:
	/// The status control.
	AMBeamlineControlGroupStatus *status_;
};

#endif // AMBEAMLINECONTROLGROUP_H
