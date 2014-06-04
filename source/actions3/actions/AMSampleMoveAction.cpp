#include "AMSampleMoveAction.h"

#include "beamline/camera/AMSampleCamera.h"
#include "beamline/AMBeamline.h"
#include "dataman/AMSamplePlate.h"
#include "util/AMErrorMonitor.h"

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
