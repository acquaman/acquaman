#include "AMGridFlowGeometryManager.h"

AMGridFlowGeometryManager::AMGridFlowGeometryManager(int width, int cellWidth, int cellHeight, int itemWidth, int itemHeight, Qt::Alignment cellContentAlignment)
{
	width_ = width;
	cellWidth_ = cellWidth;
	cellHeight_ = cellHeight;
	itemWidth_ = itemWidth;
	itemHeight_ = itemHeight;
	marginLeft_ = 3;
	marginRight_ = 3;
	marginTop_ = 3;
	marginBottom_ = 3;
	cellContentAlignment_ = cellContentAlignment;
}

int AMGridFlowGeometryManager::width() const
{
	return width_;
}

int AMGridFlowGeometryManager::heightFor(int itemCount) const
{
	int currentCellsPerRow = cellsPerRow();

	if(currentCellsPerRow == 0)
		return 0;

	int currentRowCount = itemCount / currentCellsPerRow;

	return marginTop_ + (currentRowCount * cellHeight_) + marginBottom_;
}

void AMGridFlowGeometryManager::setWidth(int width)
{
	width_ = width;
}

int AMGridFlowGeometryManager::cellWidth() const
{
	return cellWidth_;
}

void AMGridFlowGeometryManager::setCellWidth(int cellWidth)
{
	cellWidth_ = cellWidth;
}

int AMGridFlowGeometryManager::cellHeight() const
{
	return cellHeight_;
}

void AMGridFlowGeometryManager::setCellHeight(int cellHeight)
{
	cellHeight_ = cellHeight;
}

int AMGridFlowGeometryManager::itemWidth() const
{
	return itemWidth_;
}

int AMGridFlowGeometryManager::itemHeight() const
{
	return itemHeight_;
}

int AMGridFlowGeometryManager::marginLeft() const
{
	return marginLeft_;
}

void AMGridFlowGeometryManager::setMarginLeft(int marginLeft)
{
	marginLeft_ = marginLeft;
}

int AMGridFlowGeometryManager::marginRight() const
{
	return marginRight_;
}

void AMGridFlowGeometryManager::setMarginRight(int marginRight)
{
	marginRight_ = marginRight;
}

int AMGridFlowGeometryManager::marginTop() const
{
	return marginTop_;
}

void AMGridFlowGeometryManager::setMarginTop(int marginTop)
{
	marginTop_ = marginTop;
}

int AMGridFlowGeometryManager::marginBottom() const
{
	return marginBottom_;
}

void AMGridFlowGeometryManager::setMarginBottom(int marginBottom)
{
	marginBottom_ = marginBottom;
}

Qt::Alignment AMGridFlowGeometryManager::cellContentAlignment() const
{
	return cellContentAlignment_;
}

void AMGridFlowGeometryManager::setCellContentAlignment(Qt::Alignment cellContentAlignment)
{
	cellContentAlignment_ = cellContentAlignment;
}

int AMGridFlowGeometryManager::contentIndexAt(const QPoint &point, int horizontalOffset, int verticalOffset) const
{
	if(point.isNull())
		return -1;

	int cellIndex = cellIndexAt(point, horizontalOffset, verticalOffset);

	// Get the rectangle of the item in the cell at the located index
	QRect itemRectangle = contentGeometryAt(cellIndex);

	// if the item rectangle contains our point, return the cellIndex
	if(itemRectangle.contains(QPoint(point.x() + horizontalOffset, point.y() + verticalOffset)))
		return cellIndex;
	else
		// otherwise return -1
		return -1;
}

int AMGridFlowGeometryManager::cellIndexAt(const QPoint &point, int horizontalOffset, int verticalOffset) const
{
	if(point.isNull())
		return -1;

	// Calculate the index of the cell under the point
	int cellColumnIndex = (point.x() + horizontalOffset - marginLeft_) / cellWidth_;
	int cellRowIndex = (point.y() + verticalOffset - marginTop_) / cellHeight_;

	return (cellRowIndex * cellsPerRow() + cellColumnIndex);
}

