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


#include "AMScanController.h"

AMScanControllerSupervisor* AMScanControllerSupervisor::instance_ = 0;

AMScanController::AMScanController(AMScanConfiguration *cfg, QObject *parent) :
	QObject(parent)
{
	generalCfg_ = cfg;
	_pCfg_ = & generalCfg_;
	generalScan_ = NULL;
	_pScan_ = &generalScan_;
}


AMScanControllerSupervisor::AMScanControllerSupervisor(QObject *parent) :
		QObject(parent)
{
	currentScanController_ = NULL;
}


AMScanControllerSupervisor::~AMScanControllerSupervisor()
{
}

AMScanControllerSupervisor* AMScanControllerSupervisor::scanControllerSupervisor(){
	if(instance_ == 0)
		instance_ = new AMScanControllerSupervisor();
	return instance_;
}

void AMScanControllerSupervisor::releaseScanControllerSupervisor(){
	if(instance_){
		delete instance_;
		instance_ = 0;
	}
}

AMScanController* AMScanControllerSupervisor::currentScanController(){
	return currentScanController_;
}

bool AMScanControllerSupervisor::setCurrentScanController(AMScanController *newScanController){
//	qDebug() << "In setCurrentSC with " << (int)currentScanController_;
	if(currentScanController_)
		return false;
	currentScanController_ = newScanController;
	qDebug()  << " and " << currentScanController_->scan();
	if(!currentScanController_->scan())
		return false;
	connect(currentScanController_, SIGNAL(finished()), this, SLOT(onCurrentScanControllerFinished()));
	connect(currentScanController_, SIGNAL(reinitialized(bool)), this, SLOT(onCurrentScanControllerReinitialized(bool)));
	emit currentScanControllerCreated();
	return true;
}

bool AMScanControllerSupervisor::deleteCurrentScanController(){
	if(!currentScanController_)
		return false;
	onCurrentScanControllerFinished();
	return true;
}

void AMScanControllerSupervisor::onCurrentScanControllerFinished(){
	emit currentScanControllerDestroyed();
	disconnect(currentScanController_, SIGNAL(finished()), this, SLOT(onCurrentScanControllerFinished()));
	disconnect(currentScanController_, SIGNAL(reinitialized(bool)), this, SLOT(onCurrentScanControllerReinitialized(bool)));
	currentScanController_->deleteLater();
	currentScanController_ = NULL;
}

void AMScanControllerSupervisor::onCurrentScanControllerReinitialized(bool removeScan){
	emit currentScanControllerReinitialized(removeScan);
}
