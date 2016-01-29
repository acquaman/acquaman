#ifndef VESPERSTOGGLECONTROL_H
#define VESPERSTOGGLECONTROL_H

#include "beamline/AMPVControl.h"

/// This is a very specific control for VESPERS that handles "toggle" PVs.
class VESPERSToggleControl : public AMPVControl
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSToggleControl(const QString& name,
						 const QString& readPVName,
						 const QString &writePVName,
						 QObject* parent = 0,
						 double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
						 double completionTimeoutSeconds = 10.0,
						 const QString &description = "");
	/// Destructor.
	virtual ~VESPERSToggleControl();

public slots:
	/// Start a move to the value setpoint (reimplemented)
	virtual FailureExplanation move(double setpoint);

	/// Stop a move in progress (reimplemented)
	virtual bool stop();
};

#endif // VESPERSTOGGLECONTROL_H
