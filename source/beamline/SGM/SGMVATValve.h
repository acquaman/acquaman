#ifndef SGMVATVALVE_H
#define SGMVATVALVE_H

#define SGMVATVALVE_CLOSED_MIN 0
#define SGMVATVALVE_CLOSED_MAX 58000
#define SGMVATVALVE_CLOSED_SETPOINT 58000
#define SGMVATVALVE_OPEN_MIN 58001
#define SGMVATVALVE_OPEN_MAX 80000
#define SGMVATVALVE_OPEN_SETPOINT 63000

#define SGMVATVALVE_SPEED_MIN 1
#define SGMVATVALVE_SPEED_MAX 1000

#include "beamline/AMConnectedControl.h"

class AMPVControl;
class AMSinglePVControl;
class SGMVATValveState;

class SGMVATValve : public AMConnectedControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMVATValve(const QString &deviceName, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMVATValve();

	/// Returns the position control.
	AMPVControl* position() const { return position_; }
	/// Returns the state control.
	SGMVATValveState* state() const { return state_; }
	/// Returns the speed.
	AMSinglePVControl* speed() const { return speed_; }

protected:
	/// The position control.
	AMPVControl *position_;
	/// The open/closed state control.
	SGMVATValveState *state_;
	/// The VAT valve speed.
	AMSinglePVControl* speed_;
};

#endif // SGMVATVALVE_H
