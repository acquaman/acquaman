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


#ifndef AMDETECTOR_H
#define AMDETECTOR_H

#include <QObject>

#include "dataman/AMMeasurementInfo.h"
#include "dataman/info/AMDetectorInfo.h"
#include "actions3/AMAction3.h"
#include "dataman/datasource/AMDataSource.h"

class AMDetectorTriggerSource;
class AMDetectorDwellTimeSource;

/// An AMDetector is an abstract representation of all scientific tools designed to report data
/*!
  As a programmer, you may wish to implement your own AMDetector subclass. Check around first! You may find that someone has already done the work.

  You will need to look into three types of calls: functions you MUST implement, functions you MAY wish to reimplement, and functions you MUST call.

  \b MUST Implement:
  - size(int) Must be implemented to ensure that you return sensible information.

  - requiresPower() Must be implemented to say whether or not the detector needs additional powering.

  - canContinuousAcquire() Must be implemented to say whether or not the detector can support continuous acquisitions.
  - canCancel() Must be implemented to say if the detector's acquisition can be cancelled.
  - canClear() Must be implemented to say if the detector's capable of clearing its current data.

  - acquisitionTime() Must be implemented to return the detector's current dwell time.
  - setAcquisitionTime() Must be implemented to change the detector's current dwell time. Return false if this cannot be set or is set incorreclty.
  - createSetAcquisitionTimeAction() Must be implemented to return an action that can set the acquisition time for triggered detectors.

  - supportsSynchronizedDwell() Must be implemented to say if the detector can be controlled by a synchronzed dwell time tool. FLESH THIS OUT.
  - currentlySynchronizedDwell() Must be implemented to say if the detector is currently being controlled by a synchronized dwell time tool. FLESH THIS OUT.

  - readMethod() Must be implemented to say how this detector reads (request/trigger, immedite, wait). While it's unlikely that your detector can change this on the fly, it is not impossible.
  - readMode() Must be implemented to say how this detector is currently reading data (single acquisition, continuous). Some detectors can switch on the fly.
  - setReadMode() Must be implemented to deal with switching on the fly. If your detector does not support this, simply return false and do nothing.

  - reading() Must be implemented to pull out single-valued readings from your detector.
  - data() Must be implemented to return a raw data pointer for fast access.

  - dataSource() Must be implemented to return an appropriate data source for viewing the detector output.

  - acquireImplementation(AMDetectorDefinitions::ReadMode) Must be implemented to start the internal acquisition process.
  - initializeImplementation() Must be implemented to internally initialize the detector. If the detector requires no initialization you can call setInitialized() and return true. If you require many steps you may consider making a list of actions to do the work.
  - cleanupImplementation() Must be implemented to internally cleanup the detector after acquisition/scans. If the detector requires no cleanup you can call setCleanedUp() and return true. If you require many steps you may consider making a list of actions to do the work.

  \b MAY Reimplement:
  - rank(), size(), axes() There are default implementations, but these are only valid for 0D detectors

  - reading0D, reading1D, reading2D There are default implementations, but they are inefficient because they will call reading() repeatedly
  - clearImplementation() May be implemented to internally clear the current data if possible. Default implementation returns false. Also, clear() automatically checks canClear() and returns false if the detector doesn't support it.

  - createInitializationActions() A default action is created using the initialization functions and signals. If you wish to make a specific action or return a list of actions you may.
  - createCleanupActions() A default action is created using the cleanup functions and signals. If you wish to make a specific action or return a list of actions you may.

  \b MUST Call:
  - setConnected() You must call this function when your detector has connected, this way the signals are always emitted correctly.
  - setPowered() You must call this function if you detector requires some additional form of power (likely high voltage), this way signals are always emitted correctly.

  - setInitializing(), setInitialized(), setInitializationRequired() You must call these functions to change the state of the initialization state machine (read more about the initializtion state machine for further details). Since these states can change due to external events, you can call these as slots if you like. If you never require any initialization, you can call setInitialized() in your constructor.
  - setCleaningUp(), setCleanedUp(), setCleanupRequired() You must call these functions to change the state of the cleanup state machine (read more about the cleanup state machine for further details). Since these states can change due to external events, you can call these as slots if you like. If you never require any cleanup, you can call setCleanedUp() in your constructor.
  - setAcquiring(), setAcquisitionSucceeded(), setAcquisitionFailed(), setCancelling(), setCancelled(), setReadyForAcquisition(), setNotReadyForAcquisition() You must call these functions to change the state of the acquisition state machine (read more about the acquisition state machine for further details). Since these states can change due to external events, you can call these as slots if you like.

  - setAutoSetInitializing(), setAutoSetCancelling(), setAutoSetCleaningUp() You must call these functions in your constructor if you wish to redefine the automatic behavior for emitting these signals. If you expect external events to always signal these events (whether you call them in Acquaman or whether they happen elsewhere) then you can emit all of the signals yourself by turning these off.
  !*/

