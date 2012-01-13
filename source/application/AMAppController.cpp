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


#include "AMAppController.h"

#include "ui/AMWorkflowManagerView.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"

AMAppController::AMAppController(QObject *parent)
	: AMDatamanAppController(parent)
{
}

bool AMAppController::startupCreateUserInterface() {

	if (AMDatamanAppController::startupCreateUserInterface()){
		// a heading for the workflow manager...
		workflowManagerView_ = new AMWorkflowManagerView();
		mw_->insertHeading("Experiment Tools", 1);
		mw_->addPane(workflowManagerView_, "Experiment Tools", "Workflow", ":/user-away.png");

		return true;
	}

	return false;
}

// Program shutdown:
AMAppController::~AMAppController() {

}



void AMAppController::goToWorkflow() {
	mw_->setCurrentPane(workflowManagerView_);
}

void AMAppController::openScanInEditorAndTakeOwnership(AMScan *scan, bool bringEditorToFront, bool openInExistingEditor)
{
	AMGenericScanEditor* editor;

	if(openInExistingEditor && scanEditorCount()) {
		editor = scanEditorAt(scanEditorCount()-1);
	}
	else {
		editor = createNewScanEditor();
	}

	editor->addScan(scan);

	if(bringEditorToFront)
		mw_->setCurrentPane(editor);
}

#include "acquaman/AMScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMScan.h"

void AMAppController::launchScanConfigurationFromDb(const QUrl &url)
{
	// scheme correct?
	if (url.scheme() != "amd")
		return;

	// Scan configurations only come from the user databases currently.
	AMDatabase *db = AMDatabase::database("user");
	if (!db)
		return;

	QStringList path = url.path().split('/', QString::SkipEmptyParts);
	if(path.count() != 2)
		return;

	QString tableName = path.at(0);
	bool idOkay;
	int id = path.at(1).toInt(&idOkay);
	if(!idOkay || id < 1)
		return;

	// Only open scans for now (ie: things in the scans table)
	if(tableName != AMDbObjectSupport::s()->tableNameForClass<AMScan>())
		return;

	// turn off automatic raw-day loading for scans... This will make loading the scan to access it's config much faster.
	bool scanAutoLoadingOn = AMScan::autoLoadData();
	AMScan::setAutoLoadData(false);
	// Dynamically create and load a detailed subclass of AMDbObject from the database... whatever type it is.
	AMDbObject* dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(db, tableName, id);
	if(!dbo)
		return;
	// restore AMScan's auto-loading of data to whatever it was before.
	AMScan::setAutoLoadData(scanAutoLoadingOn);

	// Is it a scan?
	AMScan* scan = qobject_cast<AMScan*>( dbo );
	if(!scan) {
		delete dbo;
		return;
	}

	// need to check that this scan actually has a valid config. This hasn't always been guaranteed, especially when scans move between beamlines.
	AMScanConfiguration* config = scan->scanConfiguration();
	if(!config) {
		delete scan;
		return;
	}
	// need to create a copy of the config so we can delete the scan (and hence the config instance owned by the scan). The view will take ownership of the copy.
	config = config->createCopy();
	delete scan;
	if(!config)
		return;

	AMScanConfigurationView *view = config->createView();
	if(!view) {
		delete config;
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -401, "Unable to create view from the scan configuration loaded from the database.  Contact Acquaman developers."));
		return;
	}

	AMScanConfigurationViewHolder *viewHolder = new AMScanConfigurationViewHolder( workflowManagerView_, view);
	viewHolder->setAttribute(Qt::WA_DeleteOnClose, true);
	viewHolder->show();
}
