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


#include "AMBeamlineListAction.h"

//#include <QVBoxLayout>
//#include <QLabel>

//AMBeamlineListAction::AMBeamlineListAction(AMBeamlineParallelActionsList *list, QObject *parent) :
//		AMBeamlineActionItem(parent)
//{
//	list_ = 0; //NULL
//	if(list)
//		setList(list);
//}

//AMBeamlineActionItemView* AMBeamlineListAction::createView(int index){
//	Q_UNUSED(index)
//	return new AMBeamlineListDetailedActionView(this);
//}

//AMBeamlineParallelActionsList* AMBeamlineListAction::list(){
//	return list_;
//}

//void AMBeamlineListAction::cleanup(){
//	return;
//}

//void AMBeamlineListAction::start(){
//	if(isReady()){
//		connect(list_, SIGNAL(listSucceeded()), this, SLOT(onListSucceeded()));
//		connect(list_, SIGNAL(listFailed(int)), this, SLOT(onListFailed(int)));
//		list_->start();
//	}
//	else
//		connect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
//}

//void AMBeamlineListAction::cancel(){

//}

//void AMBeamlineListAction::setList(AMBeamlineParallelActionsList *list){
//	if(list_){
//		disconnect(list_, SIGNAL(stageProgress(int,double,double)), this, SLOT(calculateProgress(int,double,double)));
//	}
//	list_ = list;
//	if(list_){
//		connect(list_, SIGNAL(stageProgress(int,double,double)), this, SLOT(calculateProgress(int,double,double)));
//	}
//	checkReady();
//}

//void AMBeamlineListAction::delayedStart(bool ready){
//	if(ready){
//		disconnect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
//		start();
//	}
//}

//void AMBeamlineListAction::onListSucceeded(){
//	setSucceeded(true);
//}

//void AMBeamlineListAction::onListFailed(int explanation){
//	setFailed(true, explanation);
//}

//void AMBeamlineListAction::checkReady(){
//	if(!list_)
//		setReady(false);
//	else
//		setReady(true);
//}

//void AMBeamlineListAction::initialize(){
//	return;
//}

//void AMBeamlineListAction::calculateProgress(int stageIndex, double elapsed, double total){
//	// For now, pretend that each stage represents an equal part
//	/* NTBA March 23, 2011 David Chevrier
//	   Progress is having trouble with controls that don't need to move (already in position)
//	*/
//	double fractionCompleted = ((double)stageIndex)/((double)list_->stageCount());
//	double progressFraction = (elapsed/total)/((double)list_->stageCount());
//	emit progress(fractionCompleted+progressFraction, 1.0);
//}


//AMBeamlineListDetailedActionView::AMBeamlineListDetailedActionView(AMBeamlineListAction *listAction, int index, QWidget *parent) :
//		AMBeamlineActionItemView(listAction, index, parent)
//{
//	listAction_ = 0; //NULL

//	setMinimumHeight(NATURAL_ACTION_VIEW_HEIGHT);
//	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

//	messageLabel_ = new QLabel();
//	messageLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

//	mainVL_ = new QVBoxLayout();
//	mainVL_->addWidget(messageLabel_);
//	setLayout(mainVL_);

//	setAction(listAction);

//	onInfoChanged();
//}

//void AMBeamlineListDetailedActionView::setIndex(int index){
//	index_ = index;
//	onInfoChanged();
//}

//void AMBeamlineListDetailedActionView::setAction(AMBeamlineActionItem *action){
//	AMBeamlineListAction *listAction = qobject_cast<AMBeamlineListAction*>(action);
//	if(listAction_){
//		disconnect(listAction_, SIGNAL(started()), this, SLOT(onActionStarted()));
//		disconnect(listAction, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
//		disconnect(listAction, SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
//		while(actionViews_.count() > 0){
//			mainVL_->removeWidget(actionViews_.last());
//			delete actionViews_.takeLast();
//		}
//	}
//	listAction_ = listAction;
//	if(listAction_){
//		connect(listAction_, SIGNAL(started()), this, SLOT(onActionStarted()));
//		connect(listAction, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
//		connect(listAction, SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
//		for(int x = 0; x < listAction_->list()->stageCount(); x++){
//			actionViews_.append(listAction_->list()->action(x, 0)->createView());
//			mainVL_->addWidget(actionViews_.last());
//		}
//	}
//	onInfoChanged();
//}

//void AMBeamlineListDetailedActionView::onInfoChanged(){
//	if(listAction_ && messageLabel_){
//		messageLabel_->setText(listAction_->message());
//	}
//}

//void AMBeamlineListDetailedActionView::onPlayPauseButtonClicked(){
//	//UNUSED
//}

//void AMBeamlineListDetailedActionView::onStopCancelButtonClicked(){
//	//UNUSED
//}

//void AMBeamlineListDetailedActionView::onActionStarted(){
//	emit actionStarted(listAction_);
//}

//void AMBeamlineListDetailedActionView::onActionSucceeded(){
//	emit actionSucceeded(listAction_);
//}

//void AMBeamlineListDetailedActionView::onActionFailed(int explanation){
//	Q_UNUSED(explanation)
//}