#define AMDETECTOR_NOTIFIED_INITIALIZING_UNEXPECTEDLY 470001
#define AMDETECTOR_NOTIFIED_INITIALIZED_UNEXPECTEDLY 470002
#define AMDETECTOR_NOTIFIED_INITIALIZATIONREQUIRED_UNEXPECTEDLY 470003

#define AMDETECTOR_NOTIFIED_ACQUIRING_UNEXPECTEDLY 470004
#define AMDETECTOR_NOTIFIED_CANCELLING_UNEXPECTEDLY 470005
#define AMDETECTOR_NOTIFIED_CANCELLED_UNEXPECTEDLY 470006
#define AMDETECTOR_NOTIFIED_SUCCEEDED_UNEXPECTEDLY 470007
#define AMDETECTOR_NOTIFIED_FAILED_UNEXPECTEDLY 470008
#define AMDETECTOR_NOTIFIED_READYFORACQUISITION_UNEXPECTEDLY 470009
#define AMDETECTOR_NOTIFIED_NOTREADYFORACQUISITION_UNEXPECTEDLY 470010

#define AMDETECTOR_NOTIFIED_CLEANINGUP_UNEXPECTEDLY 470011
#define AMDETECTOR_NOTIFIED_CLEANEDUP_UNEXPECTEDLY 470012
#define AMDETECTOR_NOTIFIED_CLEANUPREQUIRED_UNEXPECTEDLY 470013

#define AMDETECTOR_DEFAULT_TIMEOUT_MS 5000

class AMDSClientDataRequest;

/// Small class to help with continuous data API
class AMDetectorContinuousMotionRangeData
{
public:
	/// Detectors supporting continuous data can create these objects containing indices for the start index and end index of motion in a vector, as well as the index of which vector was used if a list of vectors was given
	AMDetectorContinuousMotionRangeData(int motionStartIndex = -1, int motionEndIndex = -1, int listIndex_ = -1);

	/// Each detector will know how to determine was constitues the start of motion, this return the index in the vector
	int motionStartIndex() const { return motionStartIndex_; }
	/// Each detector will know how to determine was constitues the end of motion, this return the index in the vector
	int motionEndIndex() const { return motionEndIndex_; }

	/// If a list of vectors was given, this value represents which item in the list was chosen
	int listIndex() const { return listIndex_; }

	/// Returns valid if and only if all three member variables are not equal to -1
	bool isValid() const;

	/// Sets the start motion index
	void setMotionStartIndex(int motionStartIndex) { motionStartIndex_ = motionStartIndex; }
	/// Sets the end motion index
	void setMotionEndIndex(int motionEndIndex) { motionEndIndex_ = motionEndIndex; }
	/// Sets the list index
	void setListIndex(int listIndex) { listIndex_ = listIndex; }

protected:
	/// Each detector will know how to determine was constitues the start of motion
	int motionStartIndex_;
	/// Each detector will know how to determine was constitues the end of motion
	int motionEndIndex_;
	/// If a list of vectors was given, this value represents which item in the list was chosen
	int listIndex_;
};

class AMDetector : public QObject
{
Q_OBJECT

public:
	/// This enum describes the states a detector can take on with regard to acquisition
	enum AcqusitionState { NotReadyForAcquisition = 0,
				   ReadyForAcquisition = 1,
				   Acquiring = 2,
				   Cancelling = 3,
				   Cancelled = 4,
				   Succeeded = 5,
				   Failed = 6
				 };

