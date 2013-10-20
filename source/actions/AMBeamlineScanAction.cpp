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


#include "AMBeamlineScanAction.h"
#include "acquaman/AMScanController.h"

#include "beamline/AMBeamline.h"
#include "util/AMDateTimeUtils.h"
#include "dataman/AMScanExemplarDictionary.h"

#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/database/AMDbObjectSupport.h"

#include <QPushButton>
#include <QToolButton>
#include <QPalette>
#include <QApplication>
#include <QStyle>
#include <QMessageBox>
#include <QDir>

AMBeamlineScanAction::AMBeamlineScanAction(AMScanConfiguration *cfg, QObject *parent) :
	AMBeamlineActionItem(true, parent)
{
	scanID_ = -1;
	configurationLocked_ = false;
	cfg_ = cfg;
	lastSampleDescription_ = "<Unknown Sample>";
	exemplar_.setSampleName(lastSampleDescription_);
	exemplar_.setScanConfiguration(cfg_);
	nameDictionary_ = new AMScanExemplarDictionary(&exemplar_, this);
	exportNameDictionary_ = new AMScanExemplarDictionary(&exemplar_, this);
	nameDictionary_->setOperatingOnName(true);
	exportNameDictionary_->setOperatingOnExportName(true);
	if(cfg_){
		connect(cfg_, SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
		connect(cfg_, SIGNAL(destroyed()), this, SLOT(onConfigurationDestroyed()));
		setDescription(cfg_->description()+" on "+lastSampleDescription_);
		nameDictionary_->parseKeywordStringAndOperate(cfg_->userScanName());
		if(cfg_->autoExportEnabled())
			exportNameDictionary_->parseKeywordStringAndOperate(cfg_->userExportName());
	}
	ctrl_ = NULL;
	keepOnCancel_ = false;

	connect(AMBeamline::bl(), SIGNAL(beamlineScanningChanged(bool)), this, SLOT(onBeamlineScanningChanged(bool)));

	initialize();
}

AMBeamlineActionItemView* AMBeamlineScanAction::createView(int index){
	return new AMBeamlineScanActionView(this, index);
}

AMBeamlineActionItem* AMBeamlineScanAction::createCopy() const{
	return new AMBeamlineScanAction(cfg()->createCopy());
	/*
	if(cfg_)
		return new AMBeamlineScanAction(cfg_->createCopy());
	else{
		// turn off automatic raw-day loading for scans... This will make loading the scan to access it's config much faster.
		bool scanAutoLoadingOn = AMScan::autoLoadData();
		AMScan::setAutoLoadData(false);
		// Dynamically create and load a detailed subclass of AMDbObject from the database... whatever type it is.
		AMDbObject* dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(AMDatabase::database("user"), AMDbObjectSupport::s()->tableNameForClass<AMScan>(), scanID_);
		if(!dbo)
			return 0; //NULL
		// restore AMScan's auto-loading of data to whatever it was before.
		AMScan::setAutoLoadData(scanAutoLoadingOn);
		// Is it a scan?
		AMScan* scan = qobject_cast<AMScan*>( dbo );
		if(!scan) {
			delete dbo;
			return 0; //NULL
		}
		// Does the scan have a configuration?
		AMScanConfiguration* config = scan->scanConfiguration();
		if(!config) {
			delete scan;
			return 0; //NULL
		}
		// need to create a copy of the config so we can delete the scan (and hence the config instance owned by the scan). The view will take ownership of the copy.
		config = config->createCopy();
		delete scan;
		if(!config)
			return 0; //NULL
		cfg_ = config;
		return new AMBeamlineScanAction(cfg_->createCopy());
	}
	*/
	return 0;//NULL
}

AMScanConfiguration* AMBeamlineScanAction::cfg() const{
	if(cfg_)
		return cfg_;

	// turn off automatic raw-day loading for scans... This will make loading the scan to access it's config much faster.
	bool scanAutoLoadingOn = AMScan::autoLoadData();
	AMScan::setAutoLoadData(false);
	// Dynamically create and load a detailed subclass of AMDbObject from the database... whatever type it is.
	AMDbObject* dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(AMDatabase::database("user"), AMDbObjectSupport::s()->tableNameForClass<AMScan>(), scanID_);
	if(!dbo)
		return 0; //NULL
	// restore AMScan's auto-loading of data to whatever it was before.
	AMScan::setAutoLoadData(scanAutoLoadingOn);
	// Is it a scan?
	AMScan* scan = qobject_cast<AMScan*>( dbo );
	if(!scan) {
		delete dbo;
		return 0; //NULL
	}
	// Does the scan have a configuration?
	AMScanConfiguration* config = scan->scanConfiguration();
	if(!config) {
		scan->release();
		return 0; //NULL
	}
	// need to create a copy of the config so we can delete the scan (and hence the config instance owned by the scan). The view will take ownership of the copy.
	config = config->createCopy();
	scan->release();
	if(!config)
		return 0; //NULL
	cfg_ = config;
	return cfg_;
}

bool AMBeamlineScanAction::isRunning() const{
	return AMBeamlineActionItem::isRunning();
}

bool AMBeamlineScanAction::isPaused() const{
	if(!ctrl_)
		return false;

	return ctrl_->isPaused();
}

QString AMBeamlineScanAction::lastSampleDescription() const{
	return lastSampleDescription_;
}

QString AMBeamlineScanAction::guessScanName() const{
	return exemplar_.name();
}

QString AMBeamlineScanAction::guessExportName() const{
	return exemplar_.exportName();
}

bool AMBeamlineScanAction::configurationLocked() const{
	return configurationLocked_;
}

void AMBeamlineScanAction::start(){
	if(!isReady()){
		AMErrorMon::debug(this, AMBEAMLINEACTIONITEM_CONNECTEDWAITING_MESSAGE, QString("%1 Not ready, but connected and waiting.").arg((intptr_t)this));
		connect(this, SIGNAL(ready(bool)), this, SLOT(start(bool)));
		return;
	}

	if(!AMBeamlineActionItem::isReinitialized()){
		configurationLocked_ = true;
		AMErrorMon::debug(this, AMBEAMLINEACTIONITEM_NOTREINITIALIZED_MESSAGE, QString("%1 Not reinitialized, creating new controller.").arg((intptr_t)this));
		ctrl_ = cfg_->createController();
		if(!ctrl_) {
			AMErrorMon::report(AMErrorReport(this,
					AMErrorReport::Alert,
					AMBEAMLINEACTIONITEM_CANT_CREATE_CONTROLLER,
					"Error, could not create scan controller. Please report this bug to the Acquaman developers."));
			setFailed(true, AMBEAMLINEACTIONITEM_CANT_CREATE_CONTROLLER);
			return;
		}

		// bug, fixed Dec. 2011. if( !AMScanControllerSupervisor::scanControllerSupervisor()->setCurrentScanController(ctrl_) )
			// There are two ways for this function to fail. If setCurrentScanController() fails because the controller's scan is null (ie: not because there is already a valid currentScanController()), then this dialog will be shown when it shouldn't be.  Further, if the user chooses requestFail, then the ctrl_ will be deleted even though it's already been set as the current scan controller... So that when the next scan runs, setCurrentScanController() will crash when trying to delete the last currentScanController (because it's been deleted already, but not through deleteCurrentScanController().
		if( AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController() ) {

			QMessageBox currentScanControllerChoice;
			currentScanControllerChoice.setText(QString("Well this is embarressing"));
			currentScanControllerChoice.setInformativeText(QString("Acquaman thinks you're already running a scan. If you're not, press Ok to continue"));
			currentScanControllerChoice.setIcon(QMessageBox::Question);
			currentScanControllerChoice.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			currentScanControllerChoice.setDefaultButton(QMessageBox::Ok);
			currentScanControllerChoice.setEscapeButton(QMessageBox::Cancel);
			bool requestFail = false;

			if(currentScanControllerChoice.exec() == QMessageBox::Ok){
				AMScanControllerSupervisor::scanControllerSupervisor()->deleteCurrentScanController();
				if(!AMScanControllerSupervisor::scanControllerSupervisor()->setCurrentScanController(ctrl_))
					requestFail = true;
			}
			else
				requestFail = true;
			if(requestFail){
				delete ctrl_;
				AMErrorMon::report(AMErrorReport(this,
								 AMErrorReport::Alert,
								 AMBEAMLINEACTIONITEM_CANT_SET_CURRENT_CONTROLLER,
								 "Error, could not set current scan controller. Please report this bug to the Acquaman developers."));
				setFailed(true, AMBEAMLINEACTIONITEM_CANT_SET_CURRENT_CONTROLLER);
				return;
			}
		}
		else {
			if(!AMScanControllerSupervisor::scanControllerSupervisor()->setCurrentScanController(ctrl_)) {
				delete ctrl_;
				AMErrorMon::report(AMErrorReport(this,
								 AMErrorReport::Alert,
								 AMBEAMLINEACTIONITEM_CANT_SET_CURRENT_CONTROLLER,
								 "Could not set the current scan controller, because the controller had not created a valid scan. Please report this bug to the Acquaman developers."));
				setFailed(true, AMBEAMLINEACTIONITEM_CANT_SET_CURRENT_CONTROLLER);
				return;
			}
		}
		connect(ctrl_, SIGNAL(finished()), this, SLOT(onScanSucceeded()));
		connect(ctrl_, SIGNAL(cancelled()), this, SLOT(onScanCancelled()));
		connect(ctrl_, SIGNAL(failed()), this, SLOT(onScanFailed()));
		connect(ctrl_, SIGNAL(started()), this, SLOT(onScanStarted()));
		connect(ctrl_, SIGNAL(progress(double,double)), this, SIGNAL(progress(double,double)));
		connect(ctrl_, SIGNAL(paused()), this, SLOT(onScanPaused()));
		connect(ctrl_, SIGNAL(resumed()), this, SLOT(onScanResumed()));
	}
	else {
//		qdebug() << "Reinitialized, no controller creation";
	}

	//Moved from onScanStarted. This action is started once we start initializing
	setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Initializing]");
	emit descriptionChanged();
	setStarted(true);

	// should this connection happen all the time, even if controller re-initialized?
	//connect(ctrl_, SIGNAL(initialized()), ctrl_, SLOT(start()));
	connect(ctrl_, SIGNAL(initialized()), this, SLOT(onScanInitialized()));
	if(!ctrl_->initialize()){
		AMErrorMon::report(AMErrorReport(this,
										 AMErrorReport::Alert,
										 AMBEAMLINEACTIONITEM_CANT_INITIALIZE_CONTROLLER,
										 "Error, could not initialize scan controller. Please report this bug to the Acquaman developers."));

		// Bug, fixed Dec. 2011: delete ctrl_;
			// [At this point, the AMScanControllerSupervisor::currentScanController() was already set with this controller. Deleting it here but not deleting it through AMScanControllerSupervisor::deleteCurrentScanController() will cause a crash the next time a scan runs and this function calls setCurrentScanController(). That function will attempt to disconnect and delete the already-deleted controller through its non-zero but invalid pointer.]
		AMScanControllerSupervisor::scanControllerSupervisor()->deleteCurrentScanController();

		setFailed(true, AMBEAMLINEACTIONITEM_CANT_INITIALIZE_CONTROLLER);
		return;
	}
}

