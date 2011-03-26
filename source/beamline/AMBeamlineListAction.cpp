#include "AMBeamlineListAction.h"

AMBeamlineListAction::AMBeamlineListAction(AMBeamlineParallelActionsList *list, QObject *parent) :
		AMBeamlineActionItem(parent)
{
	list_ = 0; //NULL
	if(list)
		setList(list);
}

AMBeamlineActionView* AMBeamlineListAction::createView(int index){
	return 0;
}

AMBeamlineParallelActionsList* AMBeamlineListAction::list(){
	return list_;
}

void AMBeamlineListAction::cleanup(){
	return;
}

void AMBeamlineListAction::start(){
	if(isReady()){
		connect(list_, SIGNAL(listSucceeded()), this, SIGNAL(succeeded()));
		connect(list_, SIGNAL(listFailed(int)), this, SIGNAL(failed(int)));
		list_->start();
	}
	else
		connect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
}

void AMBeamlineListAction::cancel(){

}

void AMBeamlineListAction::setList(AMBeamlineParallelActionsList *list){
	if(list_){
		disconnect(list_, SIGNAL(stageProgress(int,double,double)), this, SLOT(calculateProgress(int,double,double)));
	}
	list_ = list;
	if(list_){
		connect(list_, SIGNAL(stageProgress(int,double,double)), this, SLOT(calculateProgress(int,double,double)));
	}
	checkReady();
}

void AMBeamlineListAction::delayedStart(bool ready){
	if(ready){
		disconnect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
		start();
	}
}

void AMBeamlineListAction::checkReady(){
	if(!list_)
		setReady(false);
	else
		setReady(true);
}

void AMBeamlineListAction::initialize(){
	return;
}

void AMBeamlineListAction::calculateProgress(int stageIndex, double elapsed, double total){
	// For now, pretend that each stage represents an equal part
	/* NTBA March 23, 2011 David Chevrier
	   Progress is having trouble with controls that don't need to move (already in position)
	*/
	double fractionCompleted = ((double)stageIndex)/((double)list_->stageCount());
	double progressFraction = (elapsed/total)/((double)list_->stageCount());
	emit progress(fractionCompleted+progressFraction, 1.0);
}