	/// This enum describes the states a detector can take on with regard to initialization
	enum InitializationState { InitializationRequired = 0,
				   Initializing = 1,
				   Initialized = 2
				 };

	/// This enum describes the states a detector can take on with regard to cleanup
	enum CleanupState { CleanupRequired = 0,
				CleaningUp = 1,
				CleanedUp = 2
			  };

	/// Default constructor. \c name is a unique programmer name to access this detector with. \c description is a human-readable version
	AMDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~AMDetector();

	/// One feature of a detector is that it can create a snapshot of its current state and pass it on as an AMDetectorInfo.
	AMDetectorInfo toInfo() const;

	/// Access the programmer unique name
	QString name() const { return objectName(); }
	/// Access a human-readable description
	QString description() const { return description_; }
	/// Access the describing this detector's readings (ex: "counts", "milliAmps", etc.)
	QString units() const { return units_; }

	/// Returns the number of dimensions in the output of this detector. A single point has rank 0. A spectrum output would have rank 1. An image output would have rank 2.
	virtual int rank() const { return axes_.size(); }
	/// Returns the size (ie: number of elements) along each dimension of the detector.  For a single-point detector, returns an empty AMnDIndex(). For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const;
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const;
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo>  axes() const { return axes_; }

	/// Returns (or casts) this AMDetector as an AMMeasurementInfo, which contains the bare-bones dimensional information.
	operator AMMeasurementInfo();

	/// Returns the current connected state (whether the detector has a connection to its constituent elements)
	bool isConnected() const { return connected_; }
	/// Returns whether or not the detector has timed out
	bool isTimedOut() const { return timedOut_; }
	/// Returns how long this detector will wait before timing out in milliseconds
	int timeOutMS() const { return timeOutMS_; }
	/// Returns the current powered state (whether the detector has it's high voltage on). Also see requiresPower().
	bool isPowered() const { return powered_; }
	/// Returns whether a detector requires power (high voltage, likely) to operate. Every detector subclass need to implement this function.
	virtual bool requiresPower() const = 0;

	/// Returns whether a detector can (generally) have its acquisition cancelled
	virtual bool canCancel() const = 0;
	/// Returns whether a detector can (generally) be cleared
	virtual bool canClear() const = 0;
	/// Returns whether a detector is capable of running in continuous mode
	virtual bool canContinuousAcquire() const = 0;

	/// Returns the current acquisition dwell time which is only relevant for triggered (RequestRead) detectors
	virtual double acquisitionTime() const = 0;
	/// Returns whether \param value is already within acquisition time tolerance of the detector.
	bool acquisitionTimeWithinTolerance(double value) const;
	/// Returns the acquisition time tolerance.  This is defined by subclasses because this limitation will likely be detector specific.
	virtual double acquisitionTimeTolerance() const = 0;

	/// Returns whether the detector can do current correction.
	virtual bool canDoDarkCurrentCorrection() const { return false;}
	/// Returns the current dark current measurement value.
	virtual double darkCurrentValue() const;
	/// Returns the dark current measurement time.
	virtual double darkCurrentTime() const;
	/// Returns the valid state of the current dark current measurement.
	virtual bool darkCurrentValidState() const;

	/// Returns the current acquisition state
	AMDetector::AcqusitionState acquisitionState() const { return acquisitionState_; }
	/// Returns a string describing the given state
	QString acquisitionStateDescription(AMDetector::AcqusitionState state);
	/// Returns true if the detector is currently acquiring.
	bool isAcquiring() const { return acquisitionState() == AMDetector::Acquiring; }
	/// Returns true if the detector is currently cancelling an acquisition
	bool isCancellingAcquisition() const { return acquisitionState() == AMDetector::Cancelling; }
	/// Returns true if the detector has just cancelled an acquisition
	bool isAcquisitionCancelled() const { return acquisitionState() == AMDetector::Cancelled; }
	/// Returns true if the detector has just succeeded in an acquisition
	bool isAcquisitionSucceeded() const { return acquisitionState() == AMDetector::Succeeded; }
	/// Returns true if the detector has just failed in an acquisition
	bool isAcquisitionFailed() const { return acquisitionState() == AMDetector::Failed; }
	/// Returns true if the detector is not ready for acquisition but also not currently acquiring
	bool isNotReadyForAcquisition() const { return acquisitionState() == AMDetector::NotReadyForAcquisition; }
	/// Returns true if the detector is ready to start an acquisition
	bool isReadyForAcquisition() const { return acquisitionState() == AMDetector::ReadyForAcquisition; }

