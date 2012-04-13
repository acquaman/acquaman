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

	// Re-implemented public functions.
	//////////////////////////////////////////////////
	/// Pure virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Pure virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

	/// Scan actions have the ability to pause.
	virtual bool canPause() const { return true; }

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

	/// Method that returns a string with the state of the scan controller.
	QString controllerStateString() const;

	/// A pointer to the scan controller that this action is encapsulating.
	AMScanController *controller_;
};

#endif // AMSCANACTION_H
