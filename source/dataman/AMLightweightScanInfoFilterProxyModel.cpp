#include "AMLightweightScanInfoFilterProxyModel.h"
#include "database/AMDbObject.h"
#include "AMLightweightScanInfoModel.h"
AMLightweightScanInfoFilterProxyModel::AMLightweightScanInfoFilterProxyModel(QObject *parent) :
	QSortFilterProxyModel(parent)
{
	runId_ = -1;
	experimentId_ = -1;
}

void AMLightweightScanInfoFilterProxyModel::setRunId(int id)
{
	if(experimentId_ != -1)
		experimentId_ = -1;

	runId_ = id;
	invalidateFilter();
}

int AMLightweightScanInfoFilterProxyModel::runId() const
{
	return runId_;
}

const QHash<int, QString> AMLightweightScanInfoFilterProxyModel::runMap() const
{
	QAbstractItemModel* model = sourceModel();
	if(model) {
		AMLightweightScanInfoModel* actualModel =
				qobject_cast<AMLightweightScanInfoModel*>(model);

		if(actualModel)
			return actualModel->runMap();
	}
	return QHash<int, QString>();
}

void AMLightweightScanInfoFilterProxyModel::setExperimentId(int id)
{
	if(runId_ != -1)
		runId_ = -1;

	experimentId_ = id;
	invalidateFilter();
}

int AMLightweightScanInfoFilterProxyModel::experimentId() const
{
	return experimentId_;
}


void AMLightweightScanInfoFilterProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
	QSortFilterProxyModel::setSourceModel(sourceModel);

	if(sourceModel)	{
		AMLightweightScanInfoModel* actualModel =
				qobject_cast<AMLightweightScanInfoModel*>(sourceModel);

		if(actualModel)
			connect(actualModel, SIGNAL(runMapUpdated()), this, SIGNAL(runMapUpdated()));
	}
}

bool AMLightweightScanInfoFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	if(sourceParent.isValid())
		return filterAcceptsThumbnail(sourceRow, sourceParent);
	else
		return filterAcceptsScan(sourceRow, sourceParent);
}


bool AMLightweightScanInfoFilterProxyModel::filterAcceptsThumbnail(int sourceRow, const QModelIndex& parent) const
{
	if(filterKeyColumn() != 8 || filterRegExp().isEmpty())
		return true;

	QModelIndex subTitleIndex = sourceModel()->index(sourceRow, 2, parent);
	QModelIndex titleIndex = sourceModel()->index(sourceRow, 0, parent);
	QString subTitle = sourceModel()->data(subTitleIndex, Qt::DisplayRole).toString();
	QString title = sourceModel()->data(titleIndex, Qt::DisplayRole).toString();

	QRegExp regExp = filterRegExp();

	if (subTitle.contains(regExp))
		return true;

	if(title.contains(regExp))
		return true;

	return false;

}

bool AMLightweightScanInfoFilterProxyModel::filterAcceptsScan(int sourceRow, const QModelIndex &parent) const
{
	if(filterKeyColumn() == 8)
	{
		if(runId() > -1) {
			QModelIndex sourceIndex = sourceModel()->index(sourceRow, 5, parent);
			int sourceRunId = sourceModel()->data(sourceIndex, Qt::UserRole).toInt();

			if(sourceRunId != runId())
				return false;
		} else if(experimentId() > -1) {
			QAbstractItemModel* model = sourceModel();
			if(model) {
				QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, parent);
				AMLightweightScanInfoModel* actualModel =
						qobject_cast<AMLightweightScanInfoModel*>(model);

				if(actualModel)
					return actualModel->belongsToExperiment(sourceIndex, experimentId());
			}
		}

		QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, parent);
		for (int iThumbnail = 0; iThumbnail < sourceModel()->rowCount(sourceIndex); iThumbnail++) {
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
	} else {
		if(runId() > -1) {
			QModelIndex sourceIndex = sourceModel()->index(sourceRow, 5, parent);
			int sourceRunId = sourceModel()->data(sourceIndex, Qt::UserRole).toInt();

			if(sourceRunId != runId())
				return false;
		} else if(experimentId() > -1) {
			QAbstractItemModel* model = sourceModel();
			if(model) {
				QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, parent);
				AMLightweightScanInfoModel* actualModel =
						qobject_cast<AMLightweightScanInfoModel*>(model);

				if(actualModel)
					return actualModel->belongsToExperiment(sourceIndex, experimentId());
			}
		}
	}
	return QSortFilterProxyModel::filterAcceptsRow(sourceRow, parent);
}

