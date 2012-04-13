#include "AMScanAction.h"

#include "acquaman/AMScanController.h"
#include "util/AMErrorMonitor.h"
#include "application/AMAppControllerSupport.h"

#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"

#include <QDir>

AMScanAction::AMScanAction(AMScanActionInfo *info, QObject *parent)
	: AMAction3(info, parent)
{
	controller_ = 0;
}

AMScanAction::AMScanAction(const AMScanAction &other)
	: AMAction3(other)
{
	controller_ = 0;
}

AMScanAction::~AMScanAction()
{
	if (controller_)
		delete controller_;
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

	connect(controller_, SIGNAL(initialized()), this, SLOT(onControllerInitialized()));
	connect(controller_, SIGNAL(started()), this, SLOT(onControllerStarted()));
	connect(controller_, SIGNAL(cancelled()), this, SLOT(onControllerCancelled()));
	connect(controller_, SIGNAL(failed()), this, SLOT(onControllerFailed()));
	connect(controller_, SIGNAL(finished()), this, SLOT(onControllerSucceeded()));
	connect(controller_, SIGNAL(progress(double,double)), this, SLOT(onControllerProgressChanged(double,double)));

	// The action is started the moment it tries to start the controller.
	setStarted();

	// Initialize the controller.
	if (!controller_->initialize()){

		AMErrorMon::alert(this, AMSCANACTION_CANT_INITIALIZE_CONTROLLER, "Could not initialize the scan controller.");
		controller_->disconnect();
		controller_->deleteLater();
		controller_ = 0;
		setFailed();
	}
}

void AMScanAction::pauseImplementation()
{
	if (controller_->pause())
		setPause();
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
		scanID_ = controller_->scan()->id();

	else
		AMErrorMon::alert(this, AMSCANACTION_CANT_SAVE_TO_DB, "The scan action was unable to store the scan to the database.");
}

void AMScanAction::onControllerCancelled()
{
	// If the scan was successfully added to the database then store whatever state it made it to.
	if (scanID_ != -1)
		if (!controller_->scan()->storeToDb(AMDatabase::database("user")))
			AMErrorMon::alert(this, AMSCANACTION_CANT_SAVE_TO_DB, "The scan action was unable to update the scan in the database.");

	setCancelled();
}

void AMScanAction::onControllerFailed()
{
	// If the scan was successfully added to the database then store whatever state it made it to.
	if (scanID_ != -1)
		if (!controller_->scan()->storeToDb(AMDatabase::database("user")))
			AMErrorMon::alert(this, AMSCANACTION_CANT_SAVE_TO_DB, "The scan action was unable to update the scan in the database.");

	setFailed();
}

void AMScanAction::onControllerSucceeded()
{
	setSucceeded();

	// If we managed to save the scan to the database, we should check if the scan should be auto exported, and if it should - export it.
	if (controller_->scan()->database()){

		// Save the finished scan to the database.
		bool saveSucceeded = controller_->scan()->storeToDb(controller_->scan()->database());

		if (saveSucceeded){

			if (controller_->scan()->scanConfiguration()->autoExportEnabled()){

				AMExportController *exportController = new AMExportController(QList<AMScan *>() << controller_->scan());

				// This needs to be generalized so the user can set it (on beamlines where this is acceptable)
				QDir exportDir(AMUserSettings::userDataFolder);
				exportDir.cdUp();

				if(!exportDir.entryList(QDir::AllDirs).contains("exportData")){
					if(!exportDir.mkdir("exportData")){

						AMErrorMon::alert(this, AMSCANACTION_CANT_CREATE_EXPORT_FOLDER, "Could not create the auto-export folder.");
						return;
					}
				}

				exportDir.cd("exportData");
				exportController->setDestinationFolderPath(exportDir.absolutePath());
				AMExporter *autoExporter = AMAppControllerSupport::createExporter(cfg_);

				if(!autoExporter){

					AMErrorMon::alert(this, AMSCANACTION_NO_REGISTERED_EXPORTER, "No exporter registered for this scan type.");
					return;
				}

				exportController->chooseExporter(autoExporter->metaObject()->className());

				// This next creation involves a loadFromDb ... I tested it and it seems fast (milliseconds) ... if that's a Mac only thing then we can figure out a caching system, let me know I have a few ideas
				AMExporterOption *autoExporterOption = AMAppControllerSupport::createExporterOption(cfg_);
				if(!autoExporterOption){

					AMErrorMon::alert(this, AMSCANACTION_NO_REGISTERED_EXPORTER_OPTION, "No exporter option registered for this scan type.");
					return;
				}

				exportController->setOption(autoExporterOption);
				exportController->start();
			}
		}

		else
			AMErrorMon::alert(this, AMSCANACTION_CANT_SAVE_TO_DB, "The scan action was unable to update the scan in the database.");
	}

	else
		AMErrorMon::alert(this, AMSCANACTION_DATABASE_NOT_FOUND, "Could not find the database associated with this scan.");
}

void AMScanAction::onControllerProgressChanged(double elapsed, double total)
{

}