void AMBeamlineScanAction::cancel(){
	ctrl_->cancel();
}

void AMBeamlineScanAction::cleanup(){
	if( ctrl_ == AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController() )
		AMScanControllerSupervisor::scanControllerSupervisor()->deleteCurrentScanController();
}

void AMBeamlineScanAction::setLastSampleDescription(const QString &lastSampleDescription){
	lastSampleDescription_ = lastSampleDescription;
	setDescription(cfg_->description()+" on "+lastSampleDescription_);
	exemplar_.setSampleName(lastSampleDescription_);
	emit descriptionChanged();
}

void AMBeamlineScanAction::pause(bool pause){

	if(pause){
		setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Paused]");
		emit descriptionChanged();
		ctrl_->pause();
	}
	else{

		setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Running]");
		emit descriptionChanged();
		ctrl_->resume();
	}
}

void AMBeamlineScanAction::initialize(){
	AMBeamlineActionItem::initialize();

	onBeamlineScanningChanged(AMBeamline::bl()->isBeamlineScanning());
}

void AMBeamlineScanAction::delayedStart(bool ready){
	Q_UNUSED(ready)
	start();
}

void AMBeamlineScanAction::onScanInitialized(){
	if(!ctrl_->start()){
		AMErrorMon::report(AMErrorReport(this,
										 AMErrorReport::Alert,
										 AMBEAMLINEACTIONITEM_CANT_START_CONTROLLER,
										 "Error, could not start scan controller. Please report this bug to the Acquaman developers."));
		// Bug, fixed Dec. 2011: delete ctrl_;
			// [At this point, the AMScanControllerSupervisor::currentScanController() was already set with this controller. Deleting it here but not deleting it through AMScanControllerSupervisor::deleteCurrentScanController() will cause a crash the next time a scan runs and this function calls setCurrentScanController(). That function will attempt to disconnect and delete the already-deleted controller through its non-zero but invalid pointer.]
		AMScanControllerSupervisor::scanControllerSupervisor()->deleteCurrentScanController();
		setFailed(true, AMBEAMLINEACTIONITEM_CANT_START_CONTROLLER);
		return;
	}
}

