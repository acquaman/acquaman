#ifndef AMSINGLEENUMERATEDCONTROL_H
#define AMSINGLEENUMERATEDCONTROL_H

#include "beamline/AMEnumeratedControl.h"

/*
  This is a control that is meant to map a single control's values into
  discrete enumerated value options. It implements the general AMEnumeratedControl
  interface.
  */
class AMSingleEnumeratedControl : public AMEnumeratedControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMSingleEnumeratedControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~AMSingleEnumeratedControl();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns a list of the indices for options with value ranges that contain the given setpoint.
	QList<int> indicesContaining(double setpoint) const;

protected slots:
	/// Sets the control used as the base for this control's values. Returns true if the control was set successfully, false otherwise.
	bool setBaseControl(AMControl *newControl);

	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the moving state.
	virtual void updateMoving();

	/// Adds an enum value option. Options added with duplicate indices will overwrite previous options.
	virtual bool addValueOption(int index, const QString &optionString, double optionSetpoint, double optionMin, double optionMax, bool readOnly);
	/// Adds an enum value option, assumes that the option is read-only.
	virtual bool addValueOption(int index, const QString &optionString, double optionMin, double optionMax);
	/// Adds an enum value option, assumes that the given setpoint is identical to the option min and max values.
	virtual bool addValueOption(int index, const QString &optionString, double optionSetpoint, bool readOnly);
	/// Removes an enum value option.
	virtual bool removeOption(int index);
	/// Clears all value options.
	virtual bool clearOptions();

protected:
	/// Returns a new action that moves the control to the setpoint that corresponds to the given index setpoint.
	virtual AMAction3* createMoveAction(double indexSetpoint);

	/// Returns the current index.
	virtual int currentIndex() const;

protected:
	/// The control being used as the base for this control's values.
	AMControl *control_;

	/// The mapping between an option's index value and its setpoint position.
	QMap<int, double> indexSetpointMap_;
	/// The mapping between an option's index value and its minimum position.
	QMap<int, double> indexMinimumMap_;
	/// The mapping between an option's index value and its maximum position.
	QMap<int, double> indexMaximumMap_;
};

#endif // AMSINGLEENUMERATEDCONTROL_H
