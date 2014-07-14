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
	if(parent.isValid())
		return filterAcceptsThumbnail(source_row, parent);
	else
		return filterAcceptsScan(source_row, parent);
}


bool AMLightweightScanInfoFilterProxyModel::filterAcceptsThumbnail(int source_row, const QModelIndex& parent) const
{
	if(filterKeyColumn() != 8 || filterRegExp().isEmpty())
		return true;

	QModelIndex subTitleIndex = sourceModel()->index(source_row, 2, parent);
	QModelIndex titleIndex = sourceModel()->index(source_row, 0, parent);
	QString subTitle = sourceModel()->data(subTitleIndex, Qt::DisplayRole).toString();
	QString title = sourceModel()->data(titleIndex, Qt::DisplayRole).toString();

	QRegExp regExp = filterRegExp();

	if (subTitle.contains(regExp))
		return true;

	if(title.contains(regExp))
		return true;

	return false;

}

bool AMLightweightScanInfoFilterProxyModel::filterAcceptsScan(int source_row, const QModelIndex &parent) const
{
	if(filterKeyColumn() == 8)
	{
		if(runId_ > -1)
		{
			QModelIndex sourceIndex = sourceModel()->index(source_row, 5, parent);
			int sourceRunId = sourceModel()->data(sourceIndex, Qt::UserRole).toInt();

			if(sourceRunId != runId())
				return false;
		}

		QModelIndex sourceIndex = sourceModel()->index(source_row, 0, parent);
		for (int iThumbnail = 0; iThumbnail < sourceModel()->rowCount(sourceIndex); iThumbnail++)
		{
			QModelIndex titleIndex = sourceModel()->index(iThumbnail, 0, sourceIndex);
			QModelIndex subtitleIndex = sourceModel()->index(iThumbnail, 2, sourceIndex);
			QVariant titleValue = sourceModel()->data(titleIndex, Qt::DisplayRole);
			QVariant subtitleValue = sourceModel()->data(subtitleIndex, Qt::DisplayRole);

			QRegExp regExp = filterRegExp();

			if(titleValue.toString().contains(regExp))
				return true;

			if(subtitleValue.toString().contains(regExp))
				return true;
		}
		return false;
	}
	else
	{
		if(runId() > -1)
		{
			QModelIndex sourceIndex = sourceModel()->index(source_row, 5, parent);
			int sourceRunId = sourceModel()->data(sourceIndex, Qt::UserRole).toInt();

			if(sourceRunId != runId())
				return false;
		}
	}
	return QSortFilterProxyModel::filterAcceptsRow(source_row, parent);
}
