/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef CLSIONCHAMBER_H
#define CLSIONCHAMBER_H

#include "beamline/AMIonChamber.h"
#include "beamline/AMControl.h"
#include "beamline/CLS/CLSSR570.h"

/*!
  This class is a CLS specific implementation of the AMIonChamber class.  It implements the necessary functions to make
  a CLS specific ion chamber.  It offers more functionality than AMIonChamber because it is implemented with a CLSSR570
  for setting the sensitivity.  These allow you to set the gain more precisely than simply increasing or decreasing by a
  single step.
  */
class CLSIonChamber : public AMIonChamber
{
	Q_OBJECT

public:
	/// Constructor.  Takes a the name of the counts PV and the voltage PV.
	CLSIonChamber(const QString &name, const QString &description, const QString &countsPV, const QString &voltagePV, const QString &sensitivityName, const QString &sensitivityUnits, QObject *parent = 0);

	/// Reimplemented.  Returns whether the ion chamber is connected.
	virtual bool isConnected() const { return connected_; }
	/// Implements the pure virtual function.  Returns the current counts for the ion chamber.
	double counts() const { return counts_->value(); }
	/// Implements the pure virtual function.  Returns the voltage for the ion chamber.
	double voltage() const { return voltage_->value(); }
	/// Implements the pure virtual function.  Returns whether the ion chamber is at its maximum sensitivity.
	virtual bool atMaximumSensitivity() const{ return sensitivity_->atMaximumSensitivity(); }
	/// Implements the pure virtual function.  Returns whether the ion chamber is at its minimum sensitivity.
	virtual bool atMinimumSensitivity() const{ return sensitivity_->atMinimumSensitivity(); }

	/// Returns the current sensitivity value.
	int sensitivityValue() const { return sensitivity_->value(); }
	/// Returns the current sensitivity units.
	QString sensitivityUnits() const { return sensitivity_->units(); }

public slots:
	/// Reimplemented.  Increases the sensitivity of the ion chamber.  How this is done is implementation specific.
	virtual bool increaseSensitivity() { return sensitivity_->increaseSensitivity(); }
	/// Reimplemented.  Decreases the sensitivity of the ion chamber.  How this is done is implementation specific.
	virtual bool decreaseSensitivity() { return sensitivity_->decreaseSensitivity(); }

	/// Sets the value for the sensitivity directly.  Must be a valid SR570 input.
	void setSensitivityValue(int value) { sensitivity_->setValue(value); }
	/// Sets the units for the sensitivity directly.  Must be a valid SR570 input.
	void setSensitivityUnits(QString units) { sensitivity_->setUnits(units); }

signals:
	/// Notifier that the sensitivity value has changed.  Passes the new value.
	void sensitivityValueChanged(int);
	/// Notifier that the sensitivity units have changed.  Passes the new value.
	void sensitivityUnitsChanged(QString);

protected slots:
	/// Helper slot that determines whether the ion chamber is connected or not.  Is called every time the connected status of any of the controls changes.
	void onConnectedChanged();

protected:
	/// Pointer to the counts control.
	AMControl *counts_;
	/// Pointer to the voltage control.
	AMControl *voltage_;
	/// Pointer to the SR570 used to control the sensitivity.
	CLSSR570 *sensitivity_;

	/// Bool holding the current state of connectivity for the ion chamber.
	bool connected_;
};

#endif // CLSIONCHAMBER_H
