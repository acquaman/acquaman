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

#include "beamline/AMBeamline.h"


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

	connect(AMBeamline::bl(), SIGNAL(beamlineScanningChanged(bool)), this, SLOT(reviewWorkflowStatus()));
	connect(workflowQueue_, SIGNAL(isRunningChanged(bool)), this, SLOT(reviewWorkflowStatus()));
	connect(workflowQueue_, SIGNAL(isEmptyChanged(bool)), this, SLOT(reviewWorkflowStatus()));

	//	removed with adder: connect(workflowView_, SIGNAL(queueUpdated(int)), adder_, SLOT(onQueueUpdated(int)));

	vl_ = new QVBoxLayout();
	vl_->addLayout(hl);
	vl_->addWidget(workflowView_);
	setLayout(vl_);
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

	//insertRowIndex_ = -1;
	actionsViewList_ = new AMVerticalStackWidget();
	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(actionsViewList_);
	setLayout(vl);

	/*
	gb_ = new QGroupBox();
	gb_->setTitle("Workflow");
	iib = new QVBoxLayout();
	iib->setAlignment(Qt::AlignTop);
	gb_->setLayout(iib);
	gb_->setMinimumSize(300, NATURAL_ACTION_VIEW_HEIGHT);
	gb_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);


	sa_ = new QScrollArea();
	sa_->setWidget(gb_);
	sa_->setWidgetResizable(true);
	QVBoxLayout *ob = new QVBoxLayout();
	ob->addWidget(sa_);
	setLayout(ob);

	connect(actionsList_->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(handleDataChanged(QModelIndex,QModelIndex)));
	connect(actionsList_->model(), SIGNAL(rowsInserted(const QModelIndex,int,int)), this, SLOT(handleRowsInsert(QModelIndex,int,int)));
	connect(actionsList_->model(), SIGNAL(rowsRemoved(const QModelIndex,int,int)), this, SLOT(handleRowsRemoved(QModelIndex,int,int)));
	*/
	connect(actionsList_, SIGNAL(actionChanged(int)), this, SLOT(onActionChanged(int)));
	connect(actionsList_, SIGNAL(actionAdded(int)), this, SLOT(onActionAdded(int)));
	connect(actionsList_, SIGNAL(actionRemoved(int)), this, SLOT(onActionRemoved(int)));
	connect(actionsQueue_, SIGNAL(headChanged()), this, SLOT(reindexViews()));
}

/*
AMBeamlineActionItem* AMBeamlineActionsListView::firstInQueue(){
	if(actionsQueue_.count() > 0)
		return actionsQueue_.head();
	else
		return NULL;
}

int AMBeamlineActionsListView::indexOfFirst(){
	if(actionsQueue_.count() > 0)
		return actionsList_->indexOf(firstInQueue());
	else if(actionsList_->count() == 0)
		return 0;
	else
		return actionsList_->count()-1;
}

int AMBeamlineActionsListView::visibleIndexOfFirst(){
//	if(actionsQueue_.count() > 0)
//		;
//	else
	return visibleViewList_.count()-actionsQueue_.count();
}
*/
#include "beamline/AMBeamlineControlMoveAction.h"
#include "beamline/AMBeamlineControlSetMoveAction.h"

/// \bug What happens if the action at \c index has changed type? This assumes the subclass of view is correct for the subclass of actionItem
void AMBeamlineActionsListView::onActionChanged(int index){
	AMBeamlineActionItem *tmpItem = actionsList_->action(index);
	if(tmpItem->type() == "actionItem.scanAction")
		((AMBeamlineScanActionView*)actionsViewList_->widget(index))->setAction((AMBeamlineScanAction*)tmpItem);
	else if(tmpItem->type() == "actionItem.controlMoveAction"){
		((AMBeamlineControlMoveActionView*)actionsViewList_->widget(index))->setAction((AMBeamlineControlMoveAction*)tmpItem);
	}
	else if(tmpItem->type() == "actionItem.controlSetMoveAction"){
		((AMBeamlineControlSetMoveActionView*)actionsViewList_->widget(index))->setAction((AMBeamlineControlSetMoveAction*)tmpItem);
	}
//		((AMBeamlineScanActionView*)fullViewList_.at(topLeft.row()))->setAction((AMBeamlineScanAction*)tmpItem);
}

