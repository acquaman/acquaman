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


#ifndef VESPERSROPERCCDDETECTORINFO_H
#define VESPERSROPERCCDDETECTORINFO_H

#include "dataman/info/AMOldDetectorInfo.h"

class VESPERSRoperCCDDetectorInfo : public AMOldDetectorInfo
{
	Q_OBJECT

	Q_PROPERTY(double acquireTime READ acquireTime WRITE setAcquireTime)
	Q_PROPERTY(double temperature READ temperature WRITE setTemperature)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Roper CCD Detector")

public:
	/// Constructor.
 	virtual ~VESPERSRoperCCDDetectorInfo();
	VESPERSRoperCCDDetectorInfo(const QString &name, const QString &description, QObject *parent = 0);
	/// Copy constructor.
	VESPERSRoperCCDDetectorInfo(const VESPERSRoperCCDDetectorInfo &original);

	/// Creates a new info pointer from this one, caller is reponsible for memory.
	virtual AMOldDetectorInfo *toNewInfo() const { return new AMOldDetectorInfo(*this); }

	VESPERSRoperCCDDetectorInfo &operator =(const VESPERSRoperCCDDetectorInfo &other);

	/// Returns the acquire time for the detector.
	virtual double acquireTime() const { return acquireTime_; }
	/// Returns the temperature setpoint of the detector.
	virtual double temperature() const { return temperature_; }

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has rank 0. A spectrum output would have rank 1. An image output would have rank 2.
	virtual int rank() const { return 2;}
	/// Returns the size (ie: number of elements) along each dimension of the detector.  For a single-point detector, returns an empty AMnDIndex(). For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const { return AMnDIndex(2084, 2084); }
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo>  axes() const { return axes_; }

public slots:
	/// Sets the acquire time.
	virtual void setAcquireTime(double time) { acquireTime_ = time; setModified(true); }
	/// Sets the temperature setpoint.
	virtual void setTemperature(double temperature) { temperature_ = temperature; setModified(true); }

protected:
	/// The time the detector should acquire for.
	double acquireTime_;
	/// The temperature that the detector should be set to.
	double temperature_;
	/// The axes of the detector.  They are fixed so they may as well be stored statically.
	QList<AMAxisInfo> axes_;
};

#endif // VESPERSROPERCCDDETECTORINFO_H
