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


#ifndef AM_BEAMLINE_H_
#define AM_BEAMLINE_H_

#include "beamline/AMControl.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMSynchronizedDwellTime.h"
#include "beamline/AMDetectorSet.h"
#include "beamline/AMDetectorGroup.h"
#include "beamline/AMXRFDetector.h"

#include "beamline/AMBeamlineSupport.h"

class AMSamplePlate;
class AMSample;
class AMSamplePlateBrowser;
class AMScanConfiguration;
class AMStepScanConfiguration;

#define AMBEAMLINE_BEAMLINE_NOT_CREATED_YET 280301

/// One good way for components in the Acquaman framework to access and set a variety of beamline controls is through a centralized AMBeamline object.  This class provides the basic functionality expected of every beamline, and can be subclassed to include the specific controls available on a particular machine.  It uses the singleton design pattern to ensure that only a single instance of the beamline object exists, you can access this object through AMBeamline::bl().

/*! <b>Note on subclassing and inheritance</b>
  Singleton classes are designed so that only a single instance of the object can be created.  This is accomplished by making the constructor private (or protected), and always accessing the object through a static member function (in this case, AMBeamline::bl()).

  This static function first checks to see if an instance of the object has already been created. If it hasn't, it creates a new instance, stores it in the static instance_ variable, and then returns it. In other cases, bl() simply returns the existing instance.

  When you subclass AMBeamline, it's important to provide your own accessor function, for example:
\code
static YOURBeamline* bl() {
	if(!instance_)
		instance_ = new YOURBeamline();
	return static_cast<YOURBeamline*>(instance_);
}
\endcode
which sets AMBeamline's protected instance_ variable.

As long as the FIRST call to use the beamline is through YOURBeamline::bl(), then all successive calls to AMBeamline::bl() will return the instance of your specific beamline.  If anything calls AMBeamline::bl() before this, there will be no instance_ yet, and it will return 0.  Therefore, it's necessary to call YOURBeamline::bl() to initialize the beamline object before any other code might access AMBeamline; we would normally place this intialization inside your specific version of AMAppController::startup().
*/

class AMBeamline : public AMControl, public AMBeamlineControlAPI, public AMBeamlineControlSetAPI, public AMBeamlineDetectorAPI, public AMBeamlineDetectorSetAPI, public AMBeamlineSynchronizedDwellTimeAPI {

	Q_OBJECT

public:
	/// Singleton class accessor function. Use this to access the application-wide Beamline object.
	/*! See note on subclassing and inheritance above. It's important that your own beamline's initialization function be called before this one (ex: YourBeamline::bl()) */
	static AMBeamline* bl();
	/// Call this to delete the beamline object instance
	static void releaseBl();
	/// Destructor.
	virtual ~AMBeamline();

	/// Returns a string with a human readable text of what is important about this detector.
	virtual QString details() const { return ""; }

	/// Reports whether the beamline is currently in exclusive use, and should not be changed. (For example: you or some other program is running a scan). The base class always returns false, your should re-implement this function if you know better.
	virtual bool isBeamlineScanning() const { return false; }

	/// Returns the current beamline sample positioner (you can send a sample position to it and it will there)
	virtual AMControlSet* currentSamplePositioner() { return 0; }
	/// Returns the current static fiducializations available for the sample positioner
	virtual QList<AMControlInfoList> currentFiducializations() { return QList<AMControlInfoList>(); }
	/// Returns the current sample description if available (if not, should like be <Unknown Sample>)
	virtual QString currentSampleDescription() { return "<Unknown Sample>"; }
	/// Returns the current sample plate id if available (if no sample plate is loaded, then returns -1)
	virtual int currentSamplePlateId() const { return -1;}

	/// Returns the control set that contains all of the public controls.  These are used with actions for automatic lookup.
	virtual AMControlSet *exposedControls() const { return exposedControls_; }
	/// Returns a control based on the name of the control.  Returns 0 if no control is found.
	virtual AMControl *exposedControlByName(const QString &name) { return exposedControls_->controlNamed(name); }
	/// Returns a control based on the control info.  Returns 0 if no control is found.
	virtual AMControl *exposedControlByInfo(const AMControlInfo &info) { return exposedControls_->controlNamed(info.name()); }

