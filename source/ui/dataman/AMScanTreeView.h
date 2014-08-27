#ifndef AMSCANTREEVIEW_H
#define AMSCANTREEVIEW_H

#include <QTreeView>
#include <QItemDelegate>
#include <QScrollBar>
#include <QPainter>
/**
 * Class which overrides the standard QItemDelegate, which behaved oddly when given fields with very
 * long strings for their contents. This version sets a standard sizeHint height for scans, and a separate
 * one for thumbnails
 */
class AMScanTreeViewDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	explicit AMScanTreeViewDelegate(QObject *parent = 0);
	/// Returns the sizeHint for rows depending on the validity of the index's parent:
	///  - for scans the height is the font height
	///  - for thumbnails the height is a standard 200
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	/// Reimplementation of QItemDelegate::paint(). Paints the passed QModelIndex->data()
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

/**
 * Class which overrides the standard QTreeView. Simple sets a few convenience properties that we
 * know we're going to need.
 */
class AMScanTreeView : public QTreeView
{
	Q_OBJECT
public:
	explicit AMScanTreeView(QWidget *parent = 0);
	/// Sets the model by calling base class implementation, then sets standard column widths now
	/// we know the data is in place
	void setModel(QAbstractItemModel *model);
	
};

#endif // AMSCANTREEVIEW_H
