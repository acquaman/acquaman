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


#ifndef AMSCANACTIONCONTROLLER_H
#define AMSCANACTIONCONTROLLER_H

#include "AMScanController.h"

#include "actions3/AMAction3.h"

class QThread;

#define AMSCANACTIONCONTROLLER_CANNOT_INTIALIZE 262001
#define AMSCANACTIONCONTROLLER_CANNOT_PAUSE 262002
#define AMSCANACTIONCONTROLLER_CANNOT_RESUME 262003
#define AMSCANACTIONCONTROLLER_CANNOT_CANCEL 262004

/*!
  * An abstract class which extends the AMScanController with those elements necessary
  * to interface with the AMAction and AMAgnosticData APIs. Scans which make use
  * of a scan action controller define the individual steps required to perform
  * a scan as tree of actions. The actions are defined in three sets:
  *  - Initialization Actions ~ A list of actions required to prepare the scan for
  *    running.
  *  - Scanning Actions ~ A list of actions required to actually perform the scan.
  *  - Cleanup Actions ~ A list of actions which are performed on completion of
  *	   the scan.
  */
class AMScanActionController : public AMScanController
{
	Q_OBJECT

public:

	/*!
	  * Enum which describes the final states which a scan action controller can
	  * be in. This is required for setting the final state after the cleanup actions
	  * have been executed.
	  */
	enum ScanningActionsFinalState
	{
		NotFinished = 0,
		Succeeded,
		Cancelled,
		Failed
	};

	/*!
	  * Base constructor for a scan action controller. Sets the configuration associated
	  * with the scan action controller to the provided configuration, but takes
	  * no action to initialize the scan. The created scan action controller will
	  * be in the Constructed state, with the final state as NotFinished.
	  */
	AMScanActionController(AMScanConfiguration *configuration, QObject *parent = 0);

	/*!
	  * Base empty virtual destuctor for a scan action controller.
	  */
	virtual ~AMScanActionController();

	/*!
	  * The master action which contains all the, possibly nested, actions required
	  * to run the scan.
	  */
	AMAction3 *scanningActions();

	/*!
	  * Function to build all the general aspects for the scan, such as measurements
	  * and raw data, and the file writer capabilities for the scan controller.
	  *
	  * In the base class this is a pure virutal function.
	  */
	virtual void buildScanController() = 0;

signals:

	/*!
	  * Signal indicating that the busy state of the file writer has been altered.
	  * \param fileWriterIsBusy ~ The new busy state of the file writer.
	  */
	void fileWriterIsBusyChanged(bool fileWriterIsBusy);

	/*!
	  * Signal indicating that the file writing activities are done after a scan
	  * has finished.
	  */
	void finishWritingToFile();

public slots:
	/*!
	  * Hanles the instruction that the scan controller should clean up itself
	  * and its resources.
	  */
	virtual void scheduleForDeletion();

protected slots:
	/*!
	  * Handles a change of state in the scan action controller. In the
	  * case that is an ending state (ie. Finished, Failed, Cancelled) then any
	  * open database transactions are committed, and this scan action controller
	  * is removed from the QEvent reciver list.
	  * \param oldState ~ The state the scan action controller has moved from (Unused)
	  * \param newState ~ The new state the scan action controller has moved to.
	  * The slot only takes action in the case that this is Finished, Failed or
	  * Cancelled.
	  */
	void onStateChanged(int fromState, int toState);

	/*!
	  * Handles the setting of the root action in the action tree. Optimizes and
	  * validates the action tree.
	  */
	virtual void onScanningActionsGenerated(AMAction3 *actionTree);

	/*!
	  * Handles the cleanup after the initialization actions have succeeded.
	  */
	virtual void onInitializationActionsListSucceeded();

	/*!
	  * Handles the cleanup after the initialization actions have been cancelled.
	  */
	virtual void onInitializationActionsListCancelled();

	/*!
	  * Handles the cleanup after the initialization actions fail.
	  */
	virtual void onInitializationActionsListFailed();
	virtual void onInitializationActionsListStateChanged(int fromState, int toState);

	/*!
	  * Handles the cleanup after the cleanup actions succeed.
	  */
	virtual void onCleanupActionsListSucceeded();

	/*!
	  * Handles the cleanup after the cleanup actions are cancelled.
	  */
	virtual void onCleanupActionsListCancelled();

	/*!
	  * Handles the cleanup after the cleanup actions have failed.
	  */
	virtual void onCleanupActionsListFailed();

