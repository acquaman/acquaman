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


#ifndef AM_SCANCONTROLLER_H
#define AM_SCANCONTROLLER_H

#include <QObject>

class QDateTime;

#include "AMScanConfiguration.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"

/// This class defines the interface for all Scan Controllers.  The Scan Controller API is modeled as a state machine, with the states such as initialized, running, paused, finished, etc.  To use a scan controller, call the public functions (initialize(), start(), pause(), etc.) or use an scan action to run it automatically.  Pay attention to the public signals (started(), paused(), cancelled(), finished(), etc.) to monitor the status of the scan.
/*!
<b>Implementing Custom Scan Controllers</b>

To implement your own scan controller, you must implement the pure virtual functions for the relevant state transitions (initializeImplementation(), startImplementation(), etc.).  You must also call the appropriate notification functions (setInitialized(), setStarted(), setFinished(), etc.) when your scan controller wants to trigger a transition from one state to the next.  (These might occur instantly within the virtual state transition functions [for synchronous scan controllers], or at some later time [for asynchronous scan controllers].

The top-level API handles emitting all the public signals for you. */

class AMScanController : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool running READ isRunning)
	Q_PROPERTY(bool paused READ isPaused)

public:
	explicit AMScanController(AMScanConfiguration *configuration, QObject *parent = 0);

	/// Destructor: If the scan_ has been created, releases it.
	virtual ~AMScanController();

	/// Enum describing the state/progress of the scan
	enum ScanState { Constructed,
			 Initializing,
			 Initialized,
			 Starting,
			 Running,
			 Pausing,
			 Paused,
			 Resuming,
			 Stopping,
			 Cancelling,
			 Cancelled,
			 Finished,
			 Failed };

	/// Returns the current state of the scan controller
	AMScanController::ScanState state() const;
	/// Returns the last state the scan controller was in before the current state
	AMScanController::ScanState lastState() const;

	// Convenience Functions to test the state of the scan.
	///////////////////////

	bool isInitializing() const;
	bool isInitialized() const;
	bool isStarting() const;
	bool isRunning() const;
	bool isPausing() const;
	bool isPaused() const;
	bool isResuming() const;
	bool isStopping() const;
	bool isCancelling() const;
	bool isCancelled() const;
	bool isFinished() const;
	bool isFailed() const;

	/// Public function to determine whether the implementation is able to pause while running. \see canPause().
	/*! (Necessary just because canPause() is protected.) */
	bool controllerCanPause() const { return canPause(); }

	/// Pointer to the scan this instance is controlling.
	virtual AMScan* scan() { return scan_; }

	/// Returns whether the scan controller is in a state where it can be deleted.
	virtual bool isReadyForDeletion() const;

signals:
	/// This signal provides public notification whenever the scan changes it's state() to \c newState.  The \c oldState that it is transitioning out of is also provided.  The state numbers (typed as integers for easy signal handling) correspond to the enum defined in AMScanController::ScanState.
	void stateChanged(int oldState, int newState);

	// The following signals are emitted for convenience for certain commonly-useful occurences of stateChanged().
	////////////////////////////

	/// Scan has finished initializing (ie: the beamline has been configured and moved into position, etc.), and is ready to be start()ed.  Synonym for stateChanged(Initializing, Initialized).
	void initialized();
	/// Scan has started. Synonym for stateChanged(Starting, Running).
	void started();
	/// Scan paused. Synonym for stateChanged(Pausing, Paused).
	void paused();
	/// Scan resumed from being paused. Synonym for stateChanged(Resuming, Running).
	void resumed();
	/// Scan completed. Synonym for stateChanged(Running, Finished).
	void finished();
	/// Scan canceled by user. Synonym for stateChanged(anything, Cancelled).
	void cancelled();
	/// Scan failed (due to some reason out of the user's control). Synonym for stateChanged(anything, Failed).
	void failed();

	/// Time left in scan. Implementations should emit this periodically
	void timeRemaining(double seconds);
	/// Progress of scan (arbitrary units: some amount \c elapsed of a \c total amount). Implementations should emit this periodically.
	void progress(double elapsed, double total);

	/// Notifier that the scan controller deletion state has changed.  Passes the new state.
	void readyForDeletion(bool isReady);


public slots:
	// Public API functions to control a scan
	//////////////////////////

	/// Call this to prepare the experiment/endstation/beamline/etc. for running the scan. After the initialized() signal is received, the scan is ready to start(). Return value is whether or not we are capable of this transition from the current state (we can only initialize if we are currently in the state Constructed)
	bool initialize();
	/// Start scan running if not currently running or paused. Return value is whether or not we are capable of this transition from the current state (we can only start if we are in the initialized state)
	bool start();
	/// Pause scan if currently running. Return value is whether or not we are capable of this transition from the current state (pause is only supported from the running state at the current time)
	bool pause();
	/// Resume scan if currently paused. Return value is whether or not we are capable of this transition from the current state (we can only resume if we are in the paused state)
	bool resume();
	/// Cancel the scan. We can always try to cancel.
	void cancel();
	/// Stop the scan.  The same action as cancel() but with the implication that the scan should still be considered complete.  Requires a command because there can be a variety of reasons for stopping a scan like this.  This is defaulted to an empty string because you may not need to distinguish between reasons.
	void stop(const QString &command = QString(""));

