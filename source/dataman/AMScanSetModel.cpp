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


#include "AMScanSetModel.h"
#include "util/AMDateTimeUtils.h"

#include "dataman/datasource/AMDataSource.h"
#include "dataman/AMScan.h"

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
	for(int i=0; i<scanCount(); i++)
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

	if (!index.isValid())
		return Qt::ItemIsEnabled;

	// Scans:
	if(index.internalId() == -1)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	// Data Sources:
	if(index.internalId() >= 0) {

		bool hiddenFromUsers = false;
		if(index.internalId() < scans_.count() && index.row() < scans_.at(index.internalId())->dataSourceCount())	 // makes sure index is in range; Qt might call flags() on out-of-range model indices.
			hiddenFromUsers = isHiddenFromUsers(index.internalId(), index.row());

		return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | (hiddenFromUsers ? Qt::NoItemFlags : Qt::ItemIsEnabled);
	}

	return Qt::NoItemFlags;
}

QVariant AMScanSetModel::data(const QModelIndex & index, int role) const {
	if(!index.isValid())
		return QVariant();

	// scan-level index:
	///////////////////////////
	if(index.internalId() == -1 && index.row() < scans_.count() && index.column() == 0) {
		AMScan* scan = scans_.at(index.row());

		switch(role) {
		case Qt::DisplayRole: {
				//QString rv = scan->name().append(QString(" #%1").arg(scan->number()));
				QString rv = scan->fullName();
				if(scan->modified())
					rv.append( " (modified)");
				return rv;
			}
			break;
		case Qt::DecorationRole:
			/// \bug this is temporary and meaningless; it's just the color of the first data source in the scan.
			if(scan->dataSourceCount() > 0)
				return sourcePlotSettings_.at(index.row()).at(0).linePen.color();
			else
				return QVariant();
			break;
		case Qt::ToolTipRole:
			return QString("%1, #%2 (sample: %3): %4").arg(scan->name()).arg(scan->number()).arg(scan->sampleName()).arg(AMDateTimeUtils::prettyDateTime(scan->dateTime(), "h:mm:ssap"));
			//return QString("%1, #%2 (sample: %3): %4").arg(scan->evaluatedName()).arg(scan->number()).arg(scan->sampleName()).arg(AMDateTimeUtils::prettyDateTime(scan->dateTime(), "h:mm:ssap"));
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
		case AM::NameRole: {
				//return scan->name().append(QString(" #%1").arg(scan->number()));
				return scan->fullName();
			}
			break;
		default:
			return QVariant();
			break;
		}
	}


	// data source-level index:
	////////////////////////////
	if(index.internalId() >= 0 && index.internalId() < scans_.count() ) {
		AMScan* scan = scans_.at(index.internalId());

		if(index.row() < scan->dataSourceCount() && index.column() == 0) {
			AMDataSource* dataSource = scan->dataSourceAt(index.row());

			switch(role) {
			case Qt::DisplayRole:
				return QString("%1 (%2)").arg(dataSource->description(), dataSource->name());
				break;
			case Qt::DecorationRole:
				return sourcePlotSettings_.at(index.internalId()).at(index.row()).linePen.color();
				break;
			case Qt::ToolTipRole:
			case AM::NameRole:
				return dataSource->name();
			case AM::DescriptionRole:
				return dataSource->description();
			case AM::DetailedDescriptionRole:
				return QString("%1 (%2) From scan: %3\n%4").arg(dataSource->description(),
																dataSource->name(),
																scan->name(),
																//scan->evaluatedName(),
																dataSource->typeDescription());
				break;
			case Qt::CheckStateRole:	// this controls visibility on plots.
				if(isVisible(index.internalId(), index.row()))
					return Qt::Checked;
				else
					return Qt::Unchecked;
				break;
			case AM::PointerRole:
				return qVariantFromValue(dataSource);
				break;
			case AM::PriorityRole:
				return sourcePlotSettings_.at(index.internalId()).at(index.row()).priority;
				break;
			case AM::CanCloseRole:	// allows views to show the 'close' button beside each scan, to delete it.
				return true;
			case AM::LinePenRole:
				return sourcePlotSettings_.at(index.internalId()).at(index.row()).linePen;
			case AM::RankRole:
				return dataSource->rank();
			case AMScanSetModel::ColorMapRole:
				return qVariantFromValue(sourcePlotSettings_.at(index.internalId()).at(index.row()).colorMap);

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
	return scans_.indexOf(const_cast<AMScan*>(scan));
}

int AMScanSetModel::indexOf(const AMDataSource* dataSource, const AMScan* insideHere) const {
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

	newScan->retain(this); // declare our interest in this scan.

	emit layoutAboutToBeChanged();

	beginInsertRows(QModelIndex(), scans_.count(), scans_.count());

	scans_.append(newScan);
	connect(newScan, SIGNAL(modifiedChanged(bool)), this, SLOT(onScanModifiedChanged(bool)));
	connect(newScan, SIGNAL(nameChanged(QString)), this, SLOT(onMetaDataChanged()));
	connect(newScan, SIGNAL(numberChanged(int)), this, SLOT(onMetaDataChanged()));
	connect(newScan, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(onMetaDataChanged()));
	connect(newScan, SIGNAL(sampleIdChanged(int)), this, SLOT(onMetaDataChanged()));

	QList<AMDataSourcePlotSettings> plotSettings;
	for(int i=0; i<newScan->dataSourceCount(); i++) {
		AMDataSourcePlotSettings ps; /// \todo set up nicer default colors (related within scans)
		//ps.visible = newScan->dataSourceAt(i)->visibleInPlots(); // Initial visibility is now controlled in AMDataSource::visibleInPlots().  Scan controllers can initialize this, and it will be saved in the database.
		ps.visible = false;

		// Hack for Darren's 2D XRF maps and Mark's XES scans.
		if (newScan->scanRank() == 0){

			ps.colorMap.setContrast(2.1);
			ps.colorMap.setBrightness(0.08);
			ps.colorMap.setGamma(1);
		}

		else if (newScan->scanRank() == 2 || newScan->scanRank() == 3){

			ps.colorMap.setContrast(1);
			ps.colorMap.setBrightness(0);
			ps.colorMap.setGamma(1);
		}

		plotSettings.append(ps);
	}
	sourcePlotSettings_.append(plotSettings);

	// hook up signals from newScan to catch data source addition and removal
	connect(newScan, SIGNAL(dataSourceAdded(int)), this, SLOT(onDataSourceAdded(int)));
	connect(newScan, SIGNAL(dataSourceAboutToBeAdded(int)), this, SLOT(onDataSourceAboutToBeAdded(int)));
	connect(newScan, SIGNAL(dataSourceAboutToBeRemoved(int)), this, SLOT(onDataSourceAboutToBeRemoved(int)));
	connect(newScan, SIGNAL(dataSourceRemoved(int)), this, SLOT(onDataSourceRemoved(int)));

	endInsertRows();

	emit scanAdded(newScan);
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

		removeMe->release(this);	// remove our interest in this scan.

		emit scanRemoved();
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
		/// \todo Update here: all properties we can adjust...
		case Qt::DecorationRole:
			sourcePlotSettings_[index.internalId()][index.row()].linePen.setColor( value.value<QColor>() );
			emit dataChanged(index, index);
			return true;
		case Qt::CheckStateRole:
			setVisible(index.internalId(), index.row(), value.toBool());
			return true;
		case AM::PriorityRole:
			sourcePlotSettings_[index.internalId()][index.row()].priority = value.toDouble();
			emit dataChanged(index, index);
			return true;
		case AM::LinePenRole:
			sourcePlotSettings_[index.internalId()][index.row()].linePen = value.value<QPen>();
			emit dataChanged(index, index);
			return true;
		case AMScanSetModel::ColorMapRole:
			sourcePlotSettings_[index.internalId()][index.row()].colorMap = value.value<MPlotColorMap>();
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

	AMDataSourcePlotSettings ps; /// \todo set up nicer default colors (related within scans)
	AMScan *scan = scanAt(scanIndex);

	ps.visible = scan->dataSourceAt(dataSourceIndex)->visibleInPlots();

	// Hack for Darren's 2D XRF maps and Mark's XES scans.
	if (scan->scanRank() == 0){

		ps.colorMap.setContrast(2.1);
		ps.colorMap.setBrightness(0.08);
		ps.colorMap.setGamma(1);
	}

	else if (scan->scanRank() == 2 || scan->scanRank() == 3){

		ps.colorMap.setContrast(1);
		ps.colorMap.setBrightness(0);
		ps.colorMap.setGamma(1);
	}

	sourcePlotSettings_[scanIndex].insert(dataSourceIndex, ps);

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


// shortcut for accessing a data source pointer:
AMDataSource* AMScanSetModel::dataSourceAt(int scanIndex, int dataSourceIndex) const {
	if(scanIndex < 0 || dataSourceIndex < 0 || scanIndex >= scans_.count() || dataSourceIndex >= scans_.at(scanIndex)->dataSourceCount())
		return 0;

	return scans_.at(scanIndex)->dataSourceAt(dataSourceIndex);
}


// Set the exclusive data source, by name. To clear the exclusive data source, specify an empty string. (This will cause 'exclusive' views to show nothing.)
bool AMScanSetModel::setExclusiveDataSourceByName(const QString& exclusiveDataSourceName) {
	if(exclusiveDataSourceName.isEmpty() || allDataSourceNames().contains(exclusiveDataSourceName)) {
		exclusiveDataSourceName_ = exclusiveDataSourceName;
		emit exclusiveDataSourceChanged(exclusiveDataSourceName_);
		return true;
	}
	else
		return false;
}
// returns a list of all data source names that exist (over all scans in the model). Warning: this is slow.  O(n), where n is the total number of data sources in all scans.
/* \todo Optimize with caching. */
QStringList AMScanSetModel::allDataSourceNames() const {
	QSet<QString> rv;
	for(int si = 0; si<scans_.count(); si++)
		for(int ci = 0; ci<scans_.at(si)->dataSourceCount(); ci++)
			rv << scans_.at(si)->dataSourceAt(ci)->name();
	return rv.toList();
}


// returns a list of all data source names that exist and are visible in at least one scan. Warning: this is slow.  O(n), where n is the total number of data sources in all scans.
/* \todo Optimize with caching. */
QStringList AMScanSetModel::visibleDataSourceNames() const {
	QSet<QString> rv;
	for(int si = 0; si<scans_.count(); si++)
		for(int ci = 0; ci<scans_.at(si)->dataSourceCount(); ci++)
			if(isVisible(si,ci))
				rv << scans_.at(si)->dataSourceAt(ci)->name();
	return rv.toList();
}

void AMScanSetModel::setVisible(int scanIndex, int dataSourceIndex, bool isVisible)  {

	if(sourcePlotSettings_.at(scanIndex).at(dataSourceIndex).visible == isVisible)
		return;

	sourcePlotSettings_[scanIndex][dataSourceIndex].visible = isVisible;

	QModelIndex i = indexForDataSource(scanIndex, dataSourceIndex);
	emit dataChanged(i, i);
}

bool AMScanSetModel::isVisible(int scanIndex, int dataSourceIndex) const
{
	// If the data source is hidden from users, it should not be visible regardless of the user's visibility setting.
	return !isHiddenFromUsers(scanIndex, dataSourceIndex) && sourcePlotSettings_.at(scanIndex).at(dataSourceIndex).visible;
}

void AMScanSetModel::setHiddenFromUsers(int scanIndex, int dataSourceIndex, bool isHiddenFromUsers)
{
	scans_.at(scanIndex)->dataSourceAt(dataSourceIndex)->setHiddenFromUsers(isHiddenFromUsers);
}

bool AMScanSetModel::isHiddenFromUsers(int scanIndex, int dataSourceIndex) const
{
	return scans_.at(scanIndex)->dataSourceAt(dataSourceIndex)->hiddenFromUsers();
}

void AMScanSetModel::saveVisibility()
{
	// Save the visibility settings to the database for (all scans in the model?) and all data sources.
	for(int si=0,cc=scanCount(); si<cc; si++) {
		for(int di=0,dd=scanAt(si)->dataSourceCount(); di<dd; di++) {
			dataSourceAt(si, di)->setVisibleInPlots(isVisible(si, di));
		}
	}
}

 AMScanSetModel::~AMScanSetModel(){}
