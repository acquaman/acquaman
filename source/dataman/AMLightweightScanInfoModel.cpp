#include "AMLightweightScanInfoModel.h"
#include "database/AMDbObject.h"

#include <QDebug>
AMLightweightScanInfoModel::AMLightweightScanInfoModel(AMLightweightScanInfoCollection *scanInfo, QObject *parent) :
	QAbstractItemModel(parent)
{
	scanInfo_ = scanInfo;
	connect(scanInfo_, SIGNAL(scanAboutToBeAdded(int)), this, SLOT(onScanInfoAboutToBeAdded(int)));
	connect(scanInfo_,SIGNAL(scanAboutToBeRemoved(int)), this, SLOT(onScanInfoAboutToBeRemoved(int)));
	connect(scanInfo_, SIGNAL(scanAdded()), this, SLOT(onScanInfoAdded()));
	connect(scanInfo_, SIGNAL(scanRemoved()), this, SLOT(onScanInfoRemoved()));
	connect(scanInfo_, SIGNAL(scanUpdated(int)), this, SLOT(onScanInfoUpdated(int)));
	connect(scanInfo_,SIGNAL(scanThumbnailAdded(int,int)), this, SLOT(onScanThumbnailAdded(int,int)));
	connect(scanInfo_, SIGNAL(scanThumbnailAboutToBeRemoved(int,int)), this, SLOT(onScanThumbnailAboutToBeRemoved(int,int)));
	connect(scanInfo_, SIGNAL(scanThumbnailRemoved()), this, SLOT(onScanThumbnailRemoved()));
}

QVariant AMLightweightScanInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Vertical)
		return QVariant();

	if(role != Qt::DisplayRole)
		return QVariant();

	if(section >= columnCount())
		return QVariant();

	switch (section)
	{
		case 0:
			return "Serial #";
		case 1:
			return "Name";
		case 2:
			return "#";
		case 3:
			return "When";
		case 4:
			return "Technique";
		case 5:
			return "Run";
		case 6:
			return "Sample Name";
		case 7:
			return "Notes";
		default:
			return QVariant();
	}

}

QVariant AMLightweightScanInfoModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid())
		return QVariant();
	else if(index.internalId() == -1)
		return getScanData(index, role);
	else return getThumbnailData(index, role);
}

int AMLightweightScanInfoModel::rowCount(const QModelIndex &parent) const
{
	if(!parent.isValid())
		return scanInfo_->count();
	else if(parent.internalId() == -1)
		return scanInfo_->at(parent.row())->thumbnailCount();
	else
		return 0;
}

int AMLightweightScanInfoModel::columnCount(const QModelIndex &parent) const
{
	if(!parent.isValid() || parent.internalId() == -1)
		return 8;
	else
		return 3;
}

QModelIndex AMLightweightScanInfoModel::index(int row, int column, const QModelIndex &parent) const
{
	if(!parent.isValid())
		return createIndex(row, column, -1);
	else
		return createIndex(row, column, parent.row());
}

QModelIndex AMLightweightScanInfoModel::parent(const QModelIndex &child) const
{
	if(!child.isValid() || child.internalId() == -1)
		return QModelIndex();

	return createIndex(child.internalId(), 0, -1);
}

QUrl AMLightweightScanInfoModel::rowToUrl(const QModelIndex &index)
{
	if(index.parent().isValid())
		return rowToUrl(index.parent());

	return scanInfo_->getScanUrl(index.data(Qt::DisplayRole).toInt());
}

QVariant AMLightweightScanInfoModel::getScanData(const QModelIndex &index, int role) const
{

	AMLightweightScanInfo* info = scanInfo_->at(index.row());

	if(role == Qt::DisplayRole || role == Qt::ToolTipRole)
	{
		switch (index.column())
		{
		case 0:
			return info->id();
		case 1:
			return info->name();
		case 2:
			return info->number();
		case 3:
			return info->dateTime();
		case 4:
			return info->scanType();
		case 5:
			return info->runName();
		case 6:
			return info->sampleName();
		case 7:
			return info->notes();
		default:
			return QVariant();
		}
	}
	else if(role == Qt::DecorationRole)
	{
		if(index.column() == 0)
		{
			AMDbThumbnail* thumbnailData = info->thumbnailAt(0);
			if(!thumbnailData)
				return QVariant();

			QPixmap pixmap;
			pixmap.loadFromData(thumbnailData->thumbnail);

			QIcon icon(pixmap);

			return icon;
		}
	} else if(role == Qt::UserRole) /// We use user role for obtaining data we wish not to display
	{
		switch(index.column())
		{
		case 0:
			return info->id();
		case 1:
			return info->name();
		case 2:
			return info->number();
		case 3:
			return info->dateTime();
		case 4:
			return info->scanType();
		case 5:
			return info->runId();
		case 6:
			return info->runName();
		case 7:
			return info->sampleName();
		case 8:
			return info->notes();
		case 9:
			return info->experimentId();
		default:
			return QVariant();
		}
	}

	return QVariant();
}

QVariant AMLightweightScanInfoModel::getThumbnailData(const QModelIndex &index, int role) const
{
	if(role != Qt::DisplayRole && role != Qt::DecorationRole)
		return QVariant();


	AMLightweightScanInfo* info = scanInfo_->at(index.parent().row());

	AMDbThumbnail* thumbnail = info->thumbnailAt(index.row());


	if(!thumbnail)
		return QVariant();


	switch(index.column())
	{
	case 0:
		return thumbnail->subtitle;
	case 1:
		if(role == Qt::DecorationRole){
			QPixmap pixmap;
			pixmap.loadFromData(thumbnail->thumbnail);
			return pixmap;
		}
		break;
	case 2:
		return thumbnail->title;
	}

	return QVariant();
}

void AMLightweightScanInfoModel::onScanInfoAboutToBeAdded(int newIndex)
{
	beginInsertRows(QModelIndex(), newIndex, newIndex);
}

void AMLightweightScanInfoModel::onScanInfoAdded()
{
	endInsertRows();
}

void AMLightweightScanInfoModel::onScanInfoUpdated(int updatedIndex)
{

	emit dataChanged(index(updatedIndex, 0, QModelIndex()), index(updatedIndex, columnCount()-1, QModelIndex()));
}

void AMLightweightScanInfoModel::onScanThumbnailAdded(int scanIndex, int thumbnailIndex)
{
	QModelIndex scanModelIndex = index(scanIndex, 0, QModelIndex());
	QModelIndex thumbnailModelIndexStart = index(thumbnailIndex, 0, scanModelIndex);
	QModelIndex thumbnailModelIndexEnd = index(thumbnailIndex, columnCount(scanModelIndex) -1, scanModelIndex);
	emit dataChanged(thumbnailModelIndexStart, thumbnailModelIndexEnd);
}

void AMLightweightScanInfoModel::onScanThumbnailAboutToBeRemoved(int scanIndex, int oldThumbnailIndex)
{
	QModelIndex scanModelIndex = index(scanIndex, 0, QModelIndex());
	emit beginRemoveRows(scanModelIndex, oldThumbnailIndex, oldThumbnailIndex);
}

void AMLightweightScanInfoModel::onScanThumbnailRemoved()
{
	endRemoveRows();
}

void AMLightweightScanInfoModel::onScanInfoAboutToBeRemoved(int oldIndex)
{
	beginRemoveRows(QModelIndex(), oldIndex, oldIndex);
}

void AMLightweightScanInfoModel::onScanInfoRemoved()
{
	endRemoveRows();
}


