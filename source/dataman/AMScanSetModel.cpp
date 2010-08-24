#include "AMScanSetModel.h"

QModelIndex AMScanSetModel::index ( int row, int column, const QModelIndex & parent ) const {
	// top level:
	if(!parent.isValid()) {
		if(column == 0 && row >= 0 && row < scans_.count())
			return createIndex(row, 0, -1);	// id of -1 in a model index indicates a Scan-level index. Row is the index of the scan.
		else
			return QModelIndex();
	}

	// Parent is a scan-level index
	else if(parent.internalId() == -1 && parent.row() < scans_.count() ) {
		if(column == 0 && row < scans_.at(parent.row())->numChannels() )
			return createIndex(row, 0, parent.row() );
		else
			return QModelIndex();
	}

	// anything else (For parent-indices that correspond to channels, there are no children indices)
	return QModelIndex();
}

QModelIndex AMScanSetModel::parent ( const QModelIndex & index ) const {

	// scan-level indices: parent is the top level
	if(index.internalId() == -1)
		return QModelIndex();

	// if index is a channel level index: internalId is the index of its parent scan, which becomes the row in the parent index.
	if(index.isValid() && index.column() == 0 && index.internalId() >=0 && index.internalId() < scans_.count() )
		return createIndex(index.internalId(), 0, -1);

	return QModelIndex();
}

Qt::ItemFlags AMScanSetModel::flags ( const QModelIndex & index ) const  {

	// Scans:
	if(index.internalId() == -1)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	// Channels:
	if(index.internalId() >= 0)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;

	return Qt::NoItemFlags;
}

QVariant AMScanSetModel::data ( const QModelIndex & index, int role) const {
	if(!index.isValid())
		return QVariant();

	// scan-level index:
	if(index.internalId() == -1 && index.row() < scans_.count() && index.column() == 0) {
		AMScan* scan = scans_.at(index.row());

		switch(role) {
		case Qt::DisplayRole:
			return scan->name();
			break;
		case Qt::DecorationRole:
			if(scan->numChannels() > 0)
				return chMetaData_.at(index.row()).at(0).color;
			else
				return QVariant();
			break;
		case Qt::ToolTipRole:
			return QString("%1 (%2): %3").arg(scan->name()).arg(scan->sampleName()).arg(scan->dateTime().toString());
			break;
		case AM::DescriptionRole:
			return QString("started %1, on %2").arg(scan->dateTime().toString("hh:mmap, MMM dd (yyyy)")).arg(scan->sampleName());
		case Qt::CheckStateRole:
			return QVariant();	/// \todo For now... No checking/unchecking scans.
			break;
		case AM::PointerRole:
			return qVariantFromValue(scan);
			break;
		default:
			return QVariant();
			break;
		}
	}


	// channel-level index:
	if(index.internalId() >= 0 && index.internalId() < scans_.count() ) {
		AMScan* scan = scans_.at(index.internalId());

		if(index.row() < scan->numChannels() && index.column() == 0) {
			AMChannel* channel = scan->channel(index.row());

			switch(role) {
			case Qt::DisplayRole:
				return channel->name();
				break;
			case Qt::DecorationRole:
				return chMetaData_.at(index.internalId()).at(index.row()).color;
				break;
			case Qt::ToolTipRole:
				return QString("%1 (%2): %3").arg(channel->name()).arg(scan->name()).arg(channel->expression());
				break;
			case Qt::CheckStateRole:
				return chMetaData_.at(index.internalId()).at(index.row()).visible ? Qt::Checked : Qt::Unchecked;
				break;
			case AM::PointerRole:
				return qVariantFromValue(channel);
				break;
			case AM::PriorityRole:
				return chMetaData_.at(index.internalId()).at(index.row()).priority;
				break;
			case AM::LinePenRole:
				return chMetaData_.at(index.internalId()).at(index.row()).linePen;
			default:
				return QVariant();
				break;
			}
		}
	}

	return QVariant();
}

QVariant AMScanSetModel::headerData ( int section, Qt::Orientation orientation, int role ) const {
	if(role != Qt::DisplayRole)
		return QVariant();
	if(orientation == Qt::Horizontal)
		return QString("Channel");
	if(orientation == Qt::Vertical)
		return QVariant(section);
	return QVariant();
}

int AMScanSetModel::rowCount ( const QModelIndex & parent ) const  {
	// top level: return number of scans
	if(!parent.isValid())
		return scans_.count();

	// scan-level: return number of channels:
	if(parent.internalId() == -1 && parent.row() < scans_.count())
		return scans_.at(parent.row())->numChannels();

	return 0;
}

int AMScanSetModel::columnCount ( const QModelIndex & parent ) const {
	Q_UNUSED(parent)

	return 1;
}

bool AMScanSetModel::hasChildren ( const QModelIndex & parent  ) const {

	if(!parent.isValid())
		return true;

	// scans have children, if they have channels
	if(parent.internalId() == -1 && parent.row() < scans_.count() && scans_.at(parent.row())->numChannels() > 0)
		return true;
	// channels don't.
	else
		return false;
}


/// returns the index (or row) of an AMScan in the top-level. returns -1 if not found.
int AMScanSetModel::indexOf(AMScan* scan) const {
	return scans_.indexOf(scan);
}

int AMScanSetModel::indexOf(AMChannel* channel, AMScan* insideHere) const {
	return insideHere->channelList()->indexOf(channel);
}

// Resizable Interface:

