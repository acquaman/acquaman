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


#include "AMSampleMoveAction.h"

#include "beamline/camera/AMSampleCamera.h"
#include "beamline/AMBeamline.h"
#include "dataman/AMSamplePlate.h"
#include "util/AMErrorMonitor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"

AMSampleMoveAction::AMSampleMoveAction(AMSampleMoveActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	moveListAction_ = 0; //NULL
}

AMSampleMoveAction::AMSampleMoveAction(const AMSampleMoveAction &other) :
	AMAction3(other)
{
	moveListAction_ = 0; //NULL
}

AMSampleMoveAction::~AMSampleMoveAction()
{
	if(moveListAction_)
		moveListAction_->deleteLater();
}

AMAction3* AMSampleMoveAction::createCopy() const{
	return new AMSampleMoveAction(*this);
}

void AMSampleMoveAction::onMoveListStarted(){
	setStarted();
}

void AMSampleMoveAction::onMoveListFailed(){
	disconnect(moveListAction_, SIGNAL(started()), this, SLOT(onMoveListStarted()));
	disconnect(moveListAction_, SIGNAL(failed()), this, SLOT(onMoveListFailed()));
	disconnect(moveListAction_, SIGNAL(succeeded()), this, SLOT(onMoveListSucceeded()));
	disconnect(moveListAction_, SIGNAL(cancelled()), this, SLOT(onMoveListCancelled()));
	disconnect(moveListAction_, SIGNAL(progressChanged(double,double)), this, SIGNAL(progressChanged(double,double)));

	AMErrorMon::alert(this, AMSAMPLEMOVEACTION_MOVELIST_FAILED, QString("There was an error moving one or more motors after start was called."));
	setFailed();
}

void AMSampleMoveAction::onMoveListSucceeded(){
	disconnect(moveListAction_, SIGNAL(started()), this, SLOT(onMoveListStarted()));
	disconnect(moveListAction_, SIGNAL(failed()), this, SLOT(onMoveListFailed()));
	disconnect(moveListAction_, SIGNAL(succeeded()), this, SLOT(onMoveListSucceeded()));
	disconnect(moveListAction_, SIGNAL(cancelled()), this, SLOT(onMoveListCancelled()));
	disconnect(moveListAction_, SIGNAL(progressChanged(double,double)), this, SIGNAL(progressChanged(double,double)));

	setProgress(100, 100);
	setSucceeded();
}
void AMSampleMoveAction::onMoveListCancelled(){
	disconnect(moveListAction_, SIGNAL(started()), this, SLOT(onMoveListStarted()));
	disconnect(moveListAction_, SIGNAL(failed()), this, SLOT(onMoveListFailed()));
	disconnect(moveListAction_, SIGNAL(succeeded()), this, SLOT(onMoveListSucceeded()));
	disconnect(moveListAction_, SIGNAL(cancelled()), this, SLOT(onMoveListCancelled()));
	disconnect(moveListAction_, SIGNAL(progressChanged(double,double)), this, SIGNAL(progressChanged(double,double)));

	setCancelled();
}

void AMSampleMoveAction::startImplementation(){
	AMSampleCamera *camera = AMSampleCamera::set();
	if(!camera){
		AMErrorMon::alert(this, AMSAMPLEMOVEACTION_CAMERA_NOT_AVAILABLE, QString("The action could not start because there is no sample camera available."));
		setFailed();
		return;
	}
	// Check that the camera system is fully calibrated with a beam
	if(!camera->allConfigured()){
		AMErrorMon::alert(this, AMSAMPLEMOVEACTION_CAMERA_NOT_CONFIGURED, QString("The action could not start because the sample camera is not configured properly."));
		setFailed();
		return;
	}

	// Check that we have a sample plate
	if(!AMBeamline::bl()->samplePlate()){
		AMErrorMon::alert(this, AMSAMPLEMOVEACTION_NO_SAMPLE_PLATE, QString("The action could not start because there is no sample plate."));
		setFailed();
		return;
	}
	// Check that this sample is on that plate
	if(AMBeamline::bl()->samplePlate()->indexOfSample(sampleMoveInfo()->sample()) == -1){

		if(sampleMoveInfo()->sample()){
			// Last chance attempt to find sample. If this was reloaded from the database, it might have loaded the sample out of the database directly rather than got it from a sample plate. This is because we don't have a good retain/release system yet.
			AMSamplePlate *currentSamplePlate = AMBeamline::bl()->samplePlate();
			for(int x = 0, size = currentSamplePlate->sampleCount(); x < size; x++)
				if(currentSamplePlate->sampleAt(x)->name() == sampleMoveInfo()->sample()->name())
					sampleMoveInfo()->setSample(currentSamplePlate->sampleAt(x));
		}

		if(AMBeamline::bl()->samplePlate()->indexOfSample(sampleMoveInfo()->sample()) == -1){
			AMErrorMon::alert(this, AMSAMPLEMOVEACTION_SAMPLE_NOT_ON_PLATE, QString("The action could not start because the sample requested is not on the current plate."));
			setFailed();
			return;
		}
	}

	moveListAction_ = camera->createMoveToSampleAction(sampleMoveInfo()->sample());

	AMListAction3 *asListAction = qobject_cast<AMListAction3*>(moveListAction_);
	if(asListAction){
		for(int x = 0, size = asListAction->subActionCount(); x < size; x++){
			AMControlMoveAction3 *subAsControlMoveAction = qobject_cast<AMControlMoveAction3*>(asListAction->subActionAt(x));
			if(subAsControlMoveAction && subAsControlMoveAction->controlMoveInfo()->controlInfo()->value() > subAsControlMoveAction->controlMoveInfo()->controlInfo()->maximum()){
				AMControlInfo newValueInfo(*(subAsControlMoveAction->controlMoveInfo()->controlInfo()));
				newValueInfo.setValue(subAsControlMoveAction->controlMoveInfo()->controlInfo()->maximum());
				subAsControlMoveAction->controlMoveInfo()->setControlInfo(newValueInfo);
			}
			else if(subAsControlMoveAction && subAsControlMoveAction->controlMoveInfo()->controlInfo()->value() < subAsControlMoveAction->controlMoveInfo()->controlInfo()->minimum()){
				AMControlInfo newValueInfo(*(subAsControlMoveAction->controlMoveInfo()->controlInfo()));
				newValueInfo.setValue(subAsControlMoveAction->controlMoveInfo()->controlInfo()->minimum());
				subAsControlMoveAction->controlMoveInfo()->setControlInfo(newValueInfo);
			}
		}
	}

	// connect to the list's signals
	connect(moveListAction_, SIGNAL(started()), this, SLOT(onMoveListStarted()));
	connect(moveListAction_, SIGNAL(failed()), this, SLOT(onMoveListFailed()));
	connect(moveListAction_, SIGNAL(succeeded()), this, SLOT(onMoveListSucceeded()));
	connect(moveListAction_, SIGNAL(cancelled()), this, SLOT(onMoveListCancelled()));
	connect(moveListAction_, SIGNAL(progressChanged(double,double)), this, SIGNAL(progressChanged(double,double)));

	moveListAction_->start();
}

void AMSampleMoveAction::cancelImplementation(){
	moveListAction_->cancel();
}
