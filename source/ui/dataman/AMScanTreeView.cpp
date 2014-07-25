#include "AMScanTreeView.h"

AMScanTreeView::AMScanTreeView(QWidget *parent) :
	QTreeView(parent)
{
	setItemDelegate(new AMScanTreeViewDelegate(this));
}

void AMScanTreeView::setModel(QAbstractItemModel *model)
{
	QTreeView::setModel(model);
	setColumnWidth(0, 140);
	setColumnWidth(1, 280);
	setColumnWidth(2, 140);
	setColumnWidth(3, 120);
	setColumnWidth(4, 120);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
}


AMScanTreeViewDelegate::AMScanTreeViewDelegate(QObject *parent)
	:QItemDelegate(parent)
{

}

QSize AMScanTreeViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(!index.parent().isValid())
	{
		return QSize(option.rect.width(), option.fontMetrics.height());
	}
	else
	{
		return QSize(option.rect.width(), 200);
	}
}