void AMBeamlineScanAction::onScanStarted(){
	setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Running]");
	emit descriptionChanged();
	//Moving to the start call. This action is started once the scan starts initializing
	/*
	setStarted(true);
	*/
	if(!ctrl_->scan()->storeToDb(AMDatabase::database("user"))){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				AMBEAMLINEACTIONITEM_CANT_SAVE_TO_DB,
				"Error, could not save scan to database. Please report this bug to the Acquaman developers."));
	}
	else
		scanID_ = ctrl_->scan()->id();
}

void AMBeamlineScanAction::onScanPaused()
{
	setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Paused]");
	emit descriptionChanged();
	emit paused();
}

void AMBeamlineScanAction::onScanResumed()
{
	setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Running]");
	emit descriptionChanged();
	emit resumed();
}

void AMBeamlineScanAction::onScanCancelled(){
	setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Cancelled]");
	emit descriptionChanged();

	// Only save scans that already have an id in the database.
	if (scanID_ != -1){

		if(!ctrl_->scan()->storeToDb(AMDatabase::database("user"))){
			AMErrorMon::report(AMErrorReport(this,
											 AMErrorReport::Alert,
											 AMBEAMLINEACTIONITEM_CANT_SAVE_TO_DB,
											 "Error, could not save scan to database. Please report this bug to the Acquaman developers."));
		}
	}

	setFailed(true, AMBEAMLINEACTIONITEM_SCAN_CANCELLED);
}

