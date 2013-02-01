#ifndef AMDETECTOR_H
#define AMDETECTOR_H

#include <QObject>

#include "dataman/AMMeasurementInfo.h"
#include "dataman/info/AMDetectorInfo.h"
#include "actions3/AMAction3.h"
#include "dataman/datasource/AMDataSource.h"

class AMDetectorTriggerSource;

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
  - lastContinuousReadingImplementation(double*) May be implemented to return the continuous reading. Default implementation returns false. Also, lastContinuousReading() automatically checks canContinuousAcquire() and returns false if the detector doesn't support it.
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

	/// One feature of a detector is that it can create a snapshot of its current state and pass it on as an AMDetectorInfo.
	AMDetectorInfo toInfo() const;

	/// Access the programmer unique name
	QString name() const { return objectName(); }
	/// Access a human-readable description
	QString description() const { return description_; }
	/// Access the describing this detector's readings (ex: "counts", "milliAmps", etc.)
	QString units() const { return units_; }

	/// Returns the number of dimensions in the output of this detector. A single point has rank 0. A spectrum output would have rank 1. An image output would have rank 2.
	virtual int rank() const { return 0; }
	/// Returns the size (ie: number of elements) along each dimension of the detector.  For a single-point detector, returns an empty AMnDIndex(). For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const { return AMnDIndex(); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const = 0;
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo>  axes() const { return QList<AMAxisInfo>(); }

	/// Returns (or casts) this AMDetector as an AMMeasurementInfo, which contains the bare-bones dimensional information.
	operator AMMeasurementInfo();

	/// Returns the current connected state (whether the detector has a connection to its constituent elements)
	bool isConnected() const { return connected_; }
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

	/// Returns the current acquisition state
	AMDetector::AcqusitionState acquisitionState() const { return acquisitionState_; }
	/// Returns a string describing the given state
	QString acquisitionStateDescription(AMDetector::AcqusitionState state);
	/// Returns true if the detector is currently acquiring.
	bool isAcquiring() const { return acquisitionState() == AMDetector::Acquiring; }
	/// Returns true if the detector is currently cancelling an acquisition
	bool isCancellingAcquisition() const { return acquisitionState() == AMDetector::Cancelling; }
	/// Returns true if the detector has just cancelled an acquisition
	bool acquisitionCancelled() const { return acquisitionState() == AMDetector::Cancelled; }
	/// Returns true if the detector has just succeeded in an acquisition
	bool acquisitionSucceeded() const { return acquisitionState() == AMDetector::Succeeded; }
	/// Returns true if the detector has just failed in an acquisition
	bool acquisitionFailed() const { return acquisitionState() == AMDetector::Failed; }
	/// Returns true if the detector is not ready for acquisition but also not currently acquiring
	bool isNotReadyForAcquisition() const { return acquisitionState() == AMDetector::NotReadyForAcquisition; }
	/// Returns true if the detector is ready to start an acquisition
	bool isReadyForAcquisition() const { return acquisitionState() == AMDetector::ReadyForAcquisition; }

	/// Returns the current initialization state
	AMDetector::InitializationState initializationState() const { return initializationState_; }
	/// Returns a string describing the given state
	QString initializationStateDescription(AMDetector::InitializationState state);
	/// Returns whether or not this detector currently requires initialization
	bool requiresInitialization() const { return initializationState() == AMDetector::requiresInitialization(); }
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
	virtual bool sharesDetectorTriggerSource() { return false; }
	/// Returns the trigger source for this detector. Default implementation returns a NULL pointer.
	virtual AMDetectorTriggerSource* detectorTriggerSource() { return 0; }

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
	// FINISH IMPLEMENTING THESE
	virtual bool reading0D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;
	virtual bool reading1D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;
	virtual bool reading2D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;

	/// Convenience call to access the three previous calls from a common interface. Pass 0, 1, or 2 into the dimensionality variable.
	bool readingND(int dimensionality, const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;

	// FLESH THIS ONE OUT
	/// Returns the data from the last continuous reading in the outputValues
	bool lastContinuousReading(double *outputValues) const;

	/// Returns a (hopefully) valid pointer to a block of detector data in row-major order (first axis varies slowest)
	virtual const double* data() const = 0;

	/// Returns a newly created action (possibly list of actions) to perform the detector initialization
	virtual AMAction3* createInitializationActions();

	/// Returns a newly created action to set the acquisition time on this detector
	virtual AMAction3* createSetAcquisitionTimeAction(double seconds) = 0;

	/// Returns a newly created action (possibly list of actions) to perfrom the detector cleanup
	virtual AMAction3* createCleanupActions();

	/// Returns a data source for viewing this detector's output
	virtual AMDataSource* dataSource() const = 0;

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

signals:
	/// Indicates that the detector's constituent elements are connected (each detector sub class can define this however makes most sense)
	void connected(bool isConnected);
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

	/// Emit this when the acquisition process starts and pass whether it was successful or not
	void acquisitionStarted(bool successfullyStarted);
	/// Emit this when the acquisition process finishes and pass whether it was successful or not
	void acquisitionFinished(bool successfullyFinished);

	/// Emit this signal when initialization starts
	void initializeStarted();
	/// Emit this signal when the initialization finishes and pass the success/failure
	void initializeFinished(bool initializationSucceeded);

	/// Emit this signal when cleanup starts
	void cleanupStarted();
	/// Emit this signal when the cleanup finishes and pass the success/failure
	void cleanupFinished(bool cleanupSucceeded);

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

protected:
	// The following functions are used to define the unique behaviour of the detector.  We set them up in this way so that subclasses don't need to worry about (and cannot) break the state machine logic; they only need to fill in their pieces.

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

	/// This function is called by lastContinuousReading(). It should place the values in the data pointer and return success/failure if your detector supports continuous reads.
	virtual bool lastContinuousReadingImplementation(double *outputValues) const;
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

protected:
	/// A human-readable description
	QString description_;
	/// Units describing this detector's readings (ex: "counts", "milliAmps", etc.)
	QString units_;

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

private:
	/// Internal state for connection, which referes to the constituent elements of the detector (likely AMControls) all being connected. Use setConnected(bool) to change so signals are emitted.
	bool connected_;
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

class AMDetectorInitializeActionInfo : public AMActionInfo3
{
Q_OBJECT
public:
	/// Constructor
	Q_INVOKABLE AMDetectorInitializeActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), QObject *parent = 0);

	/// Copy Constructor
	AMDetectorInitializeActionInfo(const AMDetectorInitializeActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMDetectorInitializeActionInfo* createCopy() const { return new AMDetectorInitializeActionInfo(*this); }

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Detector Initialization"; }

	/// Returns a pointer to our detectorInfo
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

	/// For database storing only
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { delete newLoadedObject; }

protected:
	/// The AMDetectorInfo that specifies which detector to initialize
	AMDetectorInfo detectorInfo_;
};