	/// Returns the current initialization state
	AMDetector::InitializationState initializationState() const { return initializationState_; }
	/// Returns a string describing the given state
	QString initializationStateDescription(AMDetector::InitializationState state);
	/// Returns whether or not this detector currently requires initialization
	bool requiresInitialization() const { return initializationState() == AMDetector::InitializationRequired; }
	/// Returns whether or not this detector is currently initializing
	bool isInitializing() const { return initializationState() == AMDetector::Initializing; }
	/// Returns whether or not this detector has been initialized
	bool isInitialized() const { return initializationState() == AMDetector::Initialized; }

	/// Returns the current cleanup state
	AMDetector::CleanupState cleanupState() const { return cleanupState_; }
	/// Returns a string describing the given state
	QString cleanupStateDescription(AMDetector::CleanupState state);
	/// Returns whether or not this detector currently requires cleanup
	bool requiresCleanup() const { return cleanupState() == AMDetector::CleanupRequired; }
	/// Returns whether or not this detector is currently cleaning up
	bool isCleaningUp() const { return cleanupState() == AMDetector::CleaningUp; }
	/// Returns whether or not this detector has been cleaned up after use (probably a scan)
	bool isCleanedUp() const { return cleanupState() == AMDetector::CleanedUp; }

	/// Returns whether or not this detector can be coordinated with a synchronized dwell system
	virtual bool supportsSynchronizedDwell() const = 0;
	/// Returns whether or not this detector is currently interfaced with a synchronized dwell system by querying the AMBeamline class. You may reimplement if you wish.
	virtual bool currentlySynchronizedDwell() const;
	/// Returns the key for this detector (for matching with synchronized dwell application)
	virtual QString synchronizedDwellKey() const = 0;

	/// Returns whether to not this detector shares a triggering source, such as synchronized dwell time or the main scaler trigger. Default implementation returns false.
	virtual bool sharesDetectorTriggerSource() const { return false; }
	/// Returns the trigger source for this detector. Default implementation returns a NULL pointer.
	virtual AMDetectorTriggerSource* detectorTriggerSource() { return 0; }
	/// Returns the dwell time source for this detector. Default implementation returns a NULL pointer
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource() { return 0; }

	/// Returns the read method for this detector
	virtual AMDetectorDefinitions::ReadMethod readMethod() const = 0;
	/// Returns the read mode for this detector
	virtual AMDetectorDefinitions::ReadMode readMode() const = 0;

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const = 0;

	/// Returns a total single reading for the detector (the value for 0D, possibliy a sum for 1D). If this is not possible, the default implementation is sufficient to return an invalid AMNumber.
	virtual AMNumber singleReading() const { return AMNumber(AMNumber::Null); }

	/// Copies a block of values from \c indexStart to \c indexEnd, into \c outputValues. 0D is an convenience call to reading, 1D copies a slice, 2D copies a plane.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
	/*! The base-class implementation simply calls reading() repeatedly, so you should absolutely re-implement this for better performance.

	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

\code
int outputSize = indexStart.totalPointsTo(indexEnd);
\endcode
*/
	virtual bool reading0D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;
	virtual bool reading1D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;
	virtual bool reading2D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;

