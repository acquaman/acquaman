#ifndef BIOXASSSRLMONOCHROMATORENERGYCONTROL_H
#define BIOXASSSRLMONOCHROMATORENERGYCONTROL_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "actions3/AMActionSupport.h"

#include "beamline/AMPseudoMotorControl.h"

class BioXASSSRLMonochromatorEnergyControl : public AMPseudoMotorControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorEnergyControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEnergyControl();

	/// Returns true if the control value is always measureable, provided it is connected. False otherwise.
	virtual bool shouldMeasure() { return true; }
	/// Returns true if a control move is always possible, provided it is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a control stop is always possible, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return false; }

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop right now. False otherwise.
	virtual bool canStop() const;

	/// Returns the hc constant.
	double hc() const { return hc_; }
	/// Returns the crystal 2D spacing.
	double crystal2D() const { return crystal2D_; }
	/// Returns the delta theta constant (energy calibration offset).
	double deltaTheta() const { return deltaTheta_; }
	/// Returns the theta offset to the bragg angle.
	double thetaBraggOffset() const { return thetaBraggOffset_; }

	/// Returns the bragg motor control.
	AMControl* braggControl() const { return bragg_; }
	/// Returns the region control.
	AMControl* regionControl() const { return region_; }

	/// Returns a new action that adjusts the angle offset s.t. the mono energy matches the desired energy.
	AMAction3* createSetEnergyCalibrationAction(double newEnergy);

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const { Q_UNUSED(value) return true; }
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const { return (value > 0); }

signals:
	/// Notifier that the bragg control has changed.
	void braggControlChanged(AMControl *newControl);
	/// Notifier that the region control has changed.
	void regionControlChanged(AMControl *newControl);

public slots:
	/// Sets the bragg control.
	void setBraggControl(AMControl *newControl);
	/// Sets the region control.
	void setRegionControl(AMControl *newControl);

	/// Sets the delta theta (energy calibration) offset such that the mono energy matches the desired energy.
	void setEnergyCalibration(double newEnergy);

protected slots:
	/// Sets the delta theta (energy calibration) offset.
	void setDeltaTheta(double newAngle);

	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateIsMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Returns the bragg motor angle offset needed in order for the old energy to match the desired new energy.
	double calibrateEnergy(double oldEnergy, double newEnergy) const;

	/// Returns the bragg angle calculation result for region A.
	static double calculateBraggAngleRegionA(double deltaTheta, double braggPosition, double m1AngleOffset, double thetaBraggOffset);
	/// Returns the bragg angle calculation result for region B.
	static double calculateBraggAngleRegionB(double deltaTheta, double braggPosition, double m1AngleOffset, double thetaBraggOffset);
	/// Returns the energy calculation result for the given values. Returns 0 if not in a valid region (can happen during normal operation).
	static double calculateEnergy(double hc, double crystal2D, double deltaTheta, double braggPosition, double region, double m1AngleOffset, double thetaBraggOffset);

protected:
	/// The hc constant.
	double hc_;
	/// The crystal 2D constant.
	double crystal2D_;
	/// The delta theta (energy calibration) offset.
	double deltaTheta_;
	/// The m1 angle offset.
	double m1AngleOffset_;
	/// The theta offset to the bragg angle.
	double thetaBraggOffset_;
	/// The goniometer bragg angle control.
	AMControl *bragg_;
	/// The region control.
	AMControl *region_;
};

#endif // BIOXASSSRLMONOCHROMATORENERGYCONTROL_H
