#include "AMWindowPaneProxyModel.h"
#include "ui/AMWindowPaneModel.h"

AMWindowPaneProxyModel::AMWindowPaneProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{

}

AMWindowPaneProxyModel::~AMWindowPaneProxyModel()
{

}

void AMWindowPaneProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);

    AMWindowPaneModel *windowPaneModel = qobject_cast<AMWindowPaneModel*>(sourceModel);

    if (windowPaneModel) {
	connect( windowPaneModel, SIGNAL(visibilityChanged(QWidget*,bool)), this, SLOT(invalidate()) );
	connect( windowPaneModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(invalidate()) );
	connect( windowPaneModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(invalidate()) );
	connect( windowPaneModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(invalidate()) );

	invalidate();
    }
}

#include <QDebug>
bool AMWindowPaneProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool result = false;

    AMWindowPaneModel *windowPaneModel = qobject_cast<AMWindowPaneModel*>(sourceModel());

    if (windowPaneModel) {
	QVariant data = windowPaneModel->data(windowPaneModel->index(sourceRow, 0, sourceParent), AMWindowPaneModel::IsVisibleRole);

	if (data.isValid())
	    result = data.toBool();

	qDebug() << "\n\nWindow pane for index" << sourceRow << "is visible:" << (result ? "Yes" : "No");
    }

    return result;
}
