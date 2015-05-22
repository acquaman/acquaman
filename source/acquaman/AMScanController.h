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

/*!
  * An abstract class which uses state machine mechanics to monitor the running
  * of a scan.
  *
  * Subclasses which implement the scan controller interface must provide implementations
  * for all the pure virtual functions:
  *   - initializeImplementation()
  *   - startImplementation()
  *   - cancelImplementation()
  *   - stopImplementation(const &QString)
  * The appropriate notification functions (setInitialized(), setStarted(), setFinished(),
  * etc.) must also be called when the  implementation scan controller wants to
  * trigger a transition from one state to the next. (These might occur instantly
  * within the virtual state transition functions [for synchronous scan controllers],
  * or at some later time [for asynchronous scan controllers].
  */
class AMScanController : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool running READ isRunning)
	Q_PROPERTY(bool paused READ isPaused)

public:
	/*!
	  * Base constructor for a scan controller. Sets the configuration associated
	  * with the controller to the provided configuration but takes no action to
	  * initialize the scan. The created scan controller will be in the Constructed state.
	  */
	explicit AMScanController(AMScanConfiguration *configuration, QObject *parent = 0);

	/*!
	  * Base destructor for a scan controller.
	  */
	virtual ~AMScanController();

	/*!
	  * Enum which describes the states a scan controller can be in.
	  */
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

	/*!
	  * The current state of the scan controller.
	  */
	AMScanController::ScanState state() const;

	/*!
	  * The state the scan controller was in previous to the current state.
	  */
	AMScanController::ScanState lastState() const;

	// Convenience Functions to test the state of the scan.
	///////////////////////

	/*!
	  * Whether the scan controller is in the initializing state.
	  */
	bool isInitializing() const;
	/*!
	  * Whether the scan controller is in the initialized state.
	  */
	bool isInitialized() const;
	/*!
	  * Whether the scan controller is in the starting state.
	  */
	bool isStarting() const;

	/*!
	  * Whether the scan controller is in the running state.
	  */
	bool isRunning() const;

	/*!
	  * Whether the scan controller is in the pausing state.
	  */
	bool isPausing() const;

	/*!
	  * Whether the scan controller is in the paused state.
	  */
	bool isPaused() const;

	/*!
	  * Whether the scan controller is in the resuming state.
	  */
	bool isResuming() const;

	/*!
	  * Whether the scan controller is in the stopping state.
	  */
	bool isStopping() const;

	/*!
	  * Whether the scan controller is in the cancelling state.
	  */
	bool isCancelling() const;

	/*!
	  * Whether the scan controller is in the cancelled state.
	  */
	bool isCancelled() const;

	/*!
	  * Whether the scan controller is in the finished state.
	  */
	bool isFinished() const;

	/*!
	  * Whether the scan controller is in the failed state.
	  */
	bool isFailed() const;

	/*!
	  * Whether the scan controller is able to pause while running.
	  */
	bool controllerCanPause() const { return canPause(); }

	/*!
	  * The scan which this scan controller is controlling.
	  */
	virtual AMScan* scan() { return scan_; }

//	/// Returns whether the scan controller is in a state where it can be deleted.
//	virtual bool isReadyForDeletion() const;

