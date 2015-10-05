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
#include "util/AMOrderedList.h"

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
	AMNumber regionStart() const { return regionStart_; }
	/// Returns the step size for the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionStep() const { return regionStep_; }
	/// Returns the end of the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionEnd() const { return regionEnd_; }
	/// Returns the time for the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionTime() const { return regionTime_; }

	/// Returns the number of points in a region.  Can be reimplemented if region is more sophisticated.
	virtual int numberOfPoints() const;
	/// Returns the amount of time the region would take using the parameters it has.
	virtual double timePerRegion() const;
	/// Returns a string containing the information in a standard way.
	virtual QString toString(const QString &units = "") const;

	/// Returns true if the region time is valid, ie a valid AMNumber and positive. Returns false otherwise.
	bool validTime() const;
	/// Returns true if the region start, step, end, and time are all valid AMNumbers. Returns false otherwise.
	virtual bool isValid() const;

	/// Returns true if the given point is within the range of this region, false otherwise.
	bool within(const AMNumber &point) const;

	/// Returns true if this region is ascending, false otherwise.
	bool ascending() const;
	/// Returns true if this region is descending, false otherwise.
	bool descending() const;

	/// Returns true if this region shares all of the same properties as the given region, false otherwise.
	bool identicalTo(AMScanAxisRegion *otherRegion) const;
	/// Returns true if this region doesn't overlap with otherRegion, but does share a region limit. False otherwise.
	bool adjacentTo(AMScanAxisRegion *otherRegion) const;
	/// Returns true if this region overlaps with the given region, false otherwise.
	bool intersects(AMScanAxisRegion *otherRegion) const;
	/// Returns true if this region is contained completely within the given region, false otherwise.
	bool containedBy(AMScanAxisRegion *otherRegion) const;
	/// Returns true if this region completely contains the given region, false otherwise.
	bool contains(AMScanAxisRegion *otherRegion) const;
	/// Returns true if this region has the same step as the given region, false otherwise.
	bool sameStep(AMScanAxisRegion *otherRegion) const;
	/// Returns true if this region has the same step size as the given region, false otherwise.
	bool sameStepSize(AMScanAxisRegion *otherRegion) const;
	/// Returns true if this region has the same dwell time as the given region, false otherwise.
	bool sameTime(AMScanAxisRegion *otherRegion) const;
	/// Returns true if this region is ascending/descending the same way as the given region, false otherwise.
	bool sameDirection(AMScanAxisRegion *otherRegion) const;

	/// Returns true if this region can be merged together into this one, false otherwise.
	bool canMerge(AMScanAxisRegion *otherRegion) const;
	/// Returns true if this region can be made adjacent to the given region, false otherwise.
	bool canMakeAdjacentTo(AMScanAxisRegion *otherRegion) const;
	/// Returns true if this region can subtract the given region, false otherwise.
	bool canSubtract(AMScanAxisRegion *region) const;
	/// Returns true if this region can be divided at the given point, false otherwise.
	bool canDivide(const AMNumber &point) const;

	/// Returns a new region that is the intersection of this region and the given region. Returns 0 if the two regions don't intersect.
	AMScanAxisRegion* intersection(AMScanAxisRegion *otherRegion) const;
	/// Returns a list of new regions that are the result of subtracting the given region from this one. Returns empty list if subtraction not possible.
	QList<AMScanAxisRegion*> subtract(AMScanAxisRegion *otherRegion) const;
	/// Returns a list of new regions that are the result of dividing this region at the given point. Returns empty list if division not possible.
	QList<AMScanAxisRegion*> divide(const AMNumber &dividePoint) const;

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

	/// Updates this region's start and end positions and step size such that it is ascending.
	void setAscending();
	/// Updates this region's start and end positions and step size such that it is descending.
	void setDescending();

	/// Attempts to merge the given region into this one. Returns true if successful, false otherwise.
	bool merge(AMScanAxisRegion *otherRegion);

	/// Attempts to make this region adjacent to the given region. Returns true if successful, false otherwise.
	bool makeAdjacentTo(AMScanAxisRegion *otherRegion);

protected slots:
	/// Returns the suggested start value for the region made by merging this region and otherRegion.
	AMNumber mergeStart(AMScanAxisRegion *otherRegion) const;
	/// Returns the suggested step value for the region made by merging this region and otherRegion.
	AMNumber mergeStep(AMScanAxisRegion *otherRegion) const;
	/// Returns the suggested end value for the region made by merging this region and otherRegion.
	AMNumber mergeEnd(AMScanAxisRegion *otherRegion) const;
	/// Returns the suggested time value for the region made by merging this region and otherRegion.
	AMNumber mergeTime(AMScanAxisRegion *otherRegion) const;

	/// Returns the intersection start for the intersection between this region and otherRegion.
	AMNumber intersectionStart(AMScanAxisRegion *otherRegion) const;
	/// Returns the intersection end for the intersection between this region and otherRegion.
	AMNumber intersectionEnd(AMScanAxisRegion *otherRegion) const;

	/// Returns the suggested start value to make this region adjacent to otherRegion.
	AMNumber makeAdjacentStart(AMScanAxisRegion *otherRegion) const;
	/// Returns the suggested end value to make this region adjacent to otherRegion.
	AMNumber makeAdjacentEnd(AMScanAxisRegion *otherRegion) const;

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
