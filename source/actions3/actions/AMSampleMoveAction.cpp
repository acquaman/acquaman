#include "AMSampleMoveAction.h"

#include "beamline/camera/AMSampleCamera.h"

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

//	AMErrorMon::alert(this, AMSAMPLEPLATEPRE2013MOVEACTION_MOVELIST_FAILED, QString("There was an error moving one or more motors after start was called."));
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
	// Check that the camera system is fully calibrated with a beam
	// Check that we have a sample plate
	// Check that this sample is on that plate

	AMSampleCamera *camera = AMSampleCamera::set();
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