signals:
	/*!
	  * Signal indicating that the state of the scan controller has altered.
	  * \param oldState ~ The state which the scan controller moved from.
	  * \param newState ~ The state which the scan controller has moved to.
	  */
	void stateChanged(int oldState, int newState);

	// The following signals are emitted for convenience for certain commonly-useful
	// occurences of stateChanged().
	////////////////////////////

	/*!
	  * Signal indicating that the scan controller has completed initialization.
	  */
	void initialized();

	/*!
	  * Signal indicating that the scan controller has moved from the starting
	  * state to the running state. Synonym for stateChanged(Starting, Running).
	  */
	void started();

	/*!
	  * Signal indicating that the scan controller has moved from the pausing
	  * state to the paused state. Synonym for stateChanged(Pausing, Paused).
	  */
	void paused();

	/*!
	  * Signal indicating that the scan controller has moved from the resuming
	  * state to the running state. Synonym for stateChanged(Resuming, Running)
	  */
	void resumed();

	/*!
	  * Signal indicating that the scan controller has moved from the running state
	  * to the finished state. Synonym for stateChanged(Running, Finished).
	  */
	void finished();

	/*!
	  * Signal indicating that the scan controller has moved to the cancelled
	  * state. Synonym for stateChanged(anything, Cancelled).
	  */
	void cancelled();

	/*!
	  * Signal indicating that the scan controller has moved to the failed state.
	  * Synonym for stateChanged(anything, Failed).
	  */
	void failed();

	/*!
	  * Signal indicating that the initialization action are starting.
	  * NOTE: This may not be emitted by all scan controllers.
	  */
	void initializingActionsStarted();

	/*!
	  * Signal indicating that the cleaning actions are being started.
	  * NOTE: This may not be emitted by all scan controllers.
	  */
	void cleaningActionsStarted();

	/*!
	  * Signal which indicates the time remaing for before the scan controller
	  * estimates it is finished.
	  * Classes implementing the scan controller interface should emit this signal
	  * periodically.
	  * \param seconds ~ The estimated time remaining before the scan controller
	  * is finished.
	  */
	void timeRemaining(double seconds);

	/*!
	  * Signal which emits the current progress of the scan controller in proportional
	  * terms.
	  * Classes implementing the scan controller interface should emit this signal
	  * periodically.
	  * \param elapsed ~ The elapsed progress of the scan controller in some arbitrary
	  * units.
	  * \param total ~ The total estimated progress required before the scan controller
	  * is finished in the same arbitrary units as the elapsed argument.
	  */
	void progress(double elapsed, double total);

//	/// Notifier that the scan controller deletion state has changed.  Passes the new state.
//	void readyForDeletion(bool isReady);

public slots:
	// Public API functions to control a scan
	//////////////////////////

	/*!
	  * Prepares the experiment/endstation/beamline/etc. for running the scan.
	  * After the initialized() signal is received, the scan is ready to start().
	  *
	  * NOTE: The abstract scan controller will move to the initializing state
	  * (if it can). Implementation classes are responsible for then correctly
	  * moving to the initialized state.
	  * \returns The if the scan controller can make the state transition,
	  * false otherwise.
	  */
	bool initialize();

	/*!
	  * Starts the scan running.
	  *
	  * NOTE: The abstract scan controller will move to the starting state
	  * (if it can). Implementation classes are responsible for then correctly
	  * moving to the running state.
	  * \returns True if the scan controller can make the state transition,
	  * false otherwise.
	  */
	bool start();

	/*!
	  * Pauses the scan if it is currently running.
	  *
	  * NOTE: The abstract scan controller will move to the pausing state
	  * (if it can). Implementation classes are responsible for then correctly moving
	  * to the paused state.
	  * \returns True if the scan controller can make the state transition,
	  * false otherwise.
	  */
	bool pause();

	/*!
	  * Resumes the scan if it is currently paused.
	  *
	  * NOTE: The abstract scan controller will move to the resuming state (if it
	  * can). Implementation classes are responsible for then correctly moving to
	  * the running state.
	  * \returns True if the scan controller can make the state transition,
	  * false otherwise.
	  */
	bool resume();

	/*!
	  * Attempts to cancel the scan.
	  *
	  * NOTE: The abstract scan controller will move to the cancelling state. Implementation
	  * classes are responsible for then moving to the cancelled state.
	  * \returns True if the scan controller can make the state transition,
	  * false otherwise.
	  */
	void cancel();
	/*!
	  * Stops the scan. The same action as cancel() but with the implication that
	  * the scan should still be considered complete.
	  * \param command ( = "" ) ~ An optional command which provides a general
	  * mechanism which sub-classes can use to determine how a stop should be performed
	  * or why.
	  */
	void stop(const QString &command = QString(""));

	/*!
	  * Tells the scan controller that it should clean up itself and its resources.
	  */
	virtual void scheduleForDeletion();

