#ifndef AMENUMERATEDCONTROL_H
#define AMENUMERATEDCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

/*
  This is a control that manages different enumerated value options.
  */
class AMEnumeratedControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Enumeration of different default values.
	enum DefaultValue { Unknown = -1 };
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

	/// Returns true if the given value corresponds to a valid window, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value corresponds to a valid window setpoint, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns true if this control is enumerated, false otherwise. Always true for this control.
	virtual bool isEnum() const { return true; }
	/// Returns the list of enum names.
	virtual QStringList enumNames() const { return indexStringMap_.values(); }
	/// Returns the string representation of the given enumeration value.
	virtual QString enumNameAt(double controlValue) const { return indexStringMap_.value(controlValue, QString()); }
	/// Returns the list of enum names that are also move destinations, a subset of the enum names.
	virtual QStringList moveEnumNames() const;

	/// Returns true if this control allows duplicate value option entries.
	bool allowsDuplicateOptions() const { return allowsDuplicateOptions_; }

	/// Returns a list of the current indices.
	QList<double> indices() const { return indices_; }
	/// Returns a list of the indices for options with the given name.
	QList<double> indicesNamed(const QString &name) const;
	/// Returns true if there is an existing option index with the given name.
	bool hasIndexNamed(const QString &name) const;

signals:
	/// Notifier that whether this control allows duplicate value option entries has changed.
	void allowsDuplicationOptionsChanged(bool newStatus);

protected slots:
	/// Sets whether this control allows duplicate value option entries.
	void setAllowsDuplicateOptions(bool newStatus);

	/// Updates the states. Reimplemented to make sure the control min/max and the enumerated options are updated before the current value.
	virtual void updateStates();
	/// Updates the enumeration options. Subclasses can reimplement if they need to dynamically modify their enumeration options.
	virtual void updateOptions() { return; }

	/// Adds an enum value option. Options added with duplicate indices will overwrite previous options.
	virtual bool addOption(double index, const QString &optionString);
	/// Removes an enum value option.
	virtual bool removeOption(double index);
	/// Clears all value options.
	virtual bool clearOptions();

	/// Adds an enum value option, assuming that the option is a move destination.
	virtual bool addMoveOption(double index, const QString &optionString);

protected:
	/// Returns the current minimum value. Subclasses can reimplement for their specific behavior/interaction, if their minimum value is a function of other controls.
	virtual double currentMinimumValue() const;
	/// Returns the current maximum value. Subclasses can reimplement for their specific behavior/interaction, if their maximum value is a function of other controls.
	virtual double currentMaximumValue() const;

protected:
	/// The flag indicating whether this control will allow value options with the same string representation.
	bool allowsDuplicateOptions_;

	/// The list of all option indices.
	QList<double> indices_;
	/// The list of move option indices, a subset of all option indices.
	QList<double> moveIndices_;
	/// The mapping between an option's index value and its string representation.
	QMap<double, QString> indexStringMap_;
};

#endif // AMENUMERATEDCONTROL_H