	/// Convenience call to access the three previous calls from a common interface.
	bool readingND(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;

	/// If the detector supports the AMDSClientDataRequest, this will return an AMDSClientDataRequest pointer for the specified period of time
	virtual AMDSClientDataRequest* lastContinuousData(double seconds) const;
	/// Sets the continuous data window for ContinuousMode reads. This will be how much data is requested on the next trigger/acquire
	virtual bool setContinuousDataWindow(double continuousDataWindowSeconds);
	/// Returns the AMDS Buffer Name of this detector if there is one. A null string should be returned if there isn't one.
	virtual QString amdsBufferName() const;
	/// If the detector supports AMDS server AND it's a polled detector, this returns the base polling rate. Otherwise returns -1
	virtual int amdsPollingBaseTimeMilliseconds() const;

	/// Fills the given double pointer with the current detector data in row-major order (first axis varies slowest).  Memory must be preallocated to the size of the detector data.
	virtual bool data(double *outputValues) const = 0;

	/// Returns a newly created action (possibly list of actions) to perform the detector initialization
	virtual AMAction3* createInitializationActions();

	/// Returns a newly created action to set the acquisition time on this detector
	virtual AMAction3* createSetAcquisitionTimeAction(double seconds);

	virtual AMAction3* createAcquisitionAction(AMDetectorDefinitions::ReadMode readMode = AMDetectorDefinitions::SingleRead);

	virtual AMAction3* createTriggerAction(AMDetectorDefinitions::ReadMode readMode = AMDetectorDefinitions::SingleRead);
	virtual AMAction3* createReadAction();

	/// Returns a newly created action (possibly list of actions) to perfrom the detector cleanup
	virtual AMAction3* createCleanupActions();

	/// Returns new action that sets the last measurement value and acquisition time as the dark current value and time, if the detector can do dark current correction. Updates the dark current valid state to true, as the DC value and time are up-to-date.
	virtual AMAction3* createSetLastMeasurementAsDarkCurrentAction();
	/// Returns new action that sets the dark current value, if the detector can do dark current correction.
	virtual AMAction3* createSetDarkCurrentValueAction(double newValue);
	/// Returns new action that sets the dark current time, if the detector can do dark current correction.
	virtual AMAction3* createSetDarkCurrentTimeAction(double newTime);
	/// Returns new action that sets the dark current valid state, if the detector can do dark current correction.
	virtual AMAction3* createSetDarkCurrentValidStateAction(bool isValid);

	/// Returns a data source for viewing this detector's output
	virtual AMDataSource* dataSource() const = 0;

	/// Handles the default visibility of the detector when added to a scan.
	bool isVisible() const { return isVisible_; }
	/// Handles the default accessibility of the detector when added to a scan.  If true, this detector will be completely hidden within the user interface.
	bool hiddenFromUsers() const { return hiddenFromUsers_; }

	/// Changes the default visibility of the detector when added to a scan.
	void setIsVisible(bool visible);
	/// Changes the default accessibility of the detector when added to a scan.
	void setHiddenFromUsers(bool hidden);

	/// Implemented to return a mapping from baseData to the applicable range data. Each detector will need to know its own specific information.
	virtual AMDetectorContinuousMotionRangeData retrieveContinuousMotionRangeData(const QList<QVector<qint32> > &baseData, int expectedDuration = -1, int threshold = -1);

public slots:
	// External requests to change the state (initialization, acquisition, cleanup): initialize(), acquire(), cancelAcquisition(), cleanup()
	//////////////////
	// All of these requests to change the state return false if not allowed from the current state.

	/// Initializes the detector (prepares it for acquisition). Allowed from InitializationRequired. State will change to Initializing if autoSetInitialzing has not been changed by the subclass.
	bool initialize();

	/// Triggers the detector to acquire. Allowed from ReadyForAcquisition.
	/// For triggered detectors (RequestRead), starts the acquisition process. For other detectors (ImmediateRead, WaitRead), does nothing. Returns if the acquistion started successfully. Pass in the type of read you wish to trigger.
	bool acquire(AMDetectorDefinitions::ReadMode readMode = AMDetectorDefinitions::SingleRead);

	/// Cancels the acquisition if this is possible. Allowed from Acquiring. State will change to Cancelling if autoSetCancelling has not been changed by the subclass.
	/// For triggered detectors (RequestRead), cancels the read if this is possible. Returns whether or not the cancelation was possible.
	bool cancelAcquisition();

	/// Cleans up the detector after its use is complete (likely after a scan). Allowed from CleanupRequired. State will change to CleaningUp if autoSetCleaningUp has not been changed by the subclass.
	bool cleanup();

	// Other non-state slots requests

	/// Sets the read mode for this detector (if possible, check with canContinuousAcquire)
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode) = 0;

	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds) = 0;

	/// For clearable detectors, clears the current data. Returns whether the clear was possible.
	bool clear();

	/// Sets the last measurement value and acquisition time as the dark current value and time, if the detector can do dark current correction. Updates the dark current valid state to true, as the DC value and time are up-to-date.
	virtual void setLastMeasurementAsDarkCurrent();
	/// Sets the dark current value, if the detector can do dark current correction.
	virtual void setDarkCurrentValue(double newValue);
	/// Sets the dark current time, if the detector can do dark current correction.
	virtual void setDarkCurrentTime(double newTime);
	/// Sets the dark current valid state, if the detector can do dark current correction.
	virtual void setDarkCurrentValidState(bool isValid);

	/// Some detectors must be armed before they can be acquired. By default, this fuction emits the armed() signal. To use this feature, subclasses by implement their own version of the arm() function.
	virtual void arm();

