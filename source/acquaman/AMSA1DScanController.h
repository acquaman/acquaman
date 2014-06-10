/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMSA1DSCANCONTROLLER_H
#define AMSA1DSCANCONTROLLER_H

#include "acquaman/AMScanController.h"

class AMControl;
class AMSADetector;
class AMRegionScanConfiguration;
//class AMInternalControlMoveAction;
class AMCDFDataStore;

/// Prototype of a 1-dimensional scan controller that steps through the regions defined in AMRegionScanConfiguration, using an AMControl for motion and an arbitrary set of detectors. Detectors need to implement the AMSADetector interface.
/*! This can be subclassed by re-implementing
- customInitializeImplementation(),
- customCleanupImplementation()
to do beamline-specific work prior to, and after, the scan runs.

-control(),
-currentPosition(), and
-currentAcquisitionTime()

can also be re-implemented; this might be sufficient to build an EXAFS scan controller with variable step sizes and acquisition (dwell) times.

This uses the CDF data store and file loader (AMCDFDataStore, AMCDFv1FileLoader), so you don't need to write a custom file loader plugin.
*/
class AMSA1DScanController : public AMScanController
{
	Q_OBJECT
public:
	/// Build a scan controller to move through the regions of \c scanConfig, using \c control to cause the motion, and a set of \c detectors to acquire the values at each step.
	AMSA1DScanController(AMControl* control, QList<AMSADetector*> detectors, AMRegionScanConfiguration* scanConfig, QObject* parent = 0);
	virtual ~AMSA1DScanController();

	/// When using the base implementation, you can use this to set the control that is used to move through the scan. Only works (and returns true) before the scan is initialized or run.
	bool setControl(AMControl* control);
	/// Set the detectors that are used to measure values at each step of the scan. Only works (and returns true) before the scan is initialized or run.
	bool setDetectors(const QList<AMSADetector*>& detectors);



signals:

public slots:

protected:
	// Re-implemented functions
	/////////////////////////////

	/// Implement to initialize the beamline and prepare the scan to run.  (ie: transition from Constructed to Initialized). Returns whether or not initialization can occur.
	/*! AMSA1DScanController has three (asynchronous) stages to initialization:
	  - Wait for the control and all the detectors to be connected. (Ends after successful reviewConnected()).
	  - Run any sub-class specific initialization (customInitializeImplementation(); ends after setCustomInitializationFinished().)
	  - Initialize all the detectors, using AMSADetector::init(), and wait for them to be initialized. (internalInitializeImplementation(), ends after successful onDetectorInitialized()).
	  */
	virtual bool initializeImplementation();
	/// Implement to start a scan (ie: transition from Initialized to Running).
	virtual bool startImplementation();

//	/// Implement to define whether or not your scan controller is capable of pausing (defaults to not able to pause). If you canPause (return true), then you need to implement a pauseImplementation and a resumeImplementation.
//	virtual bool canPause() const { return false; }
//	/// Implement to pause a running scan (ie: transition from Running to Paused).  After the scan is paused, call setPaused(). (Note: If it's not possible to pause this type of scan (ie: canPause() is false), this will never be called.)
//	virtual void pauseImplementation() {}
//	/// Implement to resume a paused scan (ie: transition from Paused to Running).  If pauseImplementation() returns false, you don't need to do anything here. After the scan is resumed, call setResumed().
//	virtual void resumeImplementation() {}

	/// Implement to cancel a scan. This could be called from any state.  After the scan is cancelled, call setCancelled().
	virtual void cancelImplementation();


	// Helper functions:
	////////////////////////////

	/// Start moving to the next measurement position
	void doNextPosition();
	/// Once in position, trigger the next acquisition (from each detector).
	void doNextAcquisition();

	/// Ensure the data file is fully stored to disk
	void flushRawData();

	/// Calculates the progress and estimated time remaining; emits progress() and timeRemaining().
	void emitProgressUpdate();

	/// Calculates the total number of steps through all regions, and stores in totalSteps_. Used for prediction of estimated time remaining.
	void computeTotalSteps();


	// Hooks to let subclasses determine which raw data sources and analysis blocks are created:
	/////////////////////////////

	/// The scan controller creates measurements for each detector in the data store. Re-implement this if you want to control which raw data sources are added to the scan. The base class creates one raw data source for each detector.
	virtual void createRawDataSources();
	/// Re-implement this if you want to add analysis blocks to the scan after it's created. The base class does not create any.
	virtual void createAnalysisBlocks() {}


	// Hooks to let subclasses add their own initialization and cleanup.
	/////////////////////////////

	/// Re-implement to set the scan's meta-data (name(), number(), sampleId(), runId(), etc.) differently than the base class.
	virtual void initializeScanMetaData();

	/// Re-implement to start any custom initialization required. (This is run before initializing the detectors).
	virtual bool customInitializeImplementation() { setCustomInitializationFinished(true); return true; }
	/// IMPORTANT: If you re-implement customInitializeImplementation(), call after any custom initialization is complete.
	void setCustomInitializationFinished(bool succeeded);


	/// Re-implement to start any custom cleanup required. (Will run after the scan is done, or after it is cancelled.)
	virtual void customCleanupImplementation() { setCustomCleanupFinished(); }
	/// IMPORTANT: If you re-implement customCleanupImplementation(), call after any custom cleanup is complete.
	void setCustomCleanupFinished();

	// Hooks to let subclasses modify the stepping behaviour
	/////////////////////////////

	/// Subclasses can optionally provide a different control to use (for example, at each step or region). The default implementation simply always uses the one that was provided in the constructor.
	virtual AMControl* control() { return control_; }

	/// Returns the control position for the currentRegion_ and currentStepInRegion_. The default implementation returns AMRegionScanConfiguration::regionStart(currentRegion_) + AMRegionScanConfiguration::regionDelta(currentRegion_)*currentStepInRegion_.
	virtual double currentPosition() const;

	/// Returns the acquisition time (dwell time) for the currentRegion_ and currentStepInRegion_. The default implementation returns AMRegionScanConfiguration::regionTime(currentRegion_).
	virtual double currentAcquisitionTime() const;


private:
	/// Implements initialization of detectors after customInitializeImplementation() has run.
	void internalInitializeImplementation();


protected slots:
	/// Called while waiting for the detectors and control to connect, when any changes.
	void reviewConnected();
	/// Called while waiting for connection: if the control and all the detectors is not connected by connectionTimeoutSeconds_
	void onConnectionTimeout();

	void onMoveActionSucceeded();
	void onMoveActionFailed();
	void onMoveActionStateChangedWhileCancelling();

	void onDetectorInitialized(bool succeeded = true);
	void onDetectorAcquisitionFinished(bool succeeded = true);
	void onAllAcquisitionFinished();


protected:
	AMControl* control_;
	QList<AMSADetector*> detectors_;
	AMRegionScanConfiguration* regionScanConfig_;

	AMCDFDataStore* dataStore_;

	QDateTime timeStarted_;
	int totalSteps_;
	int stepsCompleted_;

//	AMInternalControlMoveAction* moveAction_;

	int currentRegion_;
	int currentStepInRegion_;

	bool initializingDetectors_;
	bool triggeringDetectors_;

	bool detectorInitializationFailed_;
	bool detectorAcquisitionFailed_;

	double connectionTimeoutSeconds_;

};

#endif // AMSA1DSCANCONTROLLER_H
