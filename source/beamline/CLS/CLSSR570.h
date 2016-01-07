#ifndef CLSSR570_H
#define CLSSR570_H

#include "beamline/AMCurrentAmplifier.h"

class AMControl;

class CLSSR570 : public AMCurrentAmplifier
{
Q_OBJECT
public:
	/// Constructor takes a user name and a PV baseName (ie,, amp1608-1001)
	CLSSR570(const QString &name, const QString &baseName, QObject *parent = 0);

	/// Destructor
	virtual ~CLSSR570();

	/// Returns the current sensitivity level
	int sensitivityLevel() const;
	/// Returns the setPoint value of sens_num.
	virtual double sensNumSetpoint() const;
	/// Returns the value.
	virtual double value() const;
	/// Returns a list of the available value options.
	virtual QList<double> values() const;

	/// Returns the setPoint value of sens_unit.
	virtual double sensUnitSetpoint() const;
	/// Returns the current units.
	virtual QString units() const;
	/// Returns a string list of the available units options, suitable for a view to display.
	virtual QStringList unitsList() const;

	/// Returns the minimum value from the units pair for the given amplifier.
	virtual double minimumValueForUnits(const QString &units) const;
	/// Returns the maximum value from the given units for the given amplifier.
	virtual double maximumValueForUnits(const QString &units) const;

	/// Increases the sensitivity of the current amplifier.
	virtual bool increaseSensitivity();
	/// Decreases the sensitivity of the current amplifier.
	virtual bool decreaseSensitivity();

protected slots:
	/// Handles changes from the sensitivity control to emit valueChanged() and watch min/max sensitivity for the AMCurrentAmplifier API
	void onSensitivityControlValueChanged(double value);
	/// Handles changes in the connected state of the control, relays to AMCurrentAmplifier API
	void onSensitivityControlConnectedChanged(bool connected);

protected:
	/// Returns whether the current amplifier is at minimum sensitivity.
	virtual bool atMinimumSensitivity() const;
	/// Returns whether the current amplifier is at maximum sensitivity.
	virtual bool atMaximumSensitivity() const;


	/// Sets the current amplifier's sensitivity/gain value.
	virtual void setValueImplementation(const QString &valueArg);

	/// Helper to figure out the value (ie,, 1, 50, 200) from the sensitivity index
	double currentValueHelper() const;
	/// Helper to figure out the unit (ie,, uA/V) from the sensitivity index
	QString currentUnitHelper() const;

protected:
	/// Direct access to the sensitivity control (PV) of the SR570
	AMControl *sensitivityControl_;

	bool initialized_;
	/// Holds a latch for when we're at maximum sensitivity
	bool atMaximumSensitivity_;
	/// Holds a latch for when we're at minimum sensitivity
	bool atMinimumSensitivity_;
};

#endif // CLSSR570_H
