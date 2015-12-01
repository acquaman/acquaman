#ifndef SGMBEAMCOORDINATORCONTROL_H
#define SGMBEAMCOORDINATORCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

class AMControl;
class AMControlSet;
class SGMBeamCoordinatorControl : public AMPseudoMotorControl
{
	Q_OBJECT
public:
	SGMBeamCoordinatorControl(QObject* parent = 0);

	/// Returns true if this control can measure its value right now. False otherwise.
	bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	bool canStop() const;

protected slots:
	/// Updates the connected state.
	void updateConnected();
	/// Updates the current value.
	void updateValue();

	/// Handles emitting the appropriate signals when a move action has started.
	virtual void onMoveStarted(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when a move action is cancelled.
	virtual void onMoveCancelled(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when a move action fails.
	virtual void onMoveFailed(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when a move action succeeds.
	virtual void onMoveSucceeded(QObject *action);

protected:
	/// Creates the actions required to move to the provided beam on setpoint
	AMAction3* createMoveAction(double setpoint);

	AMControl* fastShutterVoltage_;

	AMControlSet* valveControls_;

	AMControl* frontBypassValve_;
	AMControl* backBypassValve_;

	AMControl *vvr1611_3_I10_01Shutter_;
	AMControl *vvr1611_3_I10_02Shutter_;
	AMControl *vvr1611_3_I10_03Shutter_;
	AMControl *vvr1611_3_I10_04Shutter_;
	AMControl *psh1611_3_I10_01Shutter_;
	AMControl *vvr1611_4_I10_01Shutter_;
	AMControl *vvr1611_4_I10_02Shutter_;
	AMControl *vvr1611_4_I10_03Shutter_;
	AMControl *vvr1611_4_I10_04Shutter_;
	AMControl *vvr1611_4_I10_05Shutter_;
	AMControl *vvr1611_4_I10_06Shutter_;
	AMControl *vvr1611_4_I10_07Shutter_;
	AMControl *vvr1611_4_I10_08Shutter_;
	AMControl *vvr1611_4_I10_11Shutter_;
	AMControl *psh1411_I00_01Shutter_;
	AMControl *vvr1411_I00_01Shutter_;
	AMControl *vvf1411_I00_01Shutter_;
	AMControl *psh1411_I00_02Shutter_;
	AMControl *ssh1411_I00_01Shutter_;
	AMControl *vvr1611_3_I00_01Shutter_;
	AMControl *vvr1611_3_I00_02Shutter_;

};

#endif // SGMBEAMCOORDINATORCONTROL_H