	/// Adds a control to the exposed set.
	virtual bool addExposedControl(AMControl *control) { return exposedControls_->addControl(control); }
	/// Removes a control from the exposed set.
	virtual bool removeExposedControl(AMControl *control) { return exposedControls_->removeControl(control); }

	/// Returns the detector set that contains all of the public detectors. These are used with scan actions and configurations for automatic lookup.
	virtual AMDetectorSet* exposedDetectors() const { return exposedDetectors_; }
	/// Returns a detector based on the name of the detector. Returns 0 if no detector is found.
	virtual AMDetector* exposedDetectorByName(const QString &detectorName) { return exposedDetectors_->detectorNamed(detectorName); }
	/// Returns a detector based on the detector info. Returns 0 if no control is found.
	virtual AMDetector* exposedDetectorByInfo(const AMDetectorInfo &detectorInfo) { return exposedDetectors_->detectorNamed(detectorInfo.name()); }
	/// Returns a new detector set which includes a subset of detectors for a particular keyed purpose (passing the type of a scan should return the detectors that can be used for that type of scan). The detector set is a new object, so the caller is responsible for the memory. The default implementation returns a NULL pointer.
	virtual AMDetectorSet* detectorsFor(const QString &key) { Q_UNUSED(key); return 0; }
	/// Returns true if the detector referred to by this detector info is available (connected)
	virtual bool detectorAvailable(const AMDetectorInfo &detectorInfo);
	/// Returns the detector set associated with "science".  This will usually be a parsed exposedDetectors list.
	virtual AMDetectorSet *exposedScientificDetectors() const { return exposedScientificDetectors_; }
	/// Returns a detector based on the name of the detector. Returns 0 if no detector is found.
	virtual AMDetector* exposedScientificDetectorByName(const QString &detectorName) { return exposedScientificDetectors_->detectorNamed(detectorName); }
	/// Returns a detector based on the detector info. Returns 0 if no control is found.
	virtual AMDetector* exposedScientificDetectorByInfo(const AMDetectorInfo &detectorInfo) { return exposedScientificDetectors_->detectorNamed(detectorInfo.name()); }

	/// Adds a detector to the exposed set. Returns whether or not the detector was successfully added.
	virtual bool addExposedDetector(AMDetector *detector) { return exposedDetectors_->addDetector(detector); }
	/// Removes a detector from the exposed set.  Returns whether or not the detector was successfully removed.
	virtual bool removeExposedDetector(AMDetector *detector) { return exposedDetectors_->removeDetector(detector); }
	/// Adds a detector to the exposed scientific detector set.  Returns whether or not the detector was successfully added.
	virtual bool addExposedScientificDetector(AMDetector *detector) { return exposedScientificDetectors_->addDetector(detector); }
	/// Removes a detector from the exposed scientific set.  Returns whether or not the detector was successfully removed.
	virtual bool removeExposedScientificDetector(AMDetector *detector) { return exposedScientificDetectors_->removeDetector(detector); }

	/// Returns the list of the exposed detector groups.
	QList<AMDetectorGroup*> exposedDetectorGroups() const { return exposedDetectorGroups_; }
	/// Returns an AMDetectorGroup based on the provided name.  Returns 0 if the name is not provided.
	AMDetectorGroup* exposedDetectorGroupByName(const QString &name) const;
	/// Adds a detector group to the exposed detector group list.
	bool addExposedDetectorGroup(AMDetectorGroup *detectorGroup);

	/// Returns the beamline's synchronized dwell time object if one is available. Returns 0 (NULL) otherwise.
	virtual AMSynchronizedDwellTime* synchronizedDwellTime() const { return 0; }

