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


#ifndef CLSOCEANOPTICS65000DETECTORINFO_H
#define CLSOCEANOPTICS65000DETECTORINFO_H

#include "AMOldDetectorInfo.h"

class CLSOceanOptics65000DetectorInfo : public AMOldDetectorInfo
{
Q_OBJECT

Q_PROPERTY(int binCount READ binCount WRITE setBinCount)
Q_PROPERTY(QString axisName READ axisName WRITE setAxisName)
Q_PROPERTY(QString axisUnit READ axisUnit WRITE setAxisUnit)
Q_PROPERTY(double integrationTime READ integrationTime WRITE setIntegrationTime)

Q_CLASSINFO("AMDbObject_Attributes", "description=Ocean Optics 65000 USB Spectrometer Detector")

public:
 	virtual ~CLSOceanOptics65000DetectorInfo();
	Q_INVOKABLE CLSOceanOptics65000DetectorInfo(const QString &name = "oos65000", const QString &description = "OceanOptics 65000", QObject *parent = 0);

	CLSOceanOptics65000DetectorInfo(const CLSOceanOptics65000DetectorInfo &original);

	/// Creates a new info pointer from this one, caller is responsible for memory
	virtual AMOldDetectorInfo* toNewInfo() const;

	CLSOceanOptics65000DetectorInfo& operator=(const CLSOceanOptics65000DetectorInfo& other);

	/// Number of bins in spectrometer
	int binCount() const;
	/// Name of the spectrum axis
	QString axisName() const;
	/// Units for the spectrum axis
	QString axisUnit() const;
	/// The integration time in seconds
	double integrationTime() const;

	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int rank() const;
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  Since this detector outputs a spectrum, this would contain one number (the number of pixels, points, or bins along the axis).
	virtual AMnDIndex size() const;
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.  Here we have a single axis.
	virtual QList<AMAxisInfo> axes() const;

	/// Debug helper
	QDebug qDebugPrint(QDebug &d) const;

	/// No clue, check to see if this is still needed
	virtual bool hasDetails() const;

public slots:
	/// Sets the desired integration time
	void setIntegrationTime(double integrationTime);

protected slots:
	/// Sets the number of bins (for internal use only)
	void setBinCount(int binCount);
	/// Sets the name of the spectrum axis (for internal use only)
	void setAxisName(const QString &axisName);
	/// Sets the unit of the spectrum axis (for interanl use only)
	void setAxisUnit(const QString &axisUnit);

protected:
	int binCount_;
	QString axisName_;
	QString axisUnit_;
	double integrationTime_;
};

#endif // CLSOCEANOPTICS65000DETECTORINFO_H
