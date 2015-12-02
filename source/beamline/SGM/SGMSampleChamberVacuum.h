#ifndef SGMSAMPLECHAMBERVACUUM_H
#define SGMSAMPLECHAMBERVACUUM_H

#include "beamline/AMSingleEnumeratedControl.h"

#define SGMSAMPLECHAMBERVACUUM_VENTED_MIN 760.0
#define SGMSAMPLECHAMBERVACUUM_VENTED_MAX 1000.0 // Has to be larger than any value we would encounter.
#define SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MIN 0.001
#define SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MAX 10.0
#define SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MIN 0.000000001 // Has to be smaller than any value we would encounter.
#define SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MAX 0.001

class AMControl;

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
	virtual bool canMove() const { return false; }
	/// Returns true if this control can stop a move right now. Reimplemented to consider the move control.
	virtual bool canStop() const { return false; }

	/// Returns the sample chamber pressure gauge control.
	AMControl* pressure() const { return control_; }

signals:
	/// Notifier that the pressure control has changed.
	void pressureChanged(AMControl *newControl);

public slots:
	/// Sets the sample chamber pressure gauge control.
	void setPressure(AMControl *newControl);

protected:
	/// Creates and returns a move action to the given destination.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }
};

#endif // SGMSAMPLECHAMBERVACUUM_H
