#ifndef AMGENERICSTEPSCANCONFIGURATION_H
#define AMGENERICSTEPSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"

#include "dataman/AMRegionOfInterest.h"

/// This class implements a generic scanning tool for 1D and 2D scans.
/*!
	This class uses all the exposed controls and detectors of a beamline
	to generate a completely general scanning tool.  It will have a single
	control by default, but if a second control is added then it will scan
	in two dimensions.

	By default, all exposed detectors in a beamline will be used.
  */
class AMGenericStepScanConfiguration : public AMStepScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject *i0 READ dbReadI0 WRITE dbLoadI0)
	Q_PROPERTY(AMDbObjectList regionsOfInterest READ dbReadRegionsOfInterest WRITE dbLoadRegionsOfInterest)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Step Configuration")

public:
	/// Constructor.
	Q_INVOKABLE AMGenericStepScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	AMGenericStepScanConfiguration(const AMGenericStepScanConfiguration &original);
	/// Destructor.
	virtual ~AMGenericStepScanConfiguration();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// Returns the technique string.
	QString technique() const;
	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const;
	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }

	/// Returns whether or not there is a valid I0 detector.
	bool hasI0() const;
	/// Returns the detector name for I0.
	AMDetectorInfo i0() const { return i0_; }
	/// Returns the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }

signals:
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

public slots:
	/// Sets a controlInfo to an axis.  If the axis has no control associated with it yet, then it will add it to the list, otherwise it will replace it.
	virtual void setControl(int axisId, AMControlInfo newInfo);
	/// Removes a control from the given axis.  Also removes the axis.
	virtual void removeControl(int axisId);
	/// Adds a detector to the detector info list.
	void addDetector(AMDetectorInfo newInfo);
	/// Removes the detector from the detector info list.
	void removeDetector(AMDetectorInfo info);
	/// Clears all detectors from the detector info list.
	void clearDetectors();
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region);
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region);
	/// Sets the bounding range for the given region of interest.
	void setRegionOfInterestBoundingRange(AMRegionOfInterest *region);
	/// Sets I0 name.  Must a valid name inside the detector list.
	void setI0(const AMDetectorInfo &info);

protected slots:
	/// Inserts the given region into the given scan axis index position, and makes the appropriate connections.
	virtual void addRegion(int scanAxisIndex, int regionIndex, AMScanAxisRegion *region);
	/// Sets up connections to the given region's signals.
	virtual void connectRegion(AMScanAxisRegion *region);
	/// Removes the given region from the scan axis at the given index position, and breaks the appropriate connections.
	virtual void removeRegion(int scanAxisIndex, AMScanAxisRegion *region);
	/// Removes connections to the given region's signals.
	virtual void disconnectRegion(AMScanAxisRegion *region);

	/// Computes the total time any time the regions list changes.
	void computeTotalTime();

protected:
	/// Calculates and returns the time it would take to scan the given region, 0 if there are any issues with the given region.
	double calculateRegionTotalTime(AMScanAxisRegion *region);
	/// Calculates and returns the time it would take to scan the regions of a given scan axis, 0 if there are any issues with the given axis.
	double calculateRegionsTotalTime(AMScanAxis *scanAxis);

	/// Returns the I0 detector info.
	AMDbObject *dbReadI0() { return &i0_; }
	/// For database loading (never called).
	void dbLoadI0(AMDbObject *) {}
	/// Returns the regions of interest list.
	AMDbObjectList dbReadRegionsOfInterest();
	/// Called by the dtabase system on loadFromDb() to give us our new list of AMRegionOfInterest.
	void dbLoadRegionsOfInterest(const AMDbObjectList &newRegions);

	/// Returns a string that displays all the regions of interest.
	QString regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const;

	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;
	/// The name of the detector that will be I0.
	AMDetectorInfo i0_;
	/// The list of the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest_;
};

#endif // AMGENERICSTEPSCANCONFIGURATION_H
