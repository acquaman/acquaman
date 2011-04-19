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


#include "AMWorkflowManagerView.h"
#include <QScrollArea>
#include <QPushButton>
#include <QVBoxLayout>

#include "acquaman/AMScanConfiguration.h"
#include "beamline/AMBeamlineScanAction.h"
#include "beamline/AMBeamlineControlSetMoveAction.h"

#include "beamline/AMBeamline.h"
#include "ui/AMVerticalStackWidget.h"
#include "ui/AMTopFrame.h"


AMWorkflowManagerView::AMWorkflowManagerView(QWidget *parent) :
	QWidget(parent)
{

	// removed for now:
//	adder_ = new AMBeamlineActionAdder();
//	adder_->hide();
//	connect(adder_, SIGNAL(insertActionRequested(AMBeamlineActionItem*,int)), this, SLOT(onInsertActionRequested(AMBeamlineActionItem*,int)));
//	connect(SGMBeamline::sgm()->currentSamplePlate(), SIGNAL(samplePlateChanged(bool)), adder_, SLOT(onSamplePlateChanged(bool)));
//	connect(SGMBeamline::sgm()->currentSamplePlate(), SIGNAL(samplePositionAdded(int)), adder_, SLOT(onSamplePlateUpdate(int)));
//	connect(SGMBeamline::sgm()->currentSamplePlate(), SIGNAL(samplePositionChanged(int)), adder_, SLOT(onSamplePlateUpdate(int)));
//	connect(SGMBeamline::sgm()->currentSamplePlate(), SIGNAL(samplePositionRemoved(int)), adder_, SLOT(onSamplePlateUpdate(int)));

	topFrame_ = new AMTopFrame("Workflow");
	topFrame_->setIcon(QIcon(":/user-away.png"));

	startWorkflowButton_ = new QPushButton("Start This Workflow\nReady");
	startWorkflowButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	addActionButton_ = new QPushButton("Add an Action");
	addActionButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	QHBoxLayout *hl = new QHBoxLayout();
	hl->addItem(new QSpacerItem(10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
	hl->addWidget(addActionButton_, 0, Qt::AlignRight);
	hl->addWidget(startWorkflowButton_, 0, Qt::AlignRight);
	connect(startWorkflowButton_, SIGNAL(clicked()), this, SLOT(startQueue()));
	connect(addActionButton_, SIGNAL(clicked()), this, SLOT(onAddActionRequested()));
	workflowActions_ = new AMBeamlineActionsList(this);
	workflowQueue_ = new AMBeamlineActionsQueue(workflowActions_, this);
	workflowView_ = new AMBeamlineActionsListView(workflowActions_, workflowQueue_, this);
	connect(workflowView_, SIGNAL(copyRequested(AMBeamlineActionItem*)), this, SLOT(onCopyActionRequested(AMBeamlineActionItem*)));

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setWidget(workflowView_);
	scrollArea->setWidgetResizable(true);

	connect(AMBeamline::bl(), SIGNAL(beamlineScanningChanged(bool)), this, SLOT(reviewWorkflowStatus()));
	connect(workflowQueue_, SIGNAL(isRunningChanged(bool)), this, SLOT(reviewWorkflowStatus()));
	connect(workflowQueue_, SIGNAL(isEmptyChanged(bool)), this, SLOT(reviewWorkflowStatus()));

	//	removed with adder: connect(workflowView_, SIGNAL(queueUpdated(int)), adder_, SLOT(onQueueUpdated(int)));

	vl_ = new QVBoxLayout();
	//vl_->addLayout(hl);
	vl_->addWidget(scrollArea);

	topFrame_->frameLayout()->addLayout(hl);

	QVBoxLayout *vl2 = new QVBoxLayout();
	vl2->addWidget(topFrame_);
	vl2->addLayout(vl_);
	vl2->setContentsMargins(0,0,0,0);
	vl2->setSpacing(1);

	vl_->setContentsMargins(10, 0, 10, 0);
	vl_->setSpacing(5);
	hl->setContentsMargins(0, 2, 10, 2);
	hl->setSpacing(5);

	setLayout(vl2);
}

void AMWorkflowManagerView::startQueue(){
	if(!workflowQueue_->isEmpty()){
//		qDebug() << "Trying to start queue";
		if(!workflowQueue_->head()->hasFinished() ){
//			qDebug() << "Initialized, so just start, no reset";
			workflowQueue_->startQueue();
		}
		else{
			qDebug() << "Not initialized anymore, so need to reset";
			workflowQueue_->head()->reset();
			workflowQueue_->startQueue();
		}
	}
}

#include <QMessageBox>
void AMWorkflowManagerView::onAddActionRequested(){
	// disabled for now...
//	adder_->move(addActionButton_->pos());
//	adder_->show();

	QMessageBox::information(this, "Sorry, This Action Not Available Yet", "Sorry, we haven't implemented this yet.\n\nWe're working on it...", QMessageBox::Ok);
}

void AMWorkflowManagerView::onCopyActionRequested(AMBeamlineActionItem *action){
	qDebug() << "Copy requested!";
	insertBeamlineAction(-1, action);
}


void AMWorkflowManagerView::insertBeamlineAction(int index, AMBeamlineActionItem *action, bool startNow) {
	if(index < 0 || index > workflowQueue_->count()) {
		index = workflowQueue_->count();
	}

	if(workflowQueue_->isEmpty())
		workflowActions_->appendAction(action);
	else
		workflowActions_->addAction(workflowQueue_->indexOfHead()+index, action);

	if(startNow)
		startQueue();

}


void AMWorkflowManagerView::reviewWorkflowStatus(){
	bool qEmpty = workflowQueue_->isEmpty();
	bool qRunning = workflowQueue_->isRunning();
	bool blScanning = AMBeamline::bl()->isBeamlineScanning();

	/// \todo Move this code out of the UI and into the workflow proper.
	if(qEmpty || qRunning || blScanning)
		startWorkflowButton_->setEnabled(false);
	else{
		startWorkflowButton_->setEnabled(true);
		startWorkflowButton_->setText("Start This Workflow\n-- Ready --");
	}

	if(blScanning && !qRunning){
			startWorkflowButton_->setText("Start This Workflow\n-- Beamline Busy --");
	}
	else if(qEmpty){
		startWorkflowButton_->setText("Start This Workflow\n-- No Items --");
	}
	else if(qRunning){
		startWorkflowButton_->setText("Start This Workflow\n-- Already Running --");
	}
	/////////////////////////

	emit workflowStatusChanged(blScanning, qEmpty, qRunning);
	/// \todo Emit the following only when they change:
	emit actionItemCountChanged(workflowQueue_->count());
	emit runningChanged(qRunning);
	// emit beamlineBusyChanged(blScanning);
}

bool AMWorkflowManagerView::beamlineBusy() const {
	return AMBeamline::bl()->isBeamlineScanning();
}
AMBeamlineActionsListView::AMBeamlineActionsListView(AMBeamlineActionsList *actionsList, AMBeamlineActionsQueue *actionsQueue, QWidget *parent) :
		QWidget(parent)
{
	actionsList_ = actionsList;
	actionsQueue_ = actionsQueue;
	focusAction_ = -1;

	actionsViewList_ = new AMVerticalStackWidget();
	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(actionsViewList_);
	setLayout(vl);

	connect(actionsList_, SIGNAL(actionChanged(int)), this, SLOT(onActionChanged(int)));
	connect(actionsList_, SIGNAL(actionAdded(int)), this, SLOT(onActionAdded(int)));
	connect(actionsList_, SIGNAL(actionRemoved(int)), this, SLOT(onActionRemoved(int)));
	connect(actionsQueue_, SIGNAL(headChanged()), this, SLOT(reindexViews()));
}

#include "beamline/AMBeamlineControlMoveAction.h"
#include "beamline/AMBeamlineControlSetMoveAction.h"

/// \bug What happens if the action at \c index has changed type? This assumes the subclass of view is correct for the subclass of actionItem
void AMBeamlineActionsListView::onActionChanged(int index){
	AMBeamlineActionItem *tmpItem = actionsList_->action(index);
	((AMBeamlineActionItemView*)actionsViewList_->widget(index))->setAction(tmpItem);
}

void AMBeamlineActionsListView::onActionAdded(int index){
	AMBeamlineActionItem *tmpItem = actionsList_->action(index);
	AMBeamlineActionItemView *tmpView = tmpItem->createView(index);
	if(!tmpView)
		return;
	actionsViewList_->insertItem(index, tmpView, tmpItem->description(), true);
	connect(tmpView, SIGNAL(removeRequested(AMBeamlineActionItem*)), this, SLOT(onActionRemoveRequested(AMBeamlineActionItem*)));
	connect(tmpView, SIGNAL(copyRequested(AMBeamlineActionItem*)), this, SIGNAL(copyRequested(AMBeamlineActionItem*)));
	reindexViews();
	emit queueUpdated(actionsQueue_->count());
}

void AMBeamlineActionsListView::onActionRemoved(int index){
	actionsViewList_->deleteItem(index);
	reindexViews();
	emit queueUpdated(actionsQueue_->count());
}

void AMBeamlineActionsListView::onActionRemoveRequested(AMBeamlineActionItem *item){
	int index = actionsList_->indexOf(item);
	actionsList_->deleteAction(index);
}

void AMBeamlineActionsListView::reindexViews(){
	if(actionsQueue_->indexOfHead() != -1){
		for(int x = 0; x < actionsQueue_->indexOfHead(); x++)
			if(actionsViewList_->widget(x) && ((AMBeamlineActionItemView*)(actionsViewList_->widget(x)))->index() != -1 )
				((AMBeamlineActionItemView*)(actionsViewList_->widget(x)))->setIndex(-1);
		for(int x = actionsQueue_->indexOfHead(); x < actionsList_->count(); x++)
			if(actionsViewList_->widget(x) && ((AMBeamlineActionItemView*)(actionsViewList_->widget(x)))->index() != x-actionsQueue_->indexOfHead()+1 )
				((AMBeamlineActionItemView*)(actionsViewList_->widget(x)))->setIndex(x-actionsQueue_->indexOfHead()+1);
	}
	else
		for(int x = 0; x < actionsList_->count(); x++)
			if(actionsViewList_->widget(x) && ((AMBeamlineActionItemView*)(actionsViewList_->widget(x)))->index() != -1 )
				((AMBeamlineActionItemView*)(actionsViewList_->widget(x)))->setIndex(-1);
}


