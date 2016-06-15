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

#include <QDir>
#include <QStringBuilder>

#include "acquaman/AMScanController.h"
#include "acquaman/AMScanActionController.h"
#include "application/AMAppControllerSupport.h"
#include "beamline/AMBeamline.h"

#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"

#include "util/AMErrorMonitor.h"

// These are here for the time being.  When AMScanController is updated to accommodate skipping in a more general way these need to be added here.

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

	AMScanActionInfo* infoCopy = qobject_cast<AMScanActionInfo*>(other.scanInfo_->createCopy());
	scanInfo_ = infoCopy;
}

AMScanAction::~AMScanAction()
{
	if (controller_ && hasValidScanController_){

		controller_->disconnect();
		controller_->deleteLater();
	}
}

AMAction3::ActionValidity AMScanAction::isValid(){
	// check the scan action information
	AMScanActionInfo *scanInfo = qobject_cast<AMScanActionInfo *>(info());
	if (!scanInfo){
		AMErrorMon::alert(this, AMSCANACTION_INVALILD_NO_VALID_ACTION_INFO, "This scan action is not valid because it does not have a valid info.");
		return AMAction3::ActionNeverValid;
	}

	// validate the scan action for beamline specified validation
	AMAction3::ActionValidity actionValidity = AMBeamline::bl()->validateAction(this);
	if(actionValidity != AMAction3::ActionCurrentlyValid){
		if(actionValidity == AMAction3::ActionNeverValid)
			AMErrorMon::alert(this, AMSCANACTION_INVALILD_BEAMLINE_CLAIMS_INVALID, QString("This scan action was determined to be invalid by the beamline. The reason given was: %1").arg(AMBeamline::bl()->validateActionMessage(this)));
		return actionValidity;
	}

	// validate the detector configuration
	AMDetectorInfoSet requestedDetectors = scanInfo->configuration()->detectorConfigurations();
	for(int x = 0; x < requestedDetectors.count(); x++) {
		if(!AMBeamline::bl()->detectorAvailable(requestedDetectors.at(x))) {
			AMErrorMon::alert(this, AMSCANACTION_INVALILD_BEAMLINE_CLAIMS_INVALID,
							  QString("This scan action was determined to be invalid by the beamline. The reason was the detector '%1' is unavailable.").arg(requestedDetectors.at(x).name()));
			return AMAction3::ActionNotCurrentlyValid;
		}
	}

	return AMAction3::ActionCurrentlyValid;
}

QString AMScanAction::notValidWarning()
{

	AMScanActionInfo *scanInfo = qobject_cast<AMScanActionInfo *>(info());
	if (!scanInfo)
		return "Scan Action is not valid because it does not have a valid configuration\n";

	// beamline specific validation message
	if(AMBeamline::bl()->validateAction(this) != AMAction3::ActionCurrentlyValid)
		return AMBeamline::bl()->validateActionMessage(this);

	// detector validation message
	QString retVal= "Scan Action is not valid because the following detectors are not available at the moment:\n";
	AMDetectorInfoSet requestedDetectors = scanInfo->configuration()->detectorConfigurations();
	for(int x = 0; x < requestedDetectors.count(); x++)
		if(!AMBeamline::bl()->detectorAvailable(requestedDetectors.at(x)))
			retVal.append(QString("%1").arg(requestedDetectors.at(x).name()));

	return retVal;
}

#include <QDebug>
void AMScanAction::startImplementation()
{
	qDebug() << "==== AMScanAction::startImplementation() trying to start the scan action, which will be initialization process first";
	// Setup the controller.
	AMScanActionInfo *scanInfo = qobject_cast<AMScanActionInfo *>(info());
	if (!scanInfo){

		AMErrorMon::alert(this, AMSCANACTION_NO_VALID_ACTION_INFO, "Scan action does not have a valid info.");
		setFailed();
		return;
	}

	controller_ = scanInfo->configuration()->createController();
	if (!controller_){

		AMErrorMon::alert(this, AMSCANACTION_CANT_CREATE_CONTROLLER, "Could not create a scan controller.");
		setFailed();
		return;
	}

	// the control is generally valid, start to setup the information
	hasValidScanController_ = true;

	foreach (QString command, controller_->stopCommands()) {
		skipOptions_.append(command);
	}

	connect(controller_, SIGNAL(initialized()), this, SLOT(onControllerInitialized()));
	connect(controller_, SIGNAL(started()), this, SLOT(onControllerStarted()));
	connect(controller_, SIGNAL(cancelled()), this, SLOT(onControllerCancelled()));
	connect(controller_, SIGNAL(failed()), this, SLOT(onControllerFailed()));
	connect(controller_, SIGNAL(finished()), this, SLOT(onControllerSucceeded()));
	connect(controller_, SIGNAL(initializingActionsStarted()), this, SLOT(onControllerInitializing()));
	connect(controller_, SIGNAL(cleaningActionsStarted()), this, SLOT(onControllerCleaningUp()));
	connect(controller_, SIGNAL(progress(double,double)), this, SLOT(onControllerProgressChanged(double,double)));
	connect(controller_, SIGNAL(stateChanged(int,int)), this, SLOT(onControllerStateChanged()));

	// The action is started the moment it tries to start the controller.
	setStatusText("Initializing");

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
	if (controller_->pause()){
		setPaused();
	}
}

