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


#include "AMScanAction.h"

#include "acquaman/AMScanController.h"
#include "util/AMErrorMonitor.h"
#include "application/AMAppControllerSupport.h"

#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"

#include <QDir>
#include <QStringBuilder>

AMScanAction::AMScanAction(AMScanActionInfo *info, QObject *parent)
	: AMAction3(info, parent)
{
	controller_ = 0;
	hasValidScanController_ = false;
	scanInfo_ = info;
}

AMScanAction::AMScanAction(const AMScanAction &other)
	: AMAction3(other)
{
	controller_ = 0;
	hasValidScanController_ = false;
	scanInfo_ = other.scanInfo_;
}

AMScanAction::~AMScanAction()
{
	//qdebug() << "Going to delete AMScanAction " << (intptr_t)this;
	if (controller_ && hasValidScanController_){
		//qdebug() << "I am " << (intptr_t)this << ". Thinks the controller is at " << (intptr_t)controller_ << " versus boolean check " << hasValidScanController_;

		controller_->disconnect();
		delete controller_;
	}
}

void AMScanAction::startImplementation()
{
	// Setup the controller.
	AMScanActionInfo *scanInfo = qobject_cast<AMScanActionInfo *>(info());

	if (!scanInfo){

		AMErrorMon::alert(this, AMSCANACTION_NO_VALID_ACTION_INFO, "Scan action does not have a valid info.");
		setFailed();
		return;
	}

	controller_ = scanInfo->config()->createController();

	if (!controller_){

		AMErrorMon::alert(this, AMSCANACTION_CANT_CREATE_CONTROLLER, "Could not create a scan controller.");
		setFailed();
		return;
	}
	hasValidScanController_ = true;

	connect(controller_, SIGNAL(initialized()), this, SLOT(onControllerInitialized()));
	connect(controller_, SIGNAL(started()), this, SLOT(onControllerStarted()));
	connect(controller_, SIGNAL(cancelled()), this, SLOT(onControllerCancelled()));
	connect(controller_, SIGNAL(failed()), this, SLOT(onControllerFailed()));
	connect(controller_, SIGNAL(finished()), this, SLOT(onControllerSucceeded()));
	connect(controller_, SIGNAL(progress(double,double)), this, SLOT(onControllerProgressChanged(double,double)));
	connect(controller_, SIGNAL(stateChanged(int,int)), this, SLOT(onControllerStateChanged()));

	// The action is started the moment it tries to start the controller.
	setStarted();

	// Initialize the controller.
	if (!controller_->initialize()){

		AMErrorMon::alert(this, AMSCANACTION_CANT_INITIALIZE_CONTROLLER, "Could not initialize the scan controller.");
		controller_->disconnect();
		controller_->deleteLater();
		controller_ = 0;
		hasValidScanController_ = false;
		setFailed();
	}
}

void AMScanAction::pauseImplementation()
{
	if (controller_->pause())
		setPaused();
}

void AMScanAction::resumeImplementation()
{
	if (controller_->resume())
		setResumed();
}

void AMScanAction::cancelImplementation()
{
	controller_->cancel();
}

void AMScanAction::onControllerInitialized()
{
	if (!controller_->start()){

		AMErrorMon::alert(this, AMSCANACTION_CANT_START_CONTROLLER, "Could not start the scan controller.");
		setFailed();
	}
}

void AMScanAction::onControllerStarted()
{
	if (controller_->scan()->storeToDb(AMDatabase::database("user")))
		scanInfo_->setScanID(controller_->scan()->id());
	else
		AMErrorMon::alert(this, AMSCANACTION_CANT_SAVE_TO_DB, "The scan action was unable to store the scan to the database.");
}

void AMScanAction::onControllerCancelled()
{
	// If the scan was successfully added to the database then store whatever state it made it to.
	if (scanInfo_->scanID() != -1)
		if (!controller_->scan()->storeToDb(AMDatabase::database("user")))
			AMErrorMon::alert(this, AMSCANACTION_CANT_SAVE_TO_DB, "The scan action was unable to update the scan in the database.");

	setCancelled();
}

void AMScanAction::onControllerFailed()
{
	// If the scan was successfully added to the database then store whatever state it made it to.
	if (scanInfo_->scanID() != -1)
		if (!controller_->scan()->storeToDb(AMDatabase::database("user")))
			AMErrorMon::alert(this, AMSCANACTION_CANT_SAVE_TO_DB, "The scan action was unable to update the scan in the database.");

	setFailed();
}

