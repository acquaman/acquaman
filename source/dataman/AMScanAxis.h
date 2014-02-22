#ifndef AMSCANAXIS_H
#define AMSCANAXIS_H

#include "dataman/AMScanAxisRegion.h"
#include "util/AMOrderedList.h"

class AMScanAxis : public AMDbObject
{
	Q_OBJECT

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
	AMScanAxis(AMScanAxis::AxisType axisType = AMScanAxis::StepAxis, const AMScanAxisRegion &firstRegion = AMScanAxisRegion(), QObject *parent = 0);
	/// Destructor.
	virtual ~AMScanAxis();

	/// Returns the type of axis
	AMScanAxis::AxisType axisType() const;
	/// Returns whether the axis is currently valid based on the combination of regions and axis type
	AMScanAxis::AxisValid axisValid() const;

	/// Returns the list of regions
	const AMOrderedList<AMScanAxisRegion> regions() const;
	/// Returns the region at the given index (must be between 0 and regionCount()-1)
	const AMScanAxisRegion& regionAt(int index) const;
	/// Returns the number of regions
	int regionCount() const;

	/// Returns the start of the axis. That is, the start of the first region. If this type of axis doesn't have a start, then an AMNumber with state of AMNumber::Null will be returned.
	AMNumber axisStart() const;
	/// Returns the end of the axis. That is, the end of the final region. If this type of axis doesn't have a end, then an AMNumber with state of AMNumber::Null will be returned.
	AMNumber axisEnd() const;

public slots:
	/// Inserts a the region at the index and returns true if successful. If this axis type cannot support more than one region, false is returned. If the axis would become invalid by inserting this region, the region is not inserted and false is returned.
	bool insertRegion(int index, const AMScanAxisRegion &region);
	/// Appends a the region at the end of the list and returns true if successful. If this axis type cannot support more than one region, false is returned. If the axis would become invalid by appending this region, the region is not appended and false is returned.
	bool appendRegion(const AMScanAxisRegion &region);
	/// Overwrite the region at the index and returns true if successful. If this axis would become invalid by using the new region, then the list of regions remains unchanged and false is returned.
	bool overwriteRegion(int index, const AMScanAxisRegion &region);

protected slots:
	/// Sets the axis type. If the new axis type doesn't support multiple regions, then all regions except the first are removed.
	void setAxisType(AMScanAxis::AxisType axisType);

protected:
	/// Helper function to determine if the current set of regions and axis type are compatible
	bool sanityCheckRegionsAndAxisType() const;
	/// Helper function to set the axisValid variable based on the return of the sanityCheck
	void checkAxisValidity();

protected:
	/// Holds the axis type for this axis
	AMScanAxis::AxisType axisType_;
	/// Holds whether or not the current axis is valid
	AMScanAxis::AxisValid axisValid_;

	/// Holds the current list of regions
	AMOrderedList<AMScanAxisRegion> regions_;
};

#endif // AMSCANAXIS_H
