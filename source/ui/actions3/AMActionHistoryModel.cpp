#include "AMActionHistoryModel.h"

#include <QPixmapCache>
#include <QStringBuilder>
#include <QDebug>

#include "actions3/AMActionLog3.h"

#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMDateTimeUtils.h"
#include "util/AMErrorMonitor.h"

// AMActionLogItem
////////////////////////////

AMActionLogItem3::AMActionLogItem3(AMDatabase *db, int id)
{
	db_ = db;
	id_ = id;
	loadedFromDb_ = false;
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
	columns << "name" << "longDescription" << "iconFileName" << "startDateTime" << "endDateTime" << "finalState" << "info" << "parentId" << "actionInheritedLoop";
	QVariantList values = db_->retrieve(id_, AMDbObjectSupport::s()->tableNameForClass<AMActionLog3>(), columns);
	if(values.isEmpty())
		return false;

	shortDescription_ = values.at(0).toString();
	longDescription_  = values.at(1).toString();
	iconFileName_ = values.at(2).toString();
	startDateTime_ = QDateTime::fromString(values.at(3).toString(), "yyyy-MM-ddThh:mm:ss.zzz");
	endDateTime_ = QDateTime::fromString(values.at(4).toString(), "yyyy-MM-ddThh:mm:ss.zzz");
	finalState_ = values.at(5).toInt();
	canCopy_ = db_->retrieve(values.at(6).toString().section(';', -1).toInt(), values.at(6).toString().split(';').first(), "canCopy").toBool();
	parentId_ = values.at(7).toInt();
	actionInheritedLoop_ = values.at(8).toBool();
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
	maximumActionsLimit_ = 200;

	connect(&refreshFunctionCall_, SIGNAL(executed()), this, SLOT(refreshFromDb()));
	connect(&specificRefreshFunctionCall_, SIGNAL(executed()), this, SLOT(refreshSpecificIds()));

	if(db_) {
		connect(db_, SIGNAL(created(QString,int)), this, SLOT(onDatabaseItemCreated(QString,int)));
		connect(db_, SIGNAL(removed(QString,int)), this, SLOT(onDatabaseItemRemoved(QString,int)));
		connect(db_, SIGNAL(updated(QString,int)), this, SLOT(onDatabaseItemUpdated(QString,int)));
	}

	succeededIcon_ = QPixmap(":/22x22/greenCheck.png");
	cancelledIcon_ = QPixmap(":/22x22/orangeX.png");
	failedIcon_ = QPixmap(":/22x22/redCrash.png");
	unknownIcon_ = QPixmap(":/22x22/dialog-question.png");
}

AMActionHistoryModel3::~AMActionHistoryModel3() {
	clear();
}

QModelIndex AMActionHistoryModel3::index(int row, int column, const QModelIndex &parent) const
{
	// Return bad index if it's outside the column range
	if(column < 0 || column > 4)
		return QModelIndex();

	// if no parent is top level
	if(!parent.isValid()){
		if(row < 0){// what about other limit?
			AMErrorMon::alert(this, AMACTIONHISTORYMODEL_MODELINDEX_OUT_OF_BOUNDS, "The action history attempted to access information that was entirely out of bounds. Please report this problem to the Acquaman developers ");
			return QModelIndex();
		}
		int foundTopLevels = 0;
		for(int x = 0; x < items_.count(); x++){
			if(items_.at(x)->parentId() == -1)
				foundTopLevels++;
			if(foundTopLevels == row+1)
				return createIndex(row, column, items_.at(x));
		}
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_MODELINDEX_INDEX_NOT_IN_LIST, "The action history attempted to access an index that was not in the history list. Please report this problem to the Acquaman developers ");
		return QModelIndex();
	}
	// if parent then it's sub-level
	else{
		AMActionLogItem3 *parentLogItem = logItem(parent);
		if(!parentLogItem){
			AMErrorMon::alert(this, AMACTIONHISTORYMODEL_MODELINDEX_BAD_PARENT_ITEM, "The action history attempted to access an index with a bad parent item. Please report this problem to the Acquaman developers ");
			return QModelIndex();
		}
		int parentIndexInList = items_.indexOf(parentLogItem);
		int siblingsFound = 0;
		for(int x = parentIndexInList; x < items_.count(); x++){
			if(items_.at(x)->parentId() == parentLogItem->id())
				siblingsFound++;
			if(siblingsFound == row+1)
				return createIndex(row, column, items_.at(x));
		}
		return QModelIndex();
	}
}

