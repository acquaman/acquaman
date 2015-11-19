#ifndef SGMVATVALVE_H
#define SGMVATVALVE_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/AMPVControl.h"

#define SGMVATVALVE_CLOSED_MIN 0
#define SGMVATVALVE_CLOSED_MAX 58000
#define SGMVATVALVE_CLOSED_SETPOINT 58000
#define SGMVATVALVE_OPEN_MIN 58001
#define SGMVATVALVE_OPEN_MAX 80000
#define SGMVATVALVE_OPEN_SETPOINT 70000
#define SGMVATVALVE_SPEED_MIN 1
#define SGMVATVALVE_SPEED_MAX 1

class SGMVATValve : public AMSingleEnumeratedControl
{
	Q_OBJECT

public:
	/// Enum describing the available valve states.
	enum State { Closed = 0, Open = 1 };

	/// Constructor.
	explicit SGMVATValve(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMVATValve();

	/// Returns the position.
	AMSinglePVControl* position() const { return qobject_cast<AMSinglePVControl*>(control_); }
	/// Returns the speed.
	AMSinglePVControl* speed() const { return speed_; }

	/// Creates and returns a new action that sets the speed and moves to the given position.
	AMAction3* createMoveAtSpeedAction(double speedSetpoint, double positionSetpoint);

protected:
	/// The VAT valve speed.
	AMSinglePVControl* speed_;
};

#endif // SGMVATVALVE_H
