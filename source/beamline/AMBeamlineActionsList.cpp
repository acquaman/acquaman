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


#include "AMBeamlineActionsList.h"

AMBeamlineActionsList::AMBeamlineActionsList(QObject *parent) :
		QObject(parent)
{
	insertRowLatch_ = -1;
	actions_ = NULL;
	setupModel();
}

AMBeamlineActionListModel* AMBeamlineActionsList::model(){
	return actions_;
}

int AMBeamlineActionsList::count(){
	return actions_->rowCount(QModelIndex());
}


AMBeamlineActionItem* AMBeamlineActionsList::action(int index) const{
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

//HEY DAVE, CHECK THE ORDERING ON THIS, RETURN STATEMENT SEEMS ODD
bool AMBeamlineActionsList::setAction(int index, AMBeamlineActionItem *action){
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
	#warning "Hey David, what about disconnecting these in the 'replace' section?"
	connect(action, SIGNAL(started()), this, SLOT(onActionStarted()));
	connect(action, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
	connect(action, SIGNAL(ready(bool)), this, SLOT(onActionReady(bool)));
	connect(action, SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
	return actions_->setData(actions_->index(index, 0), qVariantFromValue((void*)action), Qt::EditRole);
}

bool AMBeamlineActionsList::addAction(int index, AMBeamlineActionItem *action){
	if(!actions_->insertRows(index, 1))
		return false;
	return setAction(index, action);
}

bool AMBeamlineActionsList::appendAction(AMBeamlineActionItem *action){
	return addAction(count(), action);
}

bool AMBeamlineActionsList::deleteAction(int index){
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
		disconnect(oldAction, SIGNAL(started()), this, SLOT(onActionStarted()));
		disconnect(oldAction, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
		disconnect(oldAction, SIGNAL(ready(bool)), this, SLOT(onActionReady(bool)));
		disconnect(oldAction, SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
		oldAction->cleanup();
	}
	return retVal;
}

void AMBeamlineActionsList::onDataChanged(QModelIndex a,QModelIndex b){
	Q_UNUSED(a);
	Q_UNUSED(b);
	if(a.row() != b.row())
		return;
	if(insertRowLatch_ != -1 && insertRowLatch_ == a.row()){
		insertRowLatch_ = -1;
		emit actionAdded(a.row());
	}
	else
		emit actionChanged(a.row());
}

void AMBeamlineActionsList::onRowsInserted(QModelIndex parent, int start, int end){
	Q_UNUSED(parent);
	if( start != end )
		return;
	insertRowLatch_ = start;
}

void AMBeamlineActionsList::onRowsRemoved(QModelIndex parent, int start, int end){
	Q_UNUSED(parent);
	if( start != end )
		return;
	emit actionRemoved(start);
}

void AMBeamlineActionsList::onActionStarted(){
	AMBeamlineActionItem *tmpItem = (AMBeamlineActionItem*)QObject::sender();
	emit actionStarted(indexOf(tmpItem));
}

void AMBeamlineActionsList::onActionSucceeded(){
	AMBeamlineActionItem *tmpItem = (AMBeamlineActionItem*)QObject::sender();
	emit actionSucceeded(indexOf(tmpItem));
}

void AMBeamlineActionsList::onActionReady(bool ready){
	AMBeamlineActionItem *tmpItem = (AMBeamlineActionItem*)QObject::sender();
	emit actionReady(indexOf(tmpItem), ready);
}

void AMBeamlineActionsList::onActionFailed(int explanation){
	AMBeamlineActionItem *tmpItem = (AMBeamlineActionItem*)QObject::sender();
	emit actionFailed(indexOf(tmpItem), explanation);
}

bool AMBeamlineActionsList::setupModel(){
	actions_ = new AMBeamlineActionListModel(this);
	if(actions_){
		connect(actions_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)) );
		connect(actions_, SIGNAL(rowsInserted(const QModelIndex,int,int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
		connect(actions_, SIGNAL(rowsRemoved(const QModelIndex,int,int)), this, SLOT(onRowsRemoved(QModelIndex,int,int)));
		return true;
	}
	return false;
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

AMBeamlineActionsQueue::AMBeamlineActionsQueue(AMBeamlineActionsList *fullList, QObject *parent) :
		QObject(parent)
{
	headIndex_ = -1;
	queueRunning_ = false;
	fullList_ = NULL;
	setBeamlineActionsList(fullList);
}

AMBeamlineActionItem* AMBeamlineActionsQueue::head(){
	if(headIndex_ == -1)
		return NULL;
	else
		return fullList_->action(headIndex_);
}

AMBeamlineActionItem* AMBeamlineActionsQueue::peek(){
	if(fullList_->count() == (headIndex_+1) )
		return NULL;
	else
		return fullList_->action(headIndex_+1);
}

int AMBeamlineActionsQueue::count(){
	// weird behaviour... seems to return 1 when empty otherwise...
	if(isEmpty())
		return 0;
	return fullList_->count()-indexOfHead();
}

int AMBeamlineActionsQueue::indexOfHead(){
	return headIndex_;
}

bool AMBeamlineActionsQueue::isEmpty(){
	if(headIndex_ == -1)
		return true;
	return false;
}

bool AMBeamlineActionsQueue::peekIsEmpty(){
	if(!peek())
		return true;
	return false;
}

bool AMBeamlineActionsQueue::isRunning(){
	return queueRunning_;
}

void AMBeamlineActionsQueue::setBeamlineActionsList(AMBeamlineActionsList *fullList){
	if(fullList_)
		disconnect(fullList_, SIGNAL(actionStarted(int)), this, SLOT(onActionStarted(int)));
		disconnect(fullList_, SIGNAL(actionSucceeded(int)), this, SLOT(onActionSucceeded(int)));
		disconnect(fullList_, SIGNAL(actionAdded(int)), this, SLOT(onActionAdded(int)));
	fullList_ = fullList;
	if(fullList_){
		connect(fullList_, SIGNAL(actionAdded(int)), this, SLOT(onActionAdded(int)));
		connect(fullList_, SIGNAL(actionRemoved(int)), this, SLOT(onActionRemoved(int)));
		connect(fullList_, SIGNAL(actionStarted(int)), this, SLOT(onActionStarted(int)));
		connect(fullList_, SIGNAL(actionSucceeded(int)), this, SLOT(onActionSucceeded(int)));
		connect(fullList_, SIGNAL(actionReady(int,bool)), this, SLOT(onActionReady(int)));
		connect(fullList_, SIGNAL(actionFailed(int,int)), this, SLOT(onActionFailed(int,int)));
		initializeQueue();
	}
}

void AMBeamlineActionsQueue::startQueue(){
	if(!isEmpty())
		head()->start();
}

void AMBeamlineActionsQueue::initializeQueue(){
	for(int x = 0; x < fullList_->count(); x++){
		if(fullList_->action(x)->hasStarted() && fullList_->action(x)->isRunning()){
			headIndex_ = x;
			return;
		}
		else if(!fullList_->action(x)->hasStarted()){
			headIndex_ = x;
			return;
		}
	}
}

void AMBeamlineActionsQueue::onActionAdded(int index){
	if(headIndex_ == -1){
		headIndex_ = index;
		emit headChanged();
		emit isEmptyChanged(false);
	}
}

void AMBeamlineActionsQueue::onActionRemoved(int index){
	if(index == headIndex_){
		if(headIndex_ == fullList_->count()){
			headIndex_ = -1;
			emit headChanged();
			emit isEmptyChanged(true);
		}
		else{
			emit headChanged();
			emit isEmptyChanged(false);
		}
	}
}

void AMBeamlineActionsQueue::onActionStarted(int index){
	queueRunning_ = true;
	emit isRunningChanged(true);
}

void AMBeamlineActionsQueue::onActionSucceeded(int index){
	if(fullList_->count() == index+1){
		headIndex_ = -1;
		queueRunning_ = false;
		emit isRunningChanged(false);
		emit isEmptyChanged(true);
	}
	else
		headIndex_ = index+1;
	emit headChanged();
}

void AMBeamlineActionsQueue::onActionReady(int index){

}

void AMBeamlineActionsQueue::onActionFailed(int index, int explanation){
	queueRunning_ = false;
	emit isRunningChanged(false);
}
