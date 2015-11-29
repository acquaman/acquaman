#ifndef SGMSAMPLECHAMBERVACUUM_H
#define SGMSAMPLECHAMBERVACUUM_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/SGM/SGMSampleChamberVacuumMoveControl.h"

#define SGMSAMPLECHAMBERVACUUM_VENTED_MIN 760.0
#define SGMSAMPLECHAMBERVACUUM_VENTED_MAX 1000.0 // Has to be larger than any value we would encounter.
#define SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MIN 0.001
#define SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MAX 10.0
#define SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MIN 0.000000001 // Has to be smaller than any value we would encounter.
#define SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MAX 0.001

#define SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MIN 1
#define SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MAX 1

#define SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT 30 // the move timeout interval, in seconds.

class AMControl;
class SGMSampleChamberVacuumMoveControl;

class SGMSampleChamberVacuum : public AMSingleEnumeratedControl
{
	Q_OBJECT

public:
	/// Enum describing the possible vacuum states.
	enum State { Vented = 0, RoughVacuum = 1, HighVacuum = 2 };

	/// Constructor.
	explicit SGMSampleChamberVacuum(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuum();

	/// Returns true if this control can move right now. Reimplemented to consider the move control.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. Reimplemented to consider the move control.
	virtual bool canStop() const;

	/// Returns the sample chamber pressure gauge control.
	AMControl* pressure() const { return control_; }
	/// Returns the move control.
	SGMSampleChamberVacuumMoveControl* moveControl() const { return move_; }

signals:
	/// Notifier that the pressure control has changed.
	void pressureChanged(AMControl *newControl);
	/// Notifier that the move control has changed.
	void moveChanged(SGMSampleChamberVacuumMoveControl *newControl);

public slots:
	/// Sets the sample chamber pressure gauge control.
	void setPressure(AMControl *newControl);
	/// Sets the sample chamber vacuum move control.
	void setMoveControl(SGMSampleChamberVacuumMoveControl *newControl);

protected slots:
	/// Updates the pressure control.
	void updatePressure();
	/// Updates the move control.
	void updateMove();

protected:
	/// Creates and returns a move action to the given destination.
	virtual AMAction3* createMoveAction(double setpoint);

protected:
	/// The vacuum move control.
	SGMSampleChamberVacuumMoveControl *move_;
};

#endif // SGMSAMPLECHAMBERVACUUM_H
