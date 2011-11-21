/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMAPPCONTROLLER_H
#define AMAPPCONTROLLER_H

#include <QObject>

#include <application/AMDatamanAppController.h>

class AMWorkflowManagerView;
class AMMainWindow;
class AMScan;

/// This class extends the base dataman app controller class by adding the workflow.  This is the base class for all beamline acquisition app controllers.  The reason for the distinction between this class and the dataman version is a result for the desire to be able to take the dataman version home with the user whereas this version is meant to reside on beamlines that always have access to beamline components and controls.
class AMAppController : public AMDatamanAppController
{
Q_OBJECT
public:
	/// This constructor is empty. Call AMAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit AMAppController(QObject *parent = 0);
	/// This destructor automatically calls shutdown() if required. (ie: if startup() has run successfully, and shutdown() hasn't been called yet.)
	virtual ~AMAppController();

	/// Re-implemented from AMDatamanAppController to add the workflow pane
	virtual bool startupCreateUserInterface();

	// Not re-implemented: virtual void shutdown();

signals:

public slots:

	/// This function can be used to "hand-off" a scan to the app controller (for example, a new scan from a scan controller). An editor will be opened for the scan, and the editor will take responsibility for deleting the scan in the future (but not without checking first whether it still has a scan controller.)
	/*! By default, the new scan editor will be brought to the front of the main window and presented to the user. You can suppress this by setting \c bringEditorToFront to false.

If \c openInExistingEditor is set to true, and if there is an existing editor, the scan will be appended inside that editor. (If there is more than one open editor, the scan will be added to the most recently-created one.)  By default, a new editor window is created.*/
	void openScanInEditorAndTakeOwnership(AMScan* scan, bool bringEditorToFront = true, bool openInExistingEditor = false);

	/// Bring the Workflow view to the front level
	void goToWorkflow();

	///////////////////////////////////

protected:
	/// Top-level panes in the main window
	AMWorkflowManagerView* workflowManagerView_;
};

#endif // AMAPPCONTROLLER_H
