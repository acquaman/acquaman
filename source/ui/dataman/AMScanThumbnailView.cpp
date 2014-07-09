#include "AMScanThumbnailView.h"

AMScanThumbnailView::AMScanThumbnailView(QWidget *parent)
	:QAbstractItemView(parent)
{

}

QRect AMScanThumbnailView::visualRect(const QModelIndex &index) const
{

}

void AMScanThumbnailView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
	return;
}

QModelIndex AMScanThumbnailView::indexAt(const QPoint &point) const
{
	return QModelIndex();
}

QModelIndex AMScanThumbnailView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
	return QModelIndex();
}

int AMScanThumbnailView::horizontalOffset() const
{
	return -1;
}

int AMScanThumbnailView::verticalOffset() const
{
	return -1;
}

bool AMScanThumbnailView::isIndexHidden(const QModelIndex &index) const
{
	return false;
}

void AMScanThumbnailView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
	return;
}

QRegion AMScanThumbnailView::visualRegionForSelection(const QItemSelection &selection) const
{
	return QRegion();
}