QList<int> AMGridFlowGeometryManager::indicesWithin(const QRect &rect, int horizontalOffset, int verticalOffset) const
{
	QList<int> returnList;

	QRect offsetRect = QRect(rect.x() + horizontalOffset,
							 rect.y() + verticalOffset,
							 rect.width(),
							 rect.height());

	int firstIndex = cellIndexAt(offsetRect.topLeft());
	int lastIndex = cellIndexAt(offsetRect.bottomRight());

	for(int iCellIndex = firstIndex ;iCellIndex <= lastIndex;
		++iCellIndex) {

		QRect currentCellGeometry = contentGeometryAt(iCellIndex);

		if(offsetRect.intersects(currentCellGeometry))
			returnList.append(iCellIndex);
	}

	return returnList;
}

QRect AMGridFlowGeometryManager::cellGeometryAt(int index, int horizontalOffset,
												int verticalOffset) const
{
	if(index < 0)
		return QRect();

	int currentCellsPerRow = cellsPerRow();

	if(currentCellsPerRow == 0)
		return QRect();

	int gridColumnIndex = index % currentCellsPerRow;
	int gridRowIndex = index / currentCellsPerRow;

	int cellXPosition = marginLeft_ + (gridColumnIndex * cellWidth_) - horizontalOffset;
	int cellYPosition = marginTop_ + (gridRowIndex * cellHeight_) - verticalOffset;

	return QRect(cellXPosition, cellYPosition, cellWidth_, cellHeight_);
}

QRect AMGridFlowGeometryManager::cellGeometryAt(const QPoint &point, int horizontalOffset,
												int verticalOffset) const
{
	if(point.isNull())
		return QRect();

	// Calculate the index of the cell under the point
	int cellColumnIndex = (point.x() - marginLeft_) / cellWidth_ - horizontalOffset;
	int cellRowIndex = (point.y() - marginTop_) / cellHeight_ - verticalOffset;

	int cellIndex = cellRowIndex * cellsPerRow() + cellColumnIndex;

	return cellGeometryAt(cellIndex);
}

QRect AMGridFlowGeometryManager::contentGeometryAt(int index, int horizontalOffset, int verticalOffset) const
{
	return contentGeometryFromCell(cellGeometryAt(index, horizontalOffset, verticalOffset));
}

QRect AMGridFlowGeometryManager::contentGeometryAt(const QPoint &point, int horizontalOffset, int verticalOffset) const
{
	return contentGeometryFromCell(cellGeometryAt(point, horizontalOffset, verticalOffset));
}

int AMGridFlowGeometryManager::cellsPerRow() const
{
	int cellsPerRow = 0;

	if(cellWidth_ < 1)
		return cellsPerRow;

	// total space for cells is the width minus both margins
	int widthForCells = width_ - marginLeft_ - marginRight_;

	if(widthForCells <= 0)
		return cellsPerRow;


	cellsPerRow = widthForCells / cellWidth_;

	return cellsPerRow;
}

QRect AMGridFlowGeometryManager::contentGeometryFromCell(const QRect &cellGeometry) const
{
	if(!cellGeometry.isValid() || cellGeometry.isEmpty() || cellGeometry.isNull())
		return QRect();

	int itemXPosition = 0;
	int itemYPosition = 0;

	int cellXPosition = cellGeometry.x();
	int cellYPosition = cellGeometry.y();

	if(Qt::AlignLeft & cellContentAlignment_) {
		// If we're aligned left, the item x = cell x
		itemXPosition = cellXPosition;
	} else if(Qt::AlignRight & cellContentAlignment_) {
		// If we're aligned right, the item x = cell x + cell width - item width
		itemXPosition = cellXPosition + cellWidth_ - itemWidth_;
	} else if(Qt::AlignHCenter & cellContentAlignment_) {
		// If we're aligned Hcenter, the item x = cell x + (cell width / 2) - (item width / 2)
		itemXPosition = cellXPosition + (cellWidth_ / 2) - (itemWidth_ / 2);
	}

	if(Qt::AlignTop & cellContentAlignment_) {
		// If we're aligned top, the item y = cell y
		itemYPosition = cellYPosition;
	} else if (Qt::AlignBottom & cellContentAlignment_) {
		// If we're aligned bottom, the item y = cell y + cell height - item height
		itemYPosition = cellYPosition + cellHeight_ - itemHeight_;
	} else if(Qt::AlignVCenter & cellContentAlignment_) {
		// If we're aligned Vcenter, the item y = cell y + (cell height / 2) - (item height / 2)
		itemYPosition = cellYPosition + (cellHeight_ / 2) - (itemHeight_ / 2);
	}

	return QRect(itemXPosition, itemYPosition, itemWidth_, itemHeight_);

}


