#ifndef AMSCANTHUMBNAILVIEW_H
#define AMSCANTHUMBNAILVIEW_H

#include <QAbstractItemView>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
class AMLightweightScanInfoFilterProxyModel;
class AMDbThumbnail;

class AMScanThumbnailViewItem : public QGraphicsItem, public QGraphicsLayoutItem {
private:
	AMLightweightScanInfoFilterProxyModel* model_;
	AMDbThumbnail* currentThumbnail_;
	int thumbnailIndex_;
	int viewIndex_;
	int thumbnailCount_;
	int width_;
	int height_;
public:
	explicit AMScanThumbnailViewItem(int viewIndex, QGraphicsItem* parent = 0);
	void setModel(AMLightweightScanInfoFilterProxyModel* model);
protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	QRectF boundingRect() const;
	QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;
};

class AMScanThumbnailView : public QAbstractItemView {
	Q_OBJECT
private:
	QGraphicsView* centralView_;
public:
	explicit AMScanThumbnailView(QWidget* parent = 0);
	virtual QRect visualRect(const QModelIndex &index) const;
	virtual void scrollTo(const QModelIndex &index, ScrollHint hint);
	virtual QModelIndex indexAt(const QPoint &point) const;
	virtual void setModel(QAbstractItemModel *model);
protected:
	virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
	virtual int horizontalOffset() const;
	virtual int verticalOffset() const;
	virtual bool isIndexHidden(const QModelIndex &) const;
	virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
	virtual QRegion visualRegionForSelection(const QItemSelection &selection) const;

};



#endif // AMSCANTHUMBNAILVIEW_H
