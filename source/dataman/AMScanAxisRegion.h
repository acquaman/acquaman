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


#ifndef AMSCANAXISREGION_H
#define AMSCANAXISREGION_H

#include "dataman/AMNumber.h"
#include "dataman/database/AMDbObject.h"

class AMScanAxisRegion : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(double regionStart READ regionStart WRITE setRegionStart)
	Q_PROPERTY(double regionStep READ regionStep WRITE setRegionStep)
	Q_PROPERTY(double regionEnd READ regionEnd WRITE setRegionEnd)
	Q_PROPERTY(double regionTime READ regionTime WRITE setRegionTime)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Defines a single region that can make up an axis.")

public:
	/// Constructor. Takes values for start, step, end, and time. Defaults are AMNumbers in the AMNumber::Null state
	Q_INVOKABLE AMScanAxisRegion(const AMNumber &start = AMNumber(AMNumber::Null), const AMNumber &step = AMNumber(AMNumber::Null), const AMNumber &end = AMNumber(AMNumber::Null), const AMNumber &time = AMNumber(AMNumber::Null), QObject *parent = 0);
	/// Copy constructor.
	AMScanAxisRegion(const AMScanAxisRegion &original);
	/// Destructor.
	virtual ~AMScanAxisRegion();

	/// Method that creates an exact copy of the current object.  Caller is responisible for memory.
	virtual AMScanAxisRegion *createCopy() const;

	/// Returns the start of the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionStart() const;
	/// Returns the step size for the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionStep() const;
	/// Returns the end of the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionEnd() const;
	/// Returns the time for the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionTime() const;

	/// Returns the number of points in a region.  Can be reimplemented if region is more sophisticated.
	virtual int numberOfPoints() const;
	/// Returns the amount of time the region would take using the parameters it has.
	virtual double timePerRegion() const;
	/// Returns a string containing the information in a standard way.
	virtual QString toString(const QString &units = "") const;

signals:
	/// Notifier that the start value has changed.
	void regionStartChanged(const AMNumber &);
	/// Notifier that the step value value has changed.
	void regionStepChanged(const AMNumber &);
	/// Notifier that the end value has changed.
	void regionEndChanged(const AMNumber &);
	/// Notifier that the time per step has changed.
	void regionTimeChanged(const AMNumber &);

public slots:
	/// Sets the start of the region from the AMNumber
	void setRegionStart(const AMNumber &regionStart);
	/// Sets the step size for the region from the AMNumber
	void setRegionStep(const AMNumber &regionStep);
	/// Sets the end of the region from the AMNumber
	void setRegionEnd(const AMNumber &regionEnd);
	/// Sets the time for the region from the AMNumber
	void setRegionTime(const AMNumber &regionTime);

protected:
	/// Holds the start of the region
	AMNumber regionStart_;
	/// Holds the step size for the region
	AMNumber regionStep_;
	/// Holds the end of the region
	AMNumber regionEnd_;
	/// Holds the time for the region
	AMNumber regionTime_;
};

#endif // AMSCANAXISREGION_H
