#include "AMScanThumbnailGridView.h"
#include "ui/dataman/AMScanThumnailGridGeometryManager.h"
#include "ui/dataman/AMScanThumbnailGridViewItemDelegate.h"
#include "ui/dataman/AMScanThumbnailGridInputManager.h"

AMScanThumbnailGridView::AMScanThumbnailGridView(QWidget *parent) :
	QAbstractItemView(parent) {
	geometryManager_ = new AMScanThumnailGridGeometryManager(sizeHint().width());
	// Wire up the input manager to this view
	inputManager_ = new AMScanThumbnailGridInputManager(this);
	connect(inputManager_, SIGNAL(itemSelected(int)), this, SLOT(onItemSelected(int)));

	setItemDelegate(new AMScanThumbnailGridViewItemDelegate(this));
	horizontalScrollBar()->setVisible(false);
	updateScrollBars();


}

QModelIndex AMScanThumbnailGridView::indexAt(const QPoint &point) const {
	if(!model())
		return QModelIndex();

	QPoint adjustedPoint(point.x() + horizontalOffset(),
						 point.y() + verticalOffset());

	int integerIndexAt = geometryManager_->indexAt(adjustedPoint);

	if(integerIndexAt == -1)
		return QModelIndex();

	return model()->index(integerIndexAt, 0, QModelIndex());
}

QRect AMScanThumbnailGridView::visualRect(const QModelIndex &index) const {

	QRect rowContentRect;

	if(index.parent().isValid()) {
		// is  a thumbnail
		rowContentRect = geometryManager_->contentGeometryAt(index.parent().row(),
															 horizontalOffset(),
															 verticalOffset());
	} else {
		rowContentRect = geometryManager_->contentGeometryAt(index.row(),
															 horizontalOffset(),
															 verticalOffset());
	}

	if(!index.parent().isValid()) {
		// Is Scan
		switch(index.column()) {
		case 0:
			return rowContentRect;
		case 1:
			return geometryManager_->scanNameRectangle(rowContentRect);
		case 3:
			return geometryManager_->scanDateRectangle(rowContentRect);

		case 4:
			return geometryManager_->scanTechniqueRectangle(rowContentRect);

		default:
			return QRect();
		}
	} else {
		// Is Thumbnail
		switch(index.column()) {
		case 1:
			return geometryManager_->thumbnailImageRectangle(rowContentRect);
		case 2:
			return geometryManager_->thumbnailTitleRectangle(rowContentRect);

		default:
			return QRect();
		}
	}

}

void AMScanThumbnailGridView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint) {

}
#include <QDebug>
void AMScanThumbnailGridView::onItemSelected(int itemIndex) {
	if(itemIndex < 0)
		return;

	QModelIndex selectedItemIndex = model()->index(itemIndex, 0, QModelIndex());
	selectionModel()->select(selectedItemIndex, QItemSelectionModel::Rows);
	qDebug() << QString("Selecting item at: %1").arg(itemIndex);
}

int AMScanThumbnailGridView::horizontalOffset() const {
	return horizontalScrollBar()->value();
}

bool AMScanThumbnailGridView::isIndexHidden(const QModelIndex &index) const {
	Q_UNUSED(index)
	return false;
}

QModelIndex AMScanThumbnailGridView::moveCursor(QAbstractItemView::CursorAction cursorAction,
											Qt::KeyboardModifiers modifiers) {
	//STUB
	Q_UNUSED(cursorAction)
	Q_UNUSED(modifiers)
	return QModelIndex();
}

void AMScanThumbnailGridView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) {
	QList<int> containedCellIndices = geometryManager_->indicesWithin(rect, horizontalOffset(), verticalOffset());

	if(containedCellIndices.isEmpty())
		return;

	QItemSelection totalSelectedItems;

	for(int iContainedCell = 0, containedCellCount = containedCellIndices.count();
		iContainedCell < containedCellCount;
		++iContainedCell) {

		QModelIndex indexToAdd = model()->index(containedCellIndices.at(iContainedCell),
												1, QModelIndex());

		totalSelectedItems.select(indexToAdd, indexToAdd);
	}

	selectionModel()->select(totalSelectedItems, command | QItemSelectionModel::Rows);

}

