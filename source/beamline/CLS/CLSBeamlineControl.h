#ifndef CLSBEAMLINECONTROL_H
#define CLSBEAMLINECONTROL_H

#include "beamline/AMBeamlineControl.h"
#include "beamline/AMPVControl.h"

class CLSBeamlineControl : public AMBeamlineControl
{
	Q_OBJECT

public:
	/// An enumeration of the possible status states.
	enum Status { Bad = 0, Good = 1 };

	/// Constructor.
	CLSBeamlineControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSBeamlineControl();
};

#endif // CLSBEAMLINECONTROL_H
