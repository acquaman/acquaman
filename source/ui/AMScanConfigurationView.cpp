/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMScanConfigurationView.h"

AMScanConfigurationHolder::AMScanConfigurationHolder(QWidget *parent) :
		QWidget(parent)
{
}

AMScanConfigurationHolder::~AMScanConfigurationHolder()
{
}

void AMScanConfigurationHolder::onFreeToScan(bool queueEmpty, bool queueNotRunning){
	qDebug() << "In onFreeToScan";
	if(queueNotRunning && queueEmpty){
		canStartImmediately_ = true;
	}
	else if(queueNotRunning && !queueEmpty){
		qDebug() << "Queue is not running but also not empty, have to ask user what to do";
		sDirector->showDirector("Queue is not empty");
//		emit cancelAddToQueueRequest();
	}
	else if(!queueNotRunning && queueEmpty){
		qDebug() << "Queue is running, but is empty after this, ask user what to do";
		sDirector->showDirector("Queue is running but empty after current action");
		emit cancelAddToQueueRequest();
	}
	else{
		qDebug() << "Queue is running and has stuff in it, ask user what to do";
		sDirector->showDirector("Queue is running");
		emit cancelAddToQueueRequest();
	}
}

void AMScanConfigurationHolder::onLockdownScanning(bool isLocked, QString reason){
	emit lockdownScanning(isLocked, reason);
}

void AMScanConfigurationHolder::onAddedToQueue(AMScanConfiguration *cfg){
	qDebug() << "In onAddedToQueue";
	if(cfg == cfg_ && !canStartImmediately_)
		director->showDirector();
	else if(cfg == cfg_ && canStartImmediately_){
		canStartImmediately_ = false;
		goToQueue();
	}
}

void AMScanConfigurationHolder::onStartScanRequested(){
	requestedStart_ = true;
	qDebug() << "A holder is requesting a scan start";
	emit addToQueueRequested(cfg_, true);
}

void AMScanConfigurationHolder::onAddToQueueRequested(){
	emit addToQueueRequested(cfg_);
}

void AMScanConfigurationHolder::goToQueue(){
	qDebug() << "goToQueue emits";
	destroyScanConfigurationViewer();
	emit goToQueueRequested();
}

void AMScanConfigurationHolder::goToNewScan(){
	destroyScanConfigurationViewer();
	onBecameCurrentWidget();
}

AMXASScanConfigurationHolder::AMXASScanConfigurationHolder(QWidget *parent) :
	AMScanConfigurationHolder(parent)
{
	cfg_ = NULL;
	cfgDetectorInfoSet_ = NULL;
	sxscViewer = NULL;
	sxscWizard = NULL;
	vl_ = NULL;
	requestedStart_ = false;
	canStartImmediately_ = false;
	director = new AMScanConfigurationQueueDirector();
	director->setWindowModality(Qt::ApplicationModal);
	sDirector = new AMScanConfigurationScanDirector();
	sDirector->setWindowModality(Qt::ApplicationModal);

	connect(director, SIGNAL(goToQueue()), this, SLOT(goToQueue()));
	connect(director, SIGNAL(goToNewScan()), this, SLOT(goToNewScan()));
}

AMXASScanConfigurationHolder::~AMXASScanConfigurationHolder(){
	delete director;
	delete sDirector;
}

void AMXASScanConfigurationHolder::createScanConfiguration(){
	cfg_ = new SGMXASScanConfiguration(this);
	cfg()->setFileName("daveData.%03d.dat");
	cfg()->setFilePath(AMUserSettings::userDataFolder);
	cfg()->addRegion(0, 950, 1, 960);

	cfgDetectorInfoSet_ = new AMDetectorInfoSet(this);
	cfg()->setCfgDetectorInfoSet(cfgDetectorInfoSet_);
	AMDetectorInfo* tmpDI, *tdi;
	for(int x = 0; x < cfg()->detectorSet()->count(); x++){
		tdi = cfg()->detectorSet()->detectorAt(x);
		#warning "D: same edit to review. Was tdi a PGTDetector or a PGTDetectorInfo?"
		if( qobject_cast<PGTDetector*>(tdi) )
			tmpDI = new PGTDetectorInfo(tdi->name(), tdi->description(), this);
		else if( qobject_cast<MCPDetector*>(tdi) )
			tmpDI = new MCPDetectorInfo(tdi->name(), tdi->description(), this);
		else
			tmpDI = new AMDetectorInfo(tdi->name(), tdi->description(), this);

		/*! \bug Removed with metaData change.  Need to repair. What's going on here? Is this function even still being used? What's up with the "daveData.xxx.dat" file name?
		QList<AMMetaMetaData> all = tmpDI->metaDataAllKeys();
		for(int y = 0; y < all.count(); y++)
			tmpDI->setMetaData(all.at(y).key, tdi->metaData(all.at(y).key));
			*/
		cfgDetectorInfoSet_->addDetector(tmpDI, cfg()->detectorSet()->isDefaultAt(x));
	}
}