	/*!
	  * Handles the scan actions tree completing successfully. Starts the cleanup
	  * actions list.
	  */
	virtual void onScanningActionsSucceeded();

	/*!
	  * Handles the scan actions tree being cancelled. Starts the cleanup actions
	  * list.
	  */
	virtual void onScanningActionsCancelled();

	/*!
	  * Handles the scan actions tree failing. Starts the cleanup actions list.
	  */
	virtual void onScanningActionsFailed();
	void onScanningActionsStateChanged(int fromState, int toState);

	/*!
	  * Handles the file writer's busy state being altered.
	  */
	void onFileWriterIsBusy(bool isBusy);

	/*!
	  * Handles the file writer being destroyed.
	  */
	void onFileWriterDestroyed();

	/*!
	  * Handles the file writer having finished. If the scan action controller has
	  * been scheduled for deletion the file writer thread is deleted. If the scan
	  * action controller can be set to the finished state then it will be set to
	  * finished.
	  */
	void onFileWriterThreadFinished();

	/*!
	  * Handles the case that the current action succeeds during a skip command.
	  */
	virtual void onSkipCurrentActionSucceeded();

protected:
	/*!
	  * Creates the specific details, such as analysis blocks, for the underlying
	  * scan controller.
	  *
	  * In the base class this is a pure virutal function.
	  */
	virtual void buildScanControllerImplementation() = 0;

	/*!
	  * Whether the current action being run can pause.
	  * \returns True if the current action can pause, false if the current action
	  * is not able to pause or if no current action is running.
	  */
	virtual bool canPause() const;

	/*!
	  * Builds the initialization actions and the cleanup actions and then starts
	  * the initialization actions list.
	  */
	virtual bool initializeImplementation();

	/*!
	  * Starts the scan action controller actions.
	  */
	virtual bool startImplementation();

	/*!
	  * Attempts to pause the current scan action. When/if the scan is successfully
	  * paused, setPaused() will be called.
	  */
	virtual void pauseImplementation();

	/*!
	  * Attempts to resume the current scan action. When/if the scan is successfully
	  * resumed, setResumed() will be called.
	  */
	virtual void resumeImplementation();

	/*!
	  * Attempts to cancel the current scan action. When/if the scan is successfully
	  * cancelled, setCancelled() will be called.
	  */
	virtual void cancelImplementation();

	/*!
	  * Attempts to stop the current scan action. When/if the scan is successfully
	  * stopped, setFinished() will be called.
	  */
	virtual void stopImplementation(const QString &command);

	/*!
	  * Builds all the actions required to initialize the scan.
	  *
	  * In the base class this creates no actions and returns 0.
	  * \returns The created action tree which, when run, will properly initalize
	  * the scan, or 0 if there are no initialization steps (default).
	  */
	virtual AMAction3 *createInitializationActions() { return 0; }

	/*!
	  * Builds all the actions required to cleanup the scan.
	  *
	  * In the base class this creates no actions and returns 0.
	  * \returns The created action tree which, when run, will properly cleanup
	  * the scan, or 0 if there are no cleanup actions (default).
	  */
	virtual AMAction3 *createCleanupActions() { return 0; }

	/*!
	  * Checks whether the scan action controller is in an appropriate state to
	  * transition to the finished state.
	  * \returns True if the scan aciton controller can transition to the finished
	  * state, false otherwise.
	  */
	bool readyForFinished() const;

protected:
	/*!
	  * Sets up and starts the running of the initialization actions.
	  */
	void setupAndRunInitializationActions();

	/*!
	  * Sets up and starts the running of the cleanup actions.
	  */
	void setupAndRunCleanupActions();

	/// The pointer that holds the scanning actions.	
	AMAction3 *scanningActions_;
	/// Flag holding whether the scan was successful or not.
	ScanningActionsFinalState scanningActionsFinalState_;
	/// The intialization actions for the scan.
	AMAction3 *initializationActions_;
	/// The cleanup actions for the scan.
	AMAction3 *cleanupActions_;

	/// Pointer to the thread that handles all the file writing.
	QThread *fileWriterThread_;
	/// Pointer to the file writing object
	QObject *fileWriter_;
	/// Flag to delete file writer when finished (used for cancel and fail)
	bool deleteFileWriterImmediately_;
	/// Flag for keeping track of whether the file writer thread is busy or not.
	bool fileWriterIsBusy_;
	/// Flag for keeping track of whether the state machine has gone through a real succeeded phase
	bool scanControllerStateMachineFinished_;
};

#endif // AMSCANACTIONCONTROLLER_H
