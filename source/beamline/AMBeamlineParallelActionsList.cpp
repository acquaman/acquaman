#include "AMBeamlineParallelActionsList.h"

AMBeamlineParallelActionsList::AMBeamlineParallelActionsList(QObject *parent) :
		QObject(parent)
{
	insertRowLatch_ = -1;
	actions_ = NULL;
	currentStage_ = -1;
	isRunning_ = false;
	setupModel();
	connect(this, SIGNAL(stageStarted(int)), this, SLOT(onStageStarted(int)));
}

AMBeamlineParallelActionListModel* AMBeamlineParallelActionsList::model(){
	return actions_;
}

int AMBeamlineParallelActionsList::stageCount(){
	return actions_->rowCount(QModelIndex());
}

int AMBeamlineParallelActionsList::count(){
	int totalActions = 0;
	for(int x = 0; x < stageCount(); x++)
		totalActions += countAt(x);
	return totalActions;
}

int AMBeamlineParallelActionsList::countAt(int stageIndex){
	if(stageIndex >= 0 && stageIndex < stageCount())
		return stage(stageIndex)->count();
	return -1;
}

QList<AMBeamlineActionItem*>* AMBeamlineParallelActionsList::stage(int stageIndex) const{
	QVariant retVal = actions_->data(actions_->index(stageIndex, QModelIndex()), Qt::DisplayRole);
	if(retVal.isValid())
		return (QList<AMBeamlineActionItem*>*) retVal.value<void*>();
	return NULL;
}

AMBeamlineActionItem* AMBeamlineParallelActionsList::action(int stageIndex, int index) const{
	QVariant retVal = actions_->data(actions_->index(index, actions_->index(stageIndex, QModelIndex())), Qt::DisplayRole);
	if(retVal.isValid())
		return (AMBeamlineActionItem*) retVal.value<void*>();
	return NULL;
}

int AMBeamlineParallelActionsList::stageIndexOf(QList<AMBeamlineActionItem *> *iList){
	for(int x = 0; x < stageCount(); x++)
		if(stage(x) == iList)
			return x;
	return -1;
}

int AMBeamlineParallelActionsList::indexOf(QList<AMBeamlineActionItem *> *iList){
	return stageIndexOf(iList);
}

int AMBeamlineParallelActionsList::stageIndexOf(AMBeamlineActionItem *iAction){
	for(int x = 0; x < stageCount(); x++)
		for(int y = 0; y < count(); y++)
			if(action(x, y) == iAction)
				return x;
	return -1;
}

int AMBeamlineParallelActionsList::indexOf(AMBeamlineActionItem *iAction){
	for(int x = 0; x < stageCount(); x++)
		for(int y = 0; y < count(); y++)
			if(action(x, y) == iAction)
				return y;
	return -1;
}

QPair<int,int> AMBeamlineParallelActionsList::indicesOf(AMBeamlineActionItem *iAction){
	for(int x = 0; x < stageCount(); x++)
		for(int y = 0; y < count(); y++)
			if(action(x, y) == iAction)
				return QPair<int,int>(x,y);
	return QPair<int,int>(-1,-1);
}

bool AMBeamlineParallelActionsList::isRunning(){
	return isRunning_;
}

void AMBeamlineParallelActionsList::puke(){
	AMBeamlineNumberAction *nAction;
	QString stageString, tmpStr;
	for(int x = 0; x < stageCount(); x++){
		stageString = "";
		for(int y = 0; y < countAt(x); y++){
			if(nAction = qobject_cast<AMBeamlineNumberAction*>(action(x, y)))
				tmpStr.setNum(nAction->getNumber());
			else
				tmpStr.setNum( qint64(action(x,y)) );
			stageString.append(tmpStr+" ");
		}
		if(x != stageCount()-1)
			stageString.append("--V");
		qDebug() << stageString;
	}
}

