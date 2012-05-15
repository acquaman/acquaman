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


#ifndef AMSPLITIONCHAMBER_H
#define AMSPLITIONCHAMBER_H

#include "beamline/AMIonChamber.h"

/*!
  This class extends the abstraction for general ion chambers in order to creat a split ion chamber.  A split ion chamber is special because it has two ion chambers
  together to offer more information than a standard ion chamber.  If the sensitivity is set to the same values, some important information that can be extracted is
  the relative beam position (using A-B/A+B) and so forth.  Therefore, aside from the general properties of a regular ion chamber, the split ion chamber requires a
  couple more things specified when subclassing into specific implementations.

  The split ion chamber has the same AMIonChamberInfo as AMIonChamber, and therefore doesn't need to re-implement functions of that nature.

  It reimplements the withinTolerance function because it is dependent on two ion chambers now.
  */
class AMSplitIonChamber : public AMIonChamber
{
	Q_OBJECT

public:
	/// Constructor.  Builds a split ion chamber.
	AMSplitIonChamber(const QString &name, const QString &description, QObject *parent = 0);

	/// Get the current reading.  Returns the sum of counts A and B.
	virtual double reading() const { return sum(); }

	/// Pure-virtual function.  Returns the counts from one side of the ion chamber (A).
	virtual double countsA() const = 0;
	/// Pure-virtual function.  Returns the counts from the other side of the ion chamber (B).
	virtual double countsB() const = 0;

	/// Pure-virtual function. Returns the voltage from one side of the ion chamber (A).
	virtual double voltageA() const = 0;
	/// Pure-virtual function. Returns the voltage from the other side of the ion chamber (B).
	virtual double voltageB() const = 0;

	/// Returns the sum of the counts of the two ion chambers.  Only valid if the sensitivity is the same for both sides.
	double sum() const { return countsA() + countsB(); }
	/// Returns the difference of the counts of the two ion chambers.  Only valid if the sensitivity is the same for both sides.
	double difference() const { return countsA() - countsB(); }
	/// Returns the normalized position of the beam (based on equation |A-B|/(A+B)).
	double normalizedPosition() const { return fabs(difference())/sum(); }

	/// Re-implemented.  Returns whether the voltage is within the linear range of the ion chamber.
	virtual bool withinLinearRange() const { return (voltageA() <= maximumVoltage() && voltageA() >= minimumVoltage()) && (voltageB() <= maximumVoltage() && voltageB() >= minimumVoltage()); }
	/// Re-implemented.  Specific helper function that returns whether the voltage is too low.  Returns false if withinLinearRange is true.
	virtual bool voltageTooLow() const { return !withinLinearRange() && (voltageA() < minimumVoltage() || voltageB() < minimumVoltage()); }
	/// Re-implemented.  Specific helper function that returns whether the voltage is too high.  Returns false if withinLinearRange is true.
	virtual bool voltageTooHigh() const { return !withinLinearRange() && (voltageB() > maximumVoltage() || voltageB() > minimumVoltage()); }

	/// Implemented pure-virtual function.  Returns true if either of the sides of the ion chamber are at their maximum sensitivity.
	virtual bool atMaximumSensitivity() const { return atMaximumSensitivityA() || atMaximumSensitivityB(); }
	/// Implemented pure-virtual function.  Returns true if either of the sides of the ion chamber are at their minimum sensitivity.
	virtual bool atMinimumSensitivity() const { return atMinimumSensitivityA() || atMinimumSensitivityB(); }

	/// Pure-virtual function.  Returns whether side A of the ion chamber is at maximum senstivity.
	virtual bool atMaximumSensitivityA() const = 0;
	/// Pure-virtual function.  Returns whether side A of the ion chamber is at minimum sensitivity.
	virtual bool atMinimumSensitivityA() const = 0;

	/// Pure-virtual function.  Returns whether side B of the ion chamber is at maximum senstivity.
	virtual bool atMaximumSensitivityB() const = 0;
	/// Pure-virtual function.  Returns whether side B of the ion chamber is at minimum sensitivity.
	virtual bool atMinimumSensitivityB() const = 0;

signals:
	/// Notifier that the counts have changed on one side of the ion chamber (A).  Passes the value.
	void countsAChanged(double);
	/// Notifier that the counts have changed on the other side of the ion chamber (B).  Passes the new value.
	void countsBChanged(double);
	/// Notifier that the voltage has changed on one side of the ion chamber (A).  Passes the value.
	void voltageAChanged(double);
	/// Notifier that the voltage has changed on the other side of the ion chamber (B).  Passes the new value.
	void voltageBChanged(double);
	/// Notifier that the sensitivity has changed on one side of the ion chamber (A).
	void sensitivityAChanged();
	/// Notifier that the sensitivity has changed on the other side of the ion chamber (B).
	void sensitivityBChanged();
	/// Notifier that one side of the ion chamber (A) is at the minimum sensitivity.  Passes the truth value.
	void minimumSensitivityA(bool);
	/// Notifier that one side of the ion chamber (A) is at the maximums sensitivity.  Passes the truth value.
	void maximumSensitivityA(bool);
	/// Notifier that the other side of the ion chamber (B) is at the minimum sensitivity.  Passes the truth value.
	void minimumSensitivityB(bool);
	/// Notifier that the other side of the ion chamber (B) is at the maximums sensitivity.  Passes the truth value.
	void maximumSensitivityB(bool);

public slots:
	/// Pure virtual function.  Increases the sensitivity of side A of the ion chamber.  How this is done is implementation specific.
	virtual bool increaseSensitivityA() = 0;
	/// Pure virtual function.  Decreases the sensitivity of side A of the ion chamber.  How this is done is implementation specific.
	virtual bool decreaseSensitivityA() = 0;

	/// Pure virtual function.  Increases the sensitivity of side B of the ion chamber.  How this is done is implementation specific.
	virtual bool increaseSensitivityB() = 0;
	/// Pure virtual function.  Decreases the sensitivity of side B of the ion chamber.  How this is done is implementation specific.
	virtual bool decreaseSensitivityB() = 0;
};

#endif // AMSPLITIONCHAMBER_H
