#include "AMScanThumbnailView.h"





//////////////////////////////////////////////////
// Definitions for AMScanThumbnailViewItemDelegate
//////////////////////////////////////////////////
AMScanThumbnailViewItemDelegate::AMScanThumbnailViewItemDelegate(QObject *parent)
	: QItemDelegate(parent)
{

}

void AMScanThumbnailViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(option.rect.isEmpty())
		return;

	if(!index.parent().isValid())
	{
		if(option.state & QStyle::State_Selected)
		{
			painter->save();
			painter->setPen(option.palette.color(QPalette::Normal, QPalette::HighlightedText));
			painter->drawText(option.rect, index.data(Qt::DisplayRole).toString());
			painter->restore();
		}
		else
		{
			painter->drawText(option.rect, index.data(Qt::DisplayRole).toString());
		}

	}
	else
	{
		if(index.column() == 1)
		{
			QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
			painter->drawPixmap(option.rect, pixmap);

		}
		else
		{

				if(option.state & QStyle::State_Selected)
				{
					painter->save();
					painter->setPen(option.palette.color(QPalette::Normal, QPalette::HighlightedText));
					painter->drawText(option.rect, index.data(Qt::DisplayRole).toString());
					painter->restore();
				}
				else
				{
					painter->drawText(option.rect, index.data(Qt::DisplayRole).toString());
				}
		}
	}


}

//////////////////////////////////////
// Definitions for AMScanThumbnailView
//////////////////////////////////////
AMScanThumbnailView::AMScanThumbnailView(QWidget *parent)
	:QAbstractItemView(parent)
{
	itemDimensions_.setWidth(260);
	itemDimensions_.setHeight(260);

	imageDimensions_.setWidth(240);
	imageDimensions_.setHeight(180);

	gridDimensions_.setWidth(itemDimensions_.width());;
	gridDimensions_.setHeight(itemDimensions_.height());

	setAutoScroll(false);
	setItemDelegate(new AMScanThumbnailViewItemDelegate(this));

	QPalette newPalette = palette();
	newPalette.setColor(QPalette::Normal, QPalette::Window, QColor(216,221,227));

	setPalette(newPalette);
	selectionRubberBand_ = 0;

	setMouseTracking(true);
	currentGridRowMouseOver_ = -1;
	connect(&hoverTimer_, SIGNAL(timeout()), this, SLOT(onTimerTimout()));
}

QModelIndex AMScanThumbnailView::indexAt(const QPoint &point) const
{

	// offsetPoint is the passed point adjusted for the current scroll offsets
	QPoint offsetPoint(point.x() + horizontalOffset(), point.y() + verticalOffset());
	int itemsPerGridRow = width() / gridDimensions().width();

	// Perform integer division on the X and Y by the item widths and heights
	// in order to obtain the position in grid terms
	int xGridIndexOfPoint = offsetPoint.x() / gridDimensions().width();
	int yGridIndexOfPoint = offsetPoint.y() / gridDimensions().height();

	// Because each grid row contains multiple data rows, we need to map the grid row and column
	// to a data row
	int rowIndex = xGridIndexOfPoint + (itemsPerGridRow * yGridIndexOfPoint);

	return model()->index(rowIndex, 1, QModelIndex());
}

void AMScanThumbnailView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
	// Note: currently ignores the scroll hint. Places the corresponding scan as close to the top of
	// the view as is possible
	Q_UNUSED(hint);

	if(!index.isValid())
		return;

	// If the passed index refers to a thumbnail (i.e. its parent is valid) then we need to
	// scrollTo the index of the parent scan
	if(index.parent().isValid())
	{
		scrollTo(index.parent(), hint);
		return;
	}

	// The current grid row index and column index are obtained by doing
	// integer division and modulus of the data row
	int dataRow = index.row();
	int itemsPerGridRow = width() / gridDimensions().width();
	int gridColumn = dataRow % itemsPerGridRow;
	int gridRow = dataRow / itemsPerGridRow;

	// The grid row index and column index are converted into absolute points
	// within the view
	int absGridPosX = gridColumn * gridDimensions().width();
	int absGridPosY = gridRow * gridDimensions().height();

	verticalScrollBar()->setValue(absGridPosY);
	horizontalScrollBar()->setValue(absGridPosX);

}

