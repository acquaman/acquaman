#ifndef AMSCANACTIONCONTROLLER_H
#define AMSCANACTIONCONTROLLER_H

#include "AMScanController.h"

/// This class provides a simple extension to AMScanController to expose it to some of the AMAction API and AMAgnosticData API.
class AMScanActionController : public AMScanController
{
	Q_OBJECT

public:
	/// Constructor.  Requires the scan configuration.
	AMScanActionController(AMScanConfiguration *configuration, QObject *parent = 0);

public slots:
	/// Provides access AMAction::skip() which isn't part of the AMScanController API.
	void skip(const QString &command);

protected slots:
	/// Helper slot that ensures all the necessary database and event handler clean up is done.  Closes any open database transaction and removes this object from the QEvent receiver list.
	void onStateChanged(int oldState, int newState);

protected:
	/// Returns whether scan action can pause or not.  Returns false if there is currently no scan running.
	virtual bool canPause() const;
	/// Handles the necessary startup for a scan action based scan controller.  Adds this object to the list of QEvent receivers, starts the scan action runner, and calls setStarted().
	virtual bool startImplementation();
	/// Handles pausing the current scan action and calling setPaused() if successful.
	virtual void pauseImplementation();
	/// Handles resuming the current scan action and calling setResumed() if successful.
	virtual void resumeImplementation();
	/// Handles cancelling the current scan action and setting up the necessary communications to call setCancelled() when appropriate.
	virtual void cancelImplementation();
};

#endif // AMSCANACTIONCONTROLLER_H
