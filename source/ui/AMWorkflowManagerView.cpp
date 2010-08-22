#include "AMWorkflowManagerView.h"
#include <QScrollArea>
#include <QPushButton>

AMWorkflowManagerView::AMWorkflowManagerView(QWidget *parent) :
	QWidget(parent)
{
	queueEmpty_ = true;

	adder_ = new AMBeamlineActionAdder();
	adder_->hide();
	connect(adder_, SIGNAL(insertActionRequested(AMBeamlineActionItem*,int)), this, SLOT(onInsertActionRequested(AMBeamlineActionItem*,int)));
	startWorkflowButton_ = new QPushButton("Start This Workflow");
	startWorkflowButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	addActionButton_ = new QPushButton("Add an Action");
	addActionButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	QHBoxLayout *hl = new QHBoxLayout();
	hl->addItem(new QSpacerItem(10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
	hl->addWidget(addActionButton_, 0, Qt::AlignRight);
	hl->addWidget(startWorkflowButton_, 0, Qt::AlignRight);
	connect(startWorkflowButton_, SIGNAL(clicked()), this, SLOT(onStartQueueRequested()));
	connect(addActionButton_, SIGNAL(clicked()), this, SLOT(onAddActionRequested()));
	workflowActions_ = new AMBeamlineActionsList(this);
	workflowView_ = new AMBeamlineActionsListView(workflowActions_, this);
	connect(workflowView_, SIGNAL(queueUpdated(QQueue<AMBeamlineActionItem*>)), adder_, SLOT(onQueueUpdated(QQueue<AMBeamlineActionItem*>)));

	vl_ = new QVBoxLayout();
//	vl_->addWidget(startWorkflowButton_, 0, Qt::AlignRight);
	vl_->addLayout(hl);
	vl_->addWidget(workflowView_);
	setLayout(vl_);
}

void AMWorkflowManagerView::onStartScanRequested(){
	if(isEmpty())
		emit freeToScan(true);
}

void AMWorkflowManagerView::onStartQueueRequested(){
//	if(workflowActions_->count() > 0)
//		workflowActions_->action(0)->start();
	AMBeamlineActionItem *goItem = workflowView_->firstInQueue();
	if(!goItem)
		qDebug() << "Nothing in queue";
	else
		goItem->start();
}

void AMWorkflowManagerView::onAddActionRequested(){
	adder_->move(addActionButton_->pos());
	adder_->show();
}

void AMWorkflowManagerView::onAddToQueueRequested(AMScanConfiguration *cfg){
	SGMXASScanConfiguration *sxsc = (SGMXASScanConfiguration*)cfg;

	AMBeamlineScanAction *scanAction = new AMBeamlineScanAction(sxsc, "SGMXASScan", "Deuce", this);
	workflowActions_->appendAction(scanAction);
	//if(workflowActions_->count() > 1){
	//	scanAction->setPrevious(workflowActions_->action(workflowActions_->count()-2));
	//	connect(scanAction->previous(), SIGNAL(succeeded()), scanAction, SLOT(start()));
	//}
	emit addedToQueue(cfg);
}

void AMWorkflowManagerView::onInsertActionRequested(AMBeamlineActionItem *action, int index){
		qDebug() << "Insert request with iof " << workflowView_->indexOfFirst() << " so to " << workflowView_->indexOfFirst()+index;
		workflowActions_->addAction(workflowView_->indexOfFirst()+index, action);
}


AMBeamlineActionListModel::AMBeamlineActionListModel(QObject *parent) :
		QAbstractListModel(parent)
{
	actions_ = new QList<AMBeamlineActionItem*>();
}

int AMBeamlineActionListModel::rowCount(const QModelIndex &index) const{
	Q_UNUSED(index);
	return actions_->count();
}

QVariant AMBeamlineActionListModel::data(const QModelIndex &index, int role) const{
	if(!index.isValid())
		return QVariant();
	if(index.row() >= actions_->count())
		return QVariant();
	if(role == Qt::DisplayRole)
		return qVariantFromValue((void*)actions_->at(index.row()));
	else
		return QVariant();
}

QVariant AMBeamlineActionListModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

bool AMBeamlineActionListModel::setData(const QModelIndex &index, const QVariant &value, int role){
	if (index.isValid()  && index.row() < actions_->count() && role == Qt::EditRole) {
//		bool conversionOK = false;
		AMBeamlineActionItem* actionItem;
		actionItem = (AMBeamlineActionItem*) value.value<void*>();

		actions_->replace(index.row(), actionItem);
		emit dataChanged(index, index);
		return true;
	}
	return false;	// no value set
}

bool AMBeamlineActionListModel::insertRows(int position, int rows, const QModelIndex &index){
	if (index.row() <= actions_->count() && position <= actions_->count()) {
		beginInsertRows(QModelIndex(), position, position+rows-1);
		AMBeamlineActionItem *tmpAction = NULL;
		for (int row = 0; row < rows; ++row) {
			actions_->insert(position, tmpAction);
		}
		endInsertRows();
		return true;
	}
	return false;
}

bool AMBeamlineActionListModel::removeRows(int position, int rows, const QModelIndex &index){
	if (index.row() < actions_->count() && position < actions_->count()) {
		beginRemoveRows(QModelIndex(), position, position+rows-1);
		for (int row = 0; row < rows; ++row) {
			actions_->removeAt(position);
		}
		endRemoveRows();
		return true;
	}
	return false;
}

Qt::ItemFlags AMBeamlineActionListModel::flags(const QModelIndex &index) const{
	Qt::ItemFlags flags;
	if (index.isValid() && index.row() < actions_->count() && index.column()<4)
		flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	return flags;
}

AMBeamlineActionsList::AMBeamlineActionsList(QObject *parent) :
		QObject(parent)
{
	actions_ = NULL;
	setupModel();
}

AMBeamlineActionItem* AMBeamlineActionsList::action(size_t index) const{
	QVariant retVal = actions_->data(actions_->index(index, 0), Qt::DisplayRole);
	if(retVal.isValid())
		return (AMBeamlineActionItem*) retVal.value<void*>();
	return NULL;
}

int AMBeamlineActionsList::indexOf(AMBeamlineActionItem *iAction){
	for(int x = 0; x < count(); x++)
		if(action(x) == iAction)
			return x;
	return -1;
}

bool AMBeamlineActionsList::setAction(size_t index, AMBeamlineActionItem *action){
	AMBeamlineActionItem *oldAction = (AMBeamlineActionItem*)actions_->data( actions_->index(index, 0), Qt::DisplayRole ).value<void*>();
	AMBeamlineActionItem *prevAction = NULL;
	AMBeamlineActionItem *nextAction = NULL;
	if( (int)index != 0)
		prevAction = (AMBeamlineActionItem*)actions_->data( actions_->index(index-1, 0), Qt::DisplayRole ).value<void*>();
	if( (int)index != (count() -1) )
		nextAction = (AMBeamlineActionItem*)actions_->data( actions_->index(index+1, 0), Qt::DisplayRole ).value<void*>();
	if(oldAction){ //replace
		if(prevAction){
			disconnect(prevAction, SIGNAL(succeeded()), oldAction, SLOT(start()));
			prevAction->setNext(action);
			action->setPrevious(prevAction);
			connect(prevAction, SIGNAL(succeeded()), action, SLOT(start()));
		}
		if(nextAction){
			disconnect(oldAction, SIGNAL(succeeded()), nextAction, SLOT(start()));
			action->setNext(nextAction);
			nextAction->setPrevious(action);
			connect(action, SIGNAL(succeeded()), nextAction, SLOT(start()));
		}
		oldAction->setPrevious(NULL);
		oldAction->setNext(NULL);
	}
	else{ //insert
		if(prevAction && nextAction)
			disconnect(prevAction, SIGNAL(succeeded()), nextAction, SLOT(start()));
		if(prevAction){
			prevAction->setNext(action);
			action->setPrevious(prevAction);
			connect(prevAction, SIGNAL(succeeded()), action, SLOT(start()));
		}
		if(nextAction){
			action->setNext(nextAction);
			nextAction->setPrevious(action);
			connect(action, SIGNAL(succeeded()), nextAction, SLOT(start()));
		}
	}

	return actions_->setData(actions_->index(index, 0), qVariantFromValue((void*)action), Qt::EditRole);
}

bool AMBeamlineActionsList::addAction(size_t index, AMBeamlineActionItem *action){
	if(!actions_->insertRows(index, 1))
		return false;
	return setAction(index, action);
}

bool AMBeamlineActionsList::appendAction(AMBeamlineActionItem *action){
	return addAction(count(), action);
}

bool AMBeamlineActionsList::deleteAction(size_t index){
	if(count() == 0)
		return false;
	AMBeamlineActionItem *oldAction = (AMBeamlineActionItem*)actions_->data( actions_->index(index, 0), Qt::DisplayRole ).value<void*>();
	AMBeamlineActionItem *prevAction = NULL;
	AMBeamlineActionItem *nextAction = NULL;
	if( (int)index != 0)
		prevAction = (AMBeamlineActionItem*)actions_->data( actions_->index(index-1, 0), Qt::DisplayRole ).value<void*>();
	if( (int)index != (count() -1) )
		nextAction = (AMBeamlineActionItem*)actions_->data( actions_->index(index+1, 0), Qt::DisplayRole ).value<void*>();
	bool retVal = actions_->removeRows(index, 1);
	if(retVal){
		if(prevAction){
			disconnect(prevAction, SIGNAL(succeeded()), oldAction, SLOT(start()));
			prevAction->setNext(nextAction);
		}
		if(nextAction){
			disconnect(oldAction, SIGNAL(succeeded()), nextAction, SLOT(start()));
			nextAction->setPrevious(prevAction);
		}
		if(prevAction && nextAction)
			connect(prevAction, SIGNAL(succeeded()), nextAction, SLOT(start()));
		oldAction->setPrevious(NULL);
		oldAction->setNext(NULL);
	}
	return retVal;
}

bool AMBeamlineActionsList::setupModel(){
	actions_ = new AMBeamlineActionListModel(this);
	if(actions_){
		connect(actions_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)) );
		return true;
	}
	return false;
}

