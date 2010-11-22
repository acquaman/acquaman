#include "AMScanSetModel.h"
#include "ui/AMDateTimeUtils.h"

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
		if(column == 0 && row < scans_.at(parent.row())->dataSourceCount() )
			return createIndex(row, 0, parent.row() );
		else
			return QModelIndex();
	}

	// anything else (For parent-indices that correspond to data sources, there are no children indices)
	return QModelIndex();
}

/// Return a list of the names of all scans in this model:
QStringList AMScanSetModel::scanNames() const {
	QStringList rv;
	for(int i=0; i<numScans(); i++)
		rv << scanAt(i)->fullName();
	return rv;
}

QModelIndex AMScanSetModel::parent ( const QModelIndex & index ) const {

	// scan-level indices: parent is the top level
	if(index.internalId() == -1)
		return QModelIndex();

	// if index is a data source-level index: internalId is the index of its parent scan, which becomes the row in the parent index.
	if(index.isValid() && index.column() == 0 && index.internalId() >=0 && index.internalId() < scans_.count() )
		return createIndex(index.internalId(), 0, -1);

	return QModelIndex();
}

Qt::ItemFlags AMScanSetModel::flags ( const QModelIndex & index ) const  {

	// Scans:
	if(index.internalId() == -1)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	// Data Sources:
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
		case Qt::DisplayRole: {
				QString rv = scan->name().append(QString(" #%1").arg(scan->number()));
				if(scan->modified())
					rv.append( " (modified)");
				return rv;
			}
			break;
		case Qt::DecorationRole:
			/// \bug this is temporary and meaningless; it's just the color of the first data source in the scan.
			if(scan->dataSourceCount() > 0)
				return sourcePlotSettings_.at(index.row()).at(0).color;
			else
				return QVariant();
			break;
		case Qt::ToolTipRole:
			return QString("%1, #%2 (sample: %3): %4").arg(scan->name()).arg(scan->number()).arg(scan->sampleName()).arg(AMDateTimeUtils::prettyDateTime(scan->dateTime(), "h:mm:ssap"));
			break;
		case AM::DescriptionRole:
			return QString("%1, on %2").arg(AMDateTimeUtils::prettyDateTime(scan->dateTime())).arg(scan->sampleName());
		case AM::DateTimeRole:
			return scan->dateTime();
		case Qt::CheckStateRole:
			return QVariant();	/// \todo For now... No checking/unchecking scans.
			break;
		case AM::PointerRole:
			return qVariantFromValue(scan);
			break;
		case AM::ModifiedRole:
			return scan->modified();
		case AM::CanCloseRole:	// allows views to show the 'close' button beside each scan, to delete it. Do we want this on?
			return true;
		default:
			return QVariant();
			break;
		}
	}


	// data source-level index:
	if(index.internalId() >= 0 && index.internalId() < scans_.count() ) {
		AMScan* scan = scans_.at(index.internalId());

		if(index.row() < scan->dataSourceCount() && index.column() == 0) {
			AMDataSource* dataSource = scan->dataSourceAt(index.row());

			switch(role) {
			case Qt::DisplayRole:
				return QString("%1 (%2)").arg(dataSource->description(), dataSource->name());
				break;
			case Qt::DecorationRole:
				return sourcePlotSettings_.at(index.internalId()).at(index.row()).color;
				break;
			case Qt::ToolTipRole:
			case AM::DescriptionRole:
				return QString("%1 (%2) From scan: %3\n%4").arg(dataSource->description(), dataSource->name()).arg(scan->name()).arg(dataSource->typeDescription());
				break;
			case Qt::CheckStateRole:
				return sourcePlotSettings_.at(index.internalId()).at(index.row()).visible ? Qt::Checked : Qt::Unchecked;
				break;
			case AM::PointerRole:
				return qVariantFromValue(dataSource);
				break;
			case AM::PriorityRole:
				return sourcePlotSettings_.at(index.internalId()).at(index.row()).priority;
				break;
			case AM::LinePenRole:
				return sourcePlotSettings_.at(index.internalId()).at(index.row()).linePen;
			case AM::CanCloseRole:	// allows views to show the 'close' button beside each scan, to delete it.
				return true;
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
		return QString("Data Source");
	if(orientation == Qt::Vertical)
		return QVariant(section);
	return QVariant();
}

int AMScanSetModel::rowCount ( const QModelIndex & parent ) const  {
	// top level: return number of scans
	if(!parent.isValid())
		return scans_.count();

	// scan-level: return number of data sources:
	if(parent.internalId() == -1 && parent.row() < scans_.count())
		return scans_.at(parent.row())->dataSourceCount();

	return 0;
}



bool AMScanSetModel::hasChildren ( const QModelIndex & parent  ) const {

	if(!parent.isValid())
		return true;

	// scans have children, if they have data sources
	if(parent.internalId() == -1 && parent.row() < scans_.count() && scans_.at(parent.row())->dataSourceCount() > 0)
		return true;
	// data sources don't.
	else
		return false;
}


/// returns the index (or row) of an AMScan in the top-level. returns -1 if not found.
int AMScanSetModel::indexOf(const AMScan* scan) const {
	return scans_.indexOf(scan);
}

int AMScanSetModel::indexOf(AMDataSource* dataSource, AMScan* insideHere) const {
	if(insideHere)
		return insideHere->indexOfDataSource(dataSource);
	else
		return -1;
}

int AMScanSetModel::indexOf(const AMDataSource *dataSource, int scanIndex) const {
	return indexOf(dataSource, scanAt(scanIndex));
}

// Resizable Interface:

// Add a scan to this model.  The AMScan must exist elsewhere, for the lifetime that it is added to the model.  Model does not take ownership of the scan.
void AMScanSetModel::addScan(AMScan* newScan) {
	emit layoutAboutToBeChanged();

	beginInsertRows(QModelIndex(), scans_.count(), scans_.count());

	scans_.append(newScan);
	connect(newScan, SIGNAL(modifiedChanged(bool)), this, SLOT(onScanModifiedChanged(bool)));
	connect(newScan, SIGNAL(nameChanged(QString)), this, SLOT(onMetaDataChanged()));
	connect(newScan, SIGNAL(numberChanged(int)), this, SLOT(onMetaDataChanged()));
	connect(newScan, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(onMetaDataChanged()));
	connect(newScan, SIGNAL(sampleIdChanged(int)), this, SLOT(onMetaDataChanged()));

	QList<AMDataSourcePlotSettings> plotSettings;
	for(int i=0; i<newScan->dataSourceCount(); i++)
		plotSettings.append(AMDataSourcePlotSettings());	/// \todo set up nicer default colors (related within scans)
	sourcePlotSettings_.append(plotSettings);

	// hook up signals from newScan to catch data source addition and removal
	connect(newScan, SIGNAL(dataSourceAdded(int)), this, SLOT(onDataSourceAdded(int)));
	connect(newScan, SIGNAL(dataSourceAboutToBeAdded(int)), this, SLOT(onDataSourceAboutToBeAdded(int)));
	connect(newScan, SIGNAL(dataSourceAboutToBeRemoved(int)), this, SLOT(onDataSourceAboutToBeRemoved(int)));
	connect(newScan, SIGNAL(dataSourceRemoved(int)), this, SLOT(onDataSourceRemoved(int)));

	endInsertRows();

	/// \todo this is a hack; should not be needed... But we do need it to keep QTreeViews from getting messed up. Why?
	emit layoutChanged();
}

// removes an AMScan from this model. Does not delete the scan.  Call this before deleting a scan that has been added to the model.
bool AMScanSetModel::removeScan(AMScan* removeMe) {

	int index = scans_.indexOf(removeMe);

	if(index != -1) {
		disconnect(removeMe, 0, this, 0);

		emit layoutAboutToBeChanged();
		beginRemoveRows(QModelIndex(), index, index);
		scans_.removeAt(index);
		sourcePlotSettings_.removeAt(index);
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
		Q_UNUSED(scan)
		switch(role) {
		// no editable roles for editing scans (for now...)
		default:
			return false;
		}
	}

	// editing a (valid) data source?
	else if(index.internalId() < scans_.count() && index.row() < scans_.at(index.internalId())->dataSourceCount()) {
		// AMDataSource* dataSource = scans_.at(index.internalId())->dataSourceAt(index.row());
		switch(role) {
		case Qt::DecorationRole:
			sourcePlotSettings_[index.internalId()][index.row()].color = value.value<QColor>();
			emit dataChanged(index, index);
			return true;
		case Qt::CheckStateRole:
			sourcePlotSettings_[index.internalId()][index.row()].visible = value.toBool();
			emit dataChanged(index, index);
			return true;
		case AM::PriorityRole:
			sourcePlotSettings_[index.internalId()][index.row()].priority = value.toDouble();
			emit dataChanged(index, index);
			return true;
		case AM::LinePenRole:
			sourcePlotSettings_[index.internalId()][index.row()].linePen = value.value<QPen>();
			emit dataChanged(index, index);
			return true;
		default:
			return false;
		}
	}

	return false;
}



void AMScanSetModel::onDataSourceAboutToBeAdded(int dataSourceIndex) {

	int scanIndex = indexOf(qobject_cast<AMScan*>(sender()));
	if(scanIndex == -1)
		return;

	beginInsertRows(index(scanIndex,0), dataSourceIndex, dataSourceIndex);

}

void AMScanSetModel::onDataSourceAdded(int dataSourceIndex) {

	int scanIndex = indexOf(qobject_cast<AMScan*>(sender()));
	if(scanIndex == -1)
		return;

	sourcePlotSettings_[scanIndex].insert(dataSourceIndex, AMDataSourcePlotSettings());	/// \todo colors...

	endInsertRows();
}

void AMScanSetModel::onDataSourceAboutToBeRemoved(int dataSourceIndex) {

	int scanIndex = indexOf(qobject_cast<AMScan*>(sender()));
	if(scanIndex == -1)
		return;

	beginRemoveRows(index(scanIndex,0), dataSourceIndex, dataSourceIndex);

	sourcePlotSettings_[scanIndex].removeAt(dataSourceIndex);
}

void AMScanSetModel::onDataSourceRemoved(int dataSourceIndex) {

	Q_UNUSED(dataSourceIndex)

	int scanIndex = indexOf(qobject_cast<AMScan*>(sender()));
	if(scanIndex == -1)
		return;

	endRemoveRows();
}


void AMScanSetModel::onMetaDataChanged() {
	AMScan* scan = qobject_cast<AMScan*>(sender());
	if(!scan)
		return;

	QModelIndex i = indexForScan(indexOf(scan));
	if(i.isValid())
		emit dataChanged(i, i);
}

void AMScanSetModel::onScanModifiedChanged(bool isModified) {
	Q_UNUSED(isModified)

	AMScan* scan = qobject_cast<AMScan*>(sender());
	if(!scan)
		return;

	QModelIndex i = indexForScan(indexOf(scan));
	if(i.isValid())
		emit dataChanged(i, i);
}
