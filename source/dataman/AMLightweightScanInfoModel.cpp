#include "AMLightweightScanInfoModel.h"

AMLightweightScanInfoModel::AMLightweightScanInfoModel(AMLightweightScanInfoCollection *scanInfo, QObject *parent) :
	QAbstractItemModel(parent)
{
	scanInfo_ = scanInfo;
}

QVariant AMLightweightScanInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Vertical)
		return QVariant();

	if(role != Qt::DisplayRole)
		return QVariant();

	if(section > columnCount()-1)
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
	if(!index.isValid() || index.row() > rowCount() -1 || index.column() > columnCount() -1)
		return QVariant();

	if(role != Qt::DisplayRole)
		return QVariant();


	AMLightweightScanInfo info = scanInfo_->at(index.row());

	switch (index.column())
	{
	case 0:
		return info.id();
	case 1:
		return info.name();
	case 2:
		return info.number();
	case 3:
		return info.dateTime();
	case 4:
		return info.scanType();
	case 5:
		return info.runName();
	case 6:
		return info.sampleName();
	case 7:
		return info.notes();
	default:
		return QVariant();
	}

}

int AMLightweightScanInfoModel::rowCount(const QModelIndex &) const
{
	return scanInfo_->count();
}

int AMLightweightScanInfoModel::columnCount(const QModelIndex &) const
{
	return 8;
}

QModelIndex AMLightweightScanInfoModel::index(int row, int column, const QModelIndex &) const
{
	return createIndex(row, column);
}

QModelIndex AMLightweightScanInfoModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}
