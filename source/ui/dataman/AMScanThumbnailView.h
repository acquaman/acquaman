#ifndef AMSCANTHUMBNAILVIEW_H
#define AMSCANTHUMBNAILVIEW_H

#include <QAbstractItemView>
#include <QWidget>
class AMScanThumbnailView : public QAbstractItemView {
	Q_OBJECT
public:
	explicit AMScanThumbnailView(QWidget* parent = 0);
	virtual QRect visualRect(const QModelIndex &index) const;
	virtual void scrollTo(const QModelIndex &index, ScrollHint hint);
	virtual QModelIndex indexAt(const QPoint &point) const;
protected:
	virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
	virtual int horizontalOffset() const;
	virtual int verticalOffset() const;
	virtual bool isIndexHidden(const QModelIndex &index) const;
	virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
	virtual QRegion visualRegionForSelection(const QItemSelection &selection) const;

};

#endif // AMSCANTHUMBNAILVIEW_H
