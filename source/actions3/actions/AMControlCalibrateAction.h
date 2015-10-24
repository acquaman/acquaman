#ifndef AMCONTROLCALIBRATEACTION_H
#define AMCONTROLCALIBRATEACTION_H

#include <QTimer>

#include "actions3/AMAction3.h"
#include "actions3/actions/AMControlCalibrateActionInfo.h"

#define AMCONTROLCALIBRATEACTION_CONTROL_NOT_VALID 7739
#define AMCONTROLCALIBRATEACTION_CONTROL_CANT_CALIBRATE 7740
#define AMCONTROLCALIBRATEACTION_VALUES_NOT_VALID 7741
#define AMCONTROLCALIBRATEACTION_VALUES_IDENTICAL 7742

class AMControl;

class AMControlCalibrateAction : public AMAction3
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMControlCalibrateAction(AMControlCalibrateActionInfo *info, AMControl *control, QObject *parent = 0);
	/// Copy contructor.
	AMControlCalibrateAction(const AMControlCalibrateAction &original);
	/// Destructor.
	virtual ~AMControlCalibrateAction();

	/// Virtual copy constructor.
	virtual AMAction3* createCopy() const { return new AMControlCalibrateAction(*this); }

	/// Returns the control this action will calibrate.
	AMControl* control() const { return control_; }

	/// Specify that we cannot pause (since AMControl cannot pause).  If we wanted to get fancy, we might check if the control could stop, (and stop it for pause, and then start it again to resume). But this is much simpler for now.
	virtual bool canPause() const { return false; }
	/// This action cannot skip.
	virtual bool canSkip() const { return false; }

	/// Virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

	/// Returns the action info specific for this action.
	const AMControlCalibrateActionInfo* controlCalibrateInfo() const { return qobject_cast<const AMControlCalibrateActionInfo*>(info()); }
	/// Returns the action info specific for this action.
	AMControlCalibrateActionInfo* controlCalibrateInfo() { return qobject_cast<AMControlCalibrateActionInfo*>(info()); }

signals:

public slots:
	/// Sets the control used by this action.
	void setControl(AMControl *control);

protected slots:
	/// Handles emitting the appropriate signals when the control calibration has started.
	void onCalibrateStarted();
	/// Handles emitting the appropriate signals when the control calibration has failed.
	void onCalibrateFailed();
	/// Handles emitting the appropriate signals when the control calibration has succeeded.
	void onCalibrateSucceeded();

protected:
	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();
	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action.
	virtual void pauseImplementation() { setPaused(); }
	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action.
	virtual void resumeImplementation() { setResumed(); }
	/// All implementations must support cancelling. This function will be called from the Cancelling state when the implementation should cancel the action.
	virtual void cancelImplementation() { setCancelled(); }
	/// The function is called from the Skipping state when the implementation should skip the action. This implementation does not support skipping.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command); }

protected:
	/// The control being calibrated.
	AMControl *control_;
	/// The timer used to issue progress updates.
	QTimer progressTimer_;
};

#endif // AMCONTROLCALIBRATEACTION_H