void AMScanAction::onControllerSucceeded()
{
	//setSucceeded();

	if(!controller_){
		AMErrorMon::alert(this, AMSCANACTION_CONTROLLER_NOT_VALID_FOR_AUTOEXPORT, "Could not export, somehow the scan controller is not available.");
		setSucceeded();
		return;
	}

	// If we managed to save the scan to the database, we should check if the scan should be auto exported, and if it should - export it.
	if (controller_->scan()->database()){

		// Save the finished scan to the database.
		bool saveSucceeded = controller_->scan()->storeToDb(controller_->scan()->database());

		if (saveSucceeded){

			AMScanConfiguration *config = controller_->scan()->scanConfiguration();

			if (config->autoExportEnabled()){

				AMExportController *exportController = new AMExportController(QList<AMScan *>() << controller_->scan());

				// This needs to be generalized so the user can set it (on beamlines where this is acceptable)
				QDir exportDir(AMUserSettings::userDataFolder);
				exportDir.cdUp();

				if(!exportDir.entryList(QDir::AllDirs).contains("exportData")){
					if(!exportDir.mkdir("exportData")){

						AMErrorMon::alert(this, AMSCANACTION_CANT_CREATE_EXPORT_FOLDER, "Could not create the auto-export folder.");
						setSucceeded();
						return;
					}
				}

				exportDir.cd("exportData");
				exportController->setDestinationFolderPath(exportDir.absolutePath());
				AMExporter *autoExporter = AMAppControllerSupport::createExporter(config);

				if(!autoExporter){

					AMErrorMon::alert(this, AMSCANACTION_NO_REGISTERED_EXPORTER, "No exporter registered for this scan type.");
					setSucceeded();
					return;
				}

				exportController->chooseExporter(autoExporter->metaObject()->className());

				// This next creation involves a loadFromDb ... I tested it and it seems fast (milliseconds) ... if that's a Mac only thing then we can figure out a caching system, let me know I have a few ideas
				AMExporterOption *autoExporterOption = AMAppControllerSupport::createExporterOption(config);
				if(!autoExporterOption){

					AMErrorMon::alert(this, AMSCANACTION_NO_REGISTERED_EXPORTER_OPTION, "No exporter option registered for this scan type.");
					setSucceeded();
					return;
				}

				exportController->setOption(autoExporterOption);
				exportController->start(true);
			}
		}

		else
			AMErrorMon::alert(this, AMSCANACTION_CANT_SAVE_TO_DB, "The scan action was unable to update the scan in the database.");
	}

	else
		AMErrorMon::alert(this, AMSCANACTION_DATABASE_NOT_FOUND, "Could not find the database associated with this scan.");

	setSucceeded();
}

void AMScanAction::onControllerProgressChanged(double elapsed, double total)
{
	setProgress(elapsed, total);
}

QString AMScanAction::controllerStateString() const
{
	if (!controller_){
		//qdebug() << "No controller to pull";
		return "";
	}
	//qdebug() << "I think I have a controller " << (intptr_t)controller_;
	//qdebug() << controller_->objectName();

	QString controllerString = "Scan is ";

	switch(controller_->state()){

	case AMScanController::Constructed:
		controllerString.append("constructed.");
		break;

	case AMScanController::Initializing:
		controllerString.append("initializing.");
		break;

	case AMScanController::Initialized:
		controllerString.append("initialized.");
		break;

	case AMScanController::Starting:
		controllerString.append("starting.");
		break;

	case AMScanController::Running:
		controllerString.append("running.");
		break;

	case AMScanController::Pausing:
		controllerString.append("pausing.");
		break;

	case AMScanController::Paused:
		controllerString.append("paused.");
		break;

	case AMScanController::Resuming:
		controllerString.append("resuming.");
		break;

	case AMScanController::Cancelling:
		controllerString.append("cancelling.");
		break;

	case AMScanController::Cancelled:
		controllerString.append("cancelled.");
		break;

	case AMScanController::Finished:
		controllerString.append("finished.");
		break;

	case AMScanController::Failed:
		controllerString.append("failed.");
		break;
	}

	return controllerString;
}

void AMScanAction::onControllerStateChanged()
{
	//qdebug() << "Calling for controllerStateString, need a valid controller";
	setStatusText(stateDescription(state()) % "\n" % controllerStateString());
}
