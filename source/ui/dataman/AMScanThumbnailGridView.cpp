#include "AMScanThumbnailGridView.h"
#include "ui/dataman/AMScanThumnailGridGeometryManager.h"
#include "ui/dataman/AMScanThumbnailGridViewItemDelegate.h"
#include "ui/dataman/AMScanThumbnailGridInputManager.h"

AMScanThumbnailGridView::AMScanThumbnailGridView(QWidget *parent) :
	QAbstractItemView(parent) {
	geometryManager_ = new AMScanThumnailGridGeometryManager(sizeHint().width());
	// Wire up the input manager to this view
	inputManager_ = new AMScanThumbnailGridInputManager();
	connect(inputManager_, SIGNAL(itemSelected(int,QItemSelectionModel::SelectionFlags)), this, SLOT(onItemSelected(int,QItemSelectionModel::SelectionFlags)));
	connect(inputManager_, SIGNAL(selectionExtended(int)), this, SLOT(onSelectionExtended(int)));
	connect(inputManager_, SIGNAL(itemDoubleClicked(int)), this, SLOT(onItemDoubleClicked(int)));
	connect(inputManager_, SIGNAL(dragBegun(int)), this, SLOT(onDragBegun(int)));
	connect(inputManager_, SIGNAL(selectionRectangleChanged(QRect)), this, SLOT(onSelectionRectangleChanged(QRect)));
	connect(inputManager_, SIGNAL(selectionRectangleEnded(QItemSelectionModel::SelectionFlags)), this, SLOT(onSelectionRectangleEnded(QRect,QItemSelectionModel::SelectionFlags)));

	// Have to set mouse tracking in order to receive events when no mouse button is held
	setMouseTracking(true);

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
void AMScanThumbnailGridView::onItemSelected(int itemIndex, QItemSelectionModel::SelectionFlags flags) {
	qDebug() << QString("Item selected at index %1 in slot onItemSelected").arg(itemIndex);
	if(itemIndex < 0)
		return;


	QModelIndex selectedItemIndex = model()->index(itemIndex, 0, QModelIndex());
	selectionModel()->select(selectedItemIndex, QItemSelectionModel::Rows);
}

void AMScanThumbnailGridView::onSelectionExtended(int itemIndex)
{
	qDebug() << QString("Selection extended to %1").arg(itemIndex);
}

void AMScanThumbnailGridView::onItemDoubleClicked(int itemIndex)
{
	qDebug() << QString("Item %1 double clicked").arg(itemIndex);
}

void AMScanThumbnailGridView::onDragBegun(int itemIndex)
{
	qDebug() << QString("Drag of item %1 begun").arg(itemIndex);
}

void AMScanThumbnailGridView::onSelectionRectangleChanged(const QRect &selectionRectangle)
{
}

void AMScanThumbnailGridView::onSelectionRectangleEnded(const QRect &selectionRectangle, QItemSelectionModel::SelectionFlags flags)
{
	qDebug() << QString("Selection rectangle x:%1, y:%2, width:%3, height:%4 ended").arg(selectionRectangle.x()).arg(selectionRectangle.y()).arg(selectionRectangle.width()).arg(selectionRectangle.height());
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
	qDebug() << "Mouse Move event";
		inputManager_->mouseMovedTo(geometryManager_->indexAt(event->pos(), horizontalOffset(), verticalOffset()),
									event->pos().x(), event->pos().y(), event->buttons());

}


void AMScanThumbnailGridView::mouseReleaseEvent(QMouseEvent *event) {

	inputManager_->mouseReleasedAt(event->pos().x(), event->pos().y(), event->buttons(), event->modifiers());
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