	/// Call to check on the validity of a potential workflow action. Returns an ActionValidity enum (CurrentlyValid by default).
	virtual AMAction3::ActionValidity validateAction(AMAction3 *action) { Q_UNUSED(action); return AMAction3::ActionCurrentlyValid; }
	/// Call to check for a message on the (in)validity of an action
	virtual QString validateActionMessage(AMAction3 *action) { Q_UNUSED(action); return QString("Action is Currently Valid"); }

	AMSamplePlate* samplePlate();
	AMSamplePlateBrowser* samplePlateBrowser();
	void setSamplePlate(AMSamplePlate *samplePlate);

	/// The current sample as a list. There better be either 0 or 1 items in it ... otherwise there are mutltiple samples currently under the beam
	QList<AMSample*> currentSamples() const;
	/// Returns the current sample if there is only one item in the list, otherwise returns null. If there are 0 samples or more than one we don't know which is the current sample.
	AMSample* currentSample() const;
	/// Sets the current sample as a list. If you pass an empty list, then there is no current sample under the beam. If there is more than 1 item, then multiple samples are under the beam.
	void setCurrentSamples(QList<AMSample*> sample);

	/// Adds an AMXRFDetector to the syncrhonized XRF detector list.
	void addSynchronizedXRFDetector(AMXRFDetector *detector);

	/// Returns an action that can turn off the beam.
	virtual AMAction3* createTurnOffBeamActions();

	void initializeBeamlineSupport();

	/// Creates and returns an action that initializes the axis controls to their start positions for the given configuration.
	virtual AMAction3* createInitializeScanAxisControlsAction(AMStepScanConfiguration *configuration);
	/// Creates and returns an action that sets up the beamline for a scan.
	virtual AMAction3* createScanInitializationAction(AMScanConfiguration *configuration);
	/// Creates and returns an action that cleans up the beamline after a scan.
	virtual AMAction3* createScanCleanupAction(AMScanConfiguration *configuration) { Q_UNUSED(configuration) return 0; }

signals:
	/// Emit this signal whenever isBeamlineScanning() changes.
	void beamlineScanningChanged(bool isScanning);
	void samplePlateAboutToChange(AMSamplePlate *lastSamplePlate);
	void samplePlateChanged(AMSamplePlate *samplePlate);

	/// Emitted when the current sample
	void currentSampleChanged(QList<AMSample*> currentSamples);


protected slots:
	/// Slot that handles ensuring all synchronized XRF detectors have added the new region of interest.
	void onRegionOfInterestAdded(AMRegionOfInterest *newRegion);
	/// Slot that handles ensuring all synchronized XRF detectors have removed the specified region of interest.
	void onRegionOfInterestRemoved(AMRegionOfInterest *removedRegion);
	/// Slot that handles ensuring all the bounding ranges of regions of interest are synchronized across all synchronized XRF detectors.
	void onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region);

protected:
	/// Singleton classes have a protected constructor, all access is through AMBeamline::bl() or YourBeamline::bl()
	AMBeamline(const QString& controlName);
	/// Instance variable
	static AMBeamline* instance_;

	/// A control set that contains all of the publicly (throughout the program) available controls for a beamline.  This is primarily used for AMControlMoveAction.
	AMControlSet *exposedControls_;

	/// A detector set that contains all of the publicly (throughout the program) available detectors for a beamline. This is primarily used for settings up scans.
	AMDetectorSet *exposedDetectors_;
	/// A parsed down detector set of exposed detectors.
	AMDetectorSet *exposedScientificDetectors_;
	/// A list of exposed detector groups.  Groups are logical groupings of detectors.
	QList<AMDetectorGroup*> exposedDetectorGroups_;

	AMSamplePlate *samplePlate_;
	AMSamplePlateBrowser *samplePlateBrowser_;
	/// The current sample as a list. There better be either 0 or 1 items in it ... otherwise there are mutltiple samples currently under the beam
	QList<AMSample*> currentSamples_;

	/// A list of XRF detectors that are synchronized together.  As of right now, they only synchronize the regions of interest, but could be expanded later.
	QList<AMXRFDetector *> synchronizedXRFDetectors_;
};

#endif /*BEAMLINE_H_*/
