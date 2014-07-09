#include "AMScanThumbnailView.h"
#include "dataman/AMLightweightScanInfoFilterProxyModel.h"
#include "ui/dataman/AMFlowGraphicsLayout.h"
#include <QHBoxLayout>
#include <QGraphicsWidget>
AMScanThumbnailView::AMScanThumbnailView(QWidget *parent)
	:QAbstractItemView(parent)
{
	centralView_ = new QGraphicsView();
	QHBoxLayout* layout = new QHBoxLayout();

	layout->addWidget(centralView_);
	setLayout(layout);
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

void AMScanThumbnailView::setModel(QAbstractItemModel *model)
{
	QAbstractItemView::setModel(model);

	AMLightweightScanInfoFilterProxyModel* actualModel = qobject_cast<AMLightweightScanInfoFilterProxyModel*>(model);

	if(!actualModel)
		return;

	QGraphicsScene* scene = new QGraphicsScene(this);
	AMFlowGraphicsLayout* layout = new AMFlowGraphicsLayout();
	for(int iScanInfo = 0; iScanInfo < actualModel->rowCount(); iScanInfo++)
	{
		AMScanThumbnailViewItem* thumbnailView = new AMScanThumbnailViewItem(iScanInfo);
		thumbnailView->setModel(actualModel);
		layout->addItem(thumbnailView);
	}
	QGraphicsWidget* widget = new QGraphicsWidget();
	widget->setLayout(layout);
	scene->addItem(widget);
	centralView_->setScene(scene);
}

QModelIndex AMScanThumbnailView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
	return QModelIndex();
}

int AMScanThumbnailView::horizontalOffset() const
{
	return 50;
}

int AMScanThumbnailView::verticalOffset() const
{
	return 50;
}

bool AMScanThumbnailView::isIndexHidden(const QModelIndex &) const
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

////////////////////////////////////////////
// Defintions for AMScanThumbnailViewItem //
////////////////////////////////////////////
AMScanThumbnailViewItem::AMScanThumbnailViewItem(int viewIndex, QGraphicsItem *parent)
	: QGraphicsItem(parent), QGraphicsLayoutItem()
{
	viewIndex_ = viewIndex;
	currentThumbnail_ = 0;
	thumbnailIndex_ = 0;
	model_ = 0;
	thumbnailCount_ = 0;
	width_ = 240;
	height_ = 180;
}

void AMScanThumbnailViewItem::setModel(AMLightweightScanInfoFilterProxyModel *model)
{
	model_ = model;
	currentThumbnail_ = model_->thumbnailAt(viewIndex_, thumbnailIndex_);
	thumbnailCount_ = model_->thumbnailCount(viewIndex_);
}

void AMScanThumbnailViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	// Are we selected? Draw a selection background
	//if(isSelected()) {
		painter->save();
		painter->setRenderHint(QPainter::Antialiasing);
		painter->setCompositionMode(QPainter::CompositionMode_Source);
		painter->setPen(Qt::NoPen);
		//painter->setBrush(QColor(56,117,215));
		painter->setBrush(QColor(25,87,172));
		//painter->setBrush(QColor(48,107,186));
		painter->drawRoundedRect(QRectF(0, 0, width_+20, height_ + 20), 5, 5);
		painter->restore();
	//}
}

void AMScanThumbnailViewItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
}

void AMScanThumbnailViewItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
}

void AMScanThumbnailViewItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
}

QRectF AMScanThumbnailViewItem::boundingRect() const
{
	return QRectF(0, 0, 260, 200);
}

QSizeF AMScanThumbnailViewItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
	return QSizeF(QSize(width_, height_));
}

