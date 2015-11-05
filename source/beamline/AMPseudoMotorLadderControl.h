#ifndef AMPSEUDOMOTORLADDERCONTROL_H
#define AMPSEUDOMOTORLADDERCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

/*
  This is a control that is meant to map a single control's 'continuous' values into
  discrete enumerated value options.

  Subclasses are responsible for providing a means of tracking value option info
  (eg a list of options) and are required to provide a way of updating this control's
  current value, creating move actions, and generating enum states.
  */
class AMPseudoMotorLadderControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMPseudoMotorLadderControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~AMPseudoMotorLadderControl();

	/// Returns true if this control is always measurable, if connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, if connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, if connected. False otherwise.
	virtual bool shouldStop() const { return true; }

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns true if the given value corresponds to a valid window, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value corresponds to a valid window setpoint, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns the 'continuous' control, the control being used as the base for this control's values.
	AMControl* control() const { return control_; }

	/// Returns the index of the option with the given name.
	int indexOf(const QString &name) const;

signals:
	/// Notifier that the 'continuous' control has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Sets the 'continuous' control.
	void setControl(AMControl *newControl);

protected slots:
	/// Updates the states. Reimplemented to make sure the control min/max and the enumerated states are updated before the current value.
	virtual void updateStates();
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the enum and move enum states.
	virtual void updateEnumStates();
	/// Updates the maximum value.
	virtual void updateMaximumValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Creates and returns a list of enum states for this control.
	virtual QStringList generateEnumStates() const = 0;
	/// Creates and returns a list of move enum states for this control.
	virtual QStringList generateMoveEnumStates() const = 0;

protected:
	/// The 'continuous' control, the control being used as the base for this control's values.
	AMControl *control_;
};

#endif // AMPSEUDOMOTORLADDERCONTROL_H
