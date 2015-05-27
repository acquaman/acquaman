#ifndef BIOXASMIRRORYAWCONTROL_H
#define BIOXASMIRRORYAWCONTROL_H

#include "beamline/BioXAS/BioXASMirrorControl.h"

class BioXASMirrorYawControl : public BioXASMirrorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorYawControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorYawControl();

	/// Returns true if the control value is always measureable, provided it is connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a control move is always possible, provided it is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a control stop is always possible, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return true; }

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop right now. False otherwise.
	virtual bool canStop() const;

	/// Returns the yaw motor control.
	AMControl* yawControl() const { return yaw_; }

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const;

signals:
	/// Notifier that the yaw motor control has changed.
	void yawControlChanged(AMControl *newControl);

public slots:
	/// Sets the yaw motor control.
	void setYawControl(AMControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action iteration.
	virtual AMAction3* createMoveActionIteration(double setpoint);

	/// Calculates and returns the yaw motor position for the given yaw and mirror lengths.
	static double calculateYawPosition(double yaw, double upstreamLength, double downstreamLength);
	/// Calculates and returns the yaw for the given yaw motor position and mirror lengths.
	static double calculateYaw(double upstreamLength, double downstreamLength, double yawPosition);

protected:
	/// The yaw motor control.
	AMControl *yaw_;
};

#endif // BIOXASMIRRORYAWCONTROL_H
