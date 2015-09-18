#ifndef CLSJJSLITS_H
#define CLSJJSLITS_H

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "beamline/AMPseudoMotorControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSJJSlitGapControl.h"
#include "beamline/CLS/CLSJJSlitCenterControl.h"
#include "dataman/AMAnalysisBlock.h"

class CLSJJSlits : public AMPseudoMotorControl
{
	Q_OBJECT

public:
	/// Enum describing different direction options.
	class Direction { public: enum Option { Invalid = 0, Vertical, Horizontal, Upper, Lower, Inboard, Outboard}; };
	/// Enum describing different control options.
	class Control { public: enum Option { Invalid = 0, Gap, Center, Blade }; };

	/// Constructor
	explicit CLSJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject*parent = 0, AMDetectorSet *optimizationDetectors = 0);
	/// Destructor
	virtual ~CLSJJSlits();

	/// Returns the current value.
	virtual double value(Direction::Option directionOption, Control::Option controlOption) const;
	/// Returns the current setpoint.
	virtual double setpoint(Direction::Option directionOption, Control::Option controlOption) const;
	/// Returns the smallest value this control can take.
	virtual double minimumValue(Direction::Option directionOption, Control::Option controlOption) const;
	/// Returns the largest value this control can take.
	virtual double maximumValue(Direction::Option directionOption, Control::Option controlOption) const;

	/// Returns true if the control with the given options is always measurable, provided the control is connected.
	virtual bool shouldMeasure(Direction::Option directionOption, Control::Option controlOption) const;
	/// Returns true if a move for the control with the given options is always possible, provided control is connected.
	virtual bool shouldMove(Direction::Option directionOption, Control::Option controlOption) const;
	/// Returns true if the control with the given options can stop, provided it is connected.
	virtual bool shouldStop(Direction::Option directionOption, Control::Option controlOption) const;

	/// Returns true if the controls with the given options are always measurable, provided the controls are connected.
	virtual bool shouldMeasure(Direction::Option directionOption) const;
	/// Returns true if moves for the controls with the given options are always possible, provided controls are connected.
	virtual bool shouldMove(Direction::Option directionOption) const;
	/// Returns true if the controls with the given options can stop, provided controls are connected.
	virtual bool shouldStop(Direction::Option directionOption) const;

	/// Returns true if the controls with the given options are always measurable, provided the controls are connected.
	virtual bool shouldMeasure(Control::Option controlOption) const;
	/// Returns true if moves for the controls with the given options are always possible, provided controls are connected.
	virtual bool shouldMove(Control::Option controlOption) const;
	/// Returns true if the controls with the given options can stop, provided controls are connected.
	virtual bool shouldStop(Control::Option controlOption) const;

	/// Returns true if all controls are always measurable, provided the controls are connected.
	virtual bool shouldMeasure() const;
	/// Returns true if moves for all controls are always possible, provided controls are connected.
	virtual bool shouldMove() const;
	/// Returns true if all controls can stop, provided controls are connected.
	virtual bool shouldStop() const;

	/// Returns true if the control with the given options can take a measurement right now.
	virtual bool canMeasure(Direction::Option directionOption, Control::Option controlOption) const;
	/// Returns true if the control with the given options can move right now.
	virtual bool canMove(Direction::Option directionOption, Control::Option controlOption) const;
	/// Returns true if the control with the given options can stop a change to a new region right now.
	virtual bool canStop(Direction::Option directionOption, Control::Option controlOption) const;

	/// Returns true if the control with the given options can take a measurement right now.
	virtual bool canMeasure(Direction::Option directionOption) const;
	/// Returns true if the control with the given options can move right now.
	virtual bool canMove(Direction::Option directionOption) const;
	/// Returns true if the control with the given options can stop a change to a new region right now.
	virtual bool canStop(Direction::Option directionOption) const;

	/// Returns true if the control with the given options can take a measurement right now.
	virtual bool canMeasure(Control::Option controlOption) const;
	/// Returns true if the control with the given options can move right now.
	virtual bool canMove(Control::Option controlOption) const;
	/// Returns true if the control with the given options can stop a change to a new region right now.
	virtual bool canStop(Control::Option controlOption) const;

	/// Returns true if all controls can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if all controls can move right now.
	virtual bool canMove() const;
	/// Returns true if all controls can stop a change to a new region right now.
	virtual bool canStop() const;

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(Direction::Option directionOption, Control::Option controlOption, double value) const;
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(Direction::Option directionOption, Control::Option controlOption, double setpoint) const;

	/// Returns the control with the given properties.
	AMControl* control(CLSJJSlits::Direction::Option directionOption, CLSJJSlits::Control::Option controlOption) const;
	/// Returns the controls with the given direction option.
	QList<AMControl*> controls(CLSJJSlits::Direction::Option control) const;
	/// Returns the controls with the given control option.
	QList<AMControl*> controls(CLSJJSlits::Control::Option control) const;
	/// Returns all controls.
	QList<AMControl*> controls() const;

	/// Returns the upper blade motor control.
	AMControl* upperBladeControl() const { return upperBlade_; }
	/// Returns the lower blade motor control.
	AMControl* lowerBladeControl() const { return lowerBlade_; }
	/// Returns the inboard blade motor control.
	AMControl* inboardBladeControl() const { return inboardBlade_; }
	/// Returns the outboard blade motor control.
	AMControl* outboardBladeControl() const { return outboardBlade_; }

	/// Returns the vertical blades gap control.
	AMControl* verticalGapControl() const { return verticalGap_; }
	/// Returns the vertical blades center control.
	AMControl* verticalCenterControl() const { return verticalCenter_; }
	/// Returns the horizontal blades gap control.
	AMControl* horizontalGapControl() const { return horizontalGap_; }
	/// Returns the horizontal blades center control.
	AMControl* horizontalCenterControl() const { return horizontalCenter_; }

	/// Returns the optimization configuration.
	AMGenericStepScanConfiguration* optimizationConfiguration() const { return optimizationConfiguration_; }

