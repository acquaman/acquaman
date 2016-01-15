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
	/// Enumeration of some default option indices.
	enum DefaultOptions { Unknown = -1, Invalid = -2 };

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

	/// Returns true if this control allows duplicate value option entries.
	bool allowsDuplicateOptions() const { return allowsDuplicateOptions_; }

	/// Returns a list of all indices.
	QList<int> indices() const { return indices_; }
	/// Returns a list of the indices for options with the given name.
	QList<int> indicesNamed(const QString &name) const;
	/// Returns true if there is an existing option index with the given name.
	bool hasIndexNamed(const QString &name) const;

signals:
	/// Notifier that whether this control allows duplicate value option entries has changed.
	void allowsDuplicationOptionsChanged(bool newStatus);

protected slots:
	/// Sets whether this control allows duplicate value option entries.
	void setAllowsDuplicateOptions(bool newStatus);

	/// Updates the states. Reimplemented to make sure the control min/max and the enumerated states are updated before the current value.
	virtual void updateStates();
	/// Updates the available options.
	virtual void updateOptions() { return; }
	/// Updates the enum and move enum states to reflect current value options.
	virtual void updateEnumStates();
	/// Updates the maximum value.
	virtual void updateMaximumValue();
	/// Updates the current value.
	virtual void updateValue();

	/// Adds an enum value option. Options added with duplicate indices will overwrite previous options.
	virtual bool addOption(int index, const QString &optionString, bool readOnly = false);
	/// Removes an enum value option.
	virtual bool removeOption(int index);
	/// Clears all value options.
	virtual bool clearOptions();

protected:
	/// Creates and returns a list of enum states, includes an "Unknown" state by default. Subclasses may reimplement if they need additional 'extra' states (ie not move states).
	virtual QStringList generateEnumStates() const;
	/// Creates and returns a list of move enum states for this control, based on the current options provided.
	virtual QStringList generateMoveEnumStates() const;

	/// Returns the current index. Subclasses must reimplement for their specific behavior/interaction.
	virtual int currentIndex() const = 0;

protected:
	/// The flag indicating whether this control will allow value options with the same string representation.
	bool allowsDuplicateOptions_;

	/// The list of option indices.
	QList<int> indices_;
	/// The mapping between an option's index value and its string representation.
	QMap<int, QString> indexStringMap_;
	/// The mapping between an option's index value and its read-only status.
	QMap<int, bool> indexReadOnlyStatusMap_;
};

#endif // AMENUMERATEDCONTROL_H
