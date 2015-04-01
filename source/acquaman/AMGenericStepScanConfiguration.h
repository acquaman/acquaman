#ifndef AMGENERICSTEPSCANCONFIGURATION_H
#define AMGENERICSTEPSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"

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

	/// Returns the controls used for this configuration.
	QList<AMControlInfo> controls() const { return controls_; }
	/// Returns the detectors that will be used for this scan.
	QList<AMDetectorInfo> detectors() const { return detectors_; }

	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }

signals:
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

public slots:
	/// Sets a controlInfo to an axis.  If the axis has no control associated with it yet, then it will add it to the list, otherwise it will replace it.
	void setControl(int axisId, AMControlInfo newInfo);
	/// Removes a control from the given axis.  Also removes the axis.
	void removeControl(int axisId);
	/// Adds a detector to the detector info list.
	void addDetector(AMDetectorInfo newInfo);
	/// Removes the detector from the detector info list.
	void removeDetector(AMDetectorInfo info);

protected slots:
	/// Computes the total time any time the regions list changes.
	void computeTotalTime();

protected:
	/// The list of controls.  This should almost always have one element in it, sometimes two.
	QList<AMControlInfo> controls_;
	/// List of detectors that will be used.
	QList<AMDetectorInfo> detectors_;

	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;
};

#endif // AMGENERICSTEPSCANCONFIGURATION_H