void AMBeamlineActionsListView::onActionAdded(int index){
	AMBeamlineActionItem *tmpItem = actionsList_->action(index);
	AMBeamlineActionView *tmpView;
	if(tmpItem->type() == "actionItem.scanAction"){
		AMBeamlineScanAction *scanAction = (AMBeamlineScanAction*)tmpItem;
		AMBeamlineScanActionView *scanActionView = new AMBeamlineScanActionView(scanAction, index);
//		actionsViewList_->addItem(scanActionView, "Scan", true);
		actionsViewList_->insertItem(index, scanActionView, "Scan", true);
		tmpView = scanActionView;
	}
	else if(tmpItem->type() == "actionItem.controlMoveAction"){
		AMBeamlineControlMoveAction *moveAction = (AMBeamlineControlMoveAction*)tmpItem;
		AMBeamlineControlMoveActionView *moveActionView = new AMBeamlineControlMoveActionView(moveAction, index);
//		actionsViewList_->addItem(moveActionView, "Move To", true);
		actionsViewList_->insertItem(index, moveActionView, "Move To", true);
		tmpView = moveActionView;
	}
	else if(tmpItem->type() == "actionItem.controlSetMoveAction"){
		AMBeamlineControlSetMoveAction *moveAction = (AMBeamlineControlSetMoveAction*)tmpItem;
		AMBeamlineControlSetMoveActionView *moveActionView = new AMBeamlineControlSetMoveActionView(moveAction, index);
		actionsViewList_->insertItem(index, moveActionView, "Go To Sample", true);
		tmpView = moveActionView;
	}
	else
		return;
	connect(tmpView, SIGNAL(removeRequested(AMBeamlineActionItem*)), this, SLOT(onActionRemoveRequested(AMBeamlineActionItem*)));
	reindexViews();
	emit queueUpdated(actionsQueue_->count());
}

void AMBeamlineActionsListView::onActionRemoved(int index){
	actionsViewList_->removeItem(index);
	reindexViews();
	emit queueUpdated(actionsQueue_->count());
}

void AMBeamlineActionsListView::onActionRemoveRequested(AMBeamlineActionItem *item){
	int index = actionsList_->indexOf(item);
	actionsList_->deleteAction(index);
}

