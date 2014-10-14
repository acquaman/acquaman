/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMActionHistoryModel.h"

#include <QPixmapCache>
#include <QStringBuilder>

#include "actions3/AMActionLog3.h"
#include "actions3/AMLoopAction3.h"

#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMDateTimeUtils.h"
#include "util/AMErrorMonitor.h"
#include "util/AMPointerTree.h"

// AMActionLogItem
////////////////////////////

 AMActionLogItem3::~AMActionLogItem3(){}
AMActionLogItem3::AMActionLogItem3(AMDatabase *db, int id)
{
	db_ = db;
	id_ = id;
	loadedFromDb_ = false;
	numberOfChildren_ = -1;
}

AMActionLogItem3::AMActionLogItem3(const AMActionLog3 &actionLog)
{
	db_ = actionLog.database();
	id_ = actionLog.id();
	numberOfChildren_ = -1;

	shortDescription_ = actionLog.info()->shortDescription();
	longDescription_  = actionLog.info()->longDescription();
	iconFileName_ = actionLog.iconFileName();
	startDateTime_ = actionLog.startDateTime();
	endDateTime_ = actionLog.endDateTime();
	finalState_ = actionLog.finalState();
	canCopy_ = actionLog.info()->canCopy();
	parentId_ = actionLog.parentId();
	failureMessage_ = actionLog.failureMessage();
	actionInheritedLoop_ = actionLog.actionInheritedLoop();
	// Only set number of loops from the database if this logAction inherited the loop action
	if(actionInheritedLoop_){
		const AMLoopActionInfo3 *loopInfo = qobject_cast<const AMLoopActionInfo3*>(actionLog.info());
		numberOfLoops_ = loopInfo->loopCount();
	}
	else
		numberOfLoops_ = -1;
	loadedFromDb_ = true;
}

QDateTime AMActionLogItem3::endDateTime() const
{
	if(!loadedFromDb_)
		loadLogDetailsFromDb();
	return endDateTime_;
}

QDateTime AMActionLogItem3::startDateTime() const
{
	if(!loadedFromDb_)
		loadLogDetailsFromDb();
	return startDateTime_;
}

QString AMActionLogItem3::shortDescription() const
{
	if(!loadedFromDb_)
		loadLogDetailsFromDb();
	return shortDescription_;
}

QString AMActionLogItem3::longDescription() const
{
	if(!loadedFromDb_)
		loadLogDetailsFromDb();
	return longDescription_;
}

QString AMActionLogItem3::failureMessage() const
{
	if(!loadedFromDb_)
		loadLogDetailsFromDb();
	return failureMessage_;
}

int AMActionLogItem3::finalState() const
{
	if(!loadedFromDb_)
		loadLogDetailsFromDb();
	return finalState_;
}

QString AMActionLogItem3::iconFileName() const
{
	if(!loadedFromDb_)
		loadLogDetailsFromDb();
	return iconFileName_;
}

bool AMActionLogItem3::canCopy() const
{
	if (!loadedFromDb_)
		loadLogDetailsFromDb();
	return canCopy_;
}

int AMActionLogItem3::parentId() const{
	if(!loadedFromDb_)
		loadLogDetailsFromDb();
	return parentId_;
}

bool AMActionLogItem3::actionInheritedLoop() const{
	if(!loadedFromDb_)
		loadLogDetailsFromDb();
	return actionInheritedLoop_;
}

int AMActionLogItem3::numberOfLoops() const{
	if(!loadedFromDb_)
		loadLogDetailsFromDb();
	return numberOfLoops_;
}

int AMActionLogItem3::numberOfChildren() const{
	return numberOfChildren_;
}

void AMActionLogItem3::updateNumberOfChildren(int numberOfChildren) const{
	numberOfChildren_ = numberOfChildren;
}

bool AMActionLogItem3::parentSelected(QAbstractItemView *viewer) const{
	if(parentSelected_.contains(viewer))
		return parentSelected_.value(viewer);
	return false;
}

ParentSelectMap AMActionLogItem3::allParentSelected() const{
	return parentSelected_;
}

void AMActionLogItem3::setParentSelected(QAbstractItemView *viewer, bool parentIsSelected){
	parentSelected_.insert(viewer, parentIsSelected);
}