protected slots:
	// Protected notification functions.  Call these to notify the AMScanController API when you want to trigger a transition between scan states.
	//////////////////
	/// Implementations must call after initialization is complete. Triggers transition from Initializing to Initialized. Return value is whether or not we are capable of this transition from the current state (we can only become initialized if the current state is initialized)
	bool setInitialized();
	/// Implementations must call after start() is complete and the scan is running. Triggers transition from Starting to Started. Return value is whether or not we are capable of this transition from the current state (we can only become started if the current state is starting)
	bool setStarted();
	/// Implementations must call after pausing a scan. Triggers transition from Pausing to Paused. Return value is whether or not we are capable of this transition from the current state (we can only become paused if the current state is pausing)
	bool setPaused();
	/// Implementations must call after resuming a scan. Triggers a transition from Resuming to Running. Return value is whether or not we are capable of this transition from the current state (we can only attempt this transition if the current state is resuming)
	bool setResumed();
	/// Implementations must call after cancelling is complete. Triggers transition from Cancelling to Cancelled. We can go to cancelled from any state.
	void setCancelled();
	/// Implementations must call after the scan is finished. Triggers a transition from Running to Finished. Return value is whether or not we are capable of this transition from the current state (we can only become finished if the current state is running)
	bool setFinished();
	/// Implementations must call this if a scan fails for any reason. This means that the scan will not continue. Triggers a transition from [anything] to Failed. \todo How to express whether the scan failed with some data collected, or no data at all? Is this relevant to the user? We can fail from any state.
	void setFailed();

protected:
	// Virtual implementation functions.  Implement these as required for relevant transitions between states.  The AMScanController API guarantees that these will only be called from the states indicated.
	///////////////////

	/// Implement to initialize the beamline and prepare the scan to run.  (ie: transition from Constructed to Initialized). Returns whether or not initialization can occur, not if it is initialized. If you return true, scan object should be a valid object at this point.  After initialization is complete, call setInitialized().
	virtual bool initializeImplementation() = 0;
	/// Implement to start a scan (ie: transition from Initialized to Running).  Returns whether or not the scan can be started, not that the scan has started. After the scan is running, call setStarted().
	virtual bool startImplementation() = 0;
	/// Implement to define whether or not your scan controller is capable of pausing (defaults to not able to pause). If you canPause (return true), then you need to implement a pauseImplementation and a resumeImplementation.
	virtual bool canPause() const { return false; }
	/// Implement to pause a running scan (ie: transition from Running to Paused).  After the scan is paused, call setPaused(). (Note: If it's not possible to pause this type of scan (ie: canPause() is false), this will never be called.)
	virtual void pauseImplementation() {}
	/// Implement to resume a paused scan (ie: transition from Paused to Running).  If pauseImplementation() returns false, you don't need to do anything here. After the scan is resumed, call setResumed().
	virtual void resumeImplementation() {}
	/// Implement to cancel a scan. This could be called from any state.  After the scan is cancelled, call setCancelled().
	virtual void cancelImplementation() = 0;
	/// Implement to stop a scan.  This can only be called from the running or paused state.  This will be responsible for calling setFinished() or ensuring that it is called.  Pure-virtual because if you can cancel, then you can also stop.  This is defaulted to an empty string because you may not need to distinguish between reasons.
	virtual void stopImplementation(const QString &command = QString("")) = 0;

protected:
	/// Configuration for this scan, typed as the general base class AMScanConfiguration
	AMScanConfiguration *generalConfig_;
	/// The pointer to the scan.
	AMScan *scan_;

private:
	/// This function returns true if a state transition is allowed between the current state and \c newState.
	bool canChangeStateTo(ScanState newState);
	/// This function calls canChangeState() to see if the requested transition is allowed, and then writes in the actual state change and emits stateChanged().
	/*! Returns false if the transition is not allowed. This is used to ensure that implementations cannot call functions like notifyFinished(), notifyPaused(), etc. from states where that transition would not be legitimate.*/
	bool changeState(ScanState newState);

private:
	/// The current state of the scan.  Private because implementations must use the protected notification functions (setStarted(), setFinished(), etc.) to change this, so that signals are properly emitted.
	AMScanController::ScanState state_;
	/// The last state of the scan before the current state. Helpful for making decisions.
	AMScanController::ScanState lastState_;
};

#endif // AM_SCANCONTROLLER_H
