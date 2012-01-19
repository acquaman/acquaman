#ifndef AMSCANCONTROLLERACTION_H
#define AMSCANCONTROLLERACTION_H

#include "actions2/AMAction.h"
#include "actions2/actions/AMScanControllerActionInfo.h"
class AMScanController;

/// This class wraps any AMScanController as an AMAction for use with Acquaman's Action/Workflow system.  It requires a detailed subclass of AMActionInfo to be passed into the constructor; normally this would be an AMActionInfo describing the parameters for your scan, with a link to the corresponding AMScanConfiguration.
/*! Design question: the AMAction and AMScanController APIs look very similar. In future versions of Acquaman, should we migrate all AMScanControllers to AMAction directly, and AMScanConfigurations to AMActionInfos? */
class AMScanControllerAction : public AMAction
{
    Q_OBJECT
public:
	/// Constructor.  \c info can be any kind of AMScanControllerActionInfo; all it needs is to be able to access an AMScanConfiguration with AMScanControllerActionInfo::scanConfig().
	Q_INVOKABLE AMScanControllerAction(AMScanControllerActionInfo* info, QObject *parent = 0);
	/// Copy Constructor
	AMScanControllerAction(const AMScanControllerAction& other);
	/// Virtual copy constructor
	virtual AMAction* createCopy() const { return new AMScanControllerAction(*this); }
	/// Destructor. Deletes the controller, if we have one running already.
	virtual ~AMScanControllerAction();

	// Re-implemented public functions
	///////////////////////////////

	/// Specify that we can pause, if our scan controller can pause.
	virtual bool canPause() const;

signals:

public slots:

protected:

	// The following functions are used to define the unique behaviour of the action.  We set them up in this way so that subclasses don't need to worry about (and cannot) break the state machine logic; they only need to fill in their pieces.

	// These virtual functions allow us to implement our unique action behaviour.  They are called at the appropriate time by the base class, when base-class-initiated state changes happen: ->Starting, ->Cancelling, ->Pausing, ->Resuming
	/////////////////////////
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation();

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation();

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

protected slots:

	void onScanControllerStateChanged(int previousState, int state);
	void onScanControllerTimeRemaining(double seconds);

protected:
	/// If we call info(), we'll get a generic AMActionInfo pointer. This makes casting it to AMScanControllerActionInfo a bit more convenient.
	const AMScanControllerActionInfo* scanControllerInfo() const;
	AMScanControllerActionInfo* scanControllerInfo();

	/// A pointer to the scan controller, created as soon as we are constructed.
	AMScanController* controller_;
	/// Since scan controllers may not like being cancelled while initializing or starting, this is used to flag that we should cancel as soon as possible.
	bool shouldCancelASAP_;
};

#endif // AMSCANCONTROLLERACTION_H