bool AMActionLogItem3::loadLogDetailsFromDb() const
{
	if(!db_)
		return false;
	if(id_ < 1)
		return false;

	QStringList columns;
	columns << "name" << "longDescription" << "iconFileName" << "startDateTime" << "endDateTime" << "finalState" << "info" << "parentId" << "failureMessage" << "actionInheritedLoop";
	QVariantList values = db_->retrieve(id_, AMDbObjectSupport::s()->tableNameForClass<AMActionLog3>(), columns);
	if(values.isEmpty())
		return false;

	shortDescription_ = values.at(0).toString();
	longDescription_  = values.at(1).toString();
	iconFileName_ = values.at(2).toString();
	startDateTime_ = QDateTime::fromString(values.at(3).toString(), "yyyy-MM-ddThh:mm:ss.zzz");
	endDateTime_ = QDateTime::fromString(values.at(4).toString(), "yyyy-MM-ddThh:mm:ss.zzz");
	finalState_ = values.at(5).toInt();
	if(values.at(6).toString().contains("|$^$|"))
		canCopy_ = db_->retrieve(values.at(6).toString().section(';', -1).toInt(), values.at(6).toString().split(';').first().split("|$^$|").last(), "canCopy").toBool();
	else
		canCopy_ = db_->retrieve(values.at(6).toString().section(';', -1).toInt(), values.at(6).toString().split(';').first(), "canCopy").toBool();
	parentId_ = values.at(7).toInt();
	failureMessage_ = values.at(8).toString();
	actionInheritedLoop_ = values.at(9).toBool();
	// Only set number of loops from the database if this logAction inherited the loop action
	if(actionInheritedLoop_)
		numberOfLoops_ = db_->retrieve(values.at(6).toString().section(';', -1).toInt(), values.at(6).toString().split(';').first(), "loopCount").toInt();
	else
		numberOfLoops_ = -1;

	loadedFromDb_ = true;

	return true;
}

// AMActionHistoryModel
////////////////////////////

AMActionHistoryModel3::AMActionHistoryModel3(AMDatabase *db, QObject *parent) : QAbstractItemModel(parent)
{
	db_ = db;
	actionLogTableName_ = AMDbObjectSupport::s()->tableNameForClass<AMActionLog3>();
	visibleActionsCount_ = 0;
	maximumActionsLimit_ = 100;

	connect(&refreshFunctionCall_, SIGNAL(executed()), this, SLOT(refreshFromDb()));
	connect(&specificRefreshFunctionCall_, SIGNAL(executed()), this, SLOT(refreshSpecificIds()));

	itemTreeRoot_ = new AMPointerTreeNode(0, 0, this);
	itemTree_ = new AMPointerTree(itemTreeRoot_, this);

	succeededIcon_ = QPixmap(":/22x22/greenCheck.png");
	cancelledIcon_ = QPixmap(":/22x22/orangeX.png");
	failedIcon_ = QPixmap(":/22x22/redCrash.png");
	unknownIcon_ = QPixmap(":/22x22/dialog-question.png");
}

AMActionHistoryModel3::~AMActionHistoryModel3() {
	clear();
}

int AMActionHistoryModel3::nextGoodMaximumActions() const{
	int nextGoodMaximum = -1;
	int indexer = topLevelIds_.count()-1;
	while(nextGoodMaximum == -1 && indexer >= 0){
		if(visibleActionsCount_-topLevelIds_.at(indexer) > maximumActionsLimit_)
			nextGoodMaximum = visibleActionsCount_-topLevelIds_.at(indexer);
		else
			indexer--;
	}

	if( nextGoodMaximum < ((4*maximumActionsLimit_)/3) )
		nextGoodMaximum = ((4*maximumActionsLimit_)/3);

	if(nextGoodMaximum == -1)
		nextGoodMaximum = visibleActionsCount_-1;

	return nextGoodMaximum+1;
}

QModelIndex AMActionHistoryModel3::index(int row, int column, const QModelIndex &parent) const
{
	// Return bad index if it's outside the column range
	if(column < 0 || column > 4)
		return QModelIndex();

	AMPointerTreeNode *parentNode;
	if(!parent.isValid())
		parentNode = itemTree_->rootNode();
	else
		parentNode = static_cast<AMPointerTreeNode*>(parent.internalPointer());

	if(!parentNode || row < 0 || row >= parentNode->childCount()){
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_MODELINDEX_OUT_OF_BOUNDS, "The action history attempted to access information that was entirely out of bounds. Please report this problem to the Acquaman developers ");
		return QModelIndex();
	}
	return createIndex(row, column, (void*)(parentNode->childNodeAt(row)));
}