#define AMDETECTORINITIALIZEACTION_NO_VALID_DETECTOR 490001

class AMDetectorInitializeAction : public AMAction3
{
Q_OBJECT
public:
	/// Constructor. Requires and takes ownership of an existing AMDetectorInitializeActionInfo \c info.  Provides a AMDetector \param control that will be controlled.  If the default is used instead, then a lookup based on AMBeamline::exposedControls will be used instead.
	Q_INVOKABLE AMDetectorInitializeAction(AMDetectorInitializeActionInfo *info, AMDetector *detector = 0, QObject *parent = 0);
	/// Copy Constructor
	AMDetectorInitializeAction(const AMDetectorInitializeAction &other);
	/// Virtual copy constructor
	virtual AMAction3* createCopy() const { return new AMDetectorInitializeAction(*this); }

	/// Returns the detector that will be initialized
	AMDetector* detector() const { return detector_; }

	/// Specify that we cannot pause
	virtual bool canPause() const { return false; }
	/// Specify that we cannot skip
	virtual bool canSkip() const { return false; }

	/// Virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

protected:
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation() { setPaused(); }

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation() { setResumed(); }

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

	/// Since this action does not support skipping, the method is empty.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command); }

protected slots:
	/// Handle signals from our detector
	void onInitializeStarted();
	void onInitializeFinished(bool initializationSucceeded);

protected:
	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMDetectorInitializeActionInfo. This makes it easier to access.
	const AMDetectorInitializeActionInfo* detectorInitializeInfo() const { return qobject_cast<const AMDetectorInitializeActionInfo*>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMDetectorInitializeActionInfo. This makes it easier to access.
	AMDetectorInitializeActionInfo* detectorInitializeInfo() { return qobject_cast<AMDetectorInitializeActionInfo*>(info()); }

	// Internal variables:

	/// A pointer to the AMDetector we use to implement the action
	AMDetector* detector_;
};

class AMDetectorCleanupActionInfo : public AMActionInfo3
{
Q_OBJECT
public:
	/// Constructor
	Q_INVOKABLE AMDetectorCleanupActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), QObject *parent = 0);

	/// Copy Constructor
	AMDetectorCleanupActionInfo(const AMDetectorCleanupActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMDetectorCleanupActionInfo* createCopy() const { return new AMDetectorCleanupActionInfo(*this); }

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Detector Cleanup"; }

	/// Returns a pointer to our detectorInfo
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

	/// For database storing only
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { delete newLoadedObject; }

protected:
	/// The AMDetectorInfo that specifies which detector to initialize
	AMDetectorInfo detectorInfo_;
};

#define AMDETECTORCLEANUPACTION_NO_VALID_DETECTOR 490002

class AMDetectorCleanupAction : public AMAction3
{
Q_OBJECT
public:
	/// Constructor. Requires and takes ownership of an existing AMDetectorCleanupActionInfo \c info.  Provides a AMDetector \param control that will be controlled.  If the default is used instead, then a lookup based on AMBeamline::exposedControls will be used instead.
	Q_INVOKABLE AMDetectorCleanupAction(AMDetectorCleanupActionInfo *info, AMDetector *detector = 0, QObject *parent = 0);
	/// Copy Constructor
	AMDetectorCleanupAction(const AMDetectorCleanupAction &other);
	/// Virtual copy constructor
	virtual AMAction3* createCopy() const { return new AMDetectorCleanupAction(*this); }

	/// Returns the detector that will be initialized
	AMDetector* detector() const { return detector_; }

	/// Specify that we cannot pause
	virtual bool canPause() const { return false; }
	/// Specify that we cannot skip
	virtual bool canSkip() const { return false; }

	/// Virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

protected:
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation() { setPaused(); }

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation() { setResumed(); }

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

	/// Since this action does not support skipping, the method is empty.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command); }

protected slots:
	/// Handle signals from our detector
	void onCleanupStarted();
	void onCleanupFinished(bool cleanupSucceeded);

protected:
	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMDetectorCleanupActionInfo. This makes it easier to access.
	const AMDetectorCleanupActionInfo* detectorCleanupInfo() const { return qobject_cast<const AMDetectorCleanupActionInfo*>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMDetectorCleanupActionInfo. This makes it easier to access.
	AMDetectorCleanupActionInfo* detectorCleanupInfo() { return qobject_cast<AMDetectorCleanupActionInfo*>(info()); }

	// Internal variables:

	/// A pointer to the AMDetector we use to implement the action
	AMDetector* detector_;
};

#endif // AMDETECTOR_H