void AMScanAction::resumeImplementation()
{
	if (controller_->resume()){
		setResumed();
	}
}

void AMScanAction::cancelImplementation()
{
	if(controller_){
		setStatusText("Cancelling");
		controller_->cancel();
	}
	else
		setCancelled();
}

void AMScanAction::skipImplementation(const QString &command)
{
	if (controller_){

		controller_->stop(command);
		setStatusText(QString("Skipping - %1").arg(command));
	}

	else
		setSucceeded();
}

void AMScanAction::scheduleForDeletionImplementation()
{
	if(controller_){
		//connect(controller_, SIGNAL(destroyed()), this, SLOT(deleteLater()));
		connect(controller_, SIGNAL(destroyed()), this, SLOT(checkReadyForDeletion()));
		controller_->scheduleForDeletion();
		controller_ = 0;
		hasValidScanController_ = false;
		return;
	}

	if(!controller_)
		checkReadyForDeletion();
}

void AMScanAction::checkReadyForDeletion()
{
	if(isLoggingFinished())
		deleteLater();
	else{
		connect(this, SIGNAL(loggingIsFinished()), this, SLOT(deleteLater()));
	}
}

void AMScanAction::onControllerInitialized()
{
	if (state() == AMAction3::Skipping){

		disconnect(controller_, SIGNAL(cancelled()), this, SLOT(onControllerCancelled()));
		connect(controller_, SIGNAL(cancelled()), this, SLOT(onControllerSucceeded()));
		controller_->cancel();
	}

	else if (controller_->start()){
		qDebug() << "==== AMScanAction::onControllerInitialized() initalization finished.... trying to start scan";

	} else {
		AMErrorMon::alert(this, AMSCANACTION_CANT_START_CONTROLLER, "Could not start the scan controller.");
		setFailed();
	}
}

void AMScanAction::onControllerStarted()
{
	qDebug() << "==== AMScanAction::onControllerStarted() .... scan started";

	if (controller_->scan()->storeToDb(AMDatabase::database("user")))
		scanInfo_->setScanID(controller_->scan()->id());
	else
		AMErrorMon::alert(this, AMSCANACTION_CANT_SAVE_TO_DB, "The scan action was unable to store the scan to the database.");

	setStarted();
}

void AMScanAction::onControllerCancelled()
{
	qDebug() << "==== AMScanAction::onControllerCancelled() .... scan cancelled";

	// If the scan was successfully added to the database then store whatever state it made it to.
	if (scanInfo_->scanID() != -1)
		if (!controller_->scan()->storeToDb(AMDatabase::database("user")))
			AMErrorMon::alert(this, AMSCANACTION_CANT_SAVE_TO_DB, "The scan action was unable to update the scan in the database.");

	setCancelled();
}

void AMScanAction::onControllerFailed()
{
	qDebug() << "==== AMScanAction::onControllerFailed() .... scan failed";

	// If the scan was successfully added to the database then store whatever state it made it to.
	if (scanInfo_->scanID() != -1)
		if (!controller_->scan()->storeToDb(AMDatabase::database("user")))
			AMErrorMon::alert(this, AMSCANACTION_CANT_SAVE_TO_DB, "The scan action was unable to update the scan in the database.");

	setFailed();
}

void AMScanAction::onControllerSucceeded()
{
	qDebug() << "==== AMScanAction::onControllerSucceeded() .... scan succeeded --- exporting";

	if (controller_){
		setStatusText("Exporting");
		autoExportScan();
	}
	else
		AMErrorMon::alert(this, AMSCANACTION_CONTROLLER_NOT_VALID_FOR_AUTOEXPORT, "Could not export, somehow the scan controller is not available.");

	setSucceeded();
}

void AMScanAction::onControllerInitializing()
{
	qDebug() << "==== AMScanAction::onControllerInitializing() .... scan initializing";
	setStatusText("Initializing");
}

void AMScanAction::onControllerCleaningUp()
{
	qDebug() << "==== AMScanAction::onControllerCleaningUp() .... scan cleaning up";
	setCleaningUp();
}

void AMScanAction::onControllerProgressChanged(double elapsed, double total)
{
	setProgress(elapsed, total);
}

void AMScanAction::autoExportScan()
{
	// If we managed to save the scan to the database, we should check if the scan should be auto exported, and if it should - export it.
	if (controller_->scan()->database()){

		// Save the finished scan to the database.
		if (controller_->scan()->storeToDb(controller_->scan()->database())){

			AMScanConfiguration *config = controller_->scan()->scanConfiguration();

			if (config->autoExportEnabled()){

				AMExportController *exportController = new AMExportController(QList<AMScan *>() << controller_->scan(), this);
				exportController->setDefaultDestinationFolderPath();
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

}