signals:
	/// Notifier that the upper blade control has changed.
	void upperBladeControlChanged(AMControl *newControl);
	/// Notifier that the lower blade control has changed.
	void lowerBladeControlChanged(AMControl *newControl);
	/// Notifier that the inboard blade control has changed.
	void inboardBladeControlChanged(AMControl *newControl);
	/// Notifier that the outboard blade control has changed.
	void outboardBladeControlChanged(AMControl *newControl);

public slots:
	/// Sets the setpoint and moves the controls with the given options, if necessary.
	virtual FailureExplanation move(Direction::Option direction, Control::Option control, double setpoint);

	/// Sets the upper blade control.
	void setUpperBladeControl(AMControl *upperBlade);
	/// Sets the lower blade control.
	void setLowerBladeControl(AMControl *lowerBlade);
	/// Sets the inboard blade control.
	void setInboardBladeControl(AMControl *inboardBlade);
	/// Sets the outboard blade control.
	void setOutboardBladeControl(AMControl *outboardBlade);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value--not used for this control.
	virtual void updateValue() { return; }
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }
	/// Creates and returns an optimization action.
	virtual AMAction3* createOptimizationAction() { return 0; }

	/// Updates the vertical gap control with the latest blade controls.
	void updateVerticalGapControl();
	/// Updates the vertical center control with the latest blade controls.
	void updateVerticalCenterControl();
	/// Updates the horizontal gap control with the latest blade controls.
	void updateHorizontalGapControl();
	/// Updates the horizontal center control with the latest blade controls.
	void updateHorizontalCenterControl();

	/// Updates the vertical gap and center, and the horizontal gap and center controls.
	void updatePseudoMotorControls();

	/// Creates and returns a blade control, using the given pv name base.
	virtual AMPVwStatusControl* createBladeControl(const QString &name, const QString &baseName);
	/// Sets up the blade controls and updates the pseudomotor controls, using the given pv base names.
	virtual void setupBladeControls(const QString &upperBladeBaseName, const QString &lowerBladeBaseName, const QString &inboardBladeBaseName, const QString &outboardBladeBaseName);

	/// Sets up a default optimization configuration.
	void setupDefaultOptimizationConfiguration(AMGenericStepScanConfiguration *configuration);

protected:
	/// Upper blade motor control.
	AMControl *upperBlade_;
	/// Lower blade motor control.
	AMControl *lowerBlade_;
	/// Inboard blade motor control.
	AMControl *inboardBlade_;
	/// Outboard blade motor control.
	AMControl *outboardBlade_;

	/// Vertical blades gap control.
	CLSJJSlitGapControl *verticalGap_;
	/// Vertical blades center control.
	CLSJJSlitCenterControl *verticalCenter_;
	/// Horizontal blades gap control.
	CLSJJSlitGapControl *horizontalGap_;
	/// Horizontal blades center control.
	CLSJJSlitCenterControl *horizontalCenter_;

	/// The optimization scan configuration.
	AMGenericStepScanConfiguration *optimizationConfiguration_;
};

#endif // CLSJJSLITS_H