void AMXASScanConfigurationHolder::destroyScanConfigurationViewer(){
	if(sxscViewer){
		disconnect(sxscViewer, SIGNAL(startScanRequested()), this, SLOT(onStartScanRequested()));
		disconnect(sxscViewer, SIGNAL(addToQueueRequested()), this, SLOT(onAddToQueueRequested()));
		disconnect(sxscViewer, SIGNAL(queueDirectorRequested()), director, SLOT(show()));
		disconnect(this, SIGNAL(lockdownScanning(bool,QString)), sxscViewer, SLOT(onLockdowScanning(bool,QString)));
		vl_->removeWidget(sxscViewer);
		delete sxscViewer;
		sxscViewer = NULL;
	}
}

void AMXASScanConfigurationHolder::onBecameCurrentWidget(){
	if(!sxscViewer && isVisible() && SGMBeamline::sgm()->isConnected()){
		createScanConfiguration();
		sxscViewer = new SGMXASScanConfigurationViewer(cfg(), cfgDetectorInfoSet_);
		connect(sxscViewer, SIGNAL(startScanRequested()), this, SLOT(onStartScanRequested()));
		connect(sxscViewer, SIGNAL(addToQueueRequested()), this, SLOT(onAddToQueueRequested()));
		connect(sxscViewer, SIGNAL(queueDirectorRequested()), director, SLOT(show()));
		connect(this, SIGNAL(lockdownScanning(bool,QString)), sxscViewer, SLOT(onLockdowScanning(bool,QString)));
		if(!vl_)
			vl_ = new QVBoxLayout();
		vl_->addWidget(sxscViewer);
		if(layout() != vl_){
			delete layout();
			this->setLayout(vl_);
		}
		emit newScanConfigurationView();
	}
//	if(!sxscWizard && isVisible() && SGMBeamline::sgm()->isConnected()){
//		sxscWizard = new SGMXASScanConfigurationWizard(cfg_, cfgDetectorInfoSet_);
//		connect(sxscWizard, SIGNAL(scanStartRequested()), this, SLOT(onStartScanRequested()));
//		sxscWizard->show();
//	}
}

AMScanConfigurationQueueDirector::AMScanConfigurationQueueDirector(QWidget *parent) :
		QWidget(parent)
{
	alwaysGoToQueue_ = false;
	alwaysGoToNewScan_ = false;

	vl_ = new QVBoxLayout();
	message_ = new QLabel("<h3>What would you like to do now?</h3>");
	vl_->addWidget(message_);
	QSpacerItem *tmpSpc = new QSpacerItem(10, 20, QSizePolicy::Preferred, QSizePolicy::Maximum);
	vl_->addSpacerItem(tmpSpc);
	choices_ = new QFormLayout();
	QHBoxLayout *tmpHB;
	tmpHB = new QHBoxLayout();
	goToQueueButton_ = new QPushButton("Workflow Queue");
	goToQueueCheck_ = new QCheckBox("Always do this");
	tmpHB->addSpacerItem(new QSpacerItem(25, 10, QSizePolicy::Maximum, QSizePolicy::Preferred));
	tmpHB->addWidget(goToQueueButton_);
	tmpHB->addWidget(goToQueueCheck_);
	choices_->addRow("Go to the Workflow Queue", tmpHB);
	tmpHB = new QHBoxLayout();
	goToNewScanButton_ = new QPushButton("New Scan");
	goToNewScanCheck_ = new QCheckBox("Always do this");
	tmpHB->addSpacerItem(new QSpacerItem(25, 10, QSizePolicy::Maximum, QSizePolicy::Preferred));
	tmpHB->addWidget(goToNewScanButton_);
	tmpHB->addWidget(goToNewScanCheck_);
	choices_->addRow("Create a New Scan Configuration", tmpHB);
	vl_->addLayout(choices_);
	connect(goToQueueButton_, SIGNAL(clicked()), this, SIGNAL(goToQueue()));
	connect(goToQueueButton_, SIGNAL(clicked()), this, SLOT(hide()));
	connect(goToNewScanButton_, SIGNAL(clicked()), this, SIGNAL(goToNewScan()));
	connect(goToNewScanButton_, SIGNAL(clicked()), this, SLOT(hide()));
	connect(goToQueueCheck_, SIGNAL(toggled(bool)), this, SLOT(onAlwaysQueueCheck(bool)));
	connect(goToNewScanCheck_, SIGNAL(toggled(bool)), this, SLOT(onAlwaysNewScanCheck(bool)));
	setLayout(vl_);
}

