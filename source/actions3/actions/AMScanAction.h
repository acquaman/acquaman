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
#define AMSCANACTION_CANT_CREATE_EXPORT_FOLDER 103107
#define AMSCANACTION_NO_REGISTERED_EXPORTER 103108
#define AMSCANACTION_NO_REGISTERED_EXPORTER_OPTION 103109
#define AMSCANACTION_DATABASE_NOT_FOUND 103110
#define AMSCANACTION_CONTROLLER_NOT_VALID_FOR_AUTOEXPORT 103111
#define AMSCANACTION_INVALILD_NO_VALID_ACTION_INFO 103112
#define AMSCANACTION_INVALILD_BEAMLINE_CLAIMS_INVALID 103113

class AMScanAction : public AMAction3
{
	Q_OBJECT

public:
	/// Constructor.  Takes an AMScanConfiguration \param config and builds a scan action around it.  This will create a scan controller which can be controlled from within the workflow.
	Q_INVOKABLE AMScanAction(AMScanActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	AMScanAction(const AMScanAction &other);
	/// Destructor.
	virtual ~AMScanAction();

	/// Creates an identical copy of the action with the exception of setting the state to Constructed.
	virtual AMAction3 *createCopy() const { return new AMScanAction(*this); }

	/// Returns a pointer to the scan controller that is encapsulated by this action.
	AMScanController *controller() const { return controller_; }

	/// Returns the ActionValidity of this scanAction
	virtual AMAction3::ActionValidity isValid();
	virtual QString notValidWarning();

	// Re-implemented public functions.
	//////////////////////////////////////////////////
	/// Pure virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Pure virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

	/// Scan actions have the ability to pause.
	virtual bool canPause() const { return true; }
	/// Scan actions MIGHT be able to skip.  It depends on which type of scan controller is being used.  In general, Dacq controllers can skip.
	virtual bool canSkip() const;
	/// Scan actions CAN NOT be parallelized.  This is for everyones sake, too many things need to be working syncronously.
	virtual bool canParallelize() const { return false; }

public slots:
	virtual void scheduleForDeletion();

protected slots:

	// Slots that handle the state transitions of the scan controller.
	////////////////////////////////////////////////////////////////////////

	/// Starts the scan controller if it was able to successfully initialize.
	void onControllerInitialized();
	/// Saves the scan to the database if the controller was able to successfully start.
	void onControllerStarted();
	/// Handles at the action level if the controller is cancelled.
	void onControllerCancelled();
	/// Handles at the action level if the controller fails.
	void onControllerFailed();
	/// Handles all the wrap up and clean up if the controller succeeds.
	void onControllerSucceeded();
	/// Handles the progress changed passed up from the controller.
	void onControllerProgressChanged(double elapsed, double total);
	/// Helper slot that updates the status text when the controller changes state.
	void onControllerStateChanged();

	void onReadyForDeletionChanged(bool isReady);

protected:
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();
	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation();
	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation();
	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();
	/// For the controllers that support skipping, this will do the necessary work.
	virtual void skipImplementation(const QString &command);

	/// Exports a the scan with the registered exporter and option when a scan successfully completes.
	void autoExportScan();
	/// Method that returns a string with the state of the scan controller.
	QString controllerStateString() const;

	/// A pointer to the scan controller that this action is encapsulating.
	AMScanController *controller_;
	/// A pointer to the specific scan info this action uses.
	AMScanActionInfo *scanInfo_;

	bool hasValidScanController_;
};

#endif // AMSCANACTION_H
