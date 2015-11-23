#ifndef SGMSAMPLECHAMBERVACUUMMOVEACTION_H
#define SGMSAMPLECHAMBERVACUUMMOVEACTION_H

#include "actions3/AMListAction3.h"
#include "actions3/SGM/SGMSampleChamberVacuumMoveActionInfo.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"

#define SGMSAMPLECHAMBERVACUUMMOVEACTION_INPUT_TIMEOUT 30

// Error codes.

#define SGMSAMPLECHAMBERVACUUMMOVEACTION_INVALID_CONTROL 1109239
#define SGMSAMPLECHAMBERVACUUMMOVEACTION_CONTROL_NOT_CONNECTED 1109240
#define SGMSAMPLECHAMBERVACUUMMOVEACTION_CONTROL_CANNOT_MOVE 1109241
#define SGMSAMPLECHAMBERVACUUMMOVEACTION_CONTROL_ALREADY_MOVING 1109242
#define SGMSAMPLECHAMBERVACUUMMOVEACTION_INVALID_SETPOINT 1109243
#define SGMSAMPLECHAMBERVACUUMMOVEACTION_CONTROL_SUBACTION_INVALID 1109244

class SGMSampleChamberVacuumMoveAction : public AMListAction3
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMSampleChamberVacuumMoveAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	SGMSampleChamberVacuumMoveAction(const SGMSampleChamberVacuumMoveAction &original);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveAction();

	/// Returns a new AMAction3 that's a copy of this one.
	virtual AMAction3* createCopy() const { return new SGMSampleChamberVacuumMoveAction(*this); }

	/// Returns whether this action can be paused, false for this action.
	virtual bool canPause() const { return false; }
	/// Returns whether this action can be skipped, false for this action.
	virtual bool canSkip() const { return false; }

protected:
	/// Creates and returns a move action to the Vented state from a Vacuum state.
	AMAction3* createMoveToVentedFromVacuumAction();

	/// Creates and returns a move action to the RoughVacuum state from the Vented state.
	AMAction3* createMoveToRoughVacuumFromVentedAction();
	/// Creates and returns a move action to the RoughVacuum state from the HighVacuum state.
	AMAction3* createMoveToRoughVacuumFromHighVacuumAction();

	/// Creates and returns a move action to the HighVacuum state from a Vented state.
	AMAction3* createMoveToHighVacuumFromVentedAction();
	/// Creates and returns a move action to the HighVacuum state from a RoughVacuum state.
	AMAction3* createMoveToHighVacuumFromRoughVacuumAction();

	/// Returns a new action that waits for the user to indicate they would like to proceed.
	AMAction3* waitForInput(const QString &instruction);

	/// Returns a new action that closes the VAT valve, at the given speed. Returns 0 if no valid valve is given.
	AMAction3* closeVATValve(double speed);
	/// Returns a new action that checks that the VAT valve is closed. Returns 0 if no valid valve is given.
	AMAction3* checkVATValveClosed();
	/// Returns a new action that opens the VAT valve, at the given speed. Returns 0 if no valid valve is given.
	AMAction3* openVATValve(double speed);
	/// Returns a new action that checks that the VAT valve is open. Returns 0 if no valid valve is given.
	AMAction3* checkVATValveOpen();

	/// Returns a new action that turns off the chamber roughing pump. Returns 0 if roughing pump is invalid.
	AMAction3* turnOffChamberRoughingPump() { return 0; }
	/// Returns a new action that checks that the chamber roughing pump is off. Returns 0 if roughing pump is invalid.
	AMAction3* checkChamberRoughingPumpOff() { return 0; }
	/// Returns a new action that turns on the chamber roughing pump. Returns 0 if no valid turbo pump is given.
	AMAction3* turnOnChamberRoughingPump() { return 0; }
	/// Returns a new action that checks that the chamber roughing pump is on. Returns 0 if roughing pump is invalid.
	AMAction3* checkChamberRoughingPumpOn() { return 0; }

	/// Returns a new action that checks that the chamber door is closed. Returns 0 if no valid door control is found.
	AMAction3* checkDoorClosed() { return 0; }
	/// Returns a new action that checks that the chamber door is open. Returns 0 if no valid door control found.
	AMAction3* checkDoorOpen() { return 0; }

	/// Returns a new action that turns off the chamber turbo pumps. Returns 0 if turbos are invalid.
	AMAction3* turnOffChamberTurbos();
	/// Returns a new action that checks that the chamber turbo pumps are off. Returns 0 if the turbos are invalid.
	AMAction3* checkChamberTurbosOff();
	/// Returns a new action that turns on the chamber turbo pumps. Returns 0 if the turbos are invalid.
	AMAction3* turnOnChamberTurbos();
	/// Returns a new action that checks that the chamber turbo pumps are on. Returns 0 if the turbos are invlaid.
	AMAction3* checkChamberTurbosOn();

	/// Returns a new action that turns off the given turbo pump. Returns 0 if no valid turbo pump is given.
	AMAction3* turnOffTurbo(SGMTurboPump *turbo);
	/// Returns a new action that checks that the given turbo pump is off. Returns 0 if no valid turbo pump is given.
	AMAction3* checkTurboOff(SGMTurboPump *turbo);
	/// Returns a new action that turns on the given turbo pump. Returns 0 if no valid turbo pump is given.
	AMAction3* turnOnTurbo(SGMTurboPump *turbo);
	/// Returns a new action that checks that the given turbo pump is on. Returns 0 if no valid turbo pump is given.
	AMAction3* checkTurboOn(SGMTurboPump *turbo);

	/// Returns the specific action info for this class.
	const SGMSampleChamberVacuumMoveActionInfo* vacuumMoveInfo() const { return qobject_cast<const SGMSampleChamberVacuumMoveActionInfo*>(info()); }
	/// Returns the specific action info for this class.
	const SGMSampleChamberVacuumMoveActionInfo* vacuumMoveInfo() { return qobject_cast<SGMSampleChamberVacuumMoveActionInfo*>(info()); }

protected:
	/// The sample chamber vacuum.
	SGMSampleChamberVacuum *vacuum_;
};

#endif // SGMSAMPLECHAMBERVACUUMMOVEACTION_H
