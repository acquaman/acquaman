#include "AMWorkflowManagerView.h"
#include <QScrollArea>
#include <QPushButton>

AMWorkflowManagerView::AMWorkflowManagerView(QWidget *parent) :
	QWidget(parent)
{
	cancelAddRequest_ = false;
	adder_ = new AMBeamlineActionAdder();
	adder_->hide();
	connect(adder_, SIGNAL(insertActionRequested(AMBeamlineActionItem*,int)), this, SLOT(onInsertActionRequested(AMBeamlineActionItem*,int)));
	connect(SGMBeamline::sgm()->currentSamplePlate(), SIGNAL(samplePlateChanged(bool)), adder_, SLOT(onSamplePlateChanged(bool)));
	connect(SGMBeamline::sgm()->currentSamplePlate(), SIGNAL(samplePositionAdded(int)), adder_, SLOT(onSamplePlateUpdate(int)));
	connect(SGMBeamline::sgm()->currentSamplePlate(), SIGNAL(samplePositionChanged(int)), adder_, SLOT(onSamplePlateUpdate(int)));
	connect(SGMBeamline::sgm()->currentSamplePlate(), SIGNAL(samplePositionRemoved(int)), adder_, SLOT(onSamplePlateUpdate(int)));
	startWorkflowButton_ = new QPushButton("Start This Workflow\nReady");
	startWorkflowButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	addActionButton_ = new QPushButton("Add an Action");
	addActionButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	QHBoxLayout *hl = new QHBoxLayout();
	hl->addItem(new QSpacerItem(10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
	hl->addWidget(addActionButton_, 0, Qt::AlignRight);
	hl->addWidget(startWorkflowButton_, 0, Qt::AlignRight);
	connect(startWorkflowButton_, SIGNAL(clicked()), this, SLOT(onStartQueueRequested()));
	connect(SGMBeamline::sgm(), SIGNAL(beamlineScanningChanged(bool)), this, SLOT(onBeamlineScanningChanged(bool)));
	connect(addActionButton_, SIGNAL(clicked()), this, SLOT(onAddActionRequested()));
	workflowActions_ = new AMBeamlineActionsList(this);
	workflowQueue_ = new AMBeamlineActionsQueue(workflowActions_, this);
	workflowView_ = new AMBeamlineActionsListView(workflowActions_, workflowQueue_, this);
	connect(workflowQueue_, SIGNAL(isRunningChanged(bool)), this, SLOT(onQueueIsRunningChanged(bool)));
	connect(workflowQueue_, SIGNAL(isEmptyChanged(bool)), this, SLOT(onQueueIsEmptyChanged(bool)));
	connect(workflowView_, SIGNAL(queueUpdated(int)), adder_, SLOT(onQueueUpdated(int)));

	vl_ = new QVBoxLayout();
	vl_->addLayout(hl);
	vl_->addWidget(workflowView_);
	setLayout(vl_);
}

void AMWorkflowManagerView::onStartQueueRequested(){
	if(!workflowQueue_->isEmpty()){
		qDebug() << "Trying to start queue";
		if(!workflowQueue_->head()->hasFinished() ){
			qDebug() << "Initialized, so just start, no reset";
			workflowQueue_->startQueue();
		}
		else{
			qDebug() << "Not initialized anymore, so need to reset";
			workflowQueue_->head()->reset();
			workflowQueue_->startQueue();
		}
		//workflowQueue_->head()->start();
	}
//	if(workflowActions_->count() > 0)
//		workflowActions_->action(0)->start();
	/*
//	if(workflowActions_->count() > 0)
//		workflowActions_->action(0)->start();
	AMBeamlineActionItem *goItem = workflowView_->firstInQueue();
	if(!goItem)
		qDebug() << "Nothing in queue";
	else
		goItem->start();
	*/
}

void AMWorkflowManagerView::onAddActionRequested(){
	adder_->move(addActionButton_->pos());
	adder_->show();
}

void AMWorkflowManagerView::onAddScanRequested(AMScanConfiguration *cfg, bool startNow){
	/// \todo fix for general scan type
	if(startNow && !workflowQueue_->isRunning())
		emit freeToScan(workflowQueue_->isEmpty(), !workflowQueue_->isRunning());
	else if(startNow && workflowQueue_->isRunning())
		emit freeToScan(workflowQueue_->peekIsEmpty(), !workflowQueue_->isRunning());
	//NOT THREAD SAFE IF WORKFLOW AND SXSCVIEWER IN DIFFERENT THREADS
	if(cancelAddRequest_){
		cancelAddRequest_ = false;
		qDebug() << "Request cancelled, doing nothing";
		return;
	}

	SGMXASScanConfiguration *sxsc = (SGMXASScanConfiguration*)cfg;

	AMBeamlineScanAction *scanAction = new AMBeamlineScanAction(sxsc, "SGMXASScan", this);
	workflowActions_->appendAction(scanAction);
	emit addedScan(cfg);
	if(startNow)
		onStartQueueRequested();
}

void AMWorkflowManagerView::onCancelAddScanRequest(){
	cancelAddRequest_ = true;
}

void AMWorkflowManagerView::onInsertActionRequested(AMBeamlineActionItem *action, int index){
	if(!workflowQueue_->isEmpty())
		workflowActions_->addAction(workflowQueue_->indexOfHead()+index, action);
	else
		workflowActions_->appendAction(action);
//		qDebug() << "Insert request with iof " << workflowView_->indexOfFirst() << " so to " << workflowView_->indexOfFirst()+index;
//		workflowActions_->addAction(workflowView_->indexOfFirst()+index, action);
}

void AMWorkflowManagerView::onBeamlineScanningChanged(bool scanning){
//	if(scanning)
//		startWorkflowButton_->setEnabled(false);
//	else
//		startWorkflowButton_->setEnabled(true);
	onQueueAndScanningStatusChanged();
}

void AMWorkflowManagerView::onQueueIsRunningChanged(bool isRunning){
//	if(isRunning)
//		startWorkflowButton_->setEnabled(false);
//	else
//		startWorkflowButton_->setEnabled(true);
	onQueueAndScanningStatusChanged();
}

void AMWorkflowManagerView::onQueueIsEmptyChanged(bool isEmpty){
	onQueueAndScanningStatusChanged();
}

void AMWorkflowManagerView::onNewScanConfigurationView(){
	onQueueAndScanningStatusChanged();
}

void AMWorkflowManagerView::onQueueAndScanningStatusChanged(){
	bool qEmpty = workflowQueue_->isEmpty();
	bool qRunning = workflowQueue_->isRunning();
	bool blScanning = SGMBeamline::sgm()->isScanning();
	if(qEmpty || qRunning || blScanning)
		startWorkflowButton_->setEnabled(false);
	else{
		startWorkflowButton_->setEnabled(true);
		startWorkflowButton_->setText("Start This Workflow\nReady");
		emit lockdownScanning(false, "Ready");
	}
	if(blScanning && !qRunning){
			startWorkflowButton_->setText("Start This Workflow\nExternal Scan");
			emit lockdownScanning(true, "-- External Scan --");
	}
	else if(qEmpty){
		startWorkflowButton_->setText("Start This Workflow\n-- No Items --");
	}
	else if(qRunning){
		startWorkflowButton_->setText("Start This Workflow\n-- Running --");
	}
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


AMBeamlineActionAdder::AMBeamlineActionAdder(QWidget *parent) :
		QWidget(parent)
{
	xPosLabel_ = NULL;
	yPosLabel_ = NULL;
	zPosLabel_ = NULL;
	rPosLabel_ = NULL;
	addWhereBox_ = new QComboBox();
	onQueueUpdated(NULL);
	actionTypeBox_ = new QComboBox();
	QStringList actionTypes;
	actionTypes << "Choose Action Type" << "Go To Sample";// << "Move Action" << "Scan Action";
	actionTypeBox_->addItems(actionTypes);
	actionSubTypeBox_ = new QComboBox();
	QStringList actionSubTypes;
	actionSubTypes << "Choose Option";
	subTypesLists_.append(actionSubTypes);
	actionSubTypes.clear();
	actionSubTypes << "Position 1" << "Position 2";
	subTypesLists_.append(actionSubTypes);
	actionSubTypes.clear();/*
	actionSubTypes << "SSA X" << "SSA Y" << "SSA Z";
	subTypesLists_.append(actionSubTypes);
	actionSubTypes << "SGM XAS Scan";
	subTypesLists_.append(actionSubTypes);
	actionSubTypes.clear();
	*/
	actionSubTypeBox_->addItems(subTypesLists_.at(0));
	nextStepWidget_ = new QLabel(".....");
	connect(actionTypeBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onActionTypeBoxUpdate(int)));
	connect(actionSubTypeBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onActionSubTypeBoxUpdate(int)));
	vl_ = new QVBoxLayout();
	vl_->addWidget(addWhereBox_);
	vl_->addWidget(actionTypeBox_);
	vl_->addWidget(actionSubTypeBox_);
	vl_->addWidget(nextStepWidget_);
	setLayout(vl_);
	moveSetpointDSB_ = NULL;
}

void AMBeamlineActionAdder::onQueueUpdated(int count){
	addWhereBox_->clear();
	QStringList positions;
	positions << "Front of queue";
	QString tmpStr;
	for(int x = 0; x < count; x++)
		positions << "After "+tmpStr.setNum(x+1);
	addWhereBox_->addItems(positions);
}

void AMBeamlineActionAdder::onSamplePlateChanged(bool valid){
	qDebug() << "Rewriting sample list in adder";
	QStringList samples = subTypesLists_.at(1);
	samples.clear();
	if(valid){
		AMSamplePlate *sp = SGMBeamline::sgm()->currentSamplePlate();
		for(int x = 0; x < sp->count(); x++)
			samples << sp->sampleAt(x)->name();
	}
	subTypesLists_.replace(1, samples);
}

/// \todo optimize this. I don't need to redo the whole list for one insertion/deletion/change
void AMBeamlineActionAdder::onSamplePlateUpdate(int index){
	onSamplePlateChanged(SGMBeamline::sgm()->currentSamplePlate()->valid());
}

void AMBeamlineActionAdder::onActionTypeBoxUpdate(int curIndex){
	actionSubTypeBox_->clear();
	actionSubTypeBox_->addItems(subTypesLists_.at(curIndex));
	vl_->removeWidget(nextStepWidget_);
	delete nextStepWidget_;
	nextStepWidget_ = NULL;
	moveSetpointDSB_ = NULL;
	QPushButton *tmpPush;
	QVBoxLayout *tmpVl;
	switch(curIndex){
	case 0:
		nextStepWidget_ = new QLabel(".....");
		break;
	case 1:
		nextStepWidget_ = new QGroupBox(actionSubTypeBox_->currentText());
		tmpVl = new QVBoxLayout();
		xPosLabel_ = new QLabel("");
		yPosLabel_ = new QLabel("");
		zPosLabel_ = new QLabel("");
		rPosLabel_ = new QLabel("");
		tmpPush = new QPushButton("Add to Workflow");
		tmpVl->addWidget(xPosLabel_);
		tmpVl->addWidget(yPosLabel_);
		tmpVl->addWidget(zPosLabel_);
		tmpVl->addWidget(rPosLabel_);
		tmpVl->addWidget(tmpPush);
		nextStepWidget_->setLayout(tmpVl);
		connect(tmpPush, SIGNAL(clicked()), this, SLOT(onAddControlSetMoveAction()));
		onActionSubTypeBoxUpdate(actionSubTypeBox_->currentIndex());
		break;
	case 2:
		nextStepWidget_ = new QGroupBox(actionSubTypeBox_->currentText());
		tmpVl = new QVBoxLayout();
		moveSetpointDSB_ = new QDoubleSpinBox();
		tmpPush = new QPushButton("Add to Workflow");
		tmpVl->addWidget(moveSetpointDSB_);
		tmpVl->addWidget(tmpPush);
		nextStepWidget_->setLayout(tmpVl);
		connect(moveSetpointDSB_, SIGNAL(valueChanged(double)), this, SLOT(onNewMoveSetpoint(double)));
		connect(tmpPush, SIGNAL(clicked()), this, SLOT(onAddMoveAction()));
		onActionSubTypeBoxUpdate(actionSubTypeBox_->currentIndex());
		break;
	case 3:
		nextStepWidget_ = new QPushButton("Configure Scan");
		break;
	}
	vl_->addWidget(nextStepWidget_);
}

void AMBeamlineActionAdder::onActionSubTypeBoxUpdate(int curIndex){
	if(curIndex < 0)
		return;
	if(actionTypeBox_->currentIndex() == 2 && nextStepWidget_ && moveSetpointDSB_){
		((QGroupBox*)(nextStepWidget_))->setTitle(actionSubTypeBox_->currentText());
		switch(curIndex){
		case 0:
			movePV_ = (AMPVwStatusControl*)SGMBeamline::sgm()->ssaManipulatorX();
			break;
		case 1:
			movePV_ = (AMPVwStatusControl*)SGMBeamline::sgm()->ssaManipulatorY();
			break;
		case 2:
			movePV_ = (AMPVwStatusControl*)SGMBeamline::sgm()->ssaManipulatorZ();
			break;
		}
		moveSetpointDSB_->setRange(movePV_->minimumValue(), movePV_->maximumValue());
		moveSetpointDSB_->setValue(movePV_->value());
	}
	if(actionTypeBox_->currentIndex() == 1 && xPosLabel_ && yPosLabel_ && zPosLabel_ && rPosLabel_){
		AMSamplePlate *sp = SGMBeamline::sgm()->currentSamplePlate();
		onSamplePlateChanged(sp->valid());
		if( sp->valid() ){
			xPosLabel_->setText(QString("%1").arg(sp->positionAt(curIndex)->valueAt(0)));
			yPosLabel_->setText(QString("%1").arg(sp->positionAt(curIndex)->valueAt(1)));
			zPosLabel_->setText(QString("%1").arg(sp->positionAt(curIndex)->valueAt(2)));
			rPosLabel_->setText(QString("%1").arg(sp->positionAt(curIndex)->valueAt(3)));
		}
		else{
			xPosLabel_->setText("N/A");
			yPosLabel_->setText("N/A");
			zPosLabel_->setText("N/A");
			rPosLabel_->setText("N/A");
		}
	}
}

void AMBeamlineActionAdder::onNewMoveSetpoint(double value){
	moveSetpoint_ = value;
}

void AMBeamlineActionAdder::onAddControlSetMoveAction(){
	AMSamplePlate *sp = SGMBeamline::sgm()->currentSamplePlate();
	if(sp->valid()){
		AMBeamlineActionItem *tmpItem = new AMBeamlineControlSetMoveAction(SGMBeamline::sgm()->ssaManipulatorSet());
		((AMBeamlineControlSetMoveAction*)tmpItem)->setSetpoint(sp->positionAt(actionSubTypeBox_->currentIndex()));
		emit insertActionRequested(tmpItem, addWhereBox_->currentIndex());
		hide();
	}
}

void AMBeamlineActionAdder::onAddMoveAction(){
	AMBeamlineActionItem *tmpItem = new AMBeamlineControlMoveAction(movePV_);
	((AMBeamlineControlMoveAction*)tmpItem)->setSetpoint(moveSetpoint_);
	emit insertActionRequested(tmpItem, addWhereBox_->currentIndex());
	hide();
}