// Add a scan to this model.  The AMScan must exist elsewhere, for the lifetime that it is added to the model.  Model does not take ownership of the scan.
void AMScanSetModel::addScan(AMScan* newScan) {
	emit layoutAboutToBeChanged();

	beginInsertRows(QModelIndex(), scans_.count(), scans_.count());

	scans_.append(newScan);
	scanChannelLists_.append(newScan->channelList());
	connect(newScan, SIGNAL(metaDataChanged(QString)), this, SLOT(onMetaDataChanged(QString)));

	QList<AMScanSetModelChannelMetaData> chs;
	for(int i=0; i<newScan->numChannels(); i++)
		chs.append(AMScanSetModelChannelMetaData());
	chMetaData_.append(chs);

	/// \todo hook up signals from newScan->channelList to catch channel creation and deletion
	connect(newScan->channelList(), SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)), this, SLOT(onChannelAboutToBeAdded(QModelIndex, int, int)));
	connect(newScan->channelList(), SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onChannelAdded(QModelIndex, int, int)));
	connect(newScan->channelList(), SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onChannelAboutToBeRemoved(QModelIndex, int, int)));
	connect(newScan->channelList(), SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onChannelRemoved(QModelIndex, int, int)));
	endInsertRows();
	/// \todo hack: should not be needed... But we do to keep QTreeViews from getting messed up. Why?
	emit layoutChanged();
}

// removes an AMScan from this model. Does not delete the scan.  Call this before deleting a scan that has been added to the model.
bool AMScanSetModel::removeScan(AMScan* removeMe) {

	int index = scans_.indexOf(removeMe);

	if(index != -1) {
		disconnect(removeMe, SIGNAL(metaDataChanged(QString)), this, SLOT(onMetaDataChanged(QString)));
		emit layoutAboutToBeChanged();
		beginRemoveRows(QModelIndex(), index, index);
		scans_.removeAt(index);
		scanChannelLists_.removeAt(index);
		chMetaData_.removeAt(index);
		endRemoveRows();

		/// \todo hack: should not be needed... But we do to keep QTreeViews from getting messed up. Why?
		emit layoutChanged();
		return true;
	}
	else
		return false;
}


bool AMScanSetModel::setData ( const QModelIndex & index, const QVariant & value, int role ) {
	// invalid index... don't do anything
	if(!index.isValid())
		return false;

	// editing a (valid) scan?
	if(index.internalId() == -1 && index.row() < scans_.count()) {
		AMScan* scan = scans_.at(index.row());
		switch(role) {
		case Qt::DisplayRole:
			scan->setName(value.toString());
			emit dataChanged(index, index);
			return true;
		default:
			return false;
		}
	}

	// editing a (valid) channel?
	else if(index.internalId() < scans_.count() && index.row() < scans_.at(index.internalId())->numChannels()) {
		// AMChannel* channel = scans_.at(index.internalId())->channel(index.row());
		switch(role) {
		case Qt::DecorationRole:
			chMetaData_[index.internalId()][index.row()].color = value.value<QColor>();
			emit dataChanged(index, index);
			return true;
		case Qt::CheckStateRole:
			chMetaData_[index.internalId()][index.row()].visible = value.toBool();
			emit dataChanged(index, index);
			return true;
		case AM::PriorityRole:
			chMetaData_[index.internalId()][index.row()].priority = value.toInt();
			emit dataChanged(index, index);
			return true;
		case AM::LinePenRole:
			chMetaData_[index.internalId()][index.row()].linePen = value.value<QPen>();
			emit dataChanged(index, index);
			return true;
		default:
			return false;
		}
	}

	return false;
}


// the AMChannelListModel is a standard Qt model, but it guarantees that only one channel will be added at a time, and it will be added at the end of all rows(channels).
void AMScanSetModel::onChannelAboutToBeAdded(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(parent)

	AMChannelListModel* source = qobject_cast<AMChannelListModel*>(sender());

	int scanIndex = scanChannelLists_.indexOf(source);
	if(scanIndex == -1)
		return;

	beginInsertRows(index(scanIndex,0), start, end);

}

void AMScanSetModel::onChannelAdded(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(parent)

	AMChannelListModel* source = qobject_cast<AMChannelListModel*>(sender());
	int scanIndex = scanChannelLists_.indexOf(source);
	if(scanIndex == -1)
		return;

	for(int i=start; i<=end; i++)
		chMetaData_[scanIndex].insert(i, AMScanSetModelChannelMetaData());

	endInsertRows();
}

void AMScanSetModel::onChannelAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(parent)

	AMChannelListModel* source = qobject_cast<AMChannelListModel*>(sender());
	int scanIndex = scanChannelLists_.indexOf(source);
	if(scanIndex == -1)
		return;

	beginRemoveRows(index(scanIndex,0), start, end);

	for(int i=end; i>=start; i--)
		chMetaData_[scanIndex].removeAt(i);
}

void AMScanSetModel::onChannelRemoved(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	AMChannelListModel* source = qobject_cast<AMChannelListModel*>(sender());
	int scanIndex = scanChannelLists_.indexOf(source);
	if(scanIndex == -1)
		return;




	endRemoveRows();
}


void AMScanSetModel::onMetaDataChanged(const QString& key) {
	AMScan* scan = qobject_cast<AMScan*>(sender());
	if(!scan)
		return;

	if(key=="name" || key=="sampleName" || key=="dateTime") {
		QModelIndex i = indexForScan(indexOf(scan));
		if(i.isValid())
			emit dataChanged(i, i);
	}
}
