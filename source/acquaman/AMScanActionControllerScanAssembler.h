#ifndef AMSCANACTIONCONTROLLERSCANASSEMBLER_H
#define AMSCANACTIONCONTROLLERSCANASSEMBLER_H

#include "beamline/AMDetectorSet.h"
#include "beamline/AMControlSet.h"
#include "dataman/AMNumber.h"

class AMScanAxisRegion : public AMDbObject
{
Q_OBJECT
public:
	/// Constructor. Takes values for start, step, end, and time. Defaults are AMNumbers in the AMNumber::Null state
	AMScanAxisRegion(AMNumber start = AMNumber(AMNumber::Null), AMNumber step = AMNumber(AMNumber::Null), AMNumber end = AMNumber(AMNumber::Null), AMNumber time = AMNumber(AMNumber::Null), QObject *parent = 0);

	/// Returns the start of the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionStart() const;
	/// Returns the step size for the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionStep() const;
	/// Returns the end of the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionEnd() const;
	/// Returns the time for the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionTime() const;

public slots:
	/// Sets the start of the region from the AMNumber
	void setRegionStart(AMNumber regionStart);
	/// Sets the step size for the region from the AMNumber
	void setRegionStep(AMNumber regionStep);
	/// Sets the end of the region from the AMNumber
	void setRegionEnd(AMNumber regionEnd);
	/// Sets the time for the region from the AMNumber
	void setRegionTime(AMNumber regionTime);

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
	/// Returns the step of the axis if all of the regions have the same step. If this axis has more than one step size for its regions then an AMNumber with state of AMNumber::Null will be returned.
	AMNumber axisStep() const;
	/// Returns the end of the axis. That is, the end of the final region. If this type of axis doesn't have a end, then an AMNumber with state of AMNumber::Null will be returned.
	AMNumber axisEnd() const;
	/// Returns the time of the axis if all of the regions have the same time. If this axis has more than one time for its regions then an AMNumber with state of AMNumber::Null will be returned.
	AMNumber axisTime() const;

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

class AMScanActionControllerScanAssembler : public QObject
{
Q_OBJECT
public:
	AMScanActionControllerScanAssembler(QObject *parent = 0);

	bool insertAxis(int index, AMControl *axisControl, AMScanAxis *scanAxis);
	bool appendAxis(AMControl *axisControl, AMScanAxis *scanAxis);
	bool addDetector(AMDetector *detector);

	AMAction3* generateActionTree();

protected:
	AMAction3* generateActionTreeForAxis(AMControl *axisControl, AMScanAxis *scanAxis);
	AMAction3* generateActionTreeForStepAxis(AMControl *axisControl, AMScanAxis *stepScanAxis);
	AMAction3* generateActionTreeForStepAxisRegion(AMControl *axisControl, const AMScanAxisRegion &stepScanAxisRegion);
	AMAction3* generateActionTreeForContinuousMoveAxis(AMControl *axisControl, AMScanAxis *continuiousMoveScanAxis);
	AMAction3* generateActionTreeForContinuousDwellAxis(AMControl *axisControl, AMScanAxis *continuousDwellScanAxis);

	AMAction3* generateActionListForDetectorAcquisition();
	AMAction3* generateActionListForStepDetectorAcquisition();
	AMAction3* generateActionListForContinuousDetectorAcquisition();

	AMAction3* generateActionListForDetectorInitialization();
	AMAction3* generateActionListForDetectorCleanup();

	QList<AMAction3*> findInsertionPoints(AMAction3 *action);

protected:
	AMDetectorSet *detectors_;
	AMControlSet *controls_;
	QList<AMScanAxis*> axes_;
};

#endif // AMSCANACTIONCONTROLLERSCANASSEMBLER_H
