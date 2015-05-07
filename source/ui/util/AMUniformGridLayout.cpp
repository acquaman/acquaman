#include "AMUniformGridLayout.h"

AMUniformGridLayout::AMUniformGridLayout(int cellWidth, int cellHeight, QWidget *parent) :
	QLayout(parent)
{
	cellWidth_ = cellWidth;
	cellHeight_ = cellHeight;
	cellContentAlignment_ = Qt::AlignCenter;
	marginLeft_ = 0;
	marginRight_ = 0;
	marginTop_ = 0;
	marginBottom_ = 0;
}

AMUniformGridLayout::~AMUniformGridLayout()
{
	while(!children_.isEmpty()) {
		QLayoutItem* childToDelete = children_.takeAt(0);
		delete childToDelete;
	}
}

void AMUniformGridLayout::addItem(QLayoutItem *item)
{
	children_.append(item);
}

void AMUniformGridLayout::addItemAt(QLayoutItem *item, int index)
{
	children_.insert(index, item);
}

QSize AMUniformGridLayout::sizeHint() const
{
	return minimumSize();
}

QSize AMUniformGridLayout::minimumSize() const
{
	// If we're empty then we'll define the minimum size as being just our padding
	if(children_.count() == 0) {
		return QSize(marginLeft_ + marginRight_, marginTop_ + marginBottom_);
	}

	// Width = number of cells per row * width of each cell + horizontal margins
	int currentCellsPerRow = cellsPerRow();
	int currentMinWidth = marginLeft_ + (currentCellsPerRow * cellWidth_) + marginRight_;

	// Height = number of rows * height of each cell + horizontal margins
	int currentNumberOfRows = 0;
	if (currentCellsPerRow > 0) {
		currentNumberOfRows = (children_.count()) / currentCellsPerRow;
	}

	int currentMinHeight = marginTop_ + (currentNumberOfRows * cellHeight_) + marginBottom_;

	return QSize(currentMinWidth, currentMinHeight);
}

int AMUniformGridLayout::count() const
{
	return children_.count();
}

int AMUniformGridLayout::layoutItemIndexAt(const QPoint &point)
{
	// Check some pre-requisites (point can't be null, point must be inside this
	// layout, and the cell width and height can't be zero
	if(point.isNull())
		return -1;

	if(point.x() > geometry().width() || point.y() > geometry().height())
		return -1;

	if(point.x() < 0 || point.y() < 0 || cellWidth_ == 0 || cellHeight_ == 0)
		return -1;

	// Calculate the index of the cell under the point
	int cellColumnIndex = point.x() / cellWidth_;
	int cellRowIndex = point.y() / cellHeight_;

	int cellIndex = cellRowIndex * cellsPerRow() + cellColumnIndex;

	// Ensure that the point is within the contents of that cell
	QRect contentRectOfCell = geometryWithinCellAt(cellIndex);

	if(!contentRectOfCell.isValid())
		return -1;

	if(contentRectOfCell.contains(point)) {
		return cellIndex;
	}

	return -1;
}

QList<int> AMUniformGridLayout::layoutItemIndicesWithin(const QRect &rect)
{
	QList<int> returnList;

	for (int iLayoutItem = 0, layoutCount = children_.count();
		 iLayoutItem < layoutCount;
		 ++iLayoutItem) {

		QLayoutItem* itemAt = children_.at(iLayoutItem);
		if(itemAt->geometry().intersects(rect))
			returnList.append(iLayoutItem);
	}

	return returnList;
}

QLayoutItem *AMUniformGridLayout::itemAt(int index) const
{
	if(index >= count()) {
		return 0;
	}

	return children_.at(index);

}

QLayoutItem *AMUniformGridLayout::takeAt(int index)
{
	if(index >= count()) {
		return 0;
	}

	return children_.takeAt(index);
}