QModelIndex AMActionHistoryModel3::parent(const QModelIndex &child) const
{
	if(!child.isValid())
		return QModelIndex();

	AMPointerTreeNode *childNode = static_cast<AMPointerTreeNode*>(child.internalPointer());
	if(!childNode || !childNode->parentNode() || (childNode->parentNode() == itemTree_->rootNode()) )
		return QModelIndex();

	const AMPointerTreeNode *parentNode = childNode->parentNode();
	if(!parentNode || !parentNode->parentNode())
		return QModelIndex();
	const AMPointerTreeNode *grandparentNode = parentNode->parentNode();

	for(int x = 0; x < grandparentNode->childCount(); x++){
		if(grandparentNode->childNodeAt(x) == parentNode)
			return createIndex(x, 0, (void*)(parentNode));
	}
	return QModelIndex();
}

int AMActionHistoryModel3::rowCount(const QModelIndex &parent) const
{
	AMPointerTreeNode *parentNode;
	if(!parent.isValid())
		parentNode = itemTree_->rootNode();
	else
		parentNode = static_cast<AMPointerTreeNode*>(parent.internalPointer());

	if(!parentNode){
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_ROWCOUNT_BAD_PARENT_ITEM, "The action history attempted to access a rowCount with a bad parent item. Please report this problem to the Acquaman developers ");
		return 0;
	}

	return parentNode->childCount();
}

int AMActionHistoryModel3::columnCount(const QModelIndex &parent) const
{
	if(!parent.isValid())
		return 5;
	else
		return 0;
}

QVariant AMActionHistoryModel3::data(const QModelIndex &index, int role) const
{
	AMPointerTreeNode *indexNode = static_cast<AMPointerTreeNode*>(index.internalPointer());
	if(!indexNode){
		//NEM
		return QVariant();
	}
	AMActionLogItem3 *item = static_cast<AMActionLogItem3*>(indexNode->item());
	if(!item){
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_MODELDATA_BAD_ITEM, QString("The action history attempted to access data with a bad item (row: %1 column: %2). Please report this problem to the Acquaman developers ").arg(index.row()).arg(index.column()));
		return QVariant();
	}

	if(role == Qt::DisplayRole) {
		switch(index.column()) {
		case 0: return item->shortDescription();
		case 1: return QVariant();
		case 2: return item->failureMessage();
		case 3: return AMDateTimeUtils::prettyDateTime(item->endDateTime());
		case 4: return AMDateTimeUtils::prettyDuration(item->startDateTime(), item->endDateTime());
		}
	}
	else if(role == Qt::DecorationRole) {
		// column 0: return the action icon.
		if(index.column() == 0) {
			QString iconFileName = item->iconFileName();
			if(iconFileName.isEmpty())
				iconFileName = ":/64x64/generic-action.png";
			QPixmap p;
			if(QPixmapCache::find("AMActionLogItemIcon" % iconFileName, &p))
				return p;
			else {
				p.load(iconFileName);
				p = p.scaledToHeight(22, Qt::SmoothTransformation);
				QPixmapCache::insert("AMActionLogItemIcon" % iconFileName, p);
				return p;
			}
		}
		// column 1: return the status icon
		else if(index.column() == 1) {
			switch(item->finalState()) {
			case AMAction3::Succeeded: return succeededIcon_;
			case AMAction3::Cancelled: return cancelledIcon_;
			case AMAction3::Failed: return failedIcon_;
			default: return unknownIcon_;
			}
		}
		else
			return QVariant();
	}
	else if(role == Qt::SizeHintRole) {
		return QSize(-1, 32);
	}
	else if(role == Qt::ToolTipRole) {
		if(index.column() == 0) {

			// In case the item didn't (or isn't yet) in some sort of finished state
			if(item && item->canCopy() && !(item->finalState() == 7 || item->finalState() == 8 || item->finalState() == 9) )
				return "Cannot select this item, it has not yet completed";
			//In case a loop or list had no logged actions in it
			if(item && item->canCopy() && item->actionInheritedLoop() && (childrenCount(index) == 0) )
				return "Cannot select this item, it has no children.";
			//In case a loop or list had no logged actions in it
			if(item && item->canCopy() && item->actionInheritedLoop() && (successfulChildrenCount(index) == 0) )
				return "Cannot select this item, none of its children succeeded.";

			return item->longDescription();
		}
		else if(index.column() == 1) {
			switch(item->finalState()) {
			case AMAction3::Succeeded: return "Succeeded";
			case AMAction3::Cancelled: return "Cancelled";
			case AMAction3::Failed:
				if(item->failureMessage().isEmpty())
					return "Failed";
				else
					return QString("Failed with message: %1").arg(item->failureMessage());
			default: return "[?]";
			}
		}
		else if(index.column() == 2)
			return QString(item->failureMessage());
	}
	else if(role == Qt::BackgroundRole) {
		switch(item->finalState()) {
		case AMAction3::Succeeded:
			return QColor(126, 255, 106);// light green
		case AMAction3::Cancelled:
			return QColor(255, 176, 106);// light orange
		case AMAction3::Failed:
			return QColor(255, 104, 106);// light red
		default:
			return QVariant();
		}
	}
	else if(role == AMActionHistoryModel3::ParentSelectRole) {
		ParentSelectMap selectMap;
		ParentSelectMap::const_iterator i = item->allParentSelected().constBegin();
		while (i != item->allParentSelected().constEnd()) {
			selectMap.insert(i.key(), i.value());
			++i;
		}
		return QVariant::fromValue(selectMap);
	}

	return QVariant();
}

