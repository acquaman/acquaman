#ifndef SGMVATVALVE_H
#define SGMVATVALVE_H

#include "beamline/SGM/SGMControl.h"
#include "beamline/AMPVControl.h"

#define SGMVATVALVE_CLOSED_POSITION 58000
#define SGMVATVALVE_OPEN_POSITION 70000
#define SGMVATVALVE_MIN_SPEED 1
#define SGMVATVALVE_MAX_SPEED 1

class SGMVATValve : public SGMControl
{
	Q_OBJECT

public:
	/// Enum describing the available valve states.
	enum State { Closed = 0, Open = 1 };

	/// Constructor.
	explicit SGMVATValve(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMVATValve();

	/// Returns true if this valve control is open, false otherwise.
	bool isOpen() const;
	/// Returns true if this valve control is closed, false otherwise.
	bool isClosed() const;

	/// Returns the position.
	AMSinglePVControl* position() const { return position_; }
	/// Returns the speed.
	AMSinglePVControl* speed() const { return speed_; }

	/// Creates and returns a new action that sets the speed and moves to the given position.
	AMAction3* createMoveAtSpeedAction(double speedSetpoint, double positionSetpoint);

protected:
	/// The VAT valve position.
	AMSinglePVControl* position_;
	/// The VAT valve speed.
	AMSinglePVControl* speed_;
};

#endif // SGMVATVALVE_H
