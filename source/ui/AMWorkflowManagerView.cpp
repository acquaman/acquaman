#include "AMWorkflowManagerView.h"
#include <QScrollArea>
#include <QPushButton>

AMWorkflowManagerView::AMWorkflowManagerView(QWidget *parent) :
	QWidget(parent)
{
	queueEmpty_ = true;

	startWorkflowButton_ = new QPushButton("Start This Workflow");
	connect(startWorkflowButton_, SIGNAL(clicked()), this, SLOT(onStartQueueRequested()));
	workflowActions_ = new AMBeamlineActionsList(this);
	workflowView_ = new AMBeamlineActionsListView(workflowActions_, this);

	vl_ = new QVBoxLayout();
	vl_->addWidget(startWorkflowButton_, 0, Qt::AlignRight);
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



AMBeamlineActionListModel::AMBeamlineActionListModel(QObject *parent)
{
	actions_ = new QList<AMBeamlineActionItem*>();
}

int AMBeamlineActionListModel::rowCount(const QModelIndex &index) const{
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
		bool conversionOK = false;
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
	if(index != 0)
		prevAction = (AMBeamlineActionItem*)actions_->data( actions_->index(index-1, 0), Qt::DisplayRole ).value<void*>();
	if(index != (count() -1) )
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
	if(index != 0)
		prevAction = (AMBeamlineActionItem*)actions_->data( actions_->index(index-1, 0), Qt::DisplayRole ).value<void*>();
	if(index != (count() -1) )
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
	gb_->setMinimumSize(300, 50);
	gb_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);


	QScrollArea *sa = new QScrollArea();
	sa->setWidget(gb_);
	sa->setWidgetResizable(true);
	QVBoxLayout *ob = new QVBoxLayout();
	ob->addWidget(sa);
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

void AMBeamlineActionsListView::handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight){
	AMBeamlineActionItem *tmpItem = actionsList_->action(topLeft.row());
	qDebug() << "In handleDataChanged on " << topLeft.row() << bottomRight.row();
	if(topLeft.row() != bottomRight.row())
		return; //WHAT SHOULD I DO FOR MULTI-ROW CHANGE
	else if( (insertRowIndex_ != -1) && (topLeft.row() == insertRowIndex_) ){
		if(tmpItem->type() == "actionItem.scanAction"){
			AMBeamlineScanAction *scanAction = (AMBeamlineScanAction*)tmpItem;
			actionsQueue_.enqueue(tmpItem);
//			AMBeamlineScanActionView *scanActionView = new AMBeamlineScanActionView(scanAction, topLeft.row()+1);
			AMBeamlineScanActionView *scanActionView = new AMBeamlineScanActionView(scanAction, viewQueue_.count()+1);
			viewList_.append(scanActionView);
			viewQueue_.enqueue(scanActionView);
			iib->addWidget(scanActionView);
			connect(scanActionView, SIGNAL(focusRequested(AMBeamlineActionItem*)), this, SLOT(onFocusRequested(AMBeamlineActionItem*)));
			connect(scanActionView, SIGNAL(removeRequested(AMBeamlineActionItem*)), this, SLOT(onRemoveRequested(AMBeamlineActionItem*)));
			connect(scanActionView, SIGNAL(scanSuceeded(AMBeamlineActionItem*)), this, SLOT(onScanSucceeded(AMBeamlineActionItem*)));
		}
		insertRowIndex_ = -1;
	}
	else if( insertRowIndex_ == -1){
		qDebug() << "Going to reset action on the view";
		viewList_.at(topLeft.row())->setAction((AMBeamlineScanAction*)tmpItem);
	}
}

void AMBeamlineActionsListView::handleRowsInsert(const QModelIndex &parent, int start, int end){
	qDebug() << "In handleRowInsert on " << start << end;
	if(start != end)
		return; //WHAT TO DO ON MULTI-ROW INSERT?
	insertRowIndex_ = start;
	gb_->setMinimumHeight(50+50*actionsList_->count());
}

void AMBeamlineActionsListView::handleRowsRemoved(const QModelIndex &parent, int start, int end){
	qDebug() << "In handleRowRemove on " << start << end;
	if(start != end)
		return; //WHAT TO DO FOR MULTI-ROW DELETE?
	qDebug() << "Counts: " << viewList_.count() << iib->count();
	AMBeamlineScanActionView *tmpView = viewList_.takeAt(start);
	viewQueue_.removeAll(tmpView);
	actionsQueue_.removeAll(tmpView->action());
	iib->removeWidget(tmpView);
	delete tmpView;
	qDebug() << "Counts: " << viewList_.count() << iib->count();
	reindexViews();
	gb_->setMinimumHeight(50+50*actionsList_->count());
}

void AMBeamlineActionsListView::redrawBeamlineActionsList(){

}

void AMBeamlineActionsListView::onFocusRequested(AMBeamlineActionItem *action){
	if(focusAction_ == actionsList_->indexOf(action))
		return;
	else if(focusAction_ != -1)
		viewList_.at(focusAction_)->defocusItem();
	focusAction_ = actionsList_->indexOf(action);
}

void AMBeamlineActionsListView::onRemoveRequested(AMBeamlineActionItem *action){
	qDebug() << "Requesting remove so deleting action from list";
	int index = actionsList_->indexOf(action);
	disconnect( viewList_.at(index), SIGNAL(focusRequested(AMBeamlineActionItem*)), this, SLOT(onFocusRequested(AMBeamlineActionItem*)));
	disconnect( viewList_.at(index), SIGNAL(removeRequested(AMBeamlineActionItem*)), this, SLOT(onRemoveRequested(AMBeamlineActionItem*)));
	disconnect( viewList_.at(index), SIGNAL(scanSuceeded(AMBeamlineActionItem*)), this, SLOT(onScanSucceeded(AMBeamlineActionItem*)));
	actionsList_->deleteAction( index );
}

void AMBeamlineActionsListView::onScanSucceeded(AMBeamlineActionItem *action){
	if(actionsQueue_.head() != action)
		return; //WHAT THE HELL, who succeeded that isn't at the head of the queue?
	actionsQueue_.dequeue();
	viewQueue_.dequeue();
	viewList_.at( actionsList_->indexOf(action) )->setIndex(-1);
	reindexViews();
}

void AMBeamlineActionsListView::reindexViews(){
//	for(int x = 0; x < viewList_.count(); x++)
//		viewList_.at(x)->setIndex(x+1);
	for(int x = 0; x < viewQueue_.count(); x++)
		viewQueue_.at(x)->setIndex(x+1);
}

BeamlineActionGraphicItem::BeamlineActionGraphicItem(int width)
{
	color = QColor("blue");
	width_ = width;
}

void BeamlineActionGraphicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(option);
	Q_UNUSED(widget);
	painter->setPen(Qt::NoPen);
	painter->setBrush(Qt::darkGray);
	painter->drawRoundedRect(0, 0, width_, 30, 1, 1, Qt::RelativeSize);
	painter->setPen(QPen(Qt::black, 1));
	painter->setBrush(Qt::gray);
	painter->drawRoundedRect(0, 0, width_, 30, 1, 1, Qt::RelativeSize);
	QString deltaVal;
	deltaVal.setNum(width_);
	QChar deltaChar(0x0394);
	deltaVal.prepend(" = ");
	deltaVal.prepend(deltaChar);
	QRectF box(0, 0, width_, 30);
	painter->drawText(box, Qt::AlignHCenter, deltaVal, &box);
}