Qt::ItemFlags AMActionHistoryModel3::flags(const QModelIndex &index) const
{
	if(!index.isValid())
		return Qt::NoItemFlags;
	if( index.column() != 0)
		return Qt::ItemIsEnabled;
	AMPointerTreeNode *indexNode = static_cast<AMPointerTreeNode*>(index.internalPointer());
	AMActionLogItem3 *item = 0;
	if(indexNode){
		//NEM
		item = static_cast<AMActionLogItem3*>(indexNode->item());
	}

	// In case the item didn't (or isn't yet) in some sort of finished state
	if(item && item->canCopy() && !(item->finalState() == 7 || item->finalState() == 8 || item->finalState() == 9) )
		return Qt::ItemIsEnabled;
	//In case a loop or list had no logged actions in it
	if(item && item->canCopy() && item->actionInheritedLoop() && (childrenCount(index) == 0) )
		return Qt::ItemIsEnabled;
	//In case a loop or list had no logged actions in it
	if(item && item->canCopy() && item->actionInheritedLoop() && (successfulChildrenCount(index) == 0) )
		return Qt::ItemIsEnabled;
	if (item && item->canCopy())
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

QVariant AMActionHistoryModel3::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		switch(section) {
		case 0: return QString("Action");
		case 1: return QString("Status");
		case 2: return QString("Message");
		case 3: return QString("Finished");
		case 4: return QString("Duration");
		default: return QVariant();
		}
	}
	return QVariant();
}

bool AMActionHistoryModel3::hasChildren(const QModelIndex &parent) const{
	if(!parent.isValid())
		return true; // top level must have children
	else{
		AMPointerTreeNode *parentNode = static_cast<AMPointerTreeNode*>(parent.internalPointer());
		if(!parentNode){
			AMErrorMon::alert(this, AMACTIONHISTORYMODEL_HASCHILDREN_BAD_PARENT_ITEM, "The action history attempted to check hasChildren on a bad parent item. Please report this problem to the Acquaman developers ");
			return false;
		}
		if(parentNode->childCount() == 0)
			return false;
		return true;
	}
}

int AMActionHistoryModel3::childrenCount(const QModelIndex &parent) const{
	AMPointerTreeNode *parentNode;
	if(!parent.isValid())
		parentNode = itemTree_->rootNode();
	else
		parentNode = static_cast<AMPointerTreeNode*>(parent.internalPointer());

	if(!parentNode){
		//NEM
		return 0;
	}

	return parentNode->descendantCount();
}

int AMActionHistoryModel3::successfulChildrenCount(const QModelIndex &parent) const{
	if( rowCount() == 0 )
		return 0;
	int successfulSubChildCount = 0;
	int successfulChildCount = 0;
	for(int x = 0; x < rowCount(parent); x++){
		if(hasChildren(index(x, 0, parent)))
			successfulSubChildCount += successfulChildrenCount(index(x, 0, parent));
		AMActionLogItem3 *childLogItem = logItem(index(x, 0, parent));
		if(childLogItem->finalState() == 8)
			successfulChildCount++;
	}
	return successfulChildCount + successfulSubChildCount;
}

AMActionLogItem3 * AMActionHistoryModel3::logItem(const QModelIndex &index) const
{
	if(!index.isValid())
		return 0;
	AMPointerTreeNode *indexNode = static_cast<AMPointerTreeNode*>(index.internalPointer());
	if(!indexNode)
		return 0;

	return static_cast<AMActionLogItem3*>(indexNode->item());
}

