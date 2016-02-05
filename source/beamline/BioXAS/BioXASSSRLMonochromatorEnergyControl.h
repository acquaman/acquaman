#ifndef BIOXASSSRLMONOCHROMATORENERGYCONTROL_H
#define BIOXASSSRLMONOCHROMATORENERGYCONTROL_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMActionSupport.h"

#include "beamline/BioXAS/BioXASMonochromatorEnergyControl.h"

class BioXASSSRLMonochromatorEnergyControl : public BioXASMonochromatorEnergyControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorEnergyControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEnergyControl();

	/// Returns true if the control value is always measureable, provided it is connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a control move is always possible, provided it is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a control stop is always possible, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return true; }
	/// Returns true if a calibration is always possible, provided this control is connected. False otherwise.
	virtual bool shouldCalibrate() const { return true; }

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop right now. False otherwise.
	virtual bool canStop() const;
	/// Returns true if this control can be calibrated right now. False otherwise.
	virtual bool canCalibrate() const;

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
	/// Returns the region control.
	AMControl* regionControl() const { return region_; }
	/// Returns the m1 mirror pitch control.
	AMControl* m1MirrorPitchControl() const { return m1MirrorPitch_; }

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
	void setBraggControl(CLSMAXvMotor *newControl);
	/// Sets the region control.
	void setRegionControl(AMControl *newControl);
	/// Sets the m1 mirror control.
	void setM1MirrorPitchControl(AMControl *newControl);

	/// Stops the control. Reimplemented to consider only a subset of children.
	virtual bool stop();

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);
	/// Creates and returns a calibration action.
	virtual AMAction3* createCalibrateAction(double oldEnergy, double newEnergy);

	/// Returns the bragg angle calculation result for region A, from the bragg motor position.
	static double calculateBraggAngleFromPositionRegionA(double braggPosition, double m1Pitch, double thetaBraggOffset, double regionOffset);
	/// Returns the bragg angle calculation result for region B, from the bragg motor position.
	static double calculateBraggAngleFromPositionRegionB(double braggPosition, double m1Pitch, double thetaBraggOffset);

	/// Returns the bragg position calculation result for region A, from the bragg angle.
	static double calculateBraggPositionFromAngleRegionA(double braggAngle, double m1Pitch, double thetaBraggOffset, double regionOffset);
	/// Returns the bragg position calculation result for region B, from the bragg angle.
	static double calculateBraggPositionFromAngleRegionB(double braggAngle, double m1Pitch, double thetaBraggOffset);

	/// Returns the bragg angle calculation result, from the energy.
	static double calculateBraggAngleFromEnergy(double hc, double crystal2D, double energy);
	/// Returns the energy calculation result, from the bragg angle.
	static double calculateEnergyFromBraggAngle(double hc, double crystal2D, double braggAngle);

	/// Returns the bragg position result for the given values.
	static double calculateBraggPositionFromEnergy(double hc, double crystal2D, double energy, double region, double m1Pitch, double thetaBraggOffset, double regionOffset);
	/// Returns the energy calculation result for the given values. Returns 0 if not in a valid region (can happen during normal operation).
	static double calculateEnergyFromBraggPosition(double hc, double crystal2D, double braggPosition, double region, double m1Pitch, double thetaBraggOffset, double regionOffset);

protected:
	/// The hc constant.
	double hc_;
	/// The crystal 2D constant.
	double crystal2D_;
	/// The theta offset to the bragg angle (deg).
	double thetaBraggOffset_;
	/// The region offset (deg).
	double regionOffset_;

	/// The goniometer motor control.
	CLSMAXvMotor *bragg_;
	/// The region control.
	AMControl *region_;
	/// The m1 mirror pitch control.
	AMControl *m1MirrorPitch_;
};

#endif // BIOXASSSRLMONOCHROMATORENERGYCONTROL_H
