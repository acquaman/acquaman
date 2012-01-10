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


#ifndef CLSSPLITIONCHAMBER_H
#define CLSSPLITIONCHAMBER_H

#include "beamline/AMSplitIonChamber.h"
#include "beamline/CLS/CLSSR570.h"

/*!
  This class is a CLS specific implementation of the AMSplitIonChamber class.  It implements the necessary functions to make
  a CLS specific split ion chamber.  It offers more functionality than AMIonChamber and AMSplitIonChamber because it is implemented with a CLSSR570
  for setting the sensitivity.  These allow you to set the gain more precisely than simply increasing or decreasing by a
  single step.
  */
class CLSSplitIonChamber : public AMSplitIonChamber
{
	Q_OBJECT

public:
	/// Constructor.  Takes a the name of the counts PV and the voltage PV for each side of the ion chamber.  The sensitivity is also set independently for each side.
	CLSSplitIonChamber(const QString &name, const QString &description, const QString &countsAPV, const QString &countsBPV, const QString &voltageAPV, const QString &voltageBPV, const QString &sensitivityNameA, const QString &sensitivityNameB, const QString &sensitivityUnitsA, const QString &sensitivityUnitsB, QObject *parent = 0);

	/// Reimplemented.  Returns whether the ion chamber is connected.
	virtual bool isConnected() const { return connected_; }

	/// Pure virtual function.  Returns the current counts for the ion chamber.  Synomonous with reading() and sum().
	virtual double counts() const { return sum(); }
	/// Pure virtual function.  Returns the voltage for the ion chamber for side A.  Synomonous with voltageA().
	virtual double voltage() const { return voltageA(); }

	/// Pure-virtual function.  Returns the counts from one side of the ion chamber (A).
	virtual double countsA() const { return countsA_->value(); }
	/// Pure-virtual function.  Returns the counts from the other side of the ion chamber (B).
	virtual double countsB() const { return countsB_->value(); }

	/// Pure-virtual function. Returns the voltage from one side of the ion chamber (A).
	virtual double voltageA() const { return voltageA_->value(); }
	/// Pure-virtual function. Returns the voltage from the other side of the ion chamber (B).
	virtual double voltageB() const { return voltageB_->value(); }

	/// Pure-virtual function.  Returns whether side A of the ion chamber is at maximum senstivity.
	virtual bool atMaximumSensitivityA() const { return sensitivityA_->atMaximumSensitivity(); }
	/// Pure-virtual function.  Returns whether side A of the ion chamber is at minimum sensitivity.
	virtual bool atMinimumSensitivityA() const { return sensitivityA_->atMinimumSensitivity(); }

	/// Pure-virtual function.  Returns whether side B of the ion chamber is at maximum senstivity.
	virtual bool atMaximumSensitivityB() const { return sensitivityB_->atMaximumSensitivity(); }
	/// Pure-virtual function.  Returns whether side B of the ion chamber is at minimum sensitivity.
	virtual bool atMinimumSensitivityB() const { return sensitivityB_->atMinimumSensitivity(); }

	/// Returns the sensitivity value for one side of the ion chamber (A).
	int sensitivityValueA() const { return sensitivityA_->value(); }
	/// Returns the sensitivity units for one side of the ion chamber (A).
	QString sensitivityUnitsA() const { return sensitivityA_->units(); }
	/// Returns the sensitivity value for the other side of the ion chamber (B).
	int sensitivityValueB() const { return sensitivityB_->value(); }
	/// Returns the sensitivity units for the other side of the ion chamber (B).
	QString sensitivityUnitsB() const { return sensitivityB_->units(); }

signals:
	/// Notifier that the sensitivity value on one side of the ion chamber (A) has changed.  Passes the new value.
	void sensitivityValueAChanged(int);
	/// Notifier that the sensitivity units on one side of the ion chamber (A) have changed.  Passes the new value.
	void sensitivityUnitsAChanged(QString);
	/// Notifier that the sensitivity value on the other side of the ion chamber (B) has changed.  Passes the new value.
	void sensitivityValueBChanged(int);
	/// Notifier that the sensitivity units on the other side of the ion chamber (B) have changed.  Passes the new value.
	void sensitivityUnitsBChanged(QString);

public slots:
	/// Pure virtual function.  Increases the sensitivity of the ion chamber.  How this is done is implementation specific.
	virtual bool increaseSensitivity() { return increaseSensitivityA() && increaseSensitivityB(); }
	/// Pure virtual function.  Decreases the sensitivity of the ion chamber.  How this is done is implementation specific.
	virtual bool decreaseSensitivity() { return decreaseSensitivityA() && decreaseSensitivityB(); }

	/// Pure virtual function.  Increases the sensitivity of side A of the ion chamber.  How this is done is implementation specific.
	virtual bool increaseSensitivityA() { return sensitivityA_->increaseSensitivity(); }
	/// Pure virtual function.  Decreases the sensitivity of side A of the ion chamber.  How this is done is implementation specific.
	virtual bool decreaseSensitivityA() { return sensitivityA_->decreaseSensitivity(); }

	/// Pure virtual function.  Increases the sensitivity of side B of the ion chamber.  How this is done is implementation specific.
	virtual bool increaseSensitivityB() { return sensitivityB_->increaseSensitivity(); }
	/// Pure virtual function.  Decreases the sensitivity of side B of the ion chamber.  How this is done is implementation specific.
	virtual bool decreaseSensitivityB() { return sensitivityB_->decreaseSensitivity(); }

	/// Sets the value for the sensitivity directly for one side of the ion chamber (A).  Must be a valid SR570 input.
	void setSensitivityValueA(int value) { sensitivityA_->setValue(value); }
	/// Sets the units for the sensitivity directly for one side of the ion chamber (A).  Must be a valid SR570 input.
	void setSensitivityUnitsA(QString units) { sensitivityA_->setUnits(units); }

	/// Sets the value for the sensitivity directly for the other side of the ion chamber (B).  Must be a valid SR570 input.
	void setSensitivityValueB(int value) { sensitivityB_->setValue(value); }
	/// Sets the units for the sensitivity directly for the other side of the ion chamber (B).  Must be a valid SR570 input.
	void setSensitivityUnitsB(QString units) { sensitivityB_->setUnits(units); }

protected slots:
	/// Helper slot that determines whether the ion chamber is connected or not.  Is called every time the connected status of any of the controls changes.
	void onConnectedChanged();
	/// Helper slot that emits various signals based on the changes in the counts.
	void onCountsChanged();
	/// Helper slot that emits various signals based on the changes in the voltage.
	void onVoltageChanged();

protected:
	/// Pointer to the counts control for one side of the ion chamber (A).
	AMControl *countsA_;
	/// Pointer to the counts control for the other side of the ion chamber (B).
	AMControl *countsB_;
	/// Pointer to the voltage control for one side of the ion chamber (A).
	AMControl *voltageA_;
	/// Pointer to the voltage control for the other side of the ion chamber (B).
	AMControl *voltageB_;
	/// Pointer to the SR570 used to control the sensitivity for one side of the ion chamber (A).
	CLSSR570 *sensitivityA_;
	/// Pointer to the SR570 used to control the sensitivity for the other side of the ion chamber (B).
	CLSSR570 *sensitivityB_;

	/// Bool holding the current state of connectivity for the ion chamber.
	bool connected_;
};

#endif // CLSSPLITIONCHAMBER_H