QModelIndex AMActionHistoryModel3::parent(const QModelIndex &child) const
{
	if(!child.isValid())
		return QModelIndex();

	AMActionLogItem3 *childLogItem = logItem(child);
	if(!childLogItem)
		return QModelIndex();

	for(int x = 0; x < items_.count(); x++)
		if(items_.at(x)->id() == childLogItem->parentId())
			return indexForLogItem(items_.at(x));

	return QModelIndex();
}

int AMActionHistoryModel3::rowCount(const QModelIndex &parent) const
{
	// top level
	if(!parent.isValid()){
		int topLevelsFound = 0;
		for(int x = 0; x < items_.count(); x++)
			if(items_.at(x)->parentId() == -1)
				topLevelsFound++;

		return topLevelsFound;
	}
	// some sub-level
	else{
		AMActionLogItem3 *parentLogItem = logItem(parent);
		if(!parentLogItem){
			AMErrorMon::alert(this, AMACTIONHISTORYMODEL_ROWCOUNT_BAD_PARENT_ITEM, "The action history attempted to access a rowCount with a bad parent item. Please report this problem to the Acquaman developers ");
			return 0;
		}
		int childrenFound = 0;
		int parentIndexInList = items_.indexOf(parentLogItem);
		for(int x = parentIndexInList; x < items_.count(); x++)//might optimize to figure a good ending point
			if(items_.at(x)->parentId() == parentLogItem->id())
				childrenFound++;

		return childrenFound;
	}
}

int AMActionHistoryModel3::columnCount(const QModelIndex &parent) const
{
	if(!parent.isValid())
		return 4;
	else
		return 0;
}

QVariant AMActionHistoryModel3::data(const QModelIndex &index, int role) const
{
	AMActionLogItem3 *item = logItem(index);
	if(!item){
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_MODELDATA_BAD_ITEM, QString("The action history attempted to access data with a bad item (row: %1 column: %2). Please report this problem to the Acquaman developers ").arg(index.row()).arg(index.column()));
		return QVariant();
	}

	if(role == Qt::DisplayRole) {
		switch(index.column()) {
		case 0: return item->shortDescription();
		case 1: return QVariant();
		case 2: return AMDateTimeUtils::prettyDateTime(item->endDateTime());
		case 3: return AMDateTimeUtils::prettyDuration(item->startDateTime(), item->endDateTime());
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
			return item->longDescription();
		}
		else if(index.column() == 1) {
			switch(item->finalState()) {
			case AMAction3::Succeeded: return "Succeeded";
			case AMAction3::Cancelled: return "Cancelled";
			case AMAction3::Failed: return "Failed";
			default: return "[?]";
			}
		}
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
	AMActionLogItem3 *item = logItem(index);
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
		case 2: return QString("Finished");
		case 3: return QString("Duration");
		default: return QVariant();
		}
	}
	return QVariant();
}

bool AMActionHistoryModel3::hasChildren(const QModelIndex &parent) const{
	if(!parent.isValid())
		return true; // top level must have children
	else{
		AMActionLogItem3 *parentLogItem = logItem(parent);
		if(!parentLogItem){
			AMErrorMon::alert(this, AMACTIONHISTORYMODEL_HASCHILDREN_BAD_PARENT_ITEM, "The action history attempted to check hasChildren on a bad parent item. Please report this problem to the Acquaman developers ");
			return false;
		}
		int parentIndexInList = items_.indexOf(parentLogItem);
		for(int x = parentIndexInList; x < items_.count(); x++)//might optimize to figure a good ending point
			if(items_.at(x)->parentId() == parentLogItem->id())
				return true;

		return false;
	}
}

int AMActionHistoryModel3::childrenCount(const QModelIndex &parent) const{
	if(rowCount() == 0)
		return 0;
	int subChildCount = 0;
	for(int x = 0; x < rowCount(parent); x++)
		if(hasChildren(index(x, 0, parent)))
			subChildCount += childrenCount(index(x, 0, parent));
	return rowCount(parent) + subChildCount;
}