int AMScanThumbnailGridView::verticalOffset() const {
	return verticalScrollBar()->value();
}

QRegion AMScanThumbnailGridView::visualRegionForSelection(const QItemSelection &selection) const {
	Q_UNUSED(selection)
	return QRegion(viewport()->rect());
}

void AMScanThumbnailGridView::paintEvent(QPaintEvent *event) {
	QAbstractItemView::paintEvent(event);

	QPainter painter(viewport());

	// Paint background
	painter.fillRect(viewport()->rect(), palette().color(QPalette::Normal, QPalette::Window));

	QStyleOptionViewItem option = viewOptions();

	for(int iDataRow = 0; iDataRow < model()->rowCount(); iDataRow++) {

		// Paint the scanSerialNumber (we've hijacked this column in the delegate to
		// paint the background for this item
		QModelIndex scanSerialIndex = model()->index(iDataRow, 0, QModelIndex());
		bool isSelected = selectionModel()->isSelected(scanSerialIndex);
		option.state = isSelected ? QStyle::State_Selected : QStyle::State_Enabled ;

		option.rect = visualRect(scanSerialIndex);
		itemDelegate()->paint(&painter, option, scanSerialIndex);

		// Paint the thumbnail
		QModelIndex scanThumbnailIndex = model()->index(0, 1, scanSerialIndex);
		option.rect = visualRect(scanThumbnailIndex);
		itemDelegate()->paint(&painter, option, scanThumbnailIndex);

		// Paint the scan name
		QModelIndex scanNameIndex = model()->index(iDataRow, 1, QModelIndex());
		option.rect = visualRect(scanNameIndex);
		itemDelegate()->paint(&painter, option, scanNameIndex);

		// Paint the scan date
		QModelIndex scanDateIndex = model()->index(iDataRow, 3, QModelIndex());
		option.rect = visualRect(scanDateIndex);
		itemDelegate()->paint(&painter, option, scanDateIndex);

		// Paint the scan type
		QModelIndex scanTypeIndex = model()->index(iDataRow, 4, QModelIndex());
		option.rect = visualRect(scanTypeIndex);
		itemDelegate()->paint(&painter, option, scanTypeIndex);

		// paint the thumbnail title
		QModelIndex thumbnailTitleIndex = model()->index(0, 2, scanSerialIndex);
		option.rect = visualRect(thumbnailTitleIndex);
		itemDelegate()->paint(&painter, option, thumbnailTitleIndex);
	}

}

void AMScanThumbnailGridView::resizeEvent(QResizeEvent *event) {
	QAbstractItemView::resizeEvent(event);
	geometryManager_->setWidth(viewport()->width());
	updateScrollBars();
}

void AMScanThumbnailGridView::mousePressEvent(QMouseEvent *event) {
	inputManager_->mouseDownAt(geometryManager_->indexAt(event->pos(), horizontalOffset(), verticalOffset()),
							   event->pos().x(), event->pos().y(), event->buttons());
}

void AMScanThumbnailGridView::mouseMoveEvent(QMouseEvent *event) {

		inputManager_->mouseMovedTo(geometryManager_->indexAt(event->pos(), horizontalOffset(), verticalOffset()),
									event->pos().x(), event->pos().y(), event->buttons());

}

void AMScanThumbnailGridView::mouseReleaseEvent(QMouseEvent *event) {
	inputManager_->mouseReleasedAt(geometryManager_->indexAt(event->pos(), horizontalOffset(), verticalOffset()),
								   event->pos().x(), event->pos().y(), event->buttons());
}

void AMScanThumbnailGridView::updateScrollBars() {

	if(!model()) {
		verticalScrollBar()->setVisible(false);
		return;
	}

	verticalScrollBar()->setVisible(true);
	int currentGeometryHeight = geometryManager_->heightFor(model()->rowCount());

	if(currentGeometryHeight < verticalOffset())
		verticalScrollBar()->setValue(0);

	verticalScrollBar()->setMaximum(currentGeometryHeight);
	verticalScrollBar()->setSingleStep(15);
	verticalScrollBar()->setPageStep(geometryManager_->cellHeight());
}

