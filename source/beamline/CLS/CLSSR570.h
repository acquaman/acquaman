/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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
	int value() const { return indexToValue(value_->getInt()); }
	/// Returns the index of the sensitivity (value between 0 and 8).
	int valueIndex() const { return value_->getInt(); }
	/// Returns the units of the sensitivity.
	QString units() const { return units_->getString(); }

	/// Returns whether the ion chamber is at maximum senstivity.
	virtual bool atMaximumSensitivity() const { return atMaximumSensitivity_; }
	/// Returns whether the ion chamber is at minimum sensitivity.
	virtual bool atMinimumSensitivity() const { return atMinimumSensitivity_; }

signals:
	/// Notifier that the sensitivity value has changed.  Passes the new value.
	void valueChanged(int);
	/// Notifier that the sensitivity value index has changed.  Passes the new index.
	void valueIndexChanged(int);
	/// Notifier that the sensitivity units have changed.  Passes the new value.
	void unitsChanged(QString);

public slots:
	/// Sets the sensitivity value.  Must be 1, 2, 5, 10, 20, 50, 100, 200, or 500.  Does nothing otherwise.
	void setValue(int value);
	/// Sets the value index.  Must be between 0 and 8.
	void setValueIndex(int index);
	/// Sets the sensitivity units.  Must be pA/V, nA/V, uA/V, or mA/V.  Does nothing otherwise.
	void setUnits(QString units);

	/// Increases the sensitivity of the ion chamber by one step.
	virtual bool increaseSensitivity();
	/// Decreases the sensitivity of the ion chamber by one step.
	virtual bool decreaseSensitivity();


protected slots:
	/// Turns the sensitivity value from an index to the value.
	void onValueChanged(int index);
	/// Determines whether the new state of the ion chamber is at either the minimum or maximum sensitivity.
	void onSensitivityChanged();
	/// Helper function that determines if the SR570 is connected.
	void onConnectedChanged();

protected:
	/// Determines if the new sensitivity value is acceptable.
	bool valueOkay(int value) const;
	/// Determines if the new sensitivity units are acceptable.
	bool unitsOkay(QString units) const;

	/// Pointer to the value PV.  This is a specific implementation for the CLS and maybe should be migrated to an AMControl at the same time the units PV is migrated to an AMControl.
	AMProcessVariable *value_;
	/// Pointer to the units PV.  This can be migrated to an AMControl once it has string functionality.
	AMProcessVariable *units_;

	/// Holds the state of whether the ion chamber is at its maximum sensitivity.
	bool atMaximumSensitivity_;
	/// Holds the state of whether the ion chamber is at its minimum sensitivity.
	bool atMinimumSensitivity_;

private:
	/// Helper function that returns the next sensitivity value.  Uses the bool \param increase to determine whether it should look up or down.  Returns -1 not possible to move or 0 if the given number is invalid.
	int nextValue(bool increase, int current);
	/// Helper function that returns the next sensitivity units.  Uses the bool \param increase to determine whether it should look up or down.  Returns a null string if not possible to move or the given unit is invalid.
	QString nextUnits(bool increase, QString current);
	/// Helper method that turns an index into a value.
	int valueToIndex(int value) const;
	/// Helper method that turns a value into an index.
	int indexToValue(int index) const;
};

class AMControl;
class AMControlSet;

class CLSSR570New : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Builds a SR570 model based on \param valueName and \param untisName.
	CLSSR570New(const QString &valueName, const QString &unitsName, QObject *parent = 0);

	/// Returns the value of the sensitivity (ie: 1, 2, 5, 10, 20, 50, 100, 200, 500).
	int value() const;
	/// Returns the index of the sensitivity (value between 0 and 8).
	int valueIndex() const;
	/// Returns the units of the sensitivity.
	QString units() const;

	/// Returns whether the SR570 is connected.
	bool isConnected() const;

	/// Returns whether the ion chamber is at maximum senstivity.
	virtual bool atMaximumSensitivity() const;
	/// Returns whether the ion chamber is at minimum sensitivity.
	virtual bool atMinimumSensitivity() const;

signals:
	/// General notifier.  Emitted if either the value or the units have changed.
	void sensitivityChanged();
	/// Notifier that the sensitivity value has changed.  Passes the new value.
	void valueChanged(int);
	/// Notifier that the sensitivity value index has changed.  Passes the new index.
	void valueIndexChanged(int);
	/// Notifier that the sensitivity units have changed.  Passes the new value.
	void unitsChanged(QString);
	/// Notifier that the ion chamber is at the minimum sensitivity.  Passes the truth value.
	void minimumSensitivity(bool);
	/// Notifier that the ion chamber is at the maximums sensitivity.  Passes the truth value.
	void maximumSensitivity(bool);
	/// Notifier that the connectivity of the SR570 has changed.  Passes the value.
	void connected(bool);

public slots:
	/// Sets the sensitivity value.  Must be 1, 2, 5, 10, 20, 50, 100, 200, or 500.  Does nothing otherwise.
	void setValue(int value);
	/// Sets the value index.  Must be between 0 and 8.
	void setValueIndex(int index);
	/// Sets the sensitivity units.  Must be pA/V, nA/V, uA/V, or mA/V.  Does nothing otherwise.
	void setUnits(QString units);

	/// Increases the sensitivity of the ion chamber by one step.
	bool increaseSensitivity();
	/// Decreases the sensitivity of the ion chamber by one step.
	bool decreaseSensitivity();

protected slots:
	void onValueControlValueChanged(double newValue);
	void onUnitsControlValueChanged(double newUnits);

protected:
	void checkSensitivityHelper();

protected:
	AMControl *valueControl_;
	AMControl *unitsControl_;
	AMControlSet *allControls_;

	/// Holds the state of whether the ion chamber is at its maximum sensitivity.
	bool atMaximumSensitivity_;
	/// Holds the state of whether the ion chamber is at its minimum sensitivity.
	bool atMinimumSensitivity_;
};

#endif // CLSSR570_H