signals:
	/// Indicates that the detector's constituent elements are connected (each detector sub class can define this however makes most sense)
	void connected(bool isConnected);
	/// Indicates that the detector failed to connected within the defined timeout
	void timedOut();
	/// Indicates that the detector is currently powered (has it's high voltage on). Also see requiresPower().
	void powered(bool isPowered);

	/// Indicates that the detector's acquisition state has changed somehow
	void acquisitionStateChanged(AMDetector::AcqusitionState newState);
	/// Indicates that the detector's initialization state has changed somehow
	void initializationStateChanged(AMDetector::InitializationState newState);
	/// Indicates that the detector's cleanup state has changed somehow
	void cleanupStateChanged(AMDetector::CleanupState newState);

	/// Synonymous for acquisitionStateChanged(ReadyForAcquisition)
	void readyForAcquisition();
	/// Synonymous for acquisitionStateChanged(NotReadyForAcquisition)
	void notReadyForAcquisition();
	/// Synonymous for acquisitionStateChanged(Acquiring)
	void acquiring();
	/// Synonymous for acquisitionStateChanged(Cancelling)
	void acquisitionCancelling();
	/// Synonymous for acquisitionStateChanged(Cancelled)
	void acquisitionCancelled();
	/// Synonymous for acquisitionStateChanged(Succeeded)
	void acquisitionSucceeded();
	/// Synonymous for acquisitionStateChanged(Failed)
	void acquisitionFailed();

	/// Synonymous for initializationStateChanged(InitializationRequired)
	void initializationRequired();
	/// Synonymous for initializationStateChanged(Initializing)
	void initializing();
	/// Synonymous for initializationStateChanged(Initialized)
	void initialized();

	/// Synonymous for cleanupStateChanged(CleanupRequired)
	void cleanupRequired();
	/// Synonymous for cleanupStateChanged(CleaningUp)
	void cleaningUp();
	/// Synonymous for cleanupStateChanged(CleanedUp)
	void cleanedUp();

	/// Indicates that the detector's acquisition read mode has changed
	void readModeChanged(AMDetectorDefinitions::ReadMode readMode);
	/// Indicates that the detector's acquisition time has changed
	void acquisitionTimeChanged(double seconds);
	void newValuesAvailable();

	/// Indicates that the axis values have changed.  This would affect things like size() and axes().
	void axisValuesChanged();

	/// Notifier that the default visibility of the detector has changed.
	void isVisibleChanged(bool);
	/// Notifier that the default accessibility of the detector has changed.
	void hiddenFromUsersChanged(bool);

	/// New dark current correction value ready, passes new dark current value.
	void darkCurrentValueChanged(double newValue);
	/// Notifier that the dark current dwell time has changed, passes new dwell time value.
	void darkCurrentTimeChanged(double newTime);
	/// Notifier that the dark current valid state has changed, passes the new state.
	void darkCurrentValidStateChanged(bool isValid);

	/// Some detectors must be armed before they can be acquired. By default, this signal is emitted immediately after the arm() function is called. To use this feature, subclasses by implement their own version of the arm() function.
	void armed();

