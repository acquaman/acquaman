#ifndef AMSCANACTION_H
#define AMSCANACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMScanActionInfo.h"

#define AMSCANACTION_CANT_CREATE_CONTROLLER 103101
#define AMSCANACTION_SCAN_CANCELLED 103102
#define AMSCANACTION_CANT_SET_CURRENT_CONTROLLER 103103
#define AMSCANACTION_CANT_INITIALIZE_CONTROLLER 103104
#define AMSCANACTION_CANT_START_CONTROLLER 103105
#define AMSCANACTION_CANT_SAVE_TO_DB 103106
#define AMSCANACTION_CANT_CREATE_EXPORT_FOLDER 103107
#define AMSCANACTION_NO_REGISTERED_EXPORTER 103108
#define AMSCANACTION_NO_REGISTERED_EXPORTER_OPTION 103109
#define AMSCANACTION_NO_DATABASE_FOR_SCAN 103110

class AMScanAction : public AMAction3
{
	Q_OBJECT

public:
	/// Constructor.  Takes an AMScanConfiguration \param config and builds a scan action around it.  This will create a scan controller which can be controlled from within the workflow.
	Q_INVOKABLE AMScanAction(AMScanActionInfo *info, QObject *parent = 0);

	// Re-implemented public functions.
	//////////////////////////////////////////////////
	/// Pure virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Pure virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

protected:
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation() {}

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation() {}

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation() {}

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation() {}
};

#endif // AMSCANACTION_H