protected slots:
	// Protected notification functions.  Call these to notify the AMScanController
	// API when you want to trigger a transition between scan states.
	//////////////////

	/*!
	  * Sets the state of the scan controller to initialized. Implementation classes
	  * must call this after initialization is complete.
	  * \returns True if the scan controller can make the state transition,
	  * false otherwise.
	  */
	bool setInitialized();

	/*!
	  * Sets the state of the scan controller to running. Implementation classes
	  * must call this when the scan is running.
	  * \returns True if the scan controller can make the state transition,
	  * false otherwise.
	  */
	bool setStarted();

	/*!
	  * Sets the state of the scan controller to paused. Implementation classes
	  * must call this after pausing a scan.
	  * \returns True if the scan controller can make the state transition,
	  * false otherwise.
	  */
	bool setPaused();

	/*!
	  * Sets the state of the scan controller to resumed. Implementation classes
	  * must call this after resuming a scan.
	  * \returns True if the scan controller can make the state transition,
	  * false otherwise.
	  */
	bool setResumed();

	/*!
	  * Sets the state of the scan controller to cancelled. Implementation classes
	  * must call this after cancelling a scan is complete.
	  * \returns True if the scan controller can make the state transition,
	  * false otherwise.
	  */
	void setCancelled();

	/*!
	  * Sets the state of the scan controller to finished. Implementation classes
	  * must call this after the scan in finished.
	  * \returns True if the scan controller can make the state transition,
	  * false otherwise.
	  */
	bool setFinished();

	/*!
	  * Sets the state of the scan controller to failed. Implementation classes
	  * must call this in the event that a scan fails for any reason.
	  */
	void setFailed();

protected:
	// Virtual implementation functions.  Implement these as required for relevant
	// transitions between states.  The AMScanController API guarantees that these
	// will only be called from the states indicated.
	///////////////////

	/*!
	  * Performs the actions required to initialize the scan.
	  *
	  * In the base class this is a pure virtual function.
	  * \returns True if the transition to initialized can occur, false otherwise.
	  * NOTE: Returns whether or not initialization can occur, not
	  * if it is initialized. If you return true, scan object should be a valid
	  * object at this point. After initialization is complete, call setInitialized().
	  */
	virtual bool initializeImplementation() = 0;

	/*!
	  * Performs the actions required to start a scan running.
	  *
	  * In the base class this is a pure virtual function.
	  * \returns True if the transition to running can occur, false otherwise.
	  * NOTE: Returns whether or not the scan can be started, not that the scan
	  * has started. Pay attention to the started() signal to determine when a scan
	  * starts.
	  */
	virtual bool startImplementation() = 0;

	/*!
	  * Whether or not the scan controller is capable of pausing. Implementing
	  * classes should alter this from the default value of false if scan controller
	  * is capable of pausing.
	  */
	virtual bool canPause() const { return false; }

	/*!
	  * Performs the actions required to pause a scan. Implementing classes which
	  * override the canPause() function to indicate that their scan is capable
	  * of pausing must re-implement this function.
	  */
	virtual void pauseImplementation() {}

	/*!
	  * Performs the actions required to resume a scan from the paused state. Implementing
	  * classes which override the canPause() function to indicate that their scan
	  * is capable of pausing must re-implement this function.
	  */
	virtual void resumeImplementation() {}

	/*!
	  * Performs the actions required to cancel a scan.
	  * NOTE: This could be called from any state.
	  *
	  * In the case class this is  pure virtual function.
	  */
	virtual void cancelImplementation() = 0;

	/*!
	  * Performs the actions required to stop a scan.
	  * \param command ( = "" ) ~ An optional command which provides a general
	  * mechanism which sub-classes can use to determine how a stop should be performed
	  * or why.
	  */
	virtual void stopImplementation(const QString &command = QString("")) = 0;

protected:
	/*!
	  * The configuration for the scan which the scan controller is controlling.
	  */
	AMScanConfiguration *generalConfig_;

	/*!
	  * The scan which the scan controller is controlling.
	  */
	AMScan *scan_;

private:
	/*!
	  * Whether a transition from the current state to newState is allowed.
	  * \returns True if transitioning from the current state to newState can
	  * be performed, false otherwise.
	  */
	bool canChangeStateTo(ScanState newState);

	/*!
	  * Changes the state of the scan controller to the provided newState if a
	  * transition to newState is allowed from the current state. Emits the
	  * stateChanged(currentState, newState) signal in the event that the change
	  * is performed.
	  */
	bool changeState(ScanState newState);

private:
	/// The current state of the scan.
	AMScanController::ScanState state_;
	/// The last state of the scan before the current state.
	AMScanController::ScanState lastState_;
};

#endif // AM_SCANCONTROLLER_H