QRect AMScanThumbnailView::visualRect(const QModelIndex &index) const
{
	if(!index.isValid())
		return QRect();

	QModelIndex topLevelIndex = index;
	// If the passed index refers to a thumbnail, (i.e. its parent is valid)
	// then we need to iterate backwards up the hierarchy, until we're looking at a scan
	while(topLevelIndex.parent().isValid())
		topLevelIndex = topLevelIndex.parent();


	QRect thumbnailRectangle = rectangleGridRow(topLevelIndex.row());

	if(thumbnailRectangle.isNull())
		return QRect();

	// Determine which piece of data we're looking at, and get the rectangle for that:
	// * If the parent of the original index is valid, this is a thumbnail
	if(index.parent().isValid())
	{
		switch (index.column())
		{
		case 1:
			return getImageRectangle(thumbnailRectangle);

		case 2:
			return getTitleRectangle(thumbnailRectangle);
		default:
			return QRect();
		}
	}
	// * If the parent is not valid, we're looking at a scan
	else
	{
		if(index.column() == 1)
			return getScanNameRectangle(thumbnailRectangle);
		else
			return thumbnailRectangle;
	}
}

void AMScanThumbnailView::setModel(QAbstractItemModel *newModel)
{
	if(model())
	{
		disconnect(model(), SIGNAL(rowsRemoved(const QModelIndex& parent, int start, int end)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
	}

	QAbstractItemView::setModel(newModel);
	connect(model(), SIGNAL(rowsRemoved(const QModelIndex&, int, int )), this, SLOT(rowsRemoved(QModelIndex,int,int)));
	updateScrollBars();
}

QRect AMScanThumbnailView::rectangleGridRow(int row) const
{
	// The current grid row index and column index are obtained by doing
	// integer division and modulus of the data row
	int itemsPerGridRow = width() / gridDimensions().width();
	int gridColumn = row % itemsPerGridRow;
	int gridRow = row / itemsPerGridRow;

	// The grid row index and column index are converted into absolute points
	// within the view
	int absGridPosX = gridColumn * gridDimensions().width();
	int absGridPosY = gridRow * gridDimensions().height();

	// These points	are then converted into points relative to the current scroll offset of
	// the view
	int relGridPosX = absGridPosX - horizontalOffset();
	int relGridPosY = absGridPosY - verticalOffset();

	// If either of the relative positions is < 0 or > the dimensions of the view, then it
	// is outside of the current visible space, and we need to return an invalid QRect
	if(relGridPosX+ gridDimensions().height() < 0 || relGridPosX > width() || relGridPosY + gridDimensions().width() < 0 || relGridPosY > height())
		return QRect();


	return QRect(relGridPosX, relGridPosY, gridDimensions().width(), gridDimensions().height());
}

QSize AMScanThumbnailView::gridDimensions() const
{
	return gridDimensions_;
}

int AMScanThumbnailView::horizontalOffset() const
{
	return horizontalScrollBar()->value();
}

bool AMScanThumbnailView::isIndexHidden(const QModelIndex &index) const
{
	Q_UNUSED(index);
	return false;
}

QModelIndex AMScanThumbnailView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
	Q_UNUSED(cursorAction);
	Q_UNUSED(modifiers);
	return QModelIndex();
}


void AMScanThumbnailView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
	QModelIndex startIndex = indexAt(rect.topLeft());
	QModelIndex endIndex = indexAt(rect.bottomRight());

	if(!startIndex.isValid() || !endIndex.isValid())
		selectionModel()->select(QModelIndex(), command);
	else
	{
		QItemSelection totalSelectedItems;

		for(int iCurrentSelection = startIndex.row(); iCurrentSelection <= endIndex.row(); iCurrentSelection++)
		{
			QModelIndex currentSelectionModelIndex = model()->index(iCurrentSelection, 0, QModelIndex());
			QRect currentItemRect = visualRect(currentSelectionModelIndex);

			if(currentItemRect.intersects(rect))
			{

				totalSelectedItems.select(currentSelectionModelIndex, currentSelectionModelIndex);
			}

		}

		selectionModel()->select(totalSelectedItems, command);
	}

}

int AMScanThumbnailView::verticalOffset() const
{
	return verticalScrollBar()->value();
}

QRegion AMScanThumbnailView::visualRegionForSelection(const QItemSelection &selection) const
{
	Q_UNUSED(selection)
	return QRegion(viewport()->rect());
}

