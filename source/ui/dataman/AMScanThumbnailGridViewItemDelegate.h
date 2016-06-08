#ifndef AMSCANTHUMBNAILGRIDVIEWITEMDELEGATE_H
#define AMSCANTHUMBNAILGRIDVIEWITEMDELEGATE_H

#include <QItemDelegate>
#include <QPainter>

/**
 * Delegate class for painting the items within an AMScanThumbnailGridView
 */
class AMScanThumbnailGridViewItemDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	/**
	 * Creates an instance of an AMScanThumbnailGridViewItemDelegate
	 * @param parent
	 */
	explicit AMScanThumbnailGridViewItemDelegate(QObject *parent = 0);

	/**
	  * Virtual destructor for AMScanThumbnailGridViewItemDelegate
	  * Ensures the desctructor hierarchy is maintained when subclasses are
	  * created
	  */
	virtual ~AMScanThumbnailGridViewItemDelegate() {}

	/// Paints the data associated with the provided index according to options stored in the given
	/// StyleOptionViewItem
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

    /**
     * Helper method which paints the background for each item
     * @param painter::QPainter ~ The painter with which to draw the background
     * @param option::QStyleOptionViewItem ~ The options which determine the style
     * of the item to draw
     */
    void paintItemBackground(QPainter* painter, const QStyleOptionViewItem &option) const;

protected:

	/**
	 * Helper method which paints the thumbnail pixmap
	 * @param painter::QPainter ~ The painter with which to draw the thumbnail
	 * @param option::QStyleOptionViewItem ~ The options which determine the style
	 * of the item to draw
	 * @param pixmap::QPixmap ~ The thumbnail pixmap to draw
	 */
	void paintThumbnail(QPainter* painter, const QStyleOptionViewItem &option,
					const QPixmap& pixmap) const;

	/**
	 * Helper method which paints the provided text
	 * @param painter::QPainter ~ The painter with which to draw the text
	 * @param option::QStyleOptionViewItem ~ The options which determine the style
	 * of the item to draw
	 * @param string::QString ~ The text to draw
	 */
	void paintText(QPainter* painter, const QStyleOptionViewItem &option,
				   const QString& string) const;

};

#endif // AMSCANTHUMBNAILGRIDVIEWITEMDELEGATE_H
