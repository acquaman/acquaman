/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef CLSSR570_H
#define CLSSR570_H

#include "beamline/AMCurrentAmplifier.h"

#include "beamline/AMPVControl.h"

/*!
  This class encapuslats the SR570's that are used at the CLS for gain sensitivity control.  They have the ability for
  setting the value and units to a reasonably high level.  It allows customizability in value: 1, 2, 5, 10, 20, 50, 100,
  200, and 500, and in units: pA/V, nA/V, uA/V, mA/V.

  As an implementation detail, the implementation always uses two PVs; one for the value and one for the units.
  */
class CLSSR570 : public AMCurrentAmplifier
{
	Q_OBJECT

public:
	/// Constructor.  Builds a SR570 model based on \param valueName and \param unitsName.
	explicit CLSSR570(const QString &name, const QString &valueName, const QString &unitsName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSSR570();

	/// Returns the value of the sensitivity (ie: 1, 2, 5, 10, 20, 50, 100, 200, 500).
	virtual double value() const { return indexToValue(value_->getInt()); }
	/// Returns the value of the sensitivity at the given index.
	int valueAt(int index) const { return indexToValue(index); }
	/// Returns the index of the sensitivity (value between 0 and 8).
	int valueIndex() const { return value_->getInt(); }
	/// Returns a string list of the available value options, suitable for a view to display.
	virtual QList<double> values() const;

	/// Returns the units of the sensitivity.
	virtual QString units() const { return units_->getString(); }
	/// Returns a string list of the available units options, suitable for a view to display.
	virtual QStringList unitsList() const;

    /// Returns the minimum/maximum value + units pair for this amplifier.
    virtual double minimumValueForUnits(const QString &units) const;
    virtual double maximumValueForUnits(const QString &units) const;

signals:
	/// General notifier. Emitted when the sensitivity of the current amplifier is changed.
	void sensitivityChanged(int valueIndex);
	/// Notifier that the current amplifier is at the minimum sensitivity.
	void minimumSensitivity(bool);
	/// Notifier that the current amplifier is at the maximum sensitivity.
	void maximumSensitivity(bool);
	/// Emitted when the amplifier's units have changed, passes new units.
	void unitsChanged(const QString &newUnits);

protected slots:
	/// Sets the sensitivity value. Must be 1, 2, 5, 10, 20, 50, 100, 200, or 500.  Does nothing otherwise.
	void setValueControl(int newValue);
	/// Sets the value index.  Must be between 0 and 8.
	void setValueIndex(int index);
	/// Sets the sensitivity units.  Must be pA/V, nA/V, uA/V, or mA/V.  Does nothing otherwise.
	void setUnits(QString units);
	/// Turns the sensitivity value from an index to the value.
	void onValueChanged(int index);
	/// Determines whether the new state of the ion chamber is at either the minimum or maximum sensitivity.
	void onSensitivityChanged();
	void onUnitsChanged(const QString &newUnits);
	/// Helper function that determines if the SR570 is connected.
	void onConnectedChanged();

protected:
	/// Determines if the new sensitivity value is acceptable.
	bool valueOkay(int value) const;
	/// Determines if the new sensitivity units are acceptable.
	bool unitsOkay(QString units) const;

	/// Returns whether the ion chamber is at maximum senstivity.
	virtual bool atMaximumSensitivity() const { return atMaximumSensitivity_; }
	/// Returns whether the ion chamber is at minimum sensitivity.
	virtual bool atMinimumSensitivity() const { return atMinimumSensitivity_; }

	/// Increases the sensitivity of the ion chamber by one step.
	virtual bool increaseSensitivity();
	/// Decreases the sensitivity of the ion chamber by one step.
	virtual bool decreaseSensitivity();

	/// Sets the sensitivity value.
	virtual void setValueImplementation(const QString &valueArg);

	/// Pointer to the value PV.  This is a specific implementation for the CLS and maybe should be migrated to an AMControl at the same time the units PV is migrated to an AMControl.
	AMProcessVariable *value_;
	/// Pointer to the units PV.  This can be migrated to an AMControl once it has string functionality.
	AMProcessVariable *units_;

	/// Holds the state of whether the ion chamber is at its maximum sensitivity.
	bool atMaximumSensitivity_;
	/// Holds the state of whether the ion chamber is at its minimum sensitivity.
	bool atMinimumSensitivity_;

private:
    /// Helper function that returns the next sensitivity value.  Uses the bool \param increase to determine whether it should look up or down.  Returns -1 if not possible to move or if the given number is invalid.
	int nextValue(bool increase, int current);
	/// Helper function that returns the next sensitivity units.  Uses the bool \param increase to determine whether it should look up or down.  Returns a null string if not possible to move or the given unit is invalid.
	QString nextUnits(bool increase, QString current);
	/// Helper method that turns an index into a value.
	int valueToIndex(int value) const;
	/// Helper method that turns a value into an index.
	int indexToValue(int index) const;
};

#endif // CLSSR570_H
