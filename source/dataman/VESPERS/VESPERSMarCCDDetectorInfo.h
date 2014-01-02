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


#ifndef VESPERSMARCCDDETECTORINFO_H
#define VESPERSMARCCDDETECTORINFO_H

#include "dataman/info/AMOldDetectorInfo.h"

class VESPERSMarCCDDetectorInfo : public AMOldDetectorInfo
{
	Q_OBJECT

	Q_PROPERTY(double acquireTime READ acquireTime WRITE setAcquireTime)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Mar CDD Detector")

public:
	/// Constructor.
 	virtual ~VESPERSMarCCDDetectorInfo();
	VESPERSMarCCDDetectorInfo(const QString &name, const QString &description, QObject *parent = 0);
	/// Copy constructor.
	VESPERSMarCCDDetectorInfo(const VESPERSMarCCDDetectorInfo &original);

	/// Creates a new info pointer from this one, caller is responsible for memory.
	virtual AMOldDetectorInfo *toNewInfo() const { return new AMOldDetectorInfo(*this); }

	/// = implementation.
	VESPERSMarCCDDetectorInfo &operator =(const VESPERSMarCCDDetectorInfo &other);

	/// Returns the acquire time for the detector.
	virtual double acquireTime() const { return acquireTime_; }

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in th eoutput of this detector.  A single point has rank 0. A spectrum output would have rank 1. An image output would have rank 2.
	virtual int rank() const { return 2; }
	/// Returns the size (ie: number of elements) along each dimension of the detector.
	virtual AMnDIndex size() const { return AMnDIndex(2048, 2048); }
	/// Returns a list of AMAxisInfo describing the size and ntature of each detector axis, in order.
	virtual QList<AMAxisInfo> axes() const { return axes_; }

public slots:
	/// Sets the acquire time.
	virtual void setAcquireTime(double time) { acquireTime_ = time; setModified(true); }

protected:
	/// The time the detector should acquire for.
	double acquireTime_;
	/// The axes of the detector.  They are fixed so they may as well be stored statically.
	QList<AMAxisInfo> axes_;
};

#endif // VESPERSMARCCDDETECTORINFO_H
