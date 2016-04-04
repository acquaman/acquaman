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


#ifndef AMSCANAXIS_H
#define AMSCANAXIS_H

#include "dataman/AMScanAxisRegion.h"
#include "util/AMOrderedList.h"

class AMScanAxis : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(int axisType READ dbReadAxisType WRITE dbLoadAxisType)
	Q_PROPERTY(AMDbObjectList axisRegions READ dbReadRegions WRITE dbLoadRegions)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Describes a single axis of an n-dimensional scan.")

public:
	/// Enum for the possible axis types
	enum AxisType{
		StepAxis = 0,			///< Step axis has a start, delta, end, and time. Can also have multiple regions.
		ContinuousMoveAxis = 1,		///< Continuous move axis has a start and an end but no delta or time. Can only have one region.
		ContinuousDwellAxis = 2,	///< Continuous dwell axis as a time but no start, delta, or end. Can only have one region.
		InvalidAxisType = 3
	};

	/// Enum for possible validity states
	enum AxisValid{
		InvalidAxis = 0,		///< This axis is invalid for some reason (maybe the regions contain elements they can't use or are missing elements they require)
		ValidAxis = 1			///< Everything is good
	};

	/// Constructor requires an axisType (defaults to StepAxis) and the first region (in the case of continuous regions, the first region is the only region)
	Q_INVOKABLE AMScanAxis(AMScanAxis::AxisType axisType = AMScanAxis::StepAxis, AMScanAxisRegion *firstRegion = new AMScanAxisRegion, QObject *parent = 0);
	/// Copy constructor.
	AMScanAxis(const AMScanAxis &original);
	/// Destructor.
	virtual ~AMScanAxis();

	/// Method that creates an exact copy of the current object.  Caller is responsible for memory.
	virtual AMScanAxis *createCopy() const;

	/// Returns the type of axis
	AMScanAxis::AxisType axisType() const;
	/// Returns whether the axis is currently valid based on the combination of regions and axis type
	AMScanAxis::AxisValid axisValid() const;

	/// Returns the list of regions
	const AMOrderedList<AMScanAxisRegion *> regions() const;
	/// Returns the region at the given index (must be between 0 and regionCount()-1)
	AMScanAxisRegion *regionAt(int index) const;
	/// Returns the number of regions
	int regionCount() const;

	/// Returns the start of the axis. That is, the start of the first region. If this type of axis doesn't have a start, then an AMNumber with state of AMNumber::Null will be returned.
	AMNumber axisStart() const;
	/// Returns the end of the axis. That is, the end of the final region. If this type of axis doesn't have a end, then an AMNumber with state of AMNumber::Null will be returned.
	AMNumber axisEnd() const;

	/// Returns the number of points from all regions.  Can be reimplemented if region is more sophisticated.
	virtual int numberOfPoints() const;
	/// Returns the amount of time from all regions.
	virtual double timePerAxis() const;
	/// Returns a string containing the information in a standard way.
	virtual QString toString(const QString &units = "") const;

	/// Returns the minimum region time.
	double minimumRegionTime() const;
	/// Returns the maximum region time.
	double maximumRegionTime() const;

signals:
	/// Notifier that a scan axis region has been added to the axis.  Passes a pointer to the new region.
	void regionAdded(AMScanAxisRegion *);
	/// Notifier that a scan axis region has been removed from the axis.  Passes a pointer to the region.
	void regionRemoved(AMScanAxisRegion *);
	/// Notifier that a region has been overwritten.  Passes a pointer to the new region.
	void regionOverwritten(AMScanAxisRegion *);

	/// Notifier that one of the regions has changed in some way (start, step, end, or time has changed)
	void regionsChanged();

public slots:
	/// Inserts a the region at the index and returns true if successful. If this axis type cannot support more than one region, false is returned. If the axis would become invalid by inserting this region, the region is not inserted and false is returned.
	bool insertRegion(int index, AMScanAxisRegion *region);
	/// Appends a the region at the end of the list and returns true if successful. If this axis type cannot support more than one region, false is returned. If the axis would become invalid by appending this region, the region is not appended and false is returned.
	bool appendRegion(AMScanAxisRegion *region);
	/// Overwrite the region at the index and returns true if successful. If this axis would become invalid by using the new region, then the list of regions remains unchanged and false is returned.
	bool overwriteRegion(int index, AMScanAxisRegion *region);
	/// Removes a region.  Returns true if successful.
	bool removeRegion(AMScanAxisRegion *region);

	/// Clears all regions. Returns true if successful.
	bool clearRegions();

protected slots:
	/// Sets the axis type. If the new axis type doesn't support multiple regions, then all regions except the first are removed.
	void setAxisType(AMScanAxis::AxisType axisType);

protected:
	/// Helper function to determine if the current set of regions and axis type are compatible
	bool sanityCheckRegionsAndAxisType() const;
	/// Helper function to set the axisValid variable based on the return of the sanityCheck
	void checkAxisValidity();

	/// For database saving of the axis type.
	int dbReadAxisType() const;
	/// For database loading of the axis type.
	void dbLoadAxisType(int newAxisType);
	/// For saving the regions list to the database.
	AMDbObjectList dbReadRegions() const;
	/// For database loading (never called).
	void dbLoadRegions(const AMDbObjectList &newAxisRegions);

protected:
	/// Holds the axis type for this axis
	AMScanAxis::AxisType axisType_;
	/// Holds whether or not the current axis is valid
	AMScanAxis::AxisValid axisValid_;

	/// Holds the current list of regions
	AMOrderedList<AMScanAxisRegion *> regions_;
};

#endif // AMSCANAXIS_H