AMActionLogItem3 * AMActionHistoryModel3::logItem(const QModelIndex &index) const
{
	if(!index.isValid())
		return 0;
	return static_cast<AMActionLogItem3*>(index.internalPointer());
}

QModelIndex AMActionHistoryModel3::indexForLogItem(AMActionLogItem3 *logItem) const{
	if(!logItem)
		return QModelIndex();

	if(logItem->parentId() == -1){
		// top level logItem
		int topLevelBefore = 0;
		for(int x = 0; x < items_.count(); x++){
			if(items_.at(x) == logItem){
				return createIndex(topLevelBefore, 0, logItem);
			}
			if(items_.at(x)->parentId() == -1)
				topLevelBefore++;
		}
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_INDEXFORLOGITEM_INDEX_NOT_IN_LIST, "The action history attempted to find the index for an item with a bad parent item. Please report this problem to the Acquaman developers ");
		return QModelIndex();
	}
	else{
		// we have a parent logitem
		bool foundParent = false;
		int backwardsIndex = items_.indexOf(logItem); //find myself
		int numberOfSiblings = 0;
		while(!foundParent){
			if(backwardsIndex == 0){
				AMErrorMon::alert(this, AMACTIONHISTORYMODEL_INDEXFORLOGITEM_PARENT_NOT_IN_LIST, "The action history attempted to find the index for an item where the parent was not in the list. Please report this problem to the Acquaman developers ");
				return QModelIndex();
			}
			backwardsIndex--;
			if(items_.at(backwardsIndex)->id() == logItem->parentId()) //mark parent found if id matches
				foundParent = true;
			else if(items_.at(backwardsIndex)->parentId() == logItem->parentId())
				numberOfSiblings++; //or incremement the number of your siblings
		}
		return createIndex(numberOfSiblings, 0, logItem);
	}
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
	emit dataChanged(index.child(0, 0), index.child(rowCount(index), 0));
}

void AMActionHistoryModel3::markIndexAsDeselected(const QModelIndex &index, QAbstractItemView *viewer){
	// For all the child index start the recursive procedure to make parentSelected (that function will emit dataChanged for their children's indices)
	for(int x = 0; x < rowCount(index); x++)
		recurseMarkParentSelected(index.child(x, 0), viewer, false);
	// Emit dataChanged for these top-level indices
	emit dataChanged(index.child(0, 0), index.child(rowCount(index), 0));
}

void AMActionHistoryModel3::markIndexGroupAsSelected(const QModelIndex &index, QAbstractItemView *viewer){
	// Call the helper function with selected=true
	markIndexGroup(index, viewer, true);
}

void AMActionHistoryModel3::markIndexGroupAsDeselected(const QModelIndex &index, QAbstractItemView *viewer){
	// Call the helper function with selected=false
	markIndexGroup(index, viewer, false);
}

