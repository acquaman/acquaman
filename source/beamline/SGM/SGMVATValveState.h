#ifndef SGMVATVALVESTATE_H
#define SGMVATVALVESTATE_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/AMPVControl.h"

class SGMVATValveState : public AMSingleEnumeratedControl
{
	Q_OBJECT

public:
	/// Enum describing the available valve states.
	enum State { Closed = 0, Open = 1 };

	/// Constructor.
	SGMVATValveState(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMVATValveState();

	/// Returns the position.
	AMPVControl* position() const { return qobject_cast<AMPVControl*>(control_); }
	/// Returns the speed.
	AMControl* speed() const { return speed_; }

signals:
	/// Notifier that the position control has changed.
	void positionChanged(AMPVControl *newControl);
	/// Notifier that the speed control has changed.
	void speedChanged(AMControl *newControl);

public slots:
	/// Sets the base position control.
	void setPosition(AMPVControl *newControl);
	/// Sets the speed control.
	void setSpeed(AMControl *newControl);

protected:
	/// Creates and returns a move action. Reimplemented to include setting the appropriate speed.
	virtual AMAction3* createMoveAction(double indexSetpoint);

	/// Updates the connected state of the vat valve
	void updateConnected();

protected:
	/// The speed control.
	AMControl *speed_;
};

#endif // SGMVATVALVESTATE_H