protected slots:
	///
	void setInitializing();
	/// Call this after receiving initializeImplementation() to inform the base class that the detector has been initialized, and we should go from Initializing to Initialized
	void setInitialized();
	/// Call this after receiving initializeImplementation() to inform the base class that the detector initialization has failed, and we should go from Initializing to InitializationRequired
	void setInitializationRequired();

	/// Call this after receiving acquireImplementation() to inform the base class that the detector has started the acquisition, and we should go from ReadyForAcquisition to Acquiring
	void setAcquiring();
	/// Call this to inform the base class that the detector has succeeded in its acquisition, and we should go from Acquiring to Succeeded
	void setAcquisitionSucceeded();
	/// Call this to inform the base class the the detector has failed in its acquisition, and we should go from Acquiring to Failed
	void setAcquisitionFailed();
	///
	void setAcquisitionCancelling();
	/// Call this after receiving cancelImplementation() to inform the base class that the acquisition has been cancelled, and we should go from Cancelling to Cancelled
	void setAcquisitionCancelled();

	/// Call this to inform the base class that the detector is ready for acquisition (or ready again). Either this or setNotReadyForAcquisition() must be called after the Succeeded, Failed, and/or Cancelled states to declare that the detector can start another acquisition. This can also be called when the state is NotReadyForAcquisition to declare that the detector is now ready.
	void setReadyForAcquisition();
	/// Call this to inform the base class that the detector is NOT ready for acquisition (or somehow stopped being ready). Either this or setReadyForAcquisition() must be called after the Succeeded, Failed, and/or Cancelled states to declare that the detector cannot start another acquisition. This can also be called when the state is ReadyForAcquisition to declare that the detector is no longer ready.
	void setNotReadyForAcquisition();

	///
	void setCleaningUp();
	/// Call this after receiving cleanupImplementation() to inform the base class that the detector has been cleaned up, and we should go from CleaningUp to CleanedUp
	void setCleanedUp();
	/// Call this after receiving cleanupImplementation() to inform the base class that the detector clean up has failed, and we should go to from CleaningUp to CleanupRequired
	void setCleanupRequired();

	/// Updates the most recently saved dark current value.
	void updateDarkCurrentValue(double newValue);
	/// Updates the most recently saved dark current dwell time.
	void updateDarkCurrentTime(double newTime);
	/// Updates the most recently saved dark current valid state.
	void updateDarkCurrentValidState(bool newState);

protected:
	// The following functions are used to define the unique behaviour of the detector.  We set them up in this way so that subclasses don't need to worry about (and cannot) break the state machine logic, they only need to fill in their pieces.

	// These pure-virtual functions allow subclasses to implement their unique action behaviour.  They are called at the appropriate time by the base class, when base-class-initiated state changes happen: ->Acquiring, ->Cancelling, ->Initializing, ->CleaningUp
	/////////////////////////

	/// This function is called from the Initializing (intialization) state when the implementation should initialize the detector. Once the detector is initialized you must call setInitialized(), if initialization has failed you must call setInitializationRequired()
	virtual bool initializeImplementation() = 0;
	/// This function is called from the Acquiring (acquisition) state when the implementation should start the detector's acquisition. Once the detector is done acquiring you must call setAcquisitionSucceeded or setAcquisitionFailed
	virtual bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode) = 0;
	/// This function is called from the Cancelling (acquisition) state for detectors that support cancelling acquisitions. Once the detector has successfully cancelled the acquisition you must call setAcquisitionCancelled()
	virtual bool cancelAcquisitionImplementation();
	/// This function is called from the CleaningUp (cleanup) state when the implementation should cleanup the detector. Once the detector is cleaned up you must call setCleanedUp(), if clean up has failed you must call setCleanupRequired()
	virtual bool cleanupImplementation() = 0;

	/// This function is called by clear(), it should internally clear the data. If the detector cannot support clearing, then it will fail before calling this function.
	virtual bool clearImplementation();

	/// Internal class trigger for setting the connected state. All AMDetector subclasses should call this to make sure that signals are emitted properly.
	void setConnected(bool isConnected);
	/// Internal class trigger for setting the powered state. Also see requiresPower(). All AMDetector subclasses should call this to make sure that signals are emitted properly.
	void setPowered(bool isPowered);

	/// Helper function for checking rank and bounds on readingXD functions
	bool checkValid(const AMnDIndex &startIndex, const AMnDIndex &endIndex) const;

	/// Changes the automatic behavior for calls to initialize()
	void setAutoSetInitializing(bool autoSetInitializing) { autoSetInitializing_ = autoSetInitializing; }
	/// Changes the automatic behvior for calls to cancelAcquisition()
	void setAutoSetCancelling(bool autoSetCancelling) { autoSetCancelling_ = autoSetCancelling; }
	/// Changes the automatic behavior for calls to cleanup()
	void setAutoSetCleaningUp(bool autoSetCleaningUp) { autoSetCleaningUp_ = autoSetCleaningUp; }

	/// Sets the timeout time to a given value
	void setTimeOutMS(int timeOutMS);

