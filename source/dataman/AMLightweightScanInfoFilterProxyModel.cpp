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
