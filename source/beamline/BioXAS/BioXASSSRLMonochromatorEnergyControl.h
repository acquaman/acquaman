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
	/// Returns the theta offset to the bragg angle.
	double thetaBraggOffset() const { return thetaBraggOffset_; }
	/// Returns the region offset.
	double regionOffset() const { return regionOffset_; }

	/// Returns the bragg motor control.
	AMControl* braggControl() const { return bragg_; }
	/// Returns the bragg motor set position control.
	AMControl* braggSetPositionControl() const { return braggSetPosition_; }
	/// Returns the region control.
	AMControl* regionControl() const { return region_; }
	/// Returns the m1 mirror control.
	AMControl* m1MirrorControl() const { return m1Mirror_; }

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const { Q_UNUSED(value) return true; }
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const { return (value > 0); }

signals:
	/// Notifier that the bragg control has changed.
	void braggControlChanged(AMControl *newControl);
	/// Notifier that the bragg set position control has changed.
	void braggSetPositionControlChanged(AMControl *newControl);
	/// Notifier that the region control has changed.
	void regionControlChanged(AMControl *newControl);
	/// Notifier that the m1 mirror control has changed.
	void m1MirrorControlChanged(AMControl *newControl);

public slots:
	/// Sets the bragg control.
	void setBraggControl(AMControl *newControl);
	/// Sets the bragg set position control.
	void setBraggSetPositionControl(AMControl *newControl);
	/// Sets the region control.
	void setRegionControl(AMControl *newControl);
	/// Sets the m1 mirror control.
	void setM1MirrorControl(AMControl *newControl);

	/// Calibrates the control such that the newEnergy is the current energy.
	void setEnergy(double newEnergy);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateIsMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Returns the bragg angle calculation result for region A, from the bragg motor position.
	static double calculateBraggAngleFromPositionRegionA(double braggPosition, double m1AngleOffset, double thetaBraggOffset, double regionOffset);
	/// Returns the bragg angle calculation result for region B, from the bragg motor position.
	static double calculateBraggAngleFromPositionRegionB(double braggPosition, double m1AngleOffset, double thetaBraggOffset);

	/// Returns the bragg position calculation result for region A, from the bragg angle.
	static double calculateBraggPositionFromAngleRegionA(double braggAngle, double m1AngleOffset, double thetaBraggOffset, double regionOffset);
	/// Returns the bragg position calculation result for region B, from the bragg angle.
	static double calculateBraggPositionFromAngleRegionB(double braggAngle, double m1AngleOffset, double thetaBraggOffset);

	/// Returns the bragg angle calculation result, from the energy.
	static double calculateBraggAngleFromEnergy(double hc, double crystal2D, double energy);
	/// Returns the energy calculation result, from the bragg angle.
	static double calculateEnergyFromBraggAngle(double hc, double crystal2D, double braggAngle);

	/// Returns the bragg position result for the given values.
	static double calculateBraggPositionFromEnergy(double hc, double crystal2D, double energy, double region, double m1AngleOffset, double thetaBraggOffset, double regionOffset);
	/// Returns the energy calculation result for the given values. Returns 0 if not in a valid region (can happen during normal operation).
	static double calculateEnergyFromBraggPosition(double hc, double crystal2D, double braggPosition, double region, double m1AngleOffset, double thetaBraggOffset, double regionOffset);

protected:
	/// The hc constant.
	double hc_;
	/// The crystal 2D constant.
	double crystal2D_;
	/// The theta offset to the bragg angle (deg).
	double thetaBraggOffset_;
	/// The region offset (deg).
	double regionOffset_;

	/// The goniometer bragg motor control.
	AMControl *bragg_;
	/// The goniometer bragg motor, set position control.
	AMControl *braggSetPosition_;
	/// The region control.
	AMControl *region_;
	/// The m1 mirror control.
	AMControl *m1Mirror_;
};

#endif // BIOXASSSRLMONOCHROMATORENERGYCONTROL_H