void AMUniformGridLayout::setGeometry(const QRect &rectangle)
{
	QLayout::setGeometry(rectangle);

	for(int iCell = 0, cellCount = children_.count();
		iCell < cellCount;
		++iCell) {
		QRect childRect = this->geometryWithinCellAt(iCell);
		if(childRect.isValid()) {
			children_.at(iCell)->setGeometry(childRect);
		}
	}
}

Qt::Alignment AMUniformGridLayout::cellContentAlignment() const
{
	return cellContentAlignment_;
}

void AMUniformGridLayout::setCellContentAlignment(Qt::Alignment cellContentAlignment)
{
	cellContentAlignment_ = cellContentAlignment;
}

int AMUniformGridLayout::marginLeft() const
{
	return marginLeft_;
}

void AMUniformGridLayout::setMarginLeft(int marginLeft)
{
	marginLeft_ = marginLeft;
}

int AMUniformGridLayout::marginRight() const
{
	return marginRight_;
}

void AMUniformGridLayout::setMarginRight(int marginRight)
{
	marginRight_ = marginRight;
}

int AMUniformGridLayout::marginTop() const
{
	return marginTop_;
}

void AMUniformGridLayout::setMarginTop(int marginTop)
{
	marginTop_ = marginTop;
}

int AMUniformGridLayout::marginBottom() const
{
	return marginBottom_;
}

void AMUniformGridLayout::setMarginBotton(int marginBottom)
{
	marginBottom_ = marginBottom;
}

QRect AMUniformGridLayout::geometryWithinCellAt(int cellIndex)
{
	// Ensure there is a child at the provided index
	QLayoutItem* childAtCellIndex = itemAt(cellIndex);

	if(!childAtCellIndex)
		return QRect();

	// Calculate the geomerty of the cell which contains the child
	int currentCellsPerRow = cellsPerRow();
	if(currentCellsPerRow == 0)
		return QRect();

	int gridColumn = cellIndex % currentCellsPerRow;
	int gridRow = cellIndex / currentCellsPerRow;
	QRect currentGeometry = geometry();

	int cellXPosition = currentGeometry.x() + marginLeft_ + (gridColumn * cellWidth_);
	int cellYPosition = currentGeometry.y() + marginTop_ + (gridRow * cellHeight_);

	// Get the size hint from the child in the cell.
	int childWidth = childAtCellIndex->sizeHint().width();
	int childHeight = childAtCellIndex->sizeHint().height();

	// Adjust the position from the cell geometry based on the size of the
	// child, and the content alignment of the cells
	int childXPosition = 0;
	int childYPosition = 0;

	if(Qt::AlignLeft & cellContentAlignment_) {
		// If we're aligned left, the child x = cell x
		childXPosition = cellXPosition;
	} else if(Qt::AlignRight & cellContentAlignment_) {
		// If we're aligned right, the child x = cell x + cell width - child width
		childXPosition = cellXPosition + cellWidth_ - childWidth;
	} else if(Qt::AlignHCenter & cellContentAlignment_) {
		// If we're aligned Hcenter, the child x = cell x + (cell width / 2) - (child width / 2)
		childXPosition = cellXPosition + (cellWidth_ / 2) - (childWidth / 2);
	}

	if(Qt::AlignTop & cellContentAlignment_) {
		// If we're aligned top, the child y = cell y
		childYPosition = cellYPosition;
	} else if (Qt::AlignBottom & cellContentAlignment_) {
		// If we're aligned bottom, the child y = cell y + cell height - childHeight
		childYPosition = cellYPosition + cellHeight_ - childHeight;
	} else if(Qt::AlignVCenter & cellContentAlignment_) {
		// If we're aligned Vcenter, the child y = cell y + (cell height / 2) - (child height / 2)
		childYPosition = cellYPosition + (cellHeight_ / 2) - (childHeight / 2);
	}

	return QRect(childXPosition, childYPosition, childWidth, childHeight);
}

int AMUniformGridLayout::cellsPerRow() const
{
	int totalWidth = geometry().width();
	int widthForCells = totalWidth - marginLeft_ - marginRight_;

	return widthForCells / cellWidth_;

}