QModelIndex AMActionHistoryModel3::indexForNode(const AMPointerTreeNode *node) const{
	if(!node){
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_INDEXFORNODE_NODE_NOT_IN_TREE, "The action history attempted to find the index for an node not in the tree. Please report this problem to the Acquaman developers ");
		return QModelIndex();
	}
	if(!node->parentNode())
		return QModelIndex();

	for(int x = 0; x < node->parentNode()->childCount(); x++)
		if(node->parentNode()->childNodeAt(x) == node)
			return createIndex(x, 0, (void*)(node));

	return QModelIndex();
}

QModelIndex AMActionHistoryModel3::indexForLogItem(AMActionLogItem3 *logItem) const{
	if(!logItem)
		return QModelIndex();

	const AMPointerTreeNode *relatedNode = itemTree_->nodeFromItem(logItem);
	if(!relatedNode){
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_INDEXFORLOGITEM_ITEM_NOT_IN_TREE, "The action history attempted to find the index for an item not in the tree. Please report this problem to the Acquaman developers ");
		return QModelIndex();
	}

	return indexForNode(relatedNode);
}

QModelIndex AMActionHistoryModel3::topLevelParent(const QModelIndex &child) const{
	if(!child.isValid())
		return QModelIndex();

	if(!child.parent().isValid())
		return child;

	return topLevelParent(child.parent());
}

const QItemSelection AMActionHistoryModel3::indicesBetween(const QModelIndex &brother, const QModelIndex &sister) const{
	QItemSelection selected;
	// Doesn't make sense if they don't share the same parent
	if(brother.parent() != sister.parent())
		return selected;
	QItemSelectionRange selectedRange;
	// Organize in the correct order
	if(brother.row() >= sister.row())
		selectedRange = QItemSelectionRange(sister, brother);
	else
		selectedRange = QItemSelectionRange(brother, sister);
	selected.append(selectedRange);
	return selected;
}

void AMActionHistoryModel3::markIndexAsSelected(const QModelIndex &index, QAbstractItemView *viewer){
	// For all the child index start the recursive procedure to make parentSelected (that function will emit dataChanged for their children's indices)
	for(int x = 0; x < rowCount(index); x++)
		recurseMarkParentSelected(index.child(x, 0), viewer, true);
	// Emit dataChanged for these top-level indices
	emit dataChanged(index, index);
}

void AMActionHistoryModel3::markIndexAsDeselected(const QModelIndex &index, QAbstractItemView *viewer){
	// For all the child index start the recursive procedure to make parentSelected (that function will emit dataChanged for their children's indices)
	for(int x = 0; x < rowCount(index); x++)
		recurseMarkParentSelected(index.child(x, 0), viewer, false);
	// Emit dataChanged for these top-level indices
	emit dataChanged(index, index);
}

void AMActionHistoryModel3::markIndexGroupAsSelected(const QModelIndex &index, QAbstractItemView *viewer){
	// Call the helper function with selected=true
	markIndexGroup(index, viewer, true);
}

void AMActionHistoryModel3::markIndexGroupAsDeselected(const QModelIndex &index, QAbstractItemView *viewer){
	// Call the helper function with selected=false
	markIndexGroup(index, viewer, false);
}

void AMActionHistoryModel3::setMaximumActionsToDisplay(int maximumActionsCount)
{
	if(maximumActionsLimit_ == maximumActionsCount)
		return;

	maximumActionsLimit_ = maximumActionsCount;
	refreshFunctionCall_.schedule();
	/// \todo Optimization: instead of a full refresh, we could just load the additional ones we need, by specifying a LIMIT and OFFSET in the SQL query.
}

