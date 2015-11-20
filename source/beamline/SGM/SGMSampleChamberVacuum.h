#ifndef SGMSAMPLECHAMBERVACUUM_H
#define SGMSAMPLECHAMBERVACUUM_H

#include "beamline/SGM/SGMSampleChamberVacuumControl.h"

#define SGMSAMPLECHAMBERVACUUM_VENTED_MIN 760.0
#define SGMSAMPLECHAMBERVACUUM_VENTED_MAX 1000.0 // Has to be larger than any value we would encounter.
#define SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MIN 0.001
#define SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MAX 10.0
#define SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MIN 0.000000001 // Has to be smaller than any value we would encounter.
#define SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MAX 0.001

#define SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MIN 1
#define SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MAX 1

#define SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT 30 // the move timeout interval, in seconds.

class SGMSampleChamberVacuum : public SGMSampleChamberVacuumControl
{
	Q_OBJECT

public:
	/// Enum describing the possible vacuum states.
	enum State { Vented = 0, RoughVacuum = 1, HighVacuum = 2 };

	/// Constructor.
	explicit SGMSampleChamberVacuum(QObject *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuum();

protected:
	/// Creates and returns a move action to the given destination.
	virtual AMAction3* createMoveAction(double indexSetpoint);

	/// Creates and returns a move action to the Vented state.
	AMAction3* createMoveToVentedAction();
	/// Creates and returns a move action to the Vented state from a Vacuum state.
	AMAction3* createMoveToVentedFromVacuumAction();

	/// Creates and returns a move action to the RoughVacuum state.
	AMAction3* createMoveToRoughVacuumAction();
	/// Creates and returns a move action to the RoughVacuum state from the Vented state.
	AMAction3* createMoveToRoughVacuumFromVentedAction();
	/// Creates and returns a move action to the RoughVacuum state from the HighVacuum state.
	AMAction3* createMoveToRoughVacuumFromHighVacuumAction();

	/// Creates and returns a move action to the HighVacuum state.
	AMAction3* createMoveToHighVacuumAction();
	/// Creates and returns a move action to the HighVacuum state from a Vented state.
	AMAction3* createMoveToHighVacuumFromVentedAction();
	/// Creates and returns a move action to the HighVacuum state from a RoughVacuum state.
	AMAction3* createMoveToHighVacuumFromRoughVacuumAction();
};

#endif // SGMSAMPLECHAMBERVACUUM_H