void AMActionHistoryModel3::setVisibleDateTimeRange(const QDateTime &oldest, const QDateTime &newest)
{
	if(oldest == visibleRangeOldest_ && newest == visibleRangeNewest_)
		return;

	visibleRangeOldest_ = oldest;
	visibleRangeNewest_ = newest;
	refreshFunctionCall_.schedule();
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

	// need to setup the query differently based on whether we have oldest and newest visible range limits.
	// both newest and oldest limits:
	if(visibleRangeOldest_.isValid() && visibleRangeNewest_.isValid()) {
		q = db_->select(actionLogTableName_,
				"id,parentId",
				"startDateTime BETWEEN ? AND ? ORDER BY startDateTime DESC LIMIT ?");
		q.bindValue(0, visibleRangeOldest_);
		q.bindValue(1, visibleRangeNewest_);
		q.bindValue(2, maximumActionsLimit_);
		q2 = db_->select(actionLogTableName_,
				 "COUNT(1)",
				 "startDateTime BETWEEN ? AND ?");
		q2.bindValue(0, visibleRangeOldest_);
		q2.bindValue(1, visibleRangeNewest_);
	}
	// only an oldest limit:
	else if(visibleRangeOldest_.isValid()) {
		q = db_->select(actionLogTableName_,
				"id,parentId",
				"startDateTime >= ? ORDER BY startDateTime DESC LIMIT ?");
		q.bindValue(0, visibleRangeOldest_);
		q.bindValue(1, maximumActionsLimit_);
		q2 = db_->select(actionLogTableName_,
				 "COUNT(1)",
				 "startDateTime >= ?");
		q2.bindValue(0, visibleRangeOldest_);
	}
	// only a newest limit:
	else if(visibleRangeNewest_.isValid()) {
		q = db_->select(actionLogTableName_,
				"id,parentId",
				"startDateTime <= ? ORDER BY startDateTime DESC LIMIT ?");
		q.bindValue(0, visibleRangeNewest_);
		q.bindValue(1, maximumActionsLimit_);
		q2 = db_->select(actionLogTableName_,
				 "COUNT(1)",
				 "startDateTime <= ?");
		q2.bindValue(0, visibleRangeNewest_);
	}
	// everything:
	else {
		q = db_->select(actionLogTableName_,
				"id,parentId",
				"1 ORDER BY startDateTime DESC LIMIT ?");
		q.bindValue(0, maximumActionsLimit_);
		q2 = db_->select(actionLogTableName_,
				 "COUNT(1)");
	}

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
			AMErrorMon::alert(this, AMACTIONHISTORYMODEL_REFRESHFROMDB_FAILED_TO_CREATE_LIST, "The action history failed to generate its internal list. Please report this problem to the Acquaman developers ");
	}

	emit modelRefreshed();
}


void AMActionHistoryModel3::onDatabaseItemCreated(const QString &tableName, int id)
{
	if(tableName != actionLogTableName_)
		return;

	// if id is -1, there could be updates to the whole table.
	if(id < 1) {
		// qDebug() << "AMActionHistoryModel: global refresh";
		refreshFunctionCall_.schedule();
		return;
	}

	// qDebug() << "AMActionHistoryModel: precision refresh";

	// OK, this is a specific update.
	// find out if this action's endDateTime is within our visible date range
	AMActionLogItem3* item = new AMActionLogItem3(db_, id);
	if(insideVisibleDateTimeRange(item->startDateTime())) {
		emit modelAboutToBeRefreshed();
		/// \todo Ordering... This may end up at the wrong spot until a full refresh is done.  Most of the time, any actions added will be the most recent ones, however that is not guaranteed.
		appendItem(item);
		visibleActionsCount_++;
		emit modelRefreshed();
	}
	else {
		delete item;
	}
}

void AMActionHistoryModel3::onDatabaseItemUpdated(const QString &tableName, int id)
{
	if(tableName != actionLogTableName_)
		return;
	// if id is -1, there could be updates to the whole table.
	if(id < 1) {
		refreshFunctionCall_.schedule();
		return;
	}

	// OK, this is a specific update.
	idsRequiringRefresh_ << id;
	specificRefreshFunctionCall_.schedule();
}

void AMActionHistoryModel3::onDatabaseItemRemoved(const QString &tableName, int id)
{
	if(tableName != actionLogTableName_)
		return;

	// This probably won't happen very often, so for now let's just take the easy way out and do a full refresh.
	Q_UNUSED(id)
	refreshFunctionCall_.schedule();
}

