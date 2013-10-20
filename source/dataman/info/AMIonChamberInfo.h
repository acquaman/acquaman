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


#ifndef AMIONCHAMBERINFO_H
#define AMIONCHAMBERINFO_H

#include "dataman/info/AMOldDetectorInfo.h"

#include <QPair>

/*!
  This class incapuslates the information contained in a general ion chamber.  Things that are important to
  an ion chamber are the counts used for data analysis, the voltage used for ensuring that the detector is in
  its linear range.  It also contains a range for which the detector can know if it is in that linear range.
  */
class AMIonChamberInfo : public AMOldDetectorInfo
{
	Q_OBJECT

	Q_PROPERTY(double minimumVoltage READ minimumVoltage WRITE setMinimumVoltage)
	Q_PROPERTY(double maximumVoltage READ maximumVoltage WRITE setMaximumVoltage)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Ion Chamber Detector")

public:
	/// Constructor.  Provide the name and description for the ion chamber.
	Q_INVOKABLE AMIonChamberInfo(const QString& name = "ionChamber", const QString& description = "Ion Chamber", QObject *parent = 0);
	/// Constructor that takes in a detector info and retrieves all the settings.
	AMIonChamberInfo(const AMIonChamberInfo &original);

	/// Implements assignment operator
	AMIonChamberInfo& operator=(const AMIonChamberInfo &other);

	/// Returns the minimum voltage range for the ion chamber.
	double minimumVoltage() const { return voltageRange_.first; }
	/// Returns the maximum voltage range for the ion chamber.
	double maximumVoltage() const { return voltageRange_.second; }
	/// Returns the voltage range for the ion chamber.
	QPair<double, double> voltageRange() const { return voltageRange_; }

public slots:
	/// Sets the minimum voltage for the linear range for the detector.
	void setMinimumVoltage(double min) { voltageRange_.first = min; setModified(true); }
	/// Sets the maximum voltage for the linear range for the detector.
	void setMaximumVoltage(double max) { voltageRange_.second = max; setModified(true); }
	/// Sets the linear voltage range.
	void setVoltagRange(QPair<double, double> range) { setMinimumVoltage(range.first); setMaximumVoltage(range.second); }
	/// Overloaded.  Sets the linear voltage range.
	void setVoltagRange(double min, double max) { setVoltagRange(qMakePair(min, max)); }

protected:
	/// The linear voltage range of the detector.
	QPair<double, double> voltageRange_;
};

#endif // AMIONCHAMBERINFO_H
