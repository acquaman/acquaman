#ifndef BIOXASMIRRORBENDCONTROL_H
#define BIOXASMIRRORBENDCONTROL_H

#include "beamline/BioXAS/BioXASMirrorControl.h"

class BioXASMirrorBendControl : public BioXASMirrorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorBendControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorBendControl();

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

	/// Returns the upstream bender motor control.
	AMControl* upstreamBenderControl() const { return upstreamBender_; }
	/// Returns the downstream bender motor control.
	AMControl* downstreamBenderControl() const { return downstreamBender_; }

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const;

signals:
	/// Notifier that the upstream bender control has changed.
	void upstreamBenderChanged(AMControl *newControl);
	/// Notifier that the downstream bender control has changed.
	void downstreamBenderChanged(AMControl *newControl);

public slots:
	/// Sets the upstream bender control.
	void setUpstreamBenderControl(AMControl *newControl);
	/// Sets the downstream bender control.
	void setDownstreamBenderControl(AMControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Calculates and returns the upstream bender value, for the given bend radius.
	virtual double calculateUpstreamBenderValue(double bendRadius) = 0;
	/// Calculates and returns the downstream bender value, for the given bend radius.
	virtual double calculateDownstreamBenderValue(double bendRadius) = 0;
	/// Calculates and returns the mirror bend radius, for the given bender values.
	virtual double calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue) = 0;

protected:
	/// The upstream bender control.
	AMControl *upstreamBender_;
	/// The downstream bender control.
	AMControl *downstreamBender_;
};

#endif // BIOXASMIRRORBENDCONTROL_H