AMBeamlineActionsListView::AMBeamlineActionsListView(AMBeamlineActionsList *actionsList, QWidget *parent) :
		QWidget(parent)
{
	actionsList_ = actionsList;
	focusAction_ = -1;
	insertRowIndex_ = -1;

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
}

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

void AMBeamlineActionsListView::reindexViews(){
//	for(int x = 0; x < fullViewList_.count(); x++)
//		fullViewList_.at(x)->setIndex(x+1);
	for(int x = 0; x < viewQueue_.count(); x++)
		viewQueue_.at(x)->setIndex(x+1);
}

AMBeamlineActionAdder::AMBeamlineActionAdder(QWidget *parent) :
		QWidget(parent)
{
	addWhereBox_ = new QComboBox();
	onQueueUpdated(QQueue<AMBeamlineActionItem *>());
	actionTypeBox_ = new QComboBox();
	QStringList actionTypes;
	actionTypes << "Choose Action Type" << "Scan Action" << "Move Action";
	actionTypeBox_->addItems(actionTypes);
	actionSubTypeBox_ = new QComboBox();
	QStringList actionSubTypes;
	actionSubTypes << "Choose Option";
	subTypesLists_.append(actionSubTypes);
	actionSubTypes.clear();
	actionSubTypes << "SGM XAS Scan";
	subTypesLists_.append(actionSubTypes);
	actionSubTypes.clear();
	actionSubTypes << "SSA X" << "SSA Y" << "SSA Z";
	subTypesLists_.append(actionSubTypes);
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

void AMBeamlineActionAdder::onQueueUpdated(QQueue<AMBeamlineActionItem *> actionsQueue){
	addWhereBox_->clear();
	QStringList positions;
	positions << "Front of queue";
	QString tmpStr;
	for(int x = 0; x < actionsQueue.count(); x++)
		positions << "After "+tmpStr.setNum(x+1);
	addWhereBox_->addItems(positions);
}

void AMBeamlineActionAdder::onActionTypeBoxUpdate(int curIndex){
	actionSubTypeBox_->clear();
	actionSubTypeBox_->addItems(subTypesLists_.at(curIndex));
	vl_->removeWidget(nextStepWidget_);
	delete nextStepWidget_;
	nextStepWidget_ = NULL;
	moveSetpointDSB_ = NULL;
	switch(curIndex){
	case 0:
		nextStepWidget_ = new QLabel(".....");
		break;
	case 1:
		nextStepWidget_ = new QPushButton("Configure Scan");
		break;
	case 2:
		nextStepWidget_ = new QGroupBox(actionSubTypeBox_->currentText());
		QVBoxLayout *tmpVl = new QVBoxLayout();
		moveSetpointDSB_ = new QDoubleSpinBox();
		QPushButton *tmpPush = new QPushButton("Add to Workflow");
		tmpVl->addWidget(moveSetpointDSB_);
		tmpVl->addWidget(tmpPush);
		nextStepWidget_->setLayout(tmpVl);
		connect(moveSetpointDSB_, SIGNAL(valueChanged(double)), this, SLOT(onNewMoveSetpoint(double)));
		connect(tmpPush, SIGNAL(clicked()), this, SLOT(onAddMoveAction()));
		onActionSubTypeBoxUpdate(actionSubTypeBox_->currentIndex());
		break;
	}
	vl_->addWidget(nextStepWidget_);
}

void AMBeamlineActionAdder::onActionSubTypeBoxUpdate(int curIndex){
	if(curIndex < 0 || !nextStepWidget_ || !moveSetpointDSB_)
		return;
	if(actionTypeBox_->currentIndex() == 2){
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
}

void AMBeamlineActionAdder::onNewMoveSetpoint(double value){
	moveSetpoint_ = value;
}

void AMBeamlineActionAdder::onAddMoveAction(){
	AMBeamlineActionItem *tmpItem = new AMBeamlineControlMoveAction(movePV_, "");
	((AMBeamlineControlMoveAction*)tmpItem)->setSetpoint(moveSetpoint_);
	emit insertActionRequested(tmpItem, addWhereBox_->currentIndex());
	hide();
}