void AMActionHistoryModel3::refreshFromDb()
{
	emit modelAboutToBeRefreshed();

	// clear the model:
	clear();

	if(!db_)
		return;

	// Get all of the ids of the AMActionLogs to display. Need to order in descending (most recent first) so we get the right limit.
	QList<int> ids;
	QList<int> parentIds;
	QSqlQuery q;
	QSqlQuery q2;	// used to count the total number in the visible range, if we didn't have a limit.
	QSqlQuery q3;	// used to find the "good" places for show more actions operations

	q = db_->select(actionLogTableName_,
			"id,parentId",
			"1 ORDER BY startDateTime DESC LIMIT ?");
	q.bindValue(0, maximumActionsLimit_);
	q2 = db_->select(actionLogTableName_,
			 "COUNT(1)");
	q3 = db_->select(actionLogTableName_, "id", "parentId=-1");

	// run the query and get the ids:
	if(!q.exec())
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_FAILED_TO_EXECUTE_DB_QUERY, "Could not execute the query to refresh the action history. Please report this problem to the Acquaman developers." % q.lastError().text());
	while(q.next()){
		ids << q.value(0).toInt();
		parentIds << q.value(1).toInt();
	}
	q.finish();

	// get the total count:
	if(q2.exec() && q2.first()) {
		visibleActionsCount_ = q2.value(0).toInt();
	}
	else {
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_FAILED_TO_EXECUTE_DB_QUERY_NUMBER_OF_ITEMS, "Could not execute the query to determine the number of items in the action history. Please report this problem to the Acquaman developers." % q.lastError().text());
		visibleActionsCount_ = -1;	// you should never see this
	}
	q2.finish();

	topLevelIds_.clear();
	if(!q3.exec())
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_FAILED_TO_EXECUTE_DB_QUERY_ALL_TOP_LEVELS, "Could not execute the query to determine the top level items in the action history. Please report this problem to the Acquaman developers." % q.lastError().text());
	while(q3.next()){
		topLevelIds_ << q3.value(0).toInt();
	}
	q3.finish();

	bool prunedLists = false;
	while(!prunedLists && (ids.count() > 0) ){
		if(parentIds.last() != -1){
			parentIds.removeLast();
			ids.removeLast();
		}
		else
			prunedLists = true;
	}

	if(!ids.isEmpty()) {
		// switch order
		QMap<int, int> parentIdsAndIdsAscending;
		for(int i=ids.count()-1; i>=0; i--)
			parentIdsAndIdsAscending.insertMulti(parentIds.at(i), ids.at(i));

		if(!recurseActionsLogLevelCreate(-1, parentIdsAndIdsAscending))
			AMErrorMon::alert(this, AMACTIONHISTORYMODEL_REFRESHFROMDB_FAILED_TO_CREATE_TREE, "The action history failed to generate its internal tree. Please report this problem to the Acquaman developers ");
	}

	emit modelRefreshed();
}

#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"
bool AMActionHistoryModel3::logUncompletedAction(const AMAction3 *uncompletedAction, AMDatabase *database, int parentLogId){
	if(uncompletedAction && !uncompletedAction->inFinalState()){

		if(database == AMDatabase::database("scanActions") && AMActionRunner3::scanActionRunner()->cachedLogCount() > 200){
			database->commitTransaction();
			AMActionRunner3::scanActionRunner()->resetCachedLogCount();
		}
		if(database == AMDatabase::database("scanActions")){
			if(!database->transactionInProgress())
				database->startTransaction();
			AMActionRunner3::scanActionRunner()->incrementCachedLogCount();
		}

		/*
		AMActionLog3 actionLog(uncompletedAction);
		actionLog.setParentId(parentLogId);
		bool success = actionLog.storeToDb(database);
		*/
		AMActionLog3 *actionLog = new AMActionLog3(uncompletedAction);
		infosToLogsForUncompletedActions_.insert(uncompletedAction->info(), actionLog);
		actionLog->setParentId(parentLogId);
		bool success = actionLog->storeToDb(database);
		const AMListAction3 *listAction = qobject_cast<const AMListAction3*>(uncompletedAction);
		if(success && listAction){
			AMListAction3 *modifyListAction = const_cast<AMListAction3*>(listAction);
			modifyListAction->setLogActionId(actionLog->id());
		}

		AMActionLogItem3* item = new AMActionLogItem3(*actionLog);
		emit modelAboutToBeRefreshed();
		/// \todo Ordering... This may end up at the wrong spot until a full refresh is done.  Most of the time, any actions added will be the most recent ones, however that is not guaranteed.
		appendItem(item);
		visibleActionsCount_++;
		emit modelRefreshed();

		return success;
	}
	return false;
}