/*
void AMBeamlineActionsListView::handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight){
	AMBeamlineActionItem *tmpItem = actionsList_->action(topLeft.row());
	qDebug() << "In handleDataChanged on " << topLeft.row() << bottomRight.row();
	if(topLeft.row() != bottomRight.row())
		return; //WHAT SHOULD I DO FOR MULTI-ROW CHANGE
	else if( (insertRowIndex_ != -1) && (topLeft.row() == insertRowIndex_) ){
		if(tmpItem->type() == "actionItem.scanAction"){
			AMBeamlineScanAction *scanAction = (AMBeamlineScanAction*)tmpItem;
			int iof = indexOfFirst();
			if(topLeft.row() < iof)
				iof--;
			int viof = visibleIndexOfFirst();
			qDebug() << "iof is " << iof << " viof is " << viof;
			qDebug() << "actionsQueue to " << topLeft.row()-iof;
			qDebug() << "fullViewList to " << topLeft.row();
			qDebug() << "visibleViewList to " << topLeft.row()-(iof-viof);
			qDebug() << "viewQueue to " << topLeft.row()-iof;
			qDebug() << "iib to " << topLeft.row()-(iof-viof);
//			actionsQueue_.enqueue(tmpItem);
			actionsQueue_.insert(topLeft.row()-iof, tmpItem);
//			AMBeamlineScanActionView *scanActionView = new AMBeamlineScanActionView(scanAction, viewQueue_.count()+1);
			AMBeamlineScanActionView *scanActionView = new AMBeamlineScanActionView(scanAction, 0);
//			fullViewList_.append(scanActionView);
			fullViewList_.insert(topLeft.row(), scanActionView);
//			visibleViewList_.append(scanActionView);
			visibleViewList_.insert(topLeft.row()-(iof-viof), scanActionView);
//			viewQueue_.enqueue(scanActionView);
			viewQueue_.insert(topLeft.row()-iof, scanActionView);
//			iib->addWidget(scanActionView);
			iib->insertWidget(topLeft.row()-(iof-viof), scanActionView);
			iib->activate();
			reindexViews();
			connect(scanActionView, SIGNAL(focusRequested(AMBeamlineActionItem*)), this, SLOT(onFocusRequested(AMBeamlineActionItem*)));
			connect(scanActionView, SIGNAL(removeRequested(AMBeamlineActionItem*)), this, SLOT(onRemoveRequested(AMBeamlineActionItem*)));
			connect(scanActionView, SIGNAL(scanSuceeded(AMBeamlineActionItem*)), this, SLOT(onActionSucceeded(AMBeamlineActionItem*)));
			connect(scanActionView, SIGNAL(hideRequested(AMBeamlineActionItem*)), this, SLOT(onHideRequested(AMBeamlineActionItem*)));
			connect(scanActionView, SIGNAL(scanStarted(AMBeamlineActionItem*)), this, SLOT(onActionStarted(AMBeamlineActionItem*)));
			qDebug() << "Add ensure visible";
			sa_->ensureWidgetVisible( fullViewList_.at(actionsList_->indexOf(firstInQueue())) );
		}
		else if(tmpItem->type() == "actionItem.controlMoveAction"){
			AMBeamlineControlMoveAction *moveAction = (AMBeamlineControlMoveAction*)tmpItem;
			int iof = indexOfFirst();
			if(topLeft.row() < iof)
				iof--;
			int viof = visibleIndexOfFirst();
			qDebug() << "iof is " << iof << " viof is " << viof;
			qDebug() << "actionsQueue to " << topLeft.row()-iof;
			qDebug() << "fullViewList to " << topLeft.row();
			qDebug() << "visibleViewList to " << topLeft.row()-(iof-viof);
			qDebug() << "viewQueue to " << topLeft.row()-iof;
			qDebug() << "iib to " << topLeft.row()-(iof-viof);
//			actionsQueue_.enqueue(tmpItem);
			actionsQueue_.insert(topLeft.row()-iof, tmpItem);
//			AMBeamlineControlMoveActionView *moveActionView = new AMBeamlineControlMoveActionView(moveAction, viewQueue_.count()+1);
			AMBeamlineControlMoveActionView *moveActionView = new AMBeamlineControlMoveActionView(moveAction, 0);
//			fullViewList_.append(moveActionView);
			fullViewList_.insert(topLeft.row(), moveActionView);
//			visibleViewList_.append(moveActionView);
			visibleViewList_.insert(topLeft.row()-(iof-viof), moveActionView);
//			viewQueue_.enqueue(moveActionView);
			viewQueue_.insert(topLeft.row()-iof, moveActionView);
//			iib->addWidget(moveActionView);
			iib->insertWidget(topLeft.row()-(iof-viof), moveActionView);
			reindexViews();
			iib->activate();
			connect(moveActionView, SIGNAL(focusRequested(AMBeamlineActionItem*)), this, SLOT(onFocusRequested(AMBeamlineActionItem*)));
			connect(moveActionView, SIGNAL(removeRequested(AMBeamlineActionItem*)), this, SLOT(onRemoveRequested(AMBeamlineActionItem*)));
			connect(moveActionView, SIGNAL(actionSucceeded(AMBeamlineActionItem*)), this, SLOT(onActionSucceeded(AMBeamlineActionItem*)));
			connect(moveActionView, SIGNAL(hideRequested(AMBeamlineActionItem*)), this, SLOT(onHideRequested(AMBeamlineActionItem*)));
			connect(moveActionView, SIGNAL(actionStarted(AMBeamlineActionItem*)), this, SLOT(onActionStarted(AMBeamlineActionItem*)));
			qDebug() << "Add ensure visible";
			sa_->ensureWidgetVisible( fullViewList_.at(actionsList_->indexOf(firstInQueue())) );
		}
		if(insertRowIndex_ <= focusAction_)
			focusAction_++;
		insertRowIndex_ = -1;
	}
	else if( insertRowIndex_ == -1){
		qDebug() << "Going to reset action on the view";

		if(tmpItem->type() == "actionItem.scanAction")
			((AMBeamlineScanActionView*)fullViewList_.at(topLeft.row()))->setAction((AMBeamlineScanAction*)tmpItem);

	}
	emit queueUpdated(actionsQueue_);
}

void AMBeamlineActionsListView::handleRowsInsert(const QModelIndex &parent, int start, int end){
	Q_UNUSED(parent);
	qDebug() << "In handleRowInsert on " << start << end;
	if(start != end)
		return; //WHAT TO DO ON MULTI-ROW INSERT?
	insertRowIndex_ = start;
	gb_->setMinimumHeight( NATURAL_ACTION_VIEW_HEIGHT * (visibleViewList_.count()+1) );
}

void AMBeamlineActionsListView::handleRowsRemoved(const QModelIndex &parent, int start, int end){
	Q_UNUSED(parent);
	if(start != end)
		return; //WHAT TO DO FOR MULTI-ROW DELETE?
	AMBeamlineActionView *tmpView = fullViewList_.takeAt(start);
	visibleViewList_.removeAll(tmpView);
	viewQueue_.removeAll(tmpView);
	actionsQueue_.removeAll(tmpView->action());
	iib->removeWidget(tmpView);
	delete tmpView;
	if(focusAction_ == start)
		focusAction_ = -1;
	else if(focusAction_ > start)
		focusAction_--;
	reindexViews();
	gb_->setMinimumHeight( NATURAL_ACTION_VIEW_HEIGHT * (visibleViewList_.count()+1) );
	emit queueUpdated(actionsQueue_);
}

void AMBeamlineActionsListView::redrawBeamlineActionsList(){

}

void AMBeamlineActionsListView::onFocusRequested(AMBeamlineActionItem *action){
	if(focusAction_ == actionsList_->indexOf(action))
		return;
	else if(focusAction_ != -1)
		fullViewList_.at(focusAction_)->defocusItem();
	focusAction_ = actionsList_->indexOf(action);
}

void AMBeamlineActionsListView::onRemoveRequested(AMBeamlineActionItem *action){
	qDebug() << "Requesting remove so deleting action from list";
	int index = actionsList_->indexOf(action);
	disconnect( fullViewList_.at(index), SIGNAL(focusRequested(AMBeamlineActionItem*)), this, SLOT(onFocusRequested(AMBeamlineActionItem*)));
	disconnect( fullViewList_.at(index), SIGNAL(removeRequested(AMBeamlineActionItem*)), this, SLOT(onRemoveRequested(AMBeamlineActionItem*)));
	disconnect( fullViewList_.at(index), SIGNAL(scanSuceeded(AMBeamlineActionItem*)), this, SLOT(onActionSucceeded(AMBeamlineActionItem*)));
	disconnect( fullViewList_.at(index), SIGNAL(hideRequested(AMBeamlineActionItem*)), this, SLOT(onHideRequested(AMBeamlineActionItem*)));
	disconnect( fullViewList_.at(index), SIGNAL(scanStarted(AMBeamlineActionItem*)), this, SLOT(onActionStarted(AMBeamlineActionItem*)));
	actionsList_->deleteAction( index );
}

void AMBeamlineActionsListView::onHideRequested(AMBeamlineActionItem *action){
	int index = actionsList_->indexOf(action);

	int visibleIndex = visibleViewList_.indexOf( fullViewList_.at(index) );
	bool doInsertion = false;
	if( ( (visibleIndex != 0) && (visibleViewList_.at(visibleIndex-1)->viewType() == "actionView.hiddenView") ) &&
		( (visibleIndex != visibleViewList_.count()-1) && (visibleViewList_.at(visibleIndex+1)->viewType() == "actionView.hiddenView") ) ){
		int pCount = ((AMBeamlineActionHiddenView*)(visibleViewList_.at(visibleIndex-1)))->count();
		int nCount = ((AMBeamlineActionHiddenView*)(visibleViewList_.at(visibleIndex+1)))->count();
		((AMBeamlineActionHiddenView*)(visibleViewList_.at(visibleIndex-1)))->setCount(  pCount+nCount+1 );
		AMBeamlineActionHiddenView *tmpHidden = (AMBeamlineActionHiddenView*)visibleViewList_.at(visibleIndex+1);
		disconnect(tmpHidden, SIGNAL(expandRequested(AMBeamlineActionItem*)), this, SLOT(onExpandRequested(AMBeamlineActionItem*)));
		visibleViewList_.removeAll(tmpHidden);
		iib->removeWidget(tmpHidden);
		delete tmpHidden;
	}
	else if( (visibleIndex != 0) && (visibleViewList_.at(visibleIndex-1)->viewType() == "actionView.hiddenView") ){
		((AMBeamlineActionHiddenView*)(visibleViewList_.at(visibleIndex-1)))->setCount( ((AMBeamlineActionHiddenView*)(visibleViewList_.at(visibleIndex-1)))->count() +1 );
	}
	else if( (visibleIndex != visibleViewList_.count()-1) && (visibleViewList_.at(visibleIndex+1)->viewType() == "actionView.hiddenView") ){
		((AMBeamlineActionHiddenView*)(visibleViewList_.at(visibleIndex+1)))->setAction(action);
		((AMBeamlineActionHiddenView*)(visibleViewList_.at(visibleIndex+1)))->setCount( ((AMBeamlineActionHiddenView*)(visibleViewList_.at(visibleIndex+1)))->count() +1 );
	}
	else{
		AMBeamlineActionHiddenView *tmpHidden = new AMBeamlineActionHiddenView(action, 1);
		connect(tmpHidden, SIGNAL(expandRequested(AMBeamlineActionItem*)), this, SLOT(onExpandRequested(AMBeamlineActionItem*)));
//		visibleViewList_.replace(index, tmpHidden);
		visibleViewList_.replace(visibleIndex, tmpHidden);
		doInsertion = true;
	}
	iib->removeWidget(fullViewList_.at(index));
	fullViewList_.at(index)->hide();
	if(doInsertion)
		iib->insertWidget(visibleIndex, visibleViewList_.at(visibleIndex));
	else
		visibleViewList_.removeAt(visibleIndex);
	gb_->setMinimumHeight( NATURAL_ACTION_VIEW_HEIGHT * (visibleViewList_.count()+1) );
}

void AMBeamlineActionsListView::onExpandRequested(AMBeamlineActionItem *action){
	int index = actionsList_->indexOf(action);
	int visibleIndex;// = visibleViewList_.indexOf( fullViewList_.at(index) );
	for(int x = 0; x < visibleViewList_.count(); x++){
		if(action == visibleViewList_.at(x)->action())
			visibleIndex = x;
	}
	AMBeamlineActionHiddenView *tmpHidden = (AMBeamlineActionHiddenView*)visibleViewList_.at(visibleIndex);
	int expandCount = tmpHidden->count();
	disconnect(tmpHidden, SIGNAL(expandRequested(AMBeamlineActionItem*)), this, SLOT(onExpandRequested(AMBeamlineActionItem*)));
//	visibleViewList_.replace(index, fullViewList_.at(index));
	visibleViewList_.removeAll(tmpHidden);
	iib->removeWidget(tmpHidden);
	delete tmpHidden;
	for(int x = index; x < index + expandCount; x++){
		iib->insertWidget(visibleIndex+(x-index), fullViewList_.at(x));
		visibleViewList_.insert(visibleIndex+(x-index), fullViewList_.at(x));
		fullViewList_.at(x)->show();
	}
	gb_->setMinimumHeight( NATURAL_ACTION_VIEW_HEIGHT * (visibleViewList_.count()+1) );
}

void AMBeamlineActionsListView::onActionStarted(AMBeamlineActionItem *action){
	int index = actionsList_->indexOf(action);
	int visibleIndex = visibleViewList_.indexOf( fullViewList_.at(index) );
	AMBeamlineActionView *tmpView = visibleViewList_.at(visibleIndex);
	qDebug() << "Trying to ensure visible";
	sa_->ensureWidgetVisible(tmpView);
}

void AMBeamlineActionsListView::onActionSucceeded(AMBeamlineActionItem *action){
	if(actionsQueue_.head() != action)
		return; //WHAT THE HELL, who succeeded that isn't at the head of the queue?
	actionsQueue_.dequeue();
	viewQueue_.dequeue();
	fullViewList_.at( actionsList_->indexOf(action) )->setIndex(-1);
	reindexViews();
	qDebug() << "iof is " << indexOfFirst();
	emit queueUpdated(actionsQueue_);
}
*/

void AMBeamlineActionsListView::reindexViews(){
	if(actionsQueue_->indexOfHead() != -1){
		for(int x = 0; x < actionsQueue_->indexOfHead(); x++)
			if(actionsViewList_->widget(x) && ((AMBeamlineActionView*)(actionsViewList_->widget(x)))->index() != -1 )
				((AMBeamlineActionView*)(actionsViewList_->widget(x)))->setIndex(-1);
		for(int x = actionsQueue_->indexOfHead(); x < actionsList_->count(); x++)
			if(actionsViewList_->widget(x) && ((AMBeamlineActionView*)(actionsViewList_->widget(x)))->index() != x-actionsQueue_->indexOfHead()+1 )
				((AMBeamlineActionView*)(actionsViewList_->widget(x)))->setIndex(x-actionsQueue_->indexOfHead()+1);
	}
	else
		for(int x = 0; x < actionsList_->count(); x++)
			if(actionsViewList_->widget(x) && ((AMBeamlineActionView*)(actionsViewList_->widget(x)))->index() != -1 )
				((AMBeamlineActionView*)(actionsViewList_->widget(x)))->setIndex(-1);
}