void AMScanConfigurationQueueDirector::showDirector(){
	if(alwaysGoToQueue_)
		emit goToQueue();
	else if(alwaysGoToNewScan_)
		emit goToNewScan();
	else
		show();
}

void AMScanConfigurationQueueDirector::onAlwaysQueueCheck(bool checked){
	alwaysGoToQueue_ = checked;
	if(checked && alwaysGoToNewScan_)
		goToNewScanCheck_->setChecked(false);
	if(alwaysGoToQueue_)
		goToNewScanButton_->setEnabled(false);
	else
		goToNewScanButton_->setEnabled(true);
}

void AMScanConfigurationQueueDirector::onAlwaysNewScanCheck(bool checked){
	alwaysGoToNewScan_ = checked;
	if(checked && alwaysGoToQueue_)
		goToQueueCheck_->setChecked(false);
	if(alwaysGoToNewScan_)
		goToQueueButton_->setEnabled(false);
	else
		goToQueueButton_->setEnabled(true);
}

AMScanConfigurationScanDirector::AMScanConfigurationScanDirector(QWidget *parent) :
		QWidget(parent)
{
	alwaysStartNow_ = false;
	alwaysWaitNext_ = false;
	alwaysAppend_ = false;
	alwaysCancel_ = false;

	vl_ = new QVBoxLayout();
	message_ = "";
	messageLabel_ = new QLabel(message_);
	vl_->addWidget(messageLabel_);
	gl_ = new QGridLayout();
	startNowButton_ = new QPushButton("Start Now");
	waitNextButton_ = new QPushButton("Wait");
	appendButton_ = new QPushButton("Add to Queue");
	cancelButton_ = new QPushButton("Cancel");
	startNowLabel_ = new QLabel("Stop the queue and start this scan immediately");
	startNowLabel_->setWordWrap(true);
	waitNextLabel_ = new QLabel("Wait until the current queue action is completed, then run this scan");
	waitNextLabel_->setWordWrap(true);
	appendLabel_ = new QLabel("Add this scan to the end of the queue");
	appendLabel_->setWordWrap(true);
	cancelLabel_ = new QLabel("Return to the scan configuration page");
	cancelLabel_->setWordWrap(true);
	setDefaultLabel_ = new QLabel("Always do this");
	startNowCheck_ = new QCheckBox("");
	waitNextCheck_ = new QCheckBox("");
	appendCheck_ = new QCheckBox("");
	cancelCheck_ = new QCheckBox("");
	gl_->addWidget(startNowButton_,		1, 0, 1, 1, Qt::AlignLeft);
	gl_->addWidget(waitNextButton_,		2, 0, 1, 1, Qt::AlignLeft);
	gl_->addWidget(appendButton_,		3, 0, 1, 1, Qt::AlignLeft);
	gl_->addWidget(cancelButton_,		4, 0, 1, 1, Qt::AlignLeft);
	gl_->addWidget(startNowLabel_,		1, 1, 1, 1, Qt::AlignLeft);
	gl_->addWidget(waitNextLabel_,		2, 1, 1, 1, Qt::AlignLeft);
	gl_->addWidget(appendLabel_,		3, 1, 1, 1, Qt::AlignLeft);
	gl_->addWidget(cancelLabel_,		4, 1, 1, 1, Qt::AlignLeft);
	gl_->addWidget(startNowCheck_,		1, 2, 1, 1, Qt::AlignRight);
	gl_->addWidget(waitNextCheck_,		2, 2, 1, 1, Qt::AlignRight);
	gl_->addWidget(appendCheck_,		3, 2, 1, 1, Qt::AlignRight);
	gl_->addWidget(cancelCheck_,		4, 2, 1, 1, Qt::AlignRight);
	gl_->addWidget(setDefaultLabel_,	0, 2, 1, 1, Qt::AlignLeft);
	vl_->addLayout(gl_);
	setLayout(vl_);
	connect(startNowButton_, SIGNAL(clicked()), this, SIGNAL(startNow()));
	connect(startNowCheck_, SIGNAL(toggled(bool)), this, SLOT(onAlwaysStartNow(bool)));
	connect(waitNextButton_, SIGNAL(clicked()), this, SLOT(onWaitNext()));
	connect(waitNextCheck_, SIGNAL(toggled(bool)), this, SLOT(onAlwaysWaitNext(bool)));
	connect(appendButton_, SIGNAL(clicked()), this, SLOT(onAppend()));
	connect(appendCheck_, SIGNAL(toggled(bool)), this, SLOT(onAlwaysAppend(bool)));
	connect(cancelButton_, SIGNAL(clicked()), this, SIGNAL(cancel()));
	connect(cancelCheck_, SIGNAL(toggled(bool)), this, SLOT(onAlwaysCancel(bool)));
}