bool AMActionHistoryModel3::updateCompletedAction(const AMAction3 *completedAction, AMDatabase *database){
	if(completedAction && completedAction->inFinalState()) {
		int infoId = completedAction->info()->id();
		if(infoId < 1){
			AMErrorMon::alert(0, AMACTIONLOG_CANNOT_UPDATE_UNSAVED_ACTIONLOG, "The actions logging system attempted to update a log action that hadn't already been saved. Please report this problem to the Acquaman developers.");
			return false;
		}

		if(database == AMDatabase::database("scanActions") && AMActionRunner3::scanActionRunner()->cachedLogCount() > 200){
			database->commitTransaction();
			AMActionRunner3::scanActionRunner()->resetCachedLogCount();
		}
		if(database == AMDatabase::database("scanActions")){
			if(!database->transactionInProgress())
				database->startTransaction();
			AMActionRunner3::scanActionRunner()->incrementCachedLogCount();
		}

		AMActionLog3 *actionLog = infosToLogsForUncompletedActions_.value(completedAction->info());
		infosToLogsForUncompletedActions_.remove(completedAction->info());
		actionLog->setFromAction(completedAction);
		if(actionLog->storeToDb(database)){
			// OK, this is a specific update.
			idsRequiringRefresh_ << actionLog->id();
			specificRefreshFunctionCall_.schedule();
			actionLog->deleteLater();
			return true;
		}

		actionLog->deleteLater();
		return false;
	}
	else {
		AMErrorMon::alert(0, AMACTIONLOG_CANNOT_UPDATE_UNCOMPLETED_ACTION, QString("The actions logging system attempted to update a log action that hadn't yet finished running. Please report this problem to the Acquaman developers."));
		return false;
	}
}

bool AMActionHistoryModel3::logCompletedAction(const AMAction3 *completedAction, AMDatabase *database, int parentLogId){
	if(completedAction && completedAction->inFinalState()) {

		if(database == AMDatabase::database("scanActions") && AMActionRunner3::scanActionRunner()->cachedLogCount() > 200){
			database->commitTransaction();
			AMActionRunner3::scanActionRunner()->resetCachedLogCount();
		}
		if(database == AMDatabase::database("scanActions")){
			if(!database->transactionInProgress())
				database->startTransaction();
			AMActionRunner3::scanActionRunner()->incrementCachedLogCount();
		}

		AMActionLog3 actionLog(completedAction);
		actionLog.setParentId(parentLogId);
		if(actionLog.storeToDb(database)){
			AMActionLogItem3* item = new AMActionLogItem3(actionLog);
			emit modelAboutToBeRefreshed();
			/// \todo Ordering... This may end up at the wrong spot until a full refresh is done.  Most of the time, any actions added will be the most recent ones, however that is not guaranteed.
			appendItem(item);
			visibleActionsCount_++;
			emit modelRefreshed();

			return true;
		}
		return false;
	}
	else {
		return false;
	}
}

void AMActionHistoryModel3::refreshSpecificIds()
{
	QSet<int>::const_iterator i = idsRequiringRefresh_.constBegin();
	while (i != idsRequiringRefresh_.constEnd()) {
		AMPointerTreeNode *oneNode = idsToTreeNodes_.value(*i);
		if(oneNode){
			AMActionLogItem3 *oneItem = static_cast<AMActionLogItem3*>(oneNode->item());
			if(oneItem){
				oneItem->refresh();
				QModelIndex changedIndexFirst = indexForLogItem(oneItem);
				QModelIndex changedIndexLast = indexForLogItem(oneItem).sibling(changedIndexFirst.row(), 2);
				emit dataChanged(changedIndexFirst, changedIndexLast);
			}
		}
		++i;
	}

	idsRequiringRefresh_.clear();
}

void AMActionHistoryModel3::appendItem(AMActionLogItem3 *item)
{
	int parentId = item->parentId();
	AMPointerTreeNode *parentNode;
	QModelIndex parentIndex;
	if(parentId == -1){
		parentNode = itemTree_->rootNode();
		parentIndex = QModelIndex();
	}
	else{
		parentNode= idsToTreeNodes_.value(parentId);
		parentIndex = indexForNode(parentNode);
	}

	beginInsertRows(parentIndex, parentNode->childCount(), parentNode->childCount());
	AMPointerTreeNode *pointerTreeNode = new AMPointerTreeNode(item, parentNode);
	idsToTreeNodes_.insert(item->id(), pointerTreeNode);
	parentNode->appendChildNode(pointerTreeNode);
	endInsertRows();
}

void AMActionHistoryModel3::clear()
{
	if(itemTree_->rootNode()->childCount() == 0)
		return;

	beginResetModel();
	QList<void*> itemsToDelete = itemTree_->clearTree();
	while(!itemsToDelete.isEmpty()){
		AMActionLogItem3 * oneItem = static_cast<AMActionLogItem3*>(itemsToDelete.takeLast());
		delete oneItem;
	}

	endResetModel();
}