void AMScanThumbnailView::paintEvent(QPaintEvent *pe)
{
	QAbstractItemView::paintEvent(pe);

	QPainter painter(viewport());
	painter.fillRect(viewport()->rect(), palette().color(QPalette::Normal, QPalette::Window));

	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

	QStyleOptionViewItem option = viewOptions();

	for(int iDataRow = 0; iDataRow < model()->rowCount(); iDataRow++)
	{
		QModelIndex scanNameIndex = model()->index(iDataRow, 1);
		QModelIndex selectionRowIndex = model()->index(iDataRow, 0);
		QRect dataRowRectangle = rectangleGridRow(scanNameIndex.row());
		QRect containerRectangle(dataRowRectangle.x() + 10, dataRowRectangle.y() + 10, dataRowRectangle.width()-20, dataRowRectangle.height()-20);
		bool isSelected = selectionModel()->isSelected(selectionRowIndex);
		option.state = isSelected ? QStyle::State_Selected : QStyle::State_Enabled ;
		if(!containerRectangle.isEmpty())
		{
			painter.save();
			if(isSelected)
			{
				painter.setBrush(palette().brush(QPalette::Normal, QPalette::Highlight));
				painter.setPen(palette().color(QPalette::Normal, QPalette::HighlightedText));
			}
			else
			{
				painter.setBrush(QBrush(Qt::white));
				painter.setPen(QPen(QBrush(QColor(150,160,168)), 2));
			}
			painter.drawRoundedRect(containerRectangle, 5, 5);
			painter.restore();
		}


		if(model()->rowCount(scanNameIndex) > 0)
		{

			int currentThumbnailIndex = rowCurrentDisplayedThumbnailMap_.value(scanNameIndex.row(), 0);
			QModelIndex thumbnailIndex = model()->index(currentThumbnailIndex, 1, scanNameIndex);
			option.rect = visualRect(thumbnailIndex);
			itemDelegate()->paint(&painter, option, thumbnailIndex);

			QModelIndex titleIndex = model()->index(currentThumbnailIndex, 2, scanNameIndex);
			option.rect = visualRect(titleIndex);
			itemDelegate()->paint(&painter, option, titleIndex);
		}



		option.rect = visualRect(scanNameIndex);
		itemDelegate()->paint(&painter, option, scanNameIndex);

	}
}


void AMScanThumbnailView::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	int itemsPerGridRow = viewport()->width() / (itemDimensions_.width());
	gridDimensions_.setWidth(viewport()->width() / itemsPerGridRow);
	gridDimensions_.setHeight(itemDimensions_.height());

	updateScrollBars();
}

void AMScanThumbnailView::updateScrollBars()
{
	if(!model())
	{
		verticalScrollBar()->setVisible(false);
		return;
	}

	int itemsPerRow = viewport()->width() / (itemDimensions_.width());
	int totalItems = model()->rowCount();
	int numberOfRows = totalItems / itemsPerRow;

	if(numberOfRows*gridDimensions().height() < verticalOffset())
		verticalScrollBar()->setValue(0);
	verticalScrollBar()->setMaximum(numberOfRows * gridDimensions().height());
	verticalScrollBar()->setSingleStep(5);
	verticalScrollBar()->setPageStep(gridDimensions().height());

}

void AMScanThumbnailView::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton || selectionModel()->selectedIndexes().count() < 2)
	{
		if(!selectionRubberBand_)
			selectionRubberBand_ = new QRubberBand(QRubberBand::Rectangle, viewport());

		rubberBandStart_  = event->pos();
		selectionRubberBand_->setGeometry(QRect(rubberBandStart_, QSize()));
		selectionRubberBand_->show();
		QAbstractItemView::mousePressEvent(event);
	}
}

