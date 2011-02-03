#include "AMBeamlineParallelActionsList.h"

/*
AMBeamlineParallelActionsList::AMBeamlineParallelActionsList()
{
}
*/


AMBeamlineParallelActionsListHolder::AMBeamlineParallelActionsListHolder(QObject *parent) :
		QObject(parent)
{

}

void AMBeamlineParallelActionsListHolder::addAction(AMBeamlineActionItem *ai){
	waitingOn_.append(ai);
}

void AMBeamlineParallelActionsListHolder::actionFinished(){
	AMBeamlineActionItem *ai = (AMBeamlineActionItem*)QObject::sender();
	waitingOn_.removeOne(ai);
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
		if(listHash_.containsF(parent.internalId())){
	//		qDebug() << "Count for a list";
			return actions_->at(listHash_.valueF(parent.internalId()))->count();
		}
	//	qDebug() << "Actions have no rows";
		return 0;
	}
	//qDebug() << "Count for number of lists";
	return actions_->count();
}

int AMBeamlineParallelActionListModel::columnCount(const QModelIndex &parent) const{
	return 1;
}

QModelIndex AMBeamlineParallelActionListModel::index(int row, int column, const QModelIndex &parent) const{
	if(parent.isValid() && column == 0){
		if(listHash_.containsF(parent.internalId())){
	//		qDebug() << "Parent ID is in the listHash, might be a list index";
			int listIndex = listHash_.valueF(parent.internalId());
			if(row < actions_->at(listIndex)->count()){
	//			qDebug() << "Valid row in a list [internal = " << actionsHash_.valueR(QPair<int, int>(listIndex, row)) << "]";
				return createIndex(row, column, actionsHash_.valueR(QPair<int, int>(listIndex, row)));
			}
		}
	//	qDebug() << "Valid but not a list must be an action, has no rows, empty index";
		return QModelIndex();
	}
	if(row < actions_->count() && column == 0){
	//	qDebug() << "Creating you an index for a list (" << listHash_.valueR(row) << ")";
		return createIndex(row, column, listHash_.valueR(row));
	}
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
	if(!index.isValid() || index.column() != 0)
		return false;

	if(listHash_.containsF(index.internalId())){
		int topIndex = listHash_.valueF(index.internalId());
		int internalID;
		QList<AMBeamlineActionItem*> *tmpList, *oldList;
		oldList = actions_->at(topIndex);
		for(int x = oldList->count()-1; x >=0; x--){
			internalID = actionsHash_.valueR(QPair<int,int>(topIndex, x));
			actionsHash_.removeF(internalID);
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
	//		qDebug() << "Need to worry about (list) ID " << listHash_.valueR(x) << " at current index " << x;
			listHash_.removeF(internalID);
			listHash_.set(internalID, x+count);
	//		qDebug() << "So I moved " << internalID << " to " << listHash_.valueF(internalID);

			i.toFront();
			while(i.hasNext()) {
				i.next();
				if(i.value().first == x){
					internalID = i.key();
					listIndex = i.value().second;
	//				qDebug() << "Need to worry about (action) ID " << i.key() << " at current index " << x << " with list index " << listIndex;
					actionsHash_.removeF(internalID);
					actionsHash_.set(internalID, QPair<int,int>(x+count, listIndex));
	//				qDebug() << "So I moved " << internalID << " to " << actionsHash_.valueF(internalID).first << " with list index " << actionsHash_.valueF(internalID).second;
				}
			}
		}
		QList<AMBeamlineActionItem*> *tmpList = NULL;
		for(int x = 0; x < count; x++){
			tmpList = new QList<AMBeamlineActionItem*>();
			actions_->insert(row, tmpList);
			listHash_.set(nextIndex_, row+count-1-x);
	//		qDebug() << "Inserted new list as ID " << nextIndex_ << " at index " << listHash_.valueF(nextIndex_);
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
		for(int x = tmpList->count()-1; x >= row; x--){
			internalID = actionsHash_.valueR(QPair<int,int>(topIndex, x));
	//		qDebug() << "Need to worry about (action) ID " << internalID << " at current index " << topIndex << " with list index " << x;
			actionsHash_.removeF(internalID);
			actionsHash_.set(internalID, QPair<int,int>(topIndex, x+count));
	//		qDebug() << "So I moved " << internalID << " to " << actionsHash_.valueF(internalID).first << " with list index " << actionsHash_.valueF(internalID).second;
		}
		AMBeamlineActionItem *actionItem;
		for(int x = 0; x < count; x++){
			actions_->at(topIndex)->insert(row, actionItem);
			actionsHash_.set(nextIndex_, QPair<int,int>(topIndex, row+count-1-x));
	//		qDebug() << "Inserted new action as ID " << nextIndex_ << " in list " << actionsHash_.valueF(nextIndex_).first << " at index " << actionsHash_.valueF(nextIndex_).second;
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
	//			qDebug() << "Just removed action with ID " << internalID << " in list " << row+y << " at index " << x;
			}
			listHash_.removeR(row+y);
	//		qDebug() << "Just removed list at index " << row+y;
		}

		QHashIterator<int, QPair<int, int> > i = actionsHash_.iterator();
		int listIndex;
		//for(int x = listHash_.count()-1; x >= row+count-1; x--){
		for(int x = row+count; x < actions_->count(); x++){
			internalID = listHash_.valueR(x);
	//		qDebug() << "Need to worry about (list) ID " << listHash_.valueR(x) << " at current index " << x;
			listHash_.removeF(internalID);
			listHash_.set(internalID, x-count);
	//		qDebug() << "So I moved " << internalID << " to " << listHash_.valueF(internalID);

			i.toFront();
			while(i.hasNext()) {
				i.next();
				if(i.value().first == x){
					internalID = i.key();
					listIndex = i.value().second;
	//				qDebug() << "Need to worry about (action) ID " << i.key() << " at current index " << x << " with list index " << listIndex;
					actionsHash_.removeF(internalID);
					actionsHash_.set(internalID, QPair<int,int>(x-count, listIndex));
	//				qDebug() << "So I moved " << internalID << " to " << actionsHash_.valueF(internalID).first << " with list index " << actionsHash_.valueF(internalID).second;
				}
			}
		}
		#warning "Hey David, who's responsible for deleting this list pointer and any item pointers it has?"
		for(int x = 0; x < count; x++)
			actions_->removeAt(row);
		endRemoveRows();
		return true;
	}
	if(listHash_.containsF(parent.internalId()) && (row >= 0) && (row+count < actions_->at(listHash_.valueF(parent.internalId()))->count()) ){
		beginRemoveRows(parent, row, row+count-1);

		int internalID;
		int topIndex = listHash_.valueF(parent.internalId());
		for(int x = row; x < row+count; x++){
			internalID = actionsHash_.valueR(QPair<int,int>(topIndex, x));
			actionsHash_.removeF(internalID);
	//		qDebug() << "Just removed action with ID " << internalID << " in list " << topIndex << " at index " << x;
		}
		for(int x = row+count; x < actions_->at(topIndex)->count(); x++){
			internalID = actionsHash_.valueR(QPair<int,int>(topIndex, x));
	//		qDebug() << "Need to worry about (action) ID " << internalID << " in list " << topIndex << " at current index " << x;
			actionsHash_.removeF(internalID);
			actionsHash_.set(internalID, QPair<int,int>(topIndex, x-count));
	//		qDebug() << "So I moved " << internalID << " to list " << actionsHash_.valueF(internalID).first << " at index " << actionsHash_.valueF(internalID).second;
		}
		#warning "Hey David, who's responsible for deleting these actionItem pointers?"
		for(int x = 0; x < count; x++)
			actions_->at(topIndex)->removeAt(row);
		endRemoveRows();
		return true;
	}
	return false;
}