void AMScanConfigurationScanDirector::showDirector(QString reason){
	message_ = reason;
	messageLabel_->setText(message_);
	if(alwaysStartNow_)
		emit startNow();
	else if(alwaysWaitNext_)
		emit addToQueue(1);
	else if(alwaysAppend_)
		emit addToQueue(-1);
	else if(alwaysCancel_)
		emit cancel();
	else
		show();
}

void AMScanConfigurationScanDirector::onWaitNext(){
	emit addToQueue(1);
}

void AMScanConfigurationScanDirector::onAppend(){
	emit addToQueue(-1);
}

void AMScanConfigurationScanDirector::onAlwaysStartNow(bool checked){
	alwaysStartNow_ = checked;
}

void AMScanConfigurationScanDirector::onAlwaysWaitNext(bool checked){
	alwaysWaitNext_ = checked;
}

void AMScanConfigurationScanDirector::onAlwaysAppend(bool checked){
	alwaysAppend_ = checked;
}

void AMScanConfigurationScanDirector::onAlwaysCancel(bool checked){
	alwaysCancel_ = checked;
}

AMFastScanConfigurationHolder::AMFastScanConfigurationHolder(QWidget *parent) :
		AMScanConfigurationHolder(parent)
{
	vl_ = NULL;
	cfg_ = NULL;
	sfscViewer_ = NULL;
}

AMFastScanConfigurationHolder::~AMFastScanConfigurationHolder()
{
}

void AMFastScanConfigurationHolder::onBecameCurrentWidget()
{
	if(!sfscViewer_ && isVisible() && SGMBeamline::sgm()->isConnected()){
		createScanConfiguration();
		sfscViewer_ = new SGMFastScanConfigurationViewer(cfg());
		connect(sfscViewer_, SIGNAL(startScanRequested()), this, SLOT(onStartScanRequested()));
		/*
		connect(sxscViewer, SIGNAL(startScanRequested()), this, SLOT(onStartScanRequested()));
		connect(sxscViewer, SIGNAL(addToQueueRequested()), this, SLOT(onAddToQueueRequested()));
		connect(sxscViewer, SIGNAL(queueDirectorRequested()), director, SLOT(show()));
		connect(this, SIGNAL(lockdownScanning(bool,QString)), sxscViewer, SLOT(onLockdowScanning(bool,QString)));
		*/
		if(!vl_)
			vl_ = new QVBoxLayout();
		vl_->addWidget(sfscViewer_);
		if(layout() != vl_){
			delete layout();
			this->setLayout(vl_);
		}
		emit newScanConfigurationView();
	}
}

void AMFastScanConfigurationHolder::createScanConfiguration(){
	cfg_ = new SGMFastScanConfiguration(this);
	cfg_->setFileName("daveData.%03d.dat");
	cfg_->setFilePath(AMUserSettings::userDataFolder);

	/*
	cfgDetectorInfoSet_ = new AMDetectorInfoSet(this);
	cfg_->setCfgDetectorInfoSet(cfgDetectorInfoSet_);
	AMDetectorInfo* tmpDI, *tdi;
	for(int x = 0; x < cfg_->detectorSet()->count(); x++){
		tdi = cfg_->detectorSet()->detectorAt(x);
		#warning "D: same edit to review. Was tdi a PGTDetector or a PGTDetectorInfo?"
		if( qobject_cast<PGTDetector*>(tdi) )
			tmpDI = new PGTDetectorInfo(tdi->name(), tdi->description(), this);
		else if( qobject_cast<MCPDetector*>(tdi) )
			tmpDI = new MCPDetectorInfo(tdi->name(), tdi->description(), this);
		else
			tmpDI = new AMDetectorInfo(tdi->name(), tdi->description(), this);


		cfgDetectorInfoSet_->addDetector(tmpDI, cfg_->detectorSet()->isDefaultAt(x));
	}
	*/
}

void AMFastScanConfigurationHolder::destroyScanConfigurationViewer(){
	/*
	if(sfscViewer){
		disconnect(sxscViewer, SIGNAL(startScanRequested()), this, SLOT(onStartScanRequested()));
		disconnect(sxscViewer, SIGNAL(addToQueueRequested()), this, SLOT(onAddToQueueRequested()));
		disconnect(sxscViewer, SIGNAL(queueDirectorRequested()), director, SLOT(show()));
		disconnect(this, SIGNAL(lockdownScanning(bool,QString)), sxscViewer, SLOT(onLockdowScanning(bool,QString)));
		vl_->removeWidget(sxscViewer);
		delete sxscViewer;
		sxscViewer = NULL;
	}
	*/
}
