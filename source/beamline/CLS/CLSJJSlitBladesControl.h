#ifndef CLSJJSLITBLADESCONTROL_H
#define CLSJJSLITBLADESCONTROL_H

#include <QSignalMapper>

#include "beamline/AMPseudoMotorControl.h"
#include "actions3/AMAction3.h"

// Values.

#define CLSJJSLITBLADESCONTROL_VALUE_MIN -11
#define CLSJJSLITBLADESCONTROL_VALUE_MAX 11

class CLSJJSlitBladesControl : public AMPseudoMotorControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitBladesControl(const QString &name, AMControl *firstMotorControl, AMControl *secondMotorControl, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlitBladesControl();

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

	/// Returns the gap.
	double gap() const { return gap_; }
	/// Returns the center position.
	double centerPosition() const { return centerPosition_; }

	/// Returns the upper blade motor.
	AMControl* upperBladeControl() const { return upperBladeControl_; }
	/// Returns the lower blade motor.
	AMControl* lowerBladeControl() const { return lowerBladeControl_; }

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const;

signals:
	/// Notifier that the gap has changed.
	void gapChanged(double newValue);
	/// Notifier that the center position has changed.
	void centerPositionChanged(double newValue);
	/// Notifier that the upper blade motor control has changed.
	void upperBladeControlChanged(AMControl *newControl);
	/// Notifier that the lower blade motor control has changed.
	void lowerBladeControlChanged(AMControl *newControl);

public slots:
	/// Sets the upper blade motor control.
	void setUpperBladeControl(AMControl *newControl);
	/// Sets the lower blade motor control.
	void setLowerBladeControl(AMControl *newControl);

protected slots:
	/// Sets the gap value.
	void setGap(double newValue);
	/// Sets the center position value.
	void setCenterPosition(double newValue);

	/// Handles updating the control's states.
	virtual void updateStates();
	/// Handles updating the control's connected state.
	virtual void updateConnected();
	/// Handles updating the control's value. Subclasses should reimplement this.
	virtual void updateValue() { return; }
	/// Handles updating the control's 'is moving' state.
	virtual void updateMoving();

	/// Handles updating the saved gap value, according to the current motor control values.
	void updateGap();
	/// Handles updating the saved center position value, according to the current motor control values.
	void updateCenterPosition();

protected:
	/// Creates and returns a move action. Returns 0 if not connected. Subclasses must implement this.
	virtual AMAction3* createMoveAction(double setpoint) = 0;

	/// Calculate the upper blade motor control's position from the given gap and center position.
	static double calculateUpperPosition(double gap, double centerPosition);
	/// Calculate the lower blade motor control's position from the given gap and center position.
	static double calculateLowerPosition(double gap, double centerPosition);

	/// Calculate the gap from the given motor positions.
	static double calculateGap(double upperBladePosition, double lowerBladePosition);
	/// Calculate the center position from the given motor positions.
	static double calculateCenterPosition(double upperBladePosition, double lowerBladePosition);

protected:
	/// The current gap value, centered about the center position.
	double gap_;
	/// The current center position, about which the gap is centered.
	double centerPosition_;

	/// The upper blade motor control.
	AMControl *upperBladeControl_;
	/// The lower blade motor control.
	AMControl *lowerBladeControl_;
};

#endif // CLSJJSLITBLADESCONTROL_H
