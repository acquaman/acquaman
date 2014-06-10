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


#ifndef CLSAMPTEKSDD123DETECTORINFO_H
#define CLSAMPTEKSDD123DETECTORINFO_H

#include "dataman/info/AMBasicXRFDetectorInfo.h"

/// This class contains the run-time configuration parameters for the Amptek SDD 123 detectors
class CLSAmptekSDD123DetectorInfo : public AMBasicXRFDetectorInfo
{
Q_OBJECT

Q_PROPERTY(double detectorTemperature READ detectorTemperature WRITE setDetectorTemperature)

Q_CLASSINFO("AMDbObject_Attributes", "description=Amptek SDD123 Detector")

public:
	/// Default constructor
 	virtual ~CLSAmptekSDD123DetectorInfo();
	Q_INVOKABLE CLSAmptekSDD123DetectorInfo(const QString &name = "amptekSDD123", const QString &description = "Amptek SDD Detector", QObject *parent = 0);
	/// Copy constructor
	CLSAmptekSDD123DetectorInfo(const CLSAmptekSDD123DetectorInfo &original);

	/// Creates a new info pointer from this one, caller is responsible for memory
	virtual AMOldDetectorInfo* toNewInfo() const;

	/// Implement assignment operator
	CLSAmptekSDD123DetectorInfo& operator =(const CLSAmptekSDD123DetectorInfo &other);

	double detectorTemperature() const;

public slots:
	void setDetectorTemperature(double detectorTemperature);

protected:
	/// The detector temperature
	double detectorTemperature_;
};

#endif // CLSAMPTEKSDD123DETECTORINFO_H