protected:
	/// A human-readable description
	QString description_;
	/// Units describing this detector's readings (ex: "counts", "milliAmps", etc.)
	QString units_;
	/// The axes that describe the dimensionality of the detector.
	QList<AMAxisInfo> axes_;
	/// The flag for the visibility of the detector when added to a scan.
	bool isVisible_;
	/// The flag for the default accessibility off the detector when added to a scan.
	bool hiddenFromUsers_;
	/// The most recent dark current value, already normalized by the dark current time.
	double darkCurrentValue_;
	/// The most recent dark current dwell time, in seconds.
	double darkCurrentTime_;
	/// Valid state of the most recent dark current measurement.
	bool darkCurrentValidState_;

private:
	/// Changes states in the acquisition state (if possible)
	void setAcquisitionState(AcqusitionState newState);
	/// Checks whether you canmake the transition to the new acquisition state
	bool acceptableChangeAcquisitionState(AcqusitionState newState) const;

	/// Changes states in the initialization state (if possible)
	void setInitializationState(InitializationState newState);
	/// Checks whether you canmake the transition to the new initialization state
	bool acceptableChangeInitializationState(InitializationState newState) const;

	/// Changes states in the cleanup state (if possible)
	void setCleanupState(CleanupState newState);
	/// Checks whether you canmake the transition to the new cleanup state
	bool acceptableChangeCleanupState(CleanupState newState) const;

	/// Helper function for cleaning up the timedOutTimer
	void timedOutTimerCleanup();

private slots:
	/// Instantiates and starts the timedOutTimer
	void initiateTimedOutTimer();
	/// Handles the timedOutTimer finishing and emit the timedOut signal
	void onTimedOutTimerTimedOut();

private:
	/// Internal state for connection, which referes to the constituent elements of the detector (likely AMControls) all being connected. Use setConnected(bool) to change so signals are emitted.
	bool connected_;
	/// Internal state for connection history. This value will start false and become true on the first connection.
	bool wasConnected_;
	/// Internal state for whether or not this detector is timed out
	bool timedOut_;
	/// Internal timer that controls the timed out signal
	QTimer *timedOutTimer_;
	/// Timeout that will be used in milliseconds
	int timeOutMS_;
	/// Internal state for powered, which refers to whether the detector is currently powered (many detectors require a high voltage to operate)
	bool powered_;

	/// The current acquisition state
	AcqusitionState acquisitionState_;
	/// The current initialization state
	InitializationState initializationState_;
	/// The current cleanup state
	CleanupState cleanupState_;

	/// When true, calls to initialize() automatically set the initialization state to Initializing before entering initializeImplementation()
	bool autoSetInitializing_;
	/// When true, calls to cancelAcquisition() automatically set the acquisition state to Cancelling before entering cancelAcquisitionImplementation()
	bool autoSetCancelling_;
	/// When true, calls to cleanup() automatically set the cleanup state to CleaningUp before entering cleanupImplementation()
	bool autoSetCleaningUp_;
};

#endif // AMDETECTOR_H
