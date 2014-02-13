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


#include "IDEASAppController.h"

#include "beamline/IDEAS/IDEASBeamline.h"

#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "application/AMAppControllerSupport.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/AMRun.h"

#include "util/AMPeriodicTable.h"

#include "ui/CLS/CLSSynchronizedDwellTimeView.h"

IDEASAppController::IDEASAppController(QObject *parent)
	: AMAppController(parent)
{

}

bool IDEASAppController::startup()
{
	// Start up the main program.
	if(AMAppController::startup()) {

		// Initialize central beamline object
		IDEASBeamline::ideas();
		// Initialize the periodic table object.
		AMPeriodicTable::table();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)){

			AMRun firstRun("VESPERS", 4);	/// \todo For now, we know that 4 is the ID of the VESPERS facility, but this is a hardcoded hack.
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		setupExporterOptions();
		setupUserInterface();
		makeConnections();

		// Github setup for adding VESPERS specific comment.
//		additionalIssueTypesAndAssignees_.append("I think it's a VESPERS specific issue", "dretrex");

		return true;
	}
	else
		return false;
}

void IDEASAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void IDEASAppController::registerClasses()
{

}

void IDEASAppController::setupExporterOptions()
{

}

void IDEASAppController::setupUserInterface()
{
	// Create panes in the main window:
	////////////////////////////////////


	mw_->insertHeading("General", 0);

	mw_->insertHeading("Detectors", 1);

	mw_->insertHeading("Scans", 2);

	ideasSynchronizedDwellTimeView_ = 0; //NULL
	connect(IDEASBeamline::ideas()->synchronizedDwellTime(), SIGNAL(connected(bool)), this, SLOT(onSynchronizedDwellTimeConnected(bool)));
	onSynchronizedDwellTimeConnected(false);
}

void IDEASAppController::makeConnections()
{

}

#include <QDebug>
void IDEASAppController::onSynchronizedDwellTimeConnected(bool connected){
	Q_UNUSED(connected)
	if(IDEASBeamline::ideas()->synchronizedDwellTime() && IDEASBeamline::ideas()->synchronizedDwellTime()->isConnected() && !ideasSynchronizedDwellTimeView_){
		CLSSynchronizedDwellTime *clsDwellTime = qobject_cast<CLSSynchronizedDwellTime*>(IDEASBeamline::ideas()->synchronizedDwellTime());
		if(clsDwellTime)
			ideasSynchronizedDwellTimeView_ = new CLSSynchronizedDwellTimeView(clsDwellTime);

		mw_->addPane(ideasSynchronizedDwellTimeView_, "Detectors", "IDEAS Sync Dwell", ":/system-software-update.png", true);
		ideasSynchronizedDwellTimeView_->setAdvancedViewVisible(true);
	}
}

void IDEASAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{

}

void IDEASAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{

}
