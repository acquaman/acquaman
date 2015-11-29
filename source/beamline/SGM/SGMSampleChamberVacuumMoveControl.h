#ifndef SGMSAMPLECHAMBERVACUUMMOVECONTROL_H
#define SGMSAMPLECHAMBERVACUUMMOVECONTROL_H

#include "beamline/AMEnumeratedControl.h"

#define SGMSAMPLECHAMBERVACUUMMOVECONTROL_TURBO_TIMEOUT 10
#define SGMSAMPLECHAMBERVACUUMMOVECONTROL_VATVALVE_TIMEOUT 10
#define SGMSAMPLECHAMBERVACUUMMOVECONTROL_INPUT_TIMEOUT 60

class AMControl;
class AMControlSet;
class SGMSampleChamberVacuum;
class SGMVATValve;
class SGMTurboPump;

class SGMSampleChamberVacuumMoveControl : public AMEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumeration of the possible move steps.
	enum Step { ClosingVATValve, OpeningVATValve, TurningOnRoughingPump, TurningOffRoughingPump, TurningOnTurbos, TurningOffTurbos };

	/// Constructor.
	SGMSampleChamberVacuumMoveControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveControl();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns the sample chamber vacuum control.
	SGMSampleChamberVacuum* vacuum() const { return vacuum_; }
	/// Returns the sample chamber door status control.
	AMControl* doorStatus() const { return doorStatus_; }
	/// Returns the sample chamber pressure gauge control.
	AMControl* pressure() const { return pressure_; }
	/// Returns the VAT valve.
	SGMVATValve* vatValve() const { return vatValve_; }
	/// Returns the sample chamber roughing pump control.
	AMControl* roughingPump() const { return roughingPump_; }
	/// Returns the sample chamber turbo pumps.
	AMControlSet* turbos() const { return turbos_; }

signals:
	/// Notifier that the vacuum control has changed.
	void vacuumChanged(SGMSampleChamberVacuum *newControl);
	/// Notifier that the door status control has changed.
	void doorStatusChanged(AMControl *newControl);
	/// Notifier that the pressure gauge control has changed.
	void pressureChanged(AMControl *newControl);
	/// Notifier that the chamber turbo pumps have changed.
	void turbosChanged(AMControlSet *newTurbos);
	/// Notifier that the VAT valve control has changed.
	void vatValveChanged(SGMVATValve *newControl);
	/// Notifier that the roughing pump control has changed.
	void roughingPumpChanged(AMControl *newControl);

public slots:
	/// Sets the sample chamber vacuum control.
	void setVacuum(SGMSampleChamberVacuum *newControl);
	/// Sets the sample chamber door status control.
	void setDoorStatus(AMControl *newControl);
	/// Sets the sample chamber pressure gauge control.
	void setPressure(AMControl *newControl);
	/// Sets the sample chamber turbo pumps.
	void setTurbos(AMControlSet *newControls);
	/// Sets the sample chamber roughing pump.
	void setRoughingPump(AMControl *newControl);
	/// Sets the sample chamber VAT valve control.
	void setVATValve(SGMVATValve *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Returns a new action that moves to the desired vacuum state.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Returns the current filter index.
	virtual int currentIndex() const;

	/// Creates and returns a new action that moves to the Vented state.
	AMAction3* createMoveToVentedAction();
	/// Creates and returns a new action that moves to the RoughVacuum state from Vented.
	AMAction3* createMoveToRoughVacuumFromVentedAction();
	/// Creates and return a new action that moves to the HighVacuum state from RoughVacuum.
	AMAction3* createMoveToHighVacuumFromRoughVacuumAction();
	/// Creates and returns a new action that moves to the HighVacuum state from Vented.
	AMAction3* createMoveToHighVacuumFromVentedAction();
	/// Creates and returns a new action that moves to the RoughVacuum state from HighVacuum.
	AMAction3* createMoveToRoughVacuumFromHighVacuumAction();

	/// Returns a new action that waits for the user to indicate they would like to proceed.
	AMAction3* waitForInput(const QString &instruction);

	/// Returns a new action that waits for the sample chamber vacuum to reach the given setpoint.
	AMAction3* waitForVacuum(double setpoint);

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

protected:
	/// The sample chamber vacuum.
	SGMSampleChamberVacuum *vacuum_;
	/// The sample chamber door status.
	AMControl *doorStatus_;
	/// The sample chamber pressure.
	AMControl *pressure_;
	/// The sample chamber turbo pumps.
	AMControlSet *turbos_;
	/// The sample chamber roughing pump.
	AMControl *roughingPump_;
	/// The sample chamber VAT valve control.
	SGMVATValve *vatValve_;
};

#endif // SGMSAMPLECHAMBERVACUUMMOVECONTROL_H
