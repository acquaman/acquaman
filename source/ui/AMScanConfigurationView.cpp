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

#include <QDebug>

#include "AMScanConfigurationView.h"
#include "acquaman/AMScanConfiguration.h"

AMScanConfigurationHolder::AMScanConfigurationHolder(AMScanConfigurationView* view, QWidget *parent) :
		QWidget(parent)
{
	view_ = view;

	startScanButton_ = new QPushButton("Start Scan");
	// startScanButton_->setDisabled(true);
	addToQueueButton_ = new QPushButton("Add to Workflow");

	goToWorkflowOption_ = new QRadioButton("Show me the workflow");
	setupAnotherScanOption_ = new QRadioButton("Setup another scan");

	statusLabel_ = new QLabel("Ready To Start Scan");

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(view_);
	QHBoxLayout* hl = new QHBoxLayout();

	hl->addWidget(statusLabel_);
	hl->addStretch();
	hl->addWidget(new QLabel("When I'm done here:"));
	hl->addWidget(goToWorkflowOption_);
	hl->addWidget(setupAnotherScanOption_);
	hl->addStretch();
	hl->addWidget(addToQueueButton_);
	hl->addWidget(startScanButton_);

	vl->addLayout(hl);

	setLayout(vl);

	connect(startScanButton_, SIGNAL(clicked()), this, SLOT(onStartScanRequested()));
	connect(addToQueueButton_, SIGNAL(clicked()), this, SLOT(onAddToQueueRequested()));
}


AMScanConfigurationHolder::~AMScanConfigurationHolder()
{
}

void AMScanConfigurationHolder::onFreeToScan(bool queueEmpty, bool queueNotRunning){
	qDebug() << "In onFreeToScan";
	if(queueNotRunning && queueEmpty){
		canStartImmediately_ = true;
		statusLabel_->setText("Ready to Start Scan");
	}
	else if(queueNotRunning && !queueEmpty){
		qDebug() << "Queue is not running but also not empty, have to ask user what to do";
		scanDirector_->showDirector("The workflow is not empty");
		statusLabel_->setText("Scans are waiting in the workflow");
//		emit cancelAddToQueueRequest();
	}
	else if(!queueNotRunning && queueEmpty){
		qDebug() << "Queue is running, but is empty after this, ask user what to do";
		scanDirector_->showDirector("The workflow is running but empty after the current action");
		statusLabel_->setText("Scans are running, but the last one is about to finish.");
		emit cancelAddToQueueRequest();
	}
	else{
		qDebug() << "Queue is running and has stuff in it, ask user what to do";
		scanDirector_->showDirector("The workflow is running");
		statusLabel_->setText("Scans are running in the workflow.");
		emit cancelAddToQueueRequest();
	}
}

void AMScanConfigurationHolder::onLockdownScanning(bool isLocked, QString reason){
	emit lockdownScanning(isLocked, reason);
}

void AMScanConfigurationHolder::onAddedToQueue(AMScanConfiguration *configuration){
	qDebug() << "In onAddedToQueue";
	/* if(configuration == configuration_ && !canStartImmediately_)
	//	director->showDirector();
	else */ if(configuration == configuration_ && canStartImmediately_){
		canStartImmediately_ = false;
		goToQueue();
	}
}

void AMScanConfigurationHolder::onStartScanRequested(){
	requestedStart_ = true;
	qDebug() << "A holder is requesting a scan start";
	configuration_ = view_->configuration()->createCopy();
	emit addToQueueRequested(configuration_, true);
}

void AMScanConfigurationHolder::onAddToQueueRequested(){
	configuration_ = view_->configuration()->createCopy();
	emit addToQueueRequested(configuration_);
}

void AMScanConfigurationHolder::goToQueue(){
	qDebug() << "goToQueue emits";
	emit goToQueueRequested();
}

void AMScanConfigurationHolder::goToNewScan(){
	/*
<<<<<<< HEAD
	qDebug() << "goToNewScan";
	destroyScanConfigurationViewer();
=======
>>>>>>> 3c1c17f0ec27cce452cc8f8501c27ba8c060a70d
	*/
	onBecameCurrentWidget();
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

void AMScanConfigurationScanDirector::showDirector(const QString& reason){
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

/*
AMFastScanConfigurationHolder::AMFastScanConfigurationHolder(QWidget *parent) :
		AMScanConfigurationHolder(parent)
{
	vl_ = NULL;
	cfg_ = NULL;
	sfscViewer_ = NULL;

	autoSavePath_ = "";
	lastSettings_ = 0; //NULL

	requestedStart_ = false;
	canStartImmediately_ = false;
	director = new AMScanConfigurationQueueDirector();
	director->setWindowModality(Qt::ApplicationModal);
	sDirector = new AMScanConfigurationScanDirector();
	sDirector->setWindowModality(Qt::ApplicationModal);

	connect(director, SIGNAL(goToQueue()), this, SLOT(goToQueue()));
	connect(director, SIGNAL(goToNewScan()), this, SLOT(goToNewScan()));
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
		connect(sfscViewer_, SIGNAL(addToQueueRequested()), this, SLOT(onAddToQueueRequested()));
		connect(sfscViewer_, SIGNAL(queueDirectorRequested()), director, SLOT(show()));
		connect(sfscViewer_, SIGNAL(lastSettings(SGMFastScanParameters*)), this, SLOT(setLastSettings(SGMFastScanParameters*)));
		connect(this, SIGNAL(lockdownScanning(bool,QString)), sfscViewer_, SLOT(onLockdowScanning(bool,QString)));

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
	if(!autoSavePath_.isEmpty())
		cfg()->setSensibleFileSavePath(autoSavePath_);
	connect(cfg(), SIGNAL(onSensibleFileSavePathChanged(QString)), this, SLOT(setAutoSavePath(QString)));
	if(lastSettings_)
		cfg()->setParameters(lastSettings_);


}

void AMFastScanConfigurationHolder::destroyScanConfigurationViewer(){
	qDebug() << "Trying to destroy fast scan viewer";
	if(sfscViewer_){
		disconnect(sfscViewer_, SIGNAL(startScanRequested()), this, SLOT(onStartScanRequested()));
		disconnect(sfscViewer_, SIGNAL(addToQueueRequested()), this, SLOT(onAddToQueueRequested()));
		disconnect(sfscViewer_, SIGNAL(queueDirectorRequested()), director, SLOT(show()));
		disconnect(sfscViewer_, SIGNAL(lastSettings(SGMFastScanParameters*)), this, SLOT(setLastSettings(SGMFastScanParameters*)));
		disconnect(this, SIGNAL(lockdownScanning(bool,QString)), sfscViewer_, SLOT(onLockdowScanning(bool,QString)));
		vl_->removeWidget(sfscViewer_);
		delete sfscViewer_;
		sfscViewer_ = NULL;
	}
}

void AMFastScanConfigurationHolder::setAutoSavePath(const QString &autoSavePath){
	autoSavePath_ = autoSavePath;
}

void AMFastScanConfigurationHolder::setLastSettings(SGMFastScanParameters *lastSettings){
	lastSettings_ = lastSettings;
}
*/
