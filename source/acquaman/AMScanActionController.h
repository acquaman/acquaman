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

#define AMSCANACTIONCONTROLLER_CANNOT_INTIALIZE 262001
#define AMSCANACTIONCONTROLLER_CANNOT_PAUSE 262002
#define AMSCANACTIONCONTROLLER_CANNOT_RESUME 262003
#define AMSCANACTIONCONTROLLER_CANNOT_CANCEL 262004

/// This class provides a simple extension to AMScanController to expose it to some of the AMAction API and AMAgnosticData API.
class AMScanActionController : public AMScanController
{
	Q_OBJECT

public:
	/// Constructor.  Requires the scan configuration.
	AMScanActionController(AMScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AMScanActionController();

	/// Returns the master action that encapsulates all of the, possibly nested, actions that run the scan.
	AMAction3 *scanningActions();

	/// Method that builds all the general aspects, such as measurements and raw data sources, and the file writer capabilities for the scan controller.
	virtual void buildScanController() = 0;

public slots:

protected slots:
	/// Helper slot that ensures all the necessary database and event handler clean up is done.  Closes any open database transaction and removes this object from the QEvent receiver list.
	void onStateChanged(int oldState, int newState);
	/// Handles setting the actions tree and then optimizing and validating to ensure the the action tree is good to run.  Can be reimplemented for more specific behaviour.
	virtual void onScanningActionsGenerated(AMAction3 *actionTree);

	/// Handles the cleanup after the initialization actions succeed.  Can be reimplemented for more specific behaviour.
	virtual void onInitializationActionsListSucceeded();
	/// Handles the cleanup after the initialization actions fail.  Can be reimplemented for more specific behaviour.
	virtual void onInitializationActionsListFailed();
	/// Handles the cleanup after the cleanup actions succeed.  Can be reimplemented for more specific behaviour.
	virtual void onCleanupActionsListSucceeded();
	/// Handles the cleanup after the cleanup actions fail.  Can be reimplemented for more specific behaviour.
	virtual void onCleanupActionsListFailed();
	/// Handles starting the cleanup actions list after the scan controller actions tree completed successfully.  Can be reimplemented for more specific behaviour.
	virtual void onScanningActionsSucceeded();
	/// Handles starting the cleanup actions list after the scan controller actions tree failed.  Can be reimplemented for more specific behaviour.
	virtual void onScanningActionsFailed();

protected:
	/// Method that does all the subclass specific additions, such as analysis blocks.
	virtual void buildScanControllerImplementation() = 0;

	/// Returns whether scan action can pause or not.  Returns false if there is currently no scan running.
	virtual bool canPause() const;
	/// Simple implementation that calls buildInitializationActions(), buildCleanupActions() and then starts the initialization list.
	virtual bool initializeImplementation();
	/// Simple implementation that starts the scan action controller actions.
	virtual bool startImplementation();
	/// Handles pausing the current scan action and calling setPaused() if successful.
	virtual void pauseImplementation();
	/// Handles resuming the current scan action and calling setResumed() if successful.
	virtual void resumeImplementation();
	/// Handles cancelling the current scan action and setting up the necessary communications to call setCancelled() when appropriate.
	virtual void cancelImplementation();
	/// Handles stopping the current scan action and setting up the necessary communications to call setFinished when appropriate.
	virtual void stopImplementation(const QString &command);

	/// Method that builds all the necessary actions to properly initialize your scan.  Default does nothing, but should be reimplemented in subclases.
	virtual AMAction3 *createInitializationActions() { return 0; }
	/// Method that builds all the necessary actions to properly cleanup your scan.  Default does nothing, but should be reimplemented in subclasses.
	virtual AMAction3 *createCleanupActions() { return 0; }

	/// The pointer that holds the scanning actions.
	AMAction3 *scanningActions_;
	/// Flag holding whether the scan was successful or not.
	bool scanningActionsSucceeded_;
	/// The intialization actions for the scan.
	AMAction3 *initializationActions_;
	/// The cleanup actions for the scan.
	AMAction3 *cleanupActions_;
};

#endif // AMSCANACTIONCONTROLLER_H