bool AMActionHistoryModel3::recurseActionsLogLevelCreate(int parentId, QMap<int, int> parentIdsAndIds){
	QList<int> childIdsInReverse = parentIdsAndIds.values(parentId);

	// No more recursion if there are no children (base case)
	if(childIdsInReverse.count() == 0)
		return true;

	AMPointerTreeNode *parentNode;
	if(parentId == -1)
		parentNode = itemTree_->rootNode();
	else
		parentNode = idsToTreeNodes_.value(parentId);

	for(int x = childIdsInReverse.count()-1; x >= 0; x--){
		AMActionLogItem3 *actionLogItem = new AMActionLogItem3(db_, childIdsInReverse.at(x));
		AMPointerTreeNode *pointerTreeNode = new AMPointerTreeNode(actionLogItem, parentNode);
		idsToTreeNodes_.insert(childIdsInReverse.at(x), pointerTreeNode);
		parentNode->appendChildNode(pointerTreeNode);
	}

	// Ask to place the children for each child in order
	bool success = true;
	for(int x = childIdsInReverse.count()-1; x >= 0; x--)
		success &= recurseActionsLogLevelCreate(childIdsInReverse.at(x), parentIdsAndIds);
	return success;
}

void AMActionHistoryModel3::recurseMarkParentSelected(const QModelIndex &index, QAbstractItemView *viewer, bool selected){
	// If a child was selected (actually selected) before the parent, unselect the child in the selection model ... later steps will automatically set the parentSelected mapping
	if(index.parent().isValid() && viewer->selectionModel()->isSelected(index))
		viewer->selectionModel()->select(index, QItemSelectionModel::Deselect);

	// Set this index parentSelected mapping and then recurse through the children if there are any
	int childrenCount = rowCount(index);
	AMActionLogItem3 *item = logItem(index);
	if(item)
		item->setParentSelected(viewer, selected);
	// No children is base case
	if(childrenCount == 0)
		return;
	for(int x = 0; x < childrenCount; x++)
		recurseMarkParentSelected(this->index(x, 0, index), viewer, selected);

	// Emit dataChanged for the children you just marked
	emit dataChanged(this->index(0, 0, index), this->index(childrenCount-1, 0, index));
}

void AMActionHistoryModel3::markIndexGroup(const QModelIndex &index, QAbstractItemView *viewer, bool selected){
	// Just do the regular markParentSelected if this is a top-level action log item (parent index is invalid)
	if(!index.parent().isValid()){
		recurseMarkParentSelected(index, viewer, selected);
		return;
	}

	// Check if the action log item inherited the loop action (if so, then we need to multi-select/deselect the children)
	AMActionLogItem3 *item = logItem(index.parent());
	if(item->actionInheritedLoop()){
		// Figure out your index in the original loop, and select/deselect all of your siblings with the same loop index
		int actionsPerLoop = rowCount(index.parent())/item->numberOfLoops();
		int indexInLoop = (index.row()%actionsPerLoop);
		QModelIndex markingIndex;
		for(int x = 0; x < rowCount(index.parent()); x++){
			if(x%actionsPerLoop == indexInLoop){
				markingIndex = this->index(x, 0, index.parent());
				recurseMarkParentSelected(markingIndex, viewer, selected);
				emit dataChanged(markingIndex, markingIndex);
			}
		}
	}
	else
		recurseMarkParentSelected(index, viewer, selected);

	// Check to see if there are any children left at this level. If not, then deselect the parent as well (no point selecting a list or loop when the user has removed all of the contents)
	int siblingsSelected = 0;
	for(int x = 0; x < rowCount(index.parent()); x++){
		ParentSelectMap selectMap = data(this->index(x, 0, index.parent()), AMActionHistoryModel3::ParentSelectRole).value<ParentSelectMap>();
		if(selectMap.contains(viewer) && selectMap.value(viewer))
			siblingsSelected++;
	}

	if(siblingsSelected == 0){
		// Actually deselect in the selection model if the parent was actually selected
		if(viewer->selectionModel()->isSelected(index.parent())){
			QItemSelectionModel::SelectionFlags deselectFlag = QItemSelectionModel::Deselect;
			viewer->selectionModel()->select(index.parent(), deselectFlag);
			ParentSelectMap selectMap = data(index.parent(), AMActionHistoryModel3::ParentSelectRole).value<ParentSelectMap>();
			if(selectMap.contains(viewer)){
				AMActionLogItem3 *item = logItem(index.parent());
				// Make sure it doesn't get parentSelected somehow
				item->setParentSelected(viewer, false);
			}
			emit dataChanged(index.parent(), index.parent());
		}
		// Otherwise just use the group deselect for the parentSelected mapping
		else
			markIndexGroupAsDeselected(index.parent(), viewer);
	}
}