void AMActionHistoryModel3::refreshSpecificIds()
{
	// will contain the indexes of any rows that should be deleted.
	QModelIndexList topLevelsToDelete;

	// go through all our items, and see if any of them need to be updated.
	for(int x = 0; x < items_.count(); x++){
		AMActionLogItem3* itemToRefresh = items_.at(x);
		if(idsRequiringRefresh_.contains(itemToRefresh->id())) {
			itemToRefresh->refresh();
			QModelIndex itemIndex = indexForLogItem(itemToRefresh);
			QModelIndex topLevelParentIndex = topLevelParent(itemIndex);
			AMActionLogItem3 *itemTopLevelParent = itemToRefresh;
			if(itemIndex != topLevelParentIndex)
				itemTopLevelParent = logItem(topLevelParentIndex);

			// If the end date time has changed to be outside of our visible range, it shouldn't be shown any more.
			// Now we're checking against the start time of the top level parent, then we can let the remove algorithm take care of all of the children
			if(!insideVisibleDateTimeRange(itemTopLevelParent->startDateTime()) && !topLevelsToDelete.contains(topLevelParentIndex)) {
				topLevelsToDelete.append(topLevelParentIndex);
			}
			else {
				QModelIndex changedIndexFirst = indexForLogItem(itemToRefresh);
				QModelIndex changedIndexLast = indexForLogItem(itemToRefresh).sibling(changedIndexFirst.row(), 2);
				emit dataChanged(changedIndexFirst, changedIndexLast);
			}
		}
	}

	idsRequiringRefresh_.clear();

	// Now delete any rows that shouldn't be there anymore. Do it by QModelIndex and things should be fine.
	if(!topLevelsToDelete.isEmpty()) {
		emit modelAboutToBeRefreshed();
		for(int x = 0; x < topLevelsToDelete.count(); x++)
			recurseActionsLogLevelClear(topLevelsToDelete.at(x));
		emit modelRefreshed();
	}
}

bool AMActionHistoryModel3::insideVisibleDateTimeRange(const QDateTime &dateTime)
{
	if(visibleRangeOldest_.isValid() && dateTime < visibleRangeOldest_)
		return false;
	if(visibleRangeNewest_.isValid() && dateTime > visibleRangeNewest_)
		return false;

	return true;
}

void AMActionHistoryModel3::appendItem(AMActionLogItem3 *item)
{
	int parentId = item->parentId();
	QModelIndex parentIndex = QModelIndex();

	// If the parent id isn't -1 (no parent) then find the parent
	if(parentId != -1)
		for(int x = 0; x < items_.count(); x++)
			if(items_.at(x)->id() == parentId)
				parentIndex = indexForLogItem(items_.at(x));

	int parentRowCount = rowCount(parentIndex);
	beginInsertRows(parentIndex, parentRowCount, parentRowCount);
	items_.append(item);
	endInsertRows();
}

void AMActionHistoryModel3::clear()
{
	if(items_.isEmpty())
		return;

	if(!recurseActionsLogLevelClear(QModelIndex()))
		AMErrorMon::alert(this, AMACTIONHISTORYMODEL_FAILED_TO_CLEAR_LIST, "The action history failed to clear its internal list. Please report this problem to the Acquaman developers ");
}

bool AMActionHistoryModel3::recurseActionsLogLevelCreate(int parentId, QMap<int, int> parentIdsAndIds){
	QList<int> childIdsInReverse = parentIdsAndIds.values(parentId);
	// No more recursion if there are no children (base case)
	if(childIdsInReverse.count() == 0)
		return true;

	// Grab the index of the parent in the list
	int indexOfParent;
	if(!items_.count() == 0){
		for(int x = 0; x < items_.count(); x++)
			if(items_.at(x)->id() == parentId)
				indexOfParent = x+1;
	}
	else
		indexOfParent = 0;

	// Place this level of items in reverse order
	for(int x = 0; x < childIdsInReverse.count(); x++)
		items_.insert(indexOfParent, new AMActionLogItem3(db_, childIdsInReverse.at(x)));

	// Ask to place the children for each child in order
	bool success = true;
	for(int x = childIdsInReverse.count()-1; x >= 0; x--)
		success &= recurseActionsLogLevelCreate(childIdsInReverse.at(x), parentIdsAndIds);
	return success;
}

bool AMActionHistoryModel3::recurseActionsLogLevelClear(QModelIndex parentIndex){
	int childrenCount = rowCount(parentIndex);
	// No more recursion if there are no children (base case)
	if(childrenCount == 0)
		return true;

	// Recursively clear children's children
	bool success = true;
	for(int x = childrenCount-1; x >= 0; x--)
		success &= recurseActionsLogLevelClear(index(x, 0, parentIndex));

	// Clear this level of children
	AMActionLogItem3 *item;
	beginRemoveRows(parentIndex, 0, childrenCount);
	for(int x = childrenCount-1; x >= 0; x--){
		item = logItem(index(x, 0, parentIndex));
		success &= items_.removeOne(item);
		delete item;
	}
	endRemoveRows();

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
	emit dataChanged(this->index(0, 0, index), this->index(childrenCount, 0, index));
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
