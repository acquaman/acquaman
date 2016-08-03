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


#ifndef AMSCANACTION_H
#define AMSCANACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMScanActionInfo.h"

class AMScanController;

#define AMSCANACTION_NO_VALID_ACTION_INFO 103100
#define AMSCANACTION_CANT_CREATE_CONTROLLER 103101
#define AMSCANACTION_CANT_INITIALIZE_CONTROLLER 103104
#define AMSCANACTION_CANT_START_CONTROLLER 103105
#define AMSCANACTION_CANT_SAVE_TO_DB 103106
#define AMSCANACTION_NO_REGISTERED_EXPORTER 103108
#define AMSCANACTION_NO_REGISTERED_EXPORTER_OPTION 103109
#define AMSCANACTION_DATABASE_NOT_FOUND 103110
#define AMSCANACTION_CONTROLLER_NOT_VALID_FOR_AUTOEXPORT 103111
#define AMSCANACTION_INVALILD_NO_VALID_ACTION_INFO 103112
#define AMSCANACTION_INVALILD_BEAMLINE_CLAIMS_INVALID 103113

/*!
  * A class which adapts between an AMScanController and the AMAction3 system.
  * State instructions passed to this class will be translated and passed on down
  * to the controller, when relevant. Conversely relevant transition signal from
  * the controller will trigger the translated state transitions from the Action3
  * interface in this class.
  */
class AMScanAction : public AMAction3
{
	Q_OBJECT

public:
	/*!
	  * Creates an instance of a scan action with the provided scan action info.
	  * The created scan action will not have a valid scan controller.
	  */

	Q_INVOKABLE AMScanAction(AMScanActionInfo *info, QObject *parent = 0);

	/*!
	  * Creates an instance of a scan action which is not yet known to the database,
	  * with details and scan action info cloned from those of the provided scan
	  * action. The new scan action will be in the Constructed state.
	  */
	AMScanAction(const AMScanAction &other);

	/*!
	  * Virtual destructor of the scan action. If there is a valid scan controller
	  * then the scan controller is disconnected and scheduled for deletion.
	  */
	virtual ~AMScanAction();

	/*!
	  * Returns a reference to a newly created scan action which is not yet known
	  * to the database with details and scan action info cloned from this scan
	  * action. The new scan action will be in the Constructed state and is returned
	  * as a reference to a base AMAction3.
	  */
	virtual AMAction3 *createCopy() const { return new AMScanAction(*this); }

	/*!
	  * The scan controller which is currently controlling this scan action.
	  */
	AMScanController *controller() const { return controller_; }

	/*!
	  * The current validity of the scan action.
	  */
	virtual AMAction3::ActionValidity isValid();

	/*!
	  * A human readable message indicating why the scan action is not currently
	  * valid.
	  */
	virtual QString validationWarningMessage();

	// Re-implemented public functions.
	//////////////////////////////////////////////////

	/*!
	  * Whether the scan action has child sub-actions.
	  */
	virtual bool hasChildren() const { return false; }

	/*!
	  * The number of child sub-actions the scan actiosetupAndRunCleanupActionsn contains.
	  */
	virtual int numberOfChildren() const { return 0; }

	/*!
	  * Whether the scan action has the ability to pause.
	  */
	virtual bool canPause() const { return true; }

	/*!
	  * Whether the scan action has the ability to skip.
	  */
	virtual bool canSkip() const { return true; }

	/*!
	  * Whether it is possible to parallelize the scan action.
	  * NOTE: Currently scan actions CAN NOT be parallelized.
	  */
	virtual bool canParallelize() const { return false; }

protected slots:

	// Slots that handle the state transitions of the scan controller.
	////////////////////////////////////////////////////////////////////////

	/*!
	  * Handles the controller signalling that it has begun the initialization
	  * steps. Sets the status text for the action to "Initializing".
	  */
	void onControllerInitializing();

	/*!
	  * Handles the controller signalling that it has initialized. Attempts to
	  * transition the controller to the running state.
	  */
	void onControllerInitialized();

	/*!
	  * Handles the controller signalling that it has successfully started. Saves
	  * the scan to the database.
	  */
	void onControllerStarted();

	/*!
	  * Handles the controller signalling that the scan has been cancelled. If the
	  * scan is known to the database, then an attempt to resave it is made. Adapts
	  * the cancelled instruction to the Action3 interface.
	  */
	void onControllerCancelled();

	/*!
	  * Handles the controller signalling that the scan has failed. If the scan
	  * is known to the database, then an attempt to resave it is made. Adapts the
	  * failed instruction to the Action3 interface.
	  */
	void onControllerFailed();

	/*!
	  * Handles the controller signalling that the scan has succeeded. Saves the
	  * finshed scan to the database and, if the configuration specifies an auto-export,
	  * then an export is performed. Adapts the succeeded instruction to the Aciton3
	  * interface.
	  */
	void onControllerSucceeded();

	/*!
	  * Handles the controller signalling that it has begun the clean-up steps.
	  * Sets the status text for the aciton to "Cleaning up".
	  */
	void onControllerCleaningUp();

	/*!
	  * Handles the controller signaling a periodic status changed update. Re-emits
	  * the progress changed signal on the Action3 interface.
	  */
	void onControllerProgressChanged(double elapsed, double total);

	/*!
	  * Handles the controller signaling that it has made a state transition.
	  * Updates the status text to reflect the new state.
	  */
	void onControllerStateChanged(int fromState, int toState);

	/*!
	  * Prepares the scan action for deletion. The scan action will be added to
	  * the deletion queue when the logging for this scan has been completed.
	  */
	void checkReadyForDeletion();

//	void onReadyForDeletionChanged(bool isReady);

protected:

	/*!
	  * The default starting implementation for a scan action. Attempts to build
	  * a scan controller from the action info, starts the action and performs the
	  * controller's initialization.
	  */
	virtual void startImplementation();

	/*!
	  * The default pause implementation for a scan action. Attempts to pause
	  * the scan controller - if successful sets the status text to "Paused" and
	  * adapts the pause instruction to the Action3 interface.
	  */
	virtual void pauseImplementation();

	/*!
	  * The default resume implementation for a scan action. Attempts to resume
	  * the scan controller - if successful sets the status text to "Running" and
	  * adapts the resume instruction to the Action3 interface.
	  */
	virtual void resumeImplementation();

	/*!
	  * The default cancel implementation for a scan action. Cancels the controller
	  * then sets the status text to "Cancelling" and adapts the cancel instruction
	  * to the Action3 interface.
	  * NOTE: This will never be called if startImplementation() was never called.
	  * The base case will handle this case.
	  */
	virtual void cancelImplementation();

	/*!
	  * Stops the controller with the provided command string, then sets the
	  * action to succeeded on the Action3 interface.
	  * \param command ~ The command description to specify to stop the controller.
	  */
	virtual void skipImplementation(const QString &command);

	/*!
	  * Schedules the scan action for deletion. If the scan action contains a controller,
	  * the controller will also be cleaned up.
	  */
	virtual void scheduleForDeletionImplementation();

	/*!
	  * Saves the scan to the database and, if the controller's scan configuration
	  * is set to auto export, an export is performed.
	  */
	void autoExportScan();

protected:
	/// A pointer to the scan controller that this action is encapsulating.
	AMScanController *controller_;
	/// A pointer to the specific scan info this action uses.
	AMScanActionInfo *scanInfo_;

	bool hasValidScanController_;
};

#endif // AMSCANACTION_H
