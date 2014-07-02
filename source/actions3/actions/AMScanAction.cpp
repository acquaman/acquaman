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


#include "AMScanAction.h"

#include "acquaman/AMScanController.h"
#include "util/AMErrorMonitor.h"
#include "application/AMAppControllerSupport.h"

#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"

// These are here for the time being.  When AMScanController is updated to accommodate skipping in a more general way these need to be added here.
#include "acquaman/AMScanActionController.h"

#include "beamline/AMBeamline.h"

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

AMAction3::ActionValidity AMScanAction::isValid(){
	AMScanActionInfo *scanInfo = qobject_cast<AMScanActionInfo *>(info());

	if (!scanInfo){
		AMErrorMon::alert(this, AMSCANACTION_INVALILD_NO_VALID_ACTION_INFO, "This scan action is not valid because it does not have a valid info.");
		return AMAction3::ActionNeverValid;
	}

	if(AMBeamline::bl()->validateAction(this) != AMAction3::ActionCurrentlyValid){
		if(AMBeamline::bl()->validateAction(this) == AMAction3::ActionNeverValid)
			AMErrorMon::alert(this, AMSCANACTION_INVALILD_BEAMLINE_CLAIMS_INVALID, QString("This scan action was determined to be invalid by the beamline. The reason given was: %1").arg(AMBeamline::bl()->validateActionMessage(this)));
		return AMBeamline::bl()->validateAction(this);
	}

	AMDetectorInfoSet requestedDetectors = scanInfo->config()->detectorConfigurations();
	for(int x = 0; x < requestedDetectors.count(); x++)
		if(!AMBeamline::bl()->detectorAvailable(requestedDetectors.at(x)))
			return AMAction3::ActionNotCurrentlyValid;

	return AMAction3::ActionCurrentlyValid;
}

QString AMScanAction::notValidWarning(){
	QString retVal;

	AMScanActionInfo *scanInfo = qobject_cast<AMScanActionInfo *>(info());

	if (!scanInfo)
		return "Scan Action is not valid because it does not have a valid configuration\n";

	if(AMBeamline::bl()->validateAction(this) != AMAction3::ActionCurrentlyValid)
		return AMBeamline::bl()->validateActionMessage(this);

	retVal = "Scan Action is not valid because the following detectors are not available at the moment:\n";
	AMDetectorInfoSet requestedDetectors = scanInfo->config()->detectorConfigurations();
	for(int x = 0; x < requestedDetectors.count(); x++)
		if(!AMBeamline::bl()->detectorAvailable(requestedDetectors.at(x)))
			retVal.append(QString("%1").arg(requestedDetectors.at(x).name()));

	return retVal;
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

	// Set the skip options if it is one of the dacq controllers.
	/*
	if (qobject_cast<AMDacqScanController *>(controller_)){

		skipOptions_.append("Stop Now");

		if (qobject_cast<AM2DDacqScanController *>(controller_))
			skipOptions_.append("Stop At The End Of Line");
	}
	*/

	if(qobject_cast<AMScanActionController *>(controller_))
		skipOptions_.append("Stop Now");

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
	if(controller_)
		controller_->cancel();
	else
		setCancelled();
}

void AMScanAction::skipImplementation(const QString &command)
{
	AMScanActionController *scanActionController = qobject_cast<AMScanActionController*>(controller_);
	if(scanActionController){
		scanActionController->skip(command);
		return;
	}

	if (command == "Stop Now"){
		/*
		AMDacqScanController *dacqController = qobject_cast<AMDacqScanController *>(controller_);
		if(dacqController)
			dacqController->stopImmediately();
		*/
	}

	else if (command == "Stop At The End Of Line"){
		/*
		AM2DDacqScanController *dacq2DController = qobject_cast<AM2DDacqScanController *>(controller_);
		if(dacq2DController)
			dacq2DController->stopAtTheEndOfLine();
		*/
	}

}

#include "acquaman/AMXASScanConfiguration.h"
bool AMScanAction::canSkip() const
{
//	// We can check against AMDacqScanController only because AM2DDacqScanController inherits from AMDacqScanController.
//	if (qobject_cast<AMDacqScanController *>(controller_))
//		return true;

	const AMScanActionInfo *scanActionInfo = qobject_cast<const AMScanActionInfo*>(info());
	if(qobject_cast<const AMXASScanConfiguration*>(scanActionInfo->config()))
		return true;

//	return false;
	return true;
}

void AMScanAction::scheduleForDeletion(){
	if(!controller_ || controller_->isReadyForDeletion())
		deleteLater();
	else
		connect(controller_, SIGNAL(readyForDeletion(bool)), this, SLOT(onReadyForDeletionChanged(bool)));
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
	if (controller_)
		autoExportScan();
	else
		AMErrorMon::alert(this, AMSCANACTION_CONTROLLER_NOT_VALID_FOR_AUTOEXPORT, "Could not export, somehow the scan controller is not available.");

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

void AMScanAction::autoExportScan()
{
	// If we managed to save the scan to the database, we should check if the scan should be auto exported, and if it should - export it.
	if (controller_->scan()->database()){

		// Save the finished scan to the database.
		if (controller_->scan()->storeToDb(controller_->scan()->database())){

			AMScanConfiguration *config = controller_->scan()->scanConfiguration();

			if (config->autoExportEnabled()){

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
				AMExporter *autoExporter = AMAppControllerSupport::createExporter(config);

				if(!autoExporter){

					AMErrorMon::alert(this, AMSCANACTION_NO_REGISTERED_EXPORTER, "No exporter registered for this scan type.");
					return;
				}

				exportController->chooseExporter(autoExporter->metaObject()->className());

				// This next creation involves a loadFromDb ... I tested it and it seems fast (milliseconds) ... if that's a Mac only thing then we can figure out a caching system, let me know I have a few ideas
				AMExporterOption *autoExporterOption = AMAppControllerSupport::createExporterOption(config);
				if(!autoExporterOption){

					AMErrorMon::alert(this, AMSCANACTION_NO_REGISTERED_EXPORTER_OPTION, "No exporter option registered for this scan type.");
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
}

void AMScanAction::onControllerStateChanged()
{
	//qdebug() << "Calling for controllerStateString, need a valid controller";
	setStatusText(stateDescription(state()) % "\n" % controllerStateString());
}

void AMScanAction::onReadyForDeletionChanged(bool isReady){
	if(isReady)
		deleteLater();
}
