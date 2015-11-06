#ifndef AMENUMERATEDCONTROL_H
#define AMENUMERATEDCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

/*
  This is a control that is meant to map a single control's 'continuous' values into
  discrete enumerated value options.

  Subclasses are responsible for providing a means of tracking value option info
  (eg a list of options) and are required to provide a way of updating this control's
  current value, creating move actions, and generating enum states.
  */
class AMEnumeratedControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMEnumeratedControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~AMEnumeratedControl();

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

	/// Returns a list of the indices for options with the given name.
	QList<int> indicesNamed(const QString &name) const;
	/// Returns a list of the indices for options with value ranges that contain the given setpoint.
	QList<int> indicesContaining(double setpoint) const;

signals:
	/// Notifier that the 'continuous' control has changed.
	void controlChanged(AMControl *newControl);
	/// Notifier that the value options have changed.
	void optionsChanged();

public slots:
	/// Sets the 'continuous' control.
	void setControl(AMControl *newControl);

	/// Adds an enum value option. Options added with duplicate indices will overwrite previous options.
	virtual void addOption(int index, const QString &optionString, double optionSetpoint, double optionMin, double optionMax);
	/// Adds an enum value option, assumes that the given setpoint is identical to the option min and max values.
	virtual void addOption(int index, const QString &optionString, double optionSetpoint);
	/// Removes an enum value option.
	virtual void removeOption(int index);
	/// Clears all value options.
	virtual void clearOptions();

protected slots:
	/// Updates the states. Reimplemented to make sure the control min/max and the enumerated states are updated before the current value.
	virtual void updateStates();
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the enum and move enum states.
	virtual void updateEnumStates();
	/// Updates the maximum value.
	virtual void updateMaximumValue();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Returns a new action that moves the control to the setpoint that corresponds to the given index setpoint.
	virtual AMAction3* createMoveAction(double indexSetpoint);

	/// Creates and returns a list of enum states, includes an "Unknown" state by default. Subclasses may reimplement if they need additional 'extra' states (ie not move states).
	virtual QStringList generateEnumStates() const;
	/// Creates and returns a list of move enum states for this control, based on the current options provided.
	virtual QStringList generateMoveEnumStates() const;

protected:
	/// The 'continuous' control, the control being used as the base for this control's values.
	AMControl *control_;

	/// The list of option indices.
	QList<int> indices_;
	/// The mapping between an option's index value and its string representation.
	QMap<int, QString> indexStringMap_;
	/// The mapping between an option's index value and its setpoint position.
	QMap<int, double> indexSetpointMap_;
	/// The mapping between an option's index value and its minimum position.
	QMap<int, double> indexMinimumMap_;
	/// The mapping between an option's index value and its maximum position.
	QMap<int, double> indexMaximumMap_;
};

#endif // AMENUMERATEDCONTROL_H