void AMBeamlineScanAction::onScanSucceeded(){
	setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Exporting "+AMDateTimeUtils::prettyDateTime(QDateTime::currentDateTime())+"]");
	emit descriptionChanged();
	autoExportScan();	// Note: this might take a long time.  Maybe try and do it in a worker thread?
	setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Completed "+AMDateTimeUtils::prettyDateTime(QDateTime::currentDateTime())+"]");
	emit descriptionChanged();
	setSucceeded(true);
}

void AMBeamlineScanAction::onScanFailed(){

	// Only save scans that already have an id in the database.
	if (scanID_ != -1){

		if(!ctrl_->scan()->storeToDb(AMDatabase::database("user"))){
			AMErrorMon::report(AMErrorReport(this,
											 AMErrorReport::Alert,
											 AMBEAMLINEACTIONITEM_CANT_SAVE_TO_DB,
											 "Error, could not save scan to database. Please report this bug to the Acquaman developers."));
		}
	}

	setFailed(true);
}

void AMBeamlineScanAction::autoExportScan()
{
	if(ctrl_->scan()->database()){
		bool saveSucceeded = ctrl_->scan()->storeToDb(ctrl_->scan()->database());
		if(saveSucceeded && cfg_->autoExportEnabled()){
			QList<AMScan*> toExport;
			toExport << ctrl_->scan();
			AMExportController *exportController = new AMExportController(toExport);

			// This needs to be generalized so the user can set it (on beamlines where this is acceptable)
			QDir exportDir(AMUserSettings::userDataFolder);
			exportDir.cdUp();
			if(!exportDir.entryList(QDir::AllDirs).contains("exportData")){
				if(!exportDir.mkdir("exportData")){
					AMErrorMon::report(AMErrorReport(this,
							AMErrorReport::Alert,
							AMBEAMLINEACTIONITEM_CANT_CREATE_EXPORT_FOLDER,
							"Error, could not create the auto-export folder. Please report this bug to the Acquaman developers."));
					return;
				}
			}
			exportDir.cd("exportData");
			exportController->setDestinationFolderPath(exportDir.absolutePath());
			AMExporter *autoExporter = AMAppControllerSupport::createExporter(cfg_);
			if(!autoExporter){
				AMErrorMon::report(AMErrorReport(this,
								 AMErrorReport::Alert,
								 AMBEAMLINEACTIONITEM_NO_REGISTERED_EXPORTER,
								 "Error, no exporter registered for this scan type. Please report this bug to the Acquaman developers."));
				return;
			}
			exportController->chooseExporter(autoExporter->metaObject()->className());
			// This next creation involves a loadFromDb ... I tested it and it seems fast (milliseconds) ... if that's a Mac only thing then we can figure out a caching system, let me know I have a few ideas
			AMExporterOption *autoExporterOption = AMAppControllerSupport::createExporterOption(cfg_);
			if(!autoExporterOption){
				AMErrorMon::report(AMErrorReport(this,
								 AMErrorReport::Alert,
								 AMBEAMLINEACTIONITEM_NO_REGISTERED_EXPORTER_OPTION,
								 "Error, no exporter option registered for this scan type. Please report this bug to the Acquaman developers."));
				return;
			}
			exportController->setOption(autoExporterOption);
			exportController->start(true);
		}
	}
	else
		AMErrorMon::report(AMErrorReport(this,
						 AMErrorReport::Alert,
						 AMBEAMLINEACTIONITEM_NO_DATABASE_FOR_SCAN,
						 "Error, the scan isn't connected to any database. Please report this bug to the Acquaman developers."));
}

