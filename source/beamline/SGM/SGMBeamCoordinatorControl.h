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
	/// Updates the current value.
	void updateValue();

	/// Ensures the value is updated when the movement fails
	void onMoveFailed(QObject *action);

	/// Ensures the value is updated when the movement succeeds
	void onMoveSucceeded(QObject *action);

protected:
	/// Creates the actions required to move to the provided beam on setpoint
	AMAction3* createMoveAction(double setpoint);

	AMControl* fastShutterVoltage_;

	QList<AMControl*> controlsToOpen_;
	QList<AMControl*> controlsToWait_;

	AMControl* frontBypassValve_;
	AMControl* backBypassValve_;

	AMControl *vvr1611_3_I10_01Valve_;
	AMControl *vvr1611_3_I10_02Valve_;
	AMControl *vvr1611_3_I10_03Valve_;
	AMControl *vvr1611_3_I10_04Valve_;
	AMControl *psh1611_3_I10_01Shutter_;
	AMControl *vvr1611_4_I10_01Valve_;
	AMControl *vvr1611_4_I10_02Valve_;
	AMControl *vvr1611_4_I10_03Valve_;
	AMControl *vvr1611_4_I10_04Valve_;
	AMControl *vvr1611_4_I10_05Valve_;
	AMControl *vvr1611_4_I10_06Valve_;
	AMControl *vvr1611_4_I10_07Valve_;
	AMControl *vvr1611_4_I10_08Valve_;
	AMControl *vvr1611_4_I10_11Valve_;
	AMControl *vvr1611_3_I00_01Valve_;
	AMControl *vvr1611_3_I00_02Valve_;

	AMControl *psh1411_I00_01Shutter_;
	AMControl *vvr1411_I00_01Valve_;
	AMControl *vvf1411_I00_01_;
	AMControl *safetyShutter_;
	AMControl *psh_2_;

	AMControl* es2Bypass_;

};

#endif // SGMBEAMCOORDINATORCONTROL_H