void AMScanThumbnailView::mouseMoveEvent(QMouseEvent *event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		if(selectionRubberBand_)
		{
			QItemSelectionModel::SelectionFlags commandFlags = QItemSelectionModel::ClearAndSelect;
			if(event->modifiers()&Qt::ControlModifier || event->modifiers()&Qt::ShiftModifier)
				commandFlags = QItemSelectionModel::ToggleCurrent;
			selectionRubberBand_->setGeometry(QRect(rubberBandStart_, event->pos()).normalized());
			setSelection(QRect(selectionRubberBand_->pos().x(), selectionRubberBand_->pos().y(), selectionRubberBand_->width() +1, selectionRubberBand_->height()+1), commandFlags);
		}
	}

	if(indexAt(QPoint(event->x(), event->y())).row() != currentGridRowMouseOver_)
	{
		isMouseHovering_ = false;
		currentGridRowMouseOver_ = indexAt(QPoint(event->x(), event->y())).row();
		hoverTimer_.start(1000);
	}
	else if(isMouseHovering_)
	{
		QModelIndex currentHoveringIndex = indexAt(QPoint(event->x(), event->y()));
		if(!currentHoveringIndex.isValid())
			return;
		QRect currentIndexRect = visualRect(currentHoveringIndex);
		QPoint posInsideRect(event->x() - currentIndexRect.x(), event->y() - currentIndexRect.y());

		int thumbnailCount = model()->rowCount(currentHoveringIndex);
		if(thumbnailCount == 0)
			return;
		int widthOfEachThumbnail = gridDimensions().width() / thumbnailCount;
		int currentThumbnailIndexToShow = 0;
		if (posInsideRect.x() > 0)
			currentThumbnailIndexToShow = posInsideRect.x() / widthOfEachThumbnail;

		if(currentThumbnailIndexToShow != rowCurrentDisplayedThumbnailMap_.value(currentHoveringIndex.row()))
		{
			rowCurrentDisplayedThumbnailMap_.remove(currentHoveringIndex.row());
			rowCurrentDisplayedThumbnailMap_.insert(currentHoveringIndex.row(), currentThumbnailIndexToShow);

			QRegion regionForIndex(viewport()->rect());

			setDirtyRegion(regionForIndex);
		}
	}

}

void AMScanThumbnailView::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		if(!selectionRubberBand_->isHidden())
		{
			QItemSelectionModel::SelectionFlags commandFlags;
			if(event->modifiers() & Qt::ControlModifier)
				commandFlags = QItemSelectionModel::ToggleCurrent;
			else
				commandFlags = QItemSelectionModel::ClearAndSelect;

			if(selectionRubberBand_->width() == 0 && selectionRubberBand_->height() == 0)
				setSelection(QRect(rubberBandStart_.x(), rubberBandStart_.y(), 1, 1), commandFlags);
			selectionRubberBand_->hide();
			rubberBandStart_.setX(0);
			rubberBandStart_.setY(0);
		}
	}
}

void AMScanThumbnailView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	rowCurrentDisplayedThumbnailMap_.clear();
	updateScrollBars();
	viewport()->update();
	QAbstractItemView::dataChanged(topLeft, bottomRight);
}

void AMScanThumbnailView::rowsInserted(const QModelIndex &parent, int start, int end)
{
	rowCurrentDisplayedThumbnailMap_.clear();
	updateScrollBars();
	viewport()->update();
	QAbstractItemView::rowsInserted(parent, start, end);
}

void AMScanThumbnailView::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	rowCurrentDisplayedThumbnailMap_.clear();
	updateScrollBars();
	viewport()->update();
	QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
}



QRect AMScanThumbnailView::getScanNameRectangle(const QRect &thumbnailRectangle) const
{
	return QRect(thumbnailRectangle.x() + 15, thumbnailRectangle.y() + 15, thumbnailRectangle.width() - 30, fontMetrics().height());
}

QRect AMScanThumbnailView::getImageRectangle(const QRect &thumbnailRectangle) const
{
	int paddingWidth = (thumbnailRectangle.width() - imageDimensions_.width()) / 2;
	int paddingHeight = (thumbnailRectangle.height() - imageDimensions_.height()) / 2;
	return QRect(thumbnailRectangle.x() + paddingWidth, thumbnailRectangle.y() + paddingHeight , imageDimensions_.width(), imageDimensions_.height());
}

QRect AMScanThumbnailView::getTitleRectangle(const QRect &thumbnailRectangle) const
{
	return QRect(thumbnailRectangle.x() + 15, thumbnailRectangle.y() + thumbnailRectangle.height() - fontMetrics().height() - 15, thumbnailRectangle.width() - 30, fontMetrics().height());
}


void AMScanThumbnailView::onTimerTimout()
{
	isMouseHovering_ = true;

}