void AMBeamlineScanAction::onBeamlineScanningChanged(bool isScanning){
	setReady(!isScanning);
}

void AMBeamlineScanAction::onConfigurationChanged(){
	setDescription(cfg_->description()+" on "+lastSampleDescription_);
	emit descriptionChanged();
}

void AMBeamlineScanAction::onConfigurationDestroyed(){
	cfg_ = 0;
}

AMBeamlineScanActionView::AMBeamlineScanActionView(AMBeamlineScanAction *scanAction, int index, QWidget *parent) :
	AMBeamlineActionItemView(scanAction, index, parent)
{
	index_ = index;
	cancelLatch_ = false;
	scanAction_ = scanAction;
	setMinimumHeight(NATURAL_ACTION_VIEW_HEIGHT);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	configurationView_ = 0; //NULL

	scanNameLabel_ = new QLabel();
	scanNameLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	scanNameLabel_->setAlignment(Qt::AlignVCenter);
	updateScanNameLabel();

	exportNameLabel_ = new QLabel("Probably Exporting as: "+scanAction_->guessExportName());

	progressBar_ = new QProgressBar();
	progressBar_->setMinimum(0);
	timeRemainingLabel_ = new QLabel("Scan Not Started");
	timeRemainingLabel_->setAlignment(Qt::AlignHCenter);
	QVBoxLayout *progressVL = new QVBoxLayout();
	progressVL->addWidget(progressBar_);
	progressVL->addWidget(timeRemainingLabel_);
	connect(scanAction_, SIGNAL(descriptionChanged()), this, SLOT(updateScanNameLabel()));
	connect(scanAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
	connect(scanAction_, SIGNAL(started()), this, SLOT(onScanStarted()));
	connect(scanAction_, SIGNAL(paused()), this, SLOT(onScanPaused()));
	connect(scanAction_, SIGNAL(resumed()), this, SLOT(onScanResumed()));
	connect(scanAction_, SIGNAL(succeeded()), this, SLOT(onScanFinished()));
	connect(scanAction_, SIGNAL(failed(int)), this, SLOT(onScanFailed(int)));
	closeIcon_ = QIcon(":/window-close.png");
	stopIcon_ = QIcon(":/media-playback-stop-dark.png");
	startIcon_ = QIcon(":/media-playback-start-dark.png");
	pauseIcon_ = QIcon(":/media-playback-pause-dark.png");
	stopCancelButton_ = new QPushButton(closeIcon_, "");
	stopCancelButton_->setMaximumHeight(progressBar_->size().height());
	stopCancelButton_->setFixedWidth(25);
	stopCancelButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	playPauseButton_ = new QPushButton(startIcon_, "");
	playPauseButton_->setMaximumHeight(progressBar_->size().height());
	playPauseButton_->setFixedWidth(25);
	playPauseButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	playPauseButton_->setEnabled(false);
	moveActionUpButton_ = new QToolButton();
	moveActionUpButton_->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
	moveActionUpButton_->setEnabled(false);
	moveActionDownButton_ = new QToolButton();
	moveActionDownButton_->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
	moveActionDownButton_->setEnabled(false);
	connect(stopCancelButton_, SIGNAL(clicked()), this, SLOT(onStopCancelButtonClicked()));
	connect(playPauseButton_, SIGNAL(clicked()), this, SLOT(onPlayPauseButtonClicked()));
	connect(moveActionUpButton_, SIGNAL(clicked()), this, SLOT(onMoveUpButtonClicked()));
	connect(moveActionDownButton_, SIGNAL(clicked()), this, SLOT(onMoveDownButtonClicked()));
	firstRowLayout_ = new QHBoxLayout();
	firstRowLayout_->addWidget(scanNameLabel_, 0, Qt::AlignTop | Qt::AlignLeft);
	firstRowLayout_->addLayout(progressVL);
	firstRowLayout_->addWidget(playPauseButton_, 0, Qt::AlignTop | Qt::AlignRight);
	firstRowLayout_->addWidget(stopCancelButton_, 0, Qt::AlignTop | Qt::AlignRight);
	QVBoxLayout *swapVL = new QVBoxLayout();
	swapVL->addWidget(moveActionUpButton_);
	swapVL->addWidget(moveActionDownButton_);
	swapVL->setContentsMargins(0,0,0,0);
	swapVL->setSpacing(0);
	firstRowLayout_->addLayout(swapVL);
	secondRowLayout_ = new QHBoxLayout;
	secondRowLayout_->addWidget(exportNameLabel_);

	vl_ = new QVBoxLayout();
	vl_->addLayout(firstRowLayout_);
	vl_->addLayout(secondRowLayout_);

	//setLayout(hl_);
	setLayout(vl_);
	onPreviousNextChanged();
}

void AMBeamlineScanActionView::setIndex(int index){
	index_ = index;
	updateScanNameLabel();
}

void AMBeamlineScanActionView::setAction(AMBeamlineActionItem *action){
	AMBeamlineScanAction *scanAction = qobject_cast<AMBeamlineScanAction*>(action);
	if(scanAction_){
		disconnect(scanAction_, SIGNAL(descriptionChanged()), this, SLOT(updateScanNameLabel()));
		disconnect(scanAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
		disconnect(scanAction_, SIGNAL(started()), this, SLOT(onScanStarted()));
		disconnect(scanAction_, SIGNAL(paused()), this, SLOT(onScanPaused()));
		disconnect(scanAction_, SIGNAL(resumed()), this, SLOT(onScanResumed()));
		disconnect(scanAction_, SIGNAL(succeeded()), this, SLOT(onScanFinished()));
		disconnect(scanAction_, SIGNAL(previousChanged()), this, SLOT(onPreviousNextChanged()));
		disconnect(scanAction_, SIGNAL(nextChanged()), this, SLOT(onPreviousNextChanged()));
	}
	scanAction_ = scanAction;
	if(scanAction_){
		updateScanNameLabel();
		connect(scanAction_, SIGNAL(descriptionChanged()), this, SLOT(updateScanNameLabel()));
		connect(scanAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
		connect(scanAction_, SIGNAL(started()), this, SLOT(onScanStarted()));
		connect(scanAction_, SIGNAL(paused()), this, SLOT(onScanPaused()));
		connect(scanAction_, SIGNAL(resumed()), this, SLOT(onScanResumed()));
		connect(scanAction_, SIGNAL(succeeded()), this, SLOT(onScanFinished()));
		connect(scanAction_, SIGNAL(previousChanged()), this, SLOT(onPreviousNextChanged()));
		connect(scanAction_, SIGNAL(nextChanged()), this, SLOT(onPreviousNextChanged()));
	}
	onPreviousNextChanged();
}

void AMBeamlineScanActionView::onInfoChanged(){

}

void AMBeamlineScanActionView::updateScanNameLabel(){
	QString scanName, tmpStr;
	if(index_ != -1){
		scanName.setNum(index_);
		scanName.append(". ");
	}

	scanName.append(scanAction_->cfg()->detailedDescription());
	scanNameLabel_->setText(scanName);
	setWindowTitle(scanAction_->description());

}

void AMBeamlineScanActionView::updateProgressBar(double elapsed, double total){
	if(cancelLatch_)
		return;
	progressBar_->setMaximum((int)total);
	progressBar_->setValue((int)elapsed);

	double secondsRemaining = total - elapsed;
	QTime tRemaining = QTime(0,0,0,0).addMSecs(int(1000*secondsRemaining));
	QString rStr = (tRemaining.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	timeRemainingLabel_->setText(tRemaining.toString(rStr)+" Remaining");
}

void AMBeamlineScanActionView::onScanStarted(){
	cancelLatch_ = false;
	disconnect(moveActionUpButton_, SIGNAL(clicked()), this, SLOT(onMoveUpButtonClicked()));
	disconnect(moveActionDownButton_, SIGNAL(clicked()), this, SLOT(onMoveDownButtonClicked()));
	moveActionUpButton_->setEnabled(false);
	moveActionDownButton_->setEnabled(false);
	stopCancelButton_->setIcon(stopIcon_);
	playPauseButton_->setIcon(pauseIcon_);
	playPauseButton_->setEnabled(true);
	updateLook();
}

void AMBeamlineScanActionView::onScanPaused()
{
	playPauseButton_->setIcon(startIcon_);
}

void AMBeamlineScanActionView::onScanResumed()
{
	playPauseButton_->setIcon(pauseIcon_);
}

void AMBeamlineScanActionView::onScanFinished(){
	progressBar_->setValue(progressBar_->maximum());

	progressBar_->setMaximum(100);
	progressBar_->setValue(100);
	if(!cancelLatch_)
		timeRemainingLabel_->setText("Scan Complete");
	cancelLatch_ = false;
	disconnect(stopCancelButton_, SIGNAL(clicked()), this, SLOT(onStopCancelButtonClicked()));
	disconnect(playPauseButton_, SIGNAL(clicked()), this, SLOT(onPlayPauseButtonClicked()));
	firstRowLayout_->removeWidget(stopCancelButton_);
	firstRowLayout_->removeWidget(playPauseButton_);
	stopCancelButton_->hide();
	playPauseButton_->hide();
//	qdebug() << "Has succeeded " << scanAction_->hasSucceeded() << " has failed " << scanAction_->hasFailed() << " is running " << scanAction_->isRunning();
	updateLook();
}

void AMBeamlineScanActionView::onScanFailed(int explanation){
	cancelLatch_ = true;
	progressBar_->setMaximum(100);
	progressBar_->setValue(100);
	if(explanation == 102)//102 is scan cancelled
		timeRemainingLabel_->setText("Scan Cancelled");
	else
		timeRemainingLabel_->setText("Scan Failed");
	disconnect(stopCancelButton_, SIGNAL(clicked()), this, SLOT(onStopCancelButtonClicked()));
	disconnect(playPauseButton_, SIGNAL(clicked()), this, SLOT(onPlayPauseButtonClicked()));
	firstRowLayout_->removeWidget(stopCancelButton_);
	firstRowLayout_->removeWidget(playPauseButton_);
	stopCancelButton_->hide();
	playPauseButton_->hide();

	updateLook();
	/* NTBD David Chevrier May 16, 2011
	cancelLatch_ = true;
	stopCancelButton_->setIcon(closeIcon_);
	playPauseButton_->setIcon(startIcon_);
	playPauseButton_->setEnabled(false);
	if(explanation == 102)//102 is scan cancelled
		timeRemainingLabel_->setText("Scan Cancelled");
	else
		timeRemainingLabel_->setText("Scan Failed");
	updateLook();
	*/
}

void AMBeamlineScanActionView::onStopCancelButtonClicked(){
	/// \todo isRunning is not true if paused (or stopped). In this case, we should do this check for any scans that have been started but not finished. How to determine that from states?
	if(scanAction_->isRunning()){
		scanAction_->pause(true);
		/*
		QMessageBox msgBox;
		msgBox.setText("You're cancelling a scan in progress.");
		msgBox.setInformativeText("Do you want to keep the data from this partially collected scan?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		switch (ret) {
		case QMessageBox::Save:
			scanAction_->cancel();
			// should be already saved to the db.
			break;
		case QMessageBox::Discard:
			scanAction_->cancel();
			scanAction_->discardScan();// move scan to recycle bin
			break;
		case QMessageBox::Cancel:
			scanAction_->pause(false);
			break;
		default:
			// should never be reached
			break;
		}
		*/
		scanAction_->cancel();
	}
	else{
		emit removeRequested(scanAction_);
	}
}

void AMBeamlineScanActionView::onPlayPauseButtonClicked(){
	if(scanAction_->isRunning() && !scanAction_->isPaused()){
		playPauseButton_->setIcon(startIcon_);
		scanAction_->pause(true);
		emit pauseRequested(scanAction_);
	}
	else if(scanAction_->isRunning() && scanAction_->isPaused()){
		playPauseButton_->setIcon(pauseIcon_);
		scanAction_->pause(false);
		emit resumeRequested(scanAction_);
	}
}

void AMBeamlineScanActionView::onPreviousNextChanged(){
	if(scanAction_ && !scanAction_->hasFinished() && scanAction_->previous() && !scanAction_->previous()->isRunning() && !scanAction_->previous()->hasFinished())
		moveActionUpButton_->setEnabled(true);
	else
		moveActionUpButton_->setEnabled(false);
	if(scanAction_ && !scanAction_->hasFinished() && scanAction_->next())
		moveActionDownButton_->setEnabled(true);
	else
		moveActionDownButton_->setEnabled(false);
}

void AMBeamlineScanActionView::onMoveUpButtonClicked(){
	emit moveUpRequested(scanAction_);
}

void AMBeamlineScanActionView::onMoveDownButtonClicked(){
	emit moveDownRequested(scanAction_);
}

#include "ui/acquaman/AMScanConfigurationView.h"

void AMBeamlineScanActionView::onScanConfigurationDestroyed(){
	delete configurationView_;
	configurationView_ = 0;
}


void AMBeamlineScanActionView::mouseDoubleClickEvent(QMouseEvent *){
	// if we don't have a configuration view yet, try to create one.  (This might fail, if no default view is defined for this scan configuration.)
	if(!configurationView_){
		configurationView_ = scanAction_->cfg()->createView();
		connect(scanAction_->cfg(), SIGNAL(destroyed()), this, SLOT(onScanConfigurationDestroyed()));
	}
	if(configurationView_ == 0)
		return;
	if(scanAction_->configurationLocked())
		configurationView_->setDisabled(true);
	configurationView_->setWindowModality(Qt::WindowModal);
	configurationView_->show();
}
