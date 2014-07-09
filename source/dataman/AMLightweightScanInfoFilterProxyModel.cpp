#include "AMLightweightScanInfoFilterProxyModel.h"
#include "database/AMDbObject.h"
#include "AMLightweightScanInfoModel.h"
AMLightweightScanInfoFilterProxyModel::AMLightweightScanInfoFilterProxyModel(QObject *parent) :
	QSortFilterProxyModel(parent)
{
	runId_ = -1;
}

void AMLightweightScanInfoFilterProxyModel::setRunId(int id)
{
	runId_ = id;
	invalidateFilter();
}

int AMLightweightScanInfoFilterProxyModel::runId() const
{
	return runId_;
}

AMDbThumbnail *AMLightweightScanInfoFilterProxyModel::thumbnailAt(int row, int thumbnailIndex)
{
	QModelIndex filteredIndex = index(row, thumbnailIndex);

	AMLightweightScanInfoModel* backgroundModel = qobject_cast<AMLightweightScanInfoModel*>(sourceModel());

	if(!backgroundModel)
		return 0;

	return backgroundModel->thumbnailAt(filteredIndex.row(), filteredIndex.column());
}

int AMLightweightScanInfoFilterProxyModel::thumbnailCount(int row)
{
	QModelIndex filteredIndex = index(row, 0);

	AMLightweightScanInfoModel* backgroundModel = qobject_cast<AMLightweightScanInfoModel*>(sourceModel());

	if(!backgroundModel)
		return 0;

	return backgroundModel->thumbnailCount(filteredIndex.row());
}

bool AMLightweightScanInfoFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& parent) const
{
	if(runId_ > -1)
	{
		QModelIndex sourceIndex = sourceModel()->index(source_row, 5);

		int sourceRunId = sourceModel()->data(sourceIndex, Qt::UserRole).toInt();

		if(sourceRunId != runId_)
			return false;
	}

	return QSortFilterProxyModel::filterAcceptsRow(source_row, parent);
}