bool AMBeamlineParallelActionsList::setStage(int stageIndex, QList<AMBeamlineActionItem *> *stageList){
	QModelIndex modelIndex = actions_->index(stageIndex, QModelIndex());
	if(modelIndex.isValid() && stageList){
		QList<AMBeamlineActionItem*> *oldStageList = (QList<AMBeamlineActionItem*>*)actions_->data(actions_->index(stageIndex, QModelIndex()), Qt::DisplayRole).value<void*>();
		QList<AMBeamlineActionItem*> *prevStageList = NULL;
		QList<AMBeamlineActionItem*> *nextStageList = NULL;
		if(stageIndex != 0)
			prevStageList = (QList<AMBeamlineActionItem*>*)actions_->data(actions_->index(stageIndex-1, QModelIndex()), Qt::DisplayRole).value<void*>();
		if(stageIndex != stageCount() -1)
			nextStageList = (QList<AMBeamlineActionItem*>*)actions_->data(actions_->index(stageIndex+1, QModelIndex()), Qt::DisplayRole).value<void*>();

		if(oldStageList){//replace
			AMBeamlineParallelActionsListHolder *thisStageHolder = holdersHash_.valueF(oldStageList);
			for(int x = 0; x < oldStageList->count(); x++){
				disconnect(oldStageList->at(x), SIGNAL(started()), this, SLOT(onActionStarted()));
				disconnect(oldStageList->at(x), SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
				disconnect(oldStageList->at(x), SIGNAL(ready(bool)), this, SLOT(onActionReady(bool)));
				disconnect(oldStageList->at(x), SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
				disconnect(oldStageList->at(x), SIGNAL(progress(double,double)), this, SLOT(onActionProgress(double,double)));
			}
			if(prevStageList){
				for(int x = 0; x < oldStageList->count(); x++)
					disconnect(holdersHash_.valueF(prevStageList), SIGNAL(everythingFinished()), oldStageList->at(x), SLOT(start()));
				for(int x = 0; x < stageList->count(); x++)
					connect(holdersHash_.valueF(prevStageList), SIGNAL(everythingFinished()), stageList->at(x), SLOT(start()));
			}
			if(nextStageList){
				for(int x = 0; x < oldStageList->count(); x++)
					thisStageHolder->removeAction(oldStageList->at(x));
			}
			for(int x = 0; x < stageList->count(); x++)
				thisStageHolder->addAction(stageList->at(x));
			holdersHash_.removeF(oldStageList);
			holdersHash_.set(stageList, thisStageHolder);
		}
		else{//insert
			holdersHash_.set(stageList, new AMBeamlineParallelActionsListHolder(this));
			connect(holdersHash_.valueF(stageList), SIGNAL(everythingFinished()), this, SLOT(onStageSucceeded()));
			#warning "Hey David, What connections need to be made (added to) the holder?"
			for(int x = 0; x < stageList->count(); x++)
				holdersHash_.valueF(stageList)->addAction(stageList->at(x));
			if(prevStageList && nextStageList)
				for(int x = 0; x < nextStageList->count(); x++)
					disconnect(holdersHash_.valueF(prevStageList), SIGNAL(everythingFinished()), nextStageList->at(x), SLOT(start()));
			if(prevStageList)
				for(int x = 0; x < stageList->count(); x++)
					connect(holdersHash_.valueF(prevStageList), SIGNAL(everythingFinished()), stageList->at(x), SLOT(start()));
			if(nextStageList)
				for(int x = 0; x < nextStageList->count(); x++)
					connect(holdersHash_.valueF(stageList), SIGNAL(everythingFinished()), nextStageList->at(x), SLOT(start()));
		}
		for(int x = 0; x < stageList->count(); x++){
			connect(stageList->at(x), SIGNAL(started()), this, SLOT(onActionStarted()));
			connect(stageList->at(x), SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
			connect(stageList->at(x), SIGNAL(ready(bool)), this, SLOT(onActionReady(bool)));
			connect(stageList->at(x), SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
			connect(stageList->at(x), SIGNAL(progress(double,double)), this, SLOT(onActionProgress(double,double)));
		}
		return actions_->setData(modelIndex, qVariantFromValue((void*)stageList), Qt::EditRole);
	}
	return false;
}

//HEY DAVE, CHECK THE ORDERING ON THIS, RETURN STATEMENT SEEMS ODD
bool AMBeamlineParallelActionsList::setAction(int stageIndex, int index, AMBeamlineActionItem *action){
	QModelIndex modelIndex = actions_->index(index, actions_->index(stageIndex, QModelIndex()));
	if(modelIndex.isValid() && action){
		AMBeamlineActionItem *oldAction = NULL;
		oldAction= (AMBeamlineActionItem*)actions_->data(actions_->index(index, actions_->index(stageIndex, QModelIndex())), Qt::DisplayRole).value<void*>();
		QList<AMBeamlineActionItem*> *thisStageList = (QList<AMBeamlineActionItem*>*)actions_->data(actions_->index(stageIndex, QModelIndex()), Qt::DisplayRole).value<void*>();
		QList<AMBeamlineActionItem*> *prevStageList = NULL;
		bool retVal;
		retVal = actions_->setData(modelIndex, qVariantFromValue((void*)action), Qt::EditRole);
		if(retVal){
			if(stageIndex != 0)
				prevStageList = (QList<AMBeamlineActionItem*>*)actions_->data(actions_->index(stageIndex-1, QModelIndex()), Qt::DisplayRole).value<void*>();
			if(prevStageList)
				connect(holdersHash_.valueF(prevStageList), SIGNAL(everythingFinished()), action, SLOT(start()));
			holdersHash_.valueF(thisStageList)->addAction(action);
			if(oldAction){//replace
				if(prevStageList)
					disconnect(holdersHash_.valueF(prevStageList), SIGNAL(everythingFinished()), oldAction, SLOT(start()));
				holdersHash_.valueF(thisStageList)->removeAction(oldAction);
				disconnect(oldAction, SIGNAL(started()), this, SLOT(onActionStarted()));
				disconnect(oldAction, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
				disconnect(oldAction, SIGNAL(ready(bool)), this, SLOT(onActionReady(bool)));
				disconnect(oldAction, SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
				disconnect(oldAction, SIGNAL(progress(double,double)), this, SLOT(onActionProgress(double,double)));
			}
			connect(action, SIGNAL(started()), this, SLOT(onActionStarted()));
			connect(action, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
			connect(action, SIGNAL(ready(bool)), this, SLOT(onActionReady(bool)));
			connect(action, SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
			connect(action, SIGNAL(progress(double,double)), this, SLOT(onActionProgress(double,double)));
		}
		return retVal;
	}
	return false;
}

bool AMBeamlineParallelActionsList::addStage(int stageIndex, QList<AMBeamlineActionItem *> *stageList){
	if( (stageIndex < 0) || (stageIndex > stageCount()))
		return false;
	if(!actions_->insertRows(stageIndex, 1, QModelIndex()))
		return false;
	return setStage(stageIndex, stageList);
}

bool AMBeamlineParallelActionsList::addAction(int stageIndex, int index, AMBeamlineActionItem *action){
	if( (stageIndex < 0) || (stageIndex >= stageCount()) || (index < 0) || (index > stage(stageIndex)->count()) )
		return false;
	bool retVal = actions_->insertRows(index, 1, actions_->index(stageIndex, QModelIndex()));
	if(!retVal)
		return false;
	return setAction(stageIndex, index, action);
}

bool AMBeamlineParallelActionsList::appendStage(QList<AMBeamlineActionItem *> *stageList){
	return addStage(stageCount(), stageList);
}

bool AMBeamlineParallelActionsList::appendAction(int stageIndex, AMBeamlineActionItem *action){
	if(stageIndex < stageCount())
		return addAction(stageIndex, stage(stageIndex)->count(), action);
	return false;
}

bool AMBeamlineParallelActionsList::deleteStage(int stageIndex){
	if( (stageCount() == 0) || (stageIndex < 0) || (stageIndex >= stageCount()) )
		return false;
	QList<AMBeamlineActionItem*> *oldStageList = (QList<AMBeamlineActionItem*>*)actions_->data(actions_->index(stageIndex, QModelIndex()), Qt::DisplayRole).value<void*>();
	QList<AMBeamlineActionItem*> *prevStageList = NULL;
	QList<AMBeamlineActionItem*> *nextStageList = NULL;
	if(stageIndex != 0)
		prevStageList = (QList<AMBeamlineActionItem*>*)actions_->data(actions_->index(stageIndex-1, QModelIndex()), Qt::DisplayRole).value<void*>();
	if(stageIndex != stageCount() -1)
		nextStageList = (QList<AMBeamlineActionItem*>*)actions_->data(actions_->index(stageIndex+1, QModelIndex()), Qt::DisplayRole).value<void*>();

	bool retVal;
	retVal = actions_->removeRows(stageIndex, 1, QModelIndex());
	if(retVal){
		if(oldStageList){
			AMBeamlineParallelActionsListHolder *thisStageHolder = holdersHash_.valueF(oldStageList);
			for(int x = 0; x < oldStageList->count(); x++){
				disconnect(oldStageList->at(x), SIGNAL(started()), this, SLOT(onActionStarted()));
				disconnect(oldStageList->at(x), SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
				disconnect(oldStageList->at(x), SIGNAL(ready(bool)), this, SLOT(onActionReady(bool)));
				disconnect(oldStageList->at(x), SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
				disconnect(oldStageList->at(x), SIGNAL(progress(double,double)), this, SLOT(onActionProgress(double,double)));
				thisStageHolder->removeAction(oldStageList->at(x));
			}
			if(prevStageList){
				for(int x = 0; x < oldStageList->count(); x++)
					disconnect(holdersHash_.valueF(prevStageList), SIGNAL(everythingFinished()), oldStageList->at(x), SLOT(start()));
			}
			if(nextStageList){
				for(int x = 0; x < nextStageList->count(); x++)
					disconnect(thisStageHolder, SIGNAL(everythingFinished()), nextStageList->at(x), SLOT(start()));
			}
			if(prevStageList && nextStageList){
				for(int x = 0; x < nextStageList->count(); x++)
					connect(holdersHash_.valueF(prevStageList), SIGNAL(everythingFinished()), nextStageList->at(x), SLOT(start()));
			}
			disconnect(holdersHash_.valueF(oldStageList), SIGNAL(everythingFinished()), this, SLOT(onStageSucceeded()));
			holdersHash_.removeF(oldStageList);
			#warning "Hey David, do you need to delete that holder? You new'd it"
		}
	}
	return retVal;
}

bool AMBeamlineParallelActionsList::deleteAction(int stageIndex, int index){
	if( (stageCount() == 0) || (stageIndex < 0) || (stageIndex >= stageCount()) || (index < 0) || (index >= stage(stageIndex)->count()) )
		return false;
	bool retVal;
	AMBeamlineActionItem *oldAction = (AMBeamlineActionItem*)actions_->data(actions_->index(index, actions_->index(stageIndex, QModelIndex())), Qt::DisplayRole).value<void*>();
	QList<AMBeamlineActionItem*> *thisStageList = (QList<AMBeamlineActionItem*>*)actions_->data(actions_->index(stageIndex, QModelIndex()), Qt::DisplayRole).value<void*>();
	QList<AMBeamlineActionItem*> *prevStageList = NULL;
	retVal = actions_->removeRows(index, 1, actions_->index(stageIndex, QModelIndex()));
	if(retVal){
		if(stageIndex != 0)
			prevStageList = (QList<AMBeamlineActionItem*>*)actions_->data(actions_->index(stageIndex-1, QModelIndex()), Qt::DisplayRole).value<void*>();
		if(prevStageList)
			disconnect(holdersHash_.valueF(prevStageList), SIGNAL(everythingFinished()), oldAction, SLOT(start()));
		if(prevStageList)
			disconnect(holdersHash_.valueF(prevStageList), SIGNAL(everythingFinished()), oldAction, SLOT(start()));
		holdersHash_.valueF(thisStageList)->removeAction(oldAction);
		disconnect(oldAction, SIGNAL(started()), this, SLOT(onActionStarted()));
		disconnect(oldAction, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
		disconnect(oldAction, SIGNAL(ready(bool)), this, SLOT(onActionReady(bool)));
		disconnect(oldAction, SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
		disconnect(oldAction, SIGNAL(progress(double,double)), this, SLOT(onActionProgress(double,double)));
		oldAction->cleanup();
	}
	return retVal;
}

void AMBeamlineParallelActionsList::start(){
	if(stageCount() > 0){
		isRunning_ = true;
		for(int x = 0; x < stage(0)->count(); x++)
			action(0,x)->start();
	}
}

void AMBeamlineParallelActionsList::cancel(){
	/*  NTBA March 14, 2011 David Chevrier
	if(isRunning_)
		for(int x = 0; x < stage(currentStage_)->count(); x++)
			action(currentStage_,x)
	*/
}

void AMBeamlineParallelActionsList::onDataChanged(QModelIndex a,QModelIndex b){
	Q_UNUSED(a);
	Q_UNUSED(b);
	if(a.row() != b.row())
		return;
	if(insertRowLatch_ != -1 && insertRowLatch_ == a.row()){
		insertRowLatch_ = -1;
		if(a.parent().isValid())
			emit actionAdded(a.parent().row(), a.row());
		else
			emit stageAdded(a.row());
	}
	else{
		if(a.parent().isValid())
			emit actionChanged(a.parent().row(), a.row());
		else
			emit stageChanged(a.row());
	}
}

void AMBeamlineParallelActionsList::onRowsInserted(QModelIndex parent, int start, int end){
	Q_UNUSED(parent);
	if( start != end )
		return;
	insertRowLatch_ = start;
}

void AMBeamlineParallelActionsList::onRowsRemoved(QModelIndex parent, int start, int end){
	Q_UNUSED(parent);
	if( start != end )
		return;
	if(parent.isValid())
		emit actionRemoved(parent.row(), start);
	else
		emit stageRemoved(start);
}

void AMBeamlineParallelActionsList::onActionStarted(){
	AMBeamlineActionItem *tmpItem = (AMBeamlineActionItem*)QObject::sender();
	QPair<int,int> indices = indicesOf(tmpItem);
	emit actionStarted(indices.first, indices.second);
	if(indices.second == 0)
		emit stageStarted(indices.first);
}

void AMBeamlineParallelActionsList::onActionSucceeded(){
	AMBeamlineActionItem *tmpItem = (AMBeamlineActionItem*)QObject::sender();
	QPair<int,int> indices = indicesOf(tmpItem);
	emit actionSucceeded(indices.first, indices.second);
}

void AMBeamlineParallelActionsList::onActionReady(bool ready){
	AMBeamlineActionItem *tmpItem = (AMBeamlineActionItem*)QObject::sender();
	QPair<int,int> indices = indicesOf(tmpItem);
	emit actionReady(indices.first, indices.second, ready);
}

void AMBeamlineParallelActionsList::onActionFailed(int explanation){
	AMBeamlineActionItem *tmpItem = (AMBeamlineActionItem*)QObject::sender();
	QPair<int,int> indices = indicesOf(tmpItem);
	emit actionFailed(indices.first, indices.second, explanation);
}

void AMBeamlineParallelActionsList::onActionProgress(double elapsed, double total){
	AMBeamlineActionItem *tmpAction;
	if(tmpAction = qobject_cast<AMBeamlineActionItem*>(QObject::sender())){
		QPair<int,int> indices = indicesOf(tmpAction);
		if(indices.first != currentStage_)
			return;
		lastIndexProgress_.replace(indices.second, (elapsed/total));
		double sum = 0;
		for(int x = 0; x < countAt(indices.first); x++)
			sum += lastIndexProgress_.at(x);
		emit stageProgress(sum/lastIndexProgress_.count(), 1.0);
	}
}

void AMBeamlineParallelActionsList::onStageStarted(int stageIndex){
	lastIndexProgress_.clear();
	currentStage_ = stageIndex;
	for(int x = 0; x < countAt(stageIndex); x++)
		lastIndexProgress_.append(0);
}

void AMBeamlineParallelActionsList::onStageSucceeded(){
	AMBeamlineParallelActionsListHolder *tmpHolder = (AMBeamlineParallelActionsListHolder*)QObject::sender();
	int stageIndex = indexOf(holdersHash_.valueR(tmpHolder));
	emit stageSucceeded(stageIndex);
	if(stageIndex == stageCount()-1){
		currentStage_ = -1;
		isRunning_ = false;
		emit listSucceeded();
	}
}

bool AMBeamlineParallelActionsList::setupModel(){
	actions_ = new AMBeamlineParallelActionListModel(this);
	if(actions_){
		connect(actions_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)) );
		connect(actions_, SIGNAL(rowsInserted(const QModelIndex,int,int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
		connect(actions_, SIGNAL(rowsRemoved(const QModelIndex,int,int)), this, SLOT(onRowsRemoved(QModelIndex,int,int)));
		return true;
	}
	return false;
}



AMBeamlineParallelActionsListHolder::AMBeamlineParallelActionsListHolder(QObject *parent) :
		QObject(parent)
{

}

void AMBeamlineParallelActionsListHolder::addAction(AMBeamlineActionItem *ai){
	if(!waitingOn_.contains(ai)){
		connect(ai, SIGNAL(succeeded()), this, SLOT(actionFinished()));
		waitingOn_.append(ai);
	}
}

void AMBeamlineParallelActionsListHolder::removeAction(AMBeamlineActionItem *ai){
	if(waitingOn_.contains(ai)){
		disconnect(ai, SIGNAL(succeeded()), this, SLOT(actionFinished()));
		waitingOn_.removeOne(ai);
	}
}

void AMBeamlineParallelActionsListHolder::actionFinished(){
	AMBeamlineActionItem *ai = (AMBeamlineActionItem*)QObject::sender();
	//waitingOn_.removeOne(ai);
	removeAction(ai);
	if(waitingOn_.isEmpty())
		emit everythingFinished();
}

AMBeamlineParallelActionListModel::AMBeamlineParallelActionListModel(QObject *parent) :
		QAbstractItemModel(parent)
{
	actions_ = new QList< QList<AMBeamlineActionItem*>* >();
	nextIndex_ = 0;
}

int AMBeamlineParallelActionListModel::rowCount(const QModelIndex &parent) const{
	if(parent.isValid() && parent.column() == 0){
		if(listHash_.containsF(parent.internalId()))
			return actions_->at(listHash_.valueF(parent.internalId()))->count();
		return 0;
	}
	return actions_->count();
}

int AMBeamlineParallelActionListModel::columnCount(const QModelIndex &parent) const{
	Q_UNUSED(parent);
	return 1;
}

QModelIndex AMBeamlineParallelActionListModel::index(int row, int column, const QModelIndex &parent) const{
	if(parent.isValid() && column == 0){
		if(listHash_.containsF(parent.internalId())){
			int listIndex = listHash_.valueF(parent.internalId());
			if(row < actions_->at(listIndex)->count())
				return createIndex(row, column, actionsHash_.valueR(QPair<int, int>(listIndex, row)));
		}
		return QModelIndex();
	}
	if(row < actions_->count() && column == 0)
		return createIndex(row, column, listHash_.valueR(row));
	return QModelIndex();
}

QModelIndex AMBeamlineParallelActionListModel::index(int row, const QModelIndex &parent){
	return index(row, 0, parent);
}

QModelIndex AMBeamlineParallelActionListModel::parent(const QModelIndex &child) const{
	if(!child.isValid())
		return QModelIndex();
	if(listHash_.containsF(child.internalId()))
		return QModelIndex();
	if(actionsHash_.containsF(child.internalId())){
		int listIndex = actionsHash_.valueF(child.internalId()).first;
		return createIndex(listIndex, 0, actions_->at(listIndex));
	}
	return QModelIndex();
}

QVariant AMBeamlineParallelActionListModel::data(const QModelIndex &index, int role) const{
	if(!index.isValid() || role != Qt::DisplayRole || index.column() != 0)
		return QVariant();
	if(listHash_.containsF(index.internalId()))
		return qVariantFromValue((void*)actions_->at(listHash_.valueF(index.internalId())));
	if(actionsHash_.containsF(index.internalId()))
		return qVariantFromValue((void*)actions_->at(actionsHash_.valueF(index.internalId()).first)->at(actionsHash_.valueF(index.internalId()).second));
	return QVariant();
}

QVariant AMBeamlineParallelActionListModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if(role != Qt::DisplayRole)
		return QVariant();

	if(orientation == Qt::Horizontal)
		return QString("Item %1").arg(section);
	else
		return QString("Action %1").arg(section);
}

bool AMBeamlineParallelActionListModel::setData(const QModelIndex &index, const QVariant &value, int role){
	if(!index.isValid() || index.column() != 0 || role != Qt::EditRole)
		return false;

	if(listHash_.containsF(index.internalId())){
		int topIndex = listHash_.valueF(index.internalId());
		int internalID;
		QList<AMBeamlineActionItem*> *tmpList, *oldList;
		oldList = actions_->at(topIndex);
		if(oldList){
			for(int x = oldList->count()-1; x >=0; x--){
				internalID = actionsHash_.valueR(QPair<int,int>(topIndex, x));
				actionsHash_.removeF(internalID);
			}
		}
		tmpList = (QList<AMBeamlineActionItem*>*) value.value<void*>();
		actions_->replace(topIndex, tmpList);
		for(int x = 0; x < tmpList->count(); x++){
			actionsHash_.set(nextIndex_, QPair<int,int>(topIndex, x));
			nextIndex_++;
		}
		emit dataChanged(index, index);
		return true;
	}
	if(actionsHash_.containsF(index.internalId())){
		int topIndex = actionsHash_.valueF(index.internalId()).first;
		int listIndex = actionsHash_.valueF(index.internalId()).second;
		AMBeamlineActionItem* actionItem;
		actionItem = (AMBeamlineActionItem*) value.value<void*>();
		actions_->at(topIndex)->replace(listIndex, actionItem);
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

bool AMBeamlineParallelActionListModel::insertRows(int row, int count, const QModelIndex &parent){
	if(!parent.isValid() && row >= 0 && row <= actions_->count() ){
		beginInsertRows(QModelIndex(), row, row+count+1);

		QHashIterator<int, QPair<int, int> > i = actionsHash_.iterator();
		int internalID;
		int listIndex;
		for(int x = listHash_.count()-1; x >= row; x--){
			internalID = listHash_.valueR(x);
			listHash_.removeF(internalID);
			listHash_.set(internalID, x+count);

			i.toFront();
			while(i.hasNext()) {
				i.next();
				if(i.value().first == x){
					internalID = i.key();
					listIndex = i.value().second;
					actionsHash_.removeF(internalID);
					actionsHash_.set(internalID, QPair<int,int>(x+count, listIndex));
				}
			}
		}
		QList<AMBeamlineActionItem*> *tmpList = NULL;
		for(int x = 0; x < count; x++){
			//the actionsList likes it better when it starts as NULL
			//tmpList = new QList<AMBeamlineActionItem*>();
			actions_->insert(row, tmpList);
			listHash_.set(nextIndex_, row+count-1-x);
			nextIndex_++;
		}
		endInsertRows();
		return true;
	}
	if(listHash_.containsF(parent.internalId()) && row >= 0 && row <= actions_->at(listHash_.valueF(parent.internalId()))->count()){
		beginInsertRows(QModelIndex(), row, row+count+1);

		int internalID;
		int topIndex = listHash_.valueF(parent.internalId());
		QList<AMBeamlineActionItem*> *tmpList = actions_->at(topIndex);
		if(tmpList->count() > 0){
			for(int x = tmpList->count()-1; x >= row; x--){
				internalID = actionsHash_.valueR(QPair<int,int>(topIndex, x));
				actionsHash_.removeF(internalID);
				actionsHash_.set(internalID, QPair<int,int>(topIndex, x+count));
			}
		}
		AMBeamlineActionItem *actionItem = NULL;
		for(int x = 0; x < count; x++){
			actions_->at(topIndex)->insert(row, actionItem);
			actionsHash_.set(nextIndex_, QPair<int,int>(topIndex, row+count-1-x));
			nextIndex_++;
		}
		endInsertRows();
		return true;
	}
	return false;
}

bool AMBeamlineParallelActionListModel::removeRows(int row, int count, const QModelIndex &parent){
	if( !parent.isValid() && (row >= 0) && (row+count < actions_->count()) ){
		beginRemoveRows(parent, row, row+count-1);
		int internalID;
		QList<AMBeamlineActionItem*> *tmpList;
		for(int y = 0; y < count; y++){
			tmpList= actions_->at(row+y);
			for(int x = tmpList->count()-1; x >= 0; x--){
				internalID = actionsHash_.valueR(QPair<int,int>(row+y, x));
				actionsHash_.removeF(internalID);
			}
			listHash_.removeR(row+y);
		}

		QHashIterator<int, QPair<int, int> > i = actionsHash_.iterator();
		int listIndex;
		for(int x = row+count; x < actions_->count(); x++){
			internalID = listHash_.valueR(x);
			listHash_.removeF(internalID);
			listHash_.set(internalID, x-count);

			i.toFront();
			while(i.hasNext()) {
				i.next();
				if(i.value().first == x){
					internalID = i.key();
					listIndex = i.value().second;
					actionsHash_.removeF(internalID);
					actionsHash_.set(internalID, QPair<int,int>(x-count, listIndex));
				}
			}
		}
		#warning "Hey David, who's responsible for deleting this list pointer and any item pointers it has?"
		for(int x = 0; x < count; x++)
			actions_->removeAt(row);
		endRemoveRows();
		return true;
	}
	if(listHash_.containsF(parent.internalId()) && (row >= 0) && (row+count <= actions_->at(listHash_.valueF(parent.internalId()))->count()) ){
		beginRemoveRows(parent, row, row+count-1);

		int internalID;
		int topIndex = listHash_.valueF(parent.internalId());
		for(int x = row; x < row+count; x++){
			internalID = actionsHash_.valueR(QPair<int,int>(topIndex, x));
			actionsHash_.removeF(internalID);
		}
		for(int x = row+count; x < actions_->at(topIndex)->count(); x++){
			internalID = actionsHash_.valueR(QPair<int,int>(topIndex, x));
			actionsHash_.removeF(internalID);
			actionsHash_.set(internalID, QPair<int,int>(topIndex, x-count));
		}
		#warning "Hey David, who's responsible for deleting these actionItem pointers?"
		for(int x = 0; x < count; x++)
			actions_->at(topIndex)->removeAt(row);
		endRemoveRows();
		return true;
	}
	return false;
}

