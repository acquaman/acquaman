#ifndef AMDETAILEDITEMDELEGATE_H
#define AMDETAILEDITEMDELEGATE_H

#include "ui/AMCloseItemDelegate.h"
#include <QStyleOptionViewItemV4>
#include <QPainter>
#include <QPixmap>

#include "AMDateTimeUtils.h"

class AMDetailedItemDelegate : public AMCloseItemDelegate
{
Q_OBJECT
public:

	explicit AMDetailedItemDelegate(QObject *parent = 0);

	/// Re-implemented from QStyledItemDelegate to request the height requested in setItemHeight().
	virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const {
		QSize rv = AMCloseItemDelegate::sizeHint(option, index);
		rv.setHeight(height_);
		return rv;
	}

	/// Re-implemented from QStyledItemDelegate to draw two lines of text, the pixmap/decoration, and a close button if closeButtonEnabled().
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;


	/// Spacing between decoration and text
	static int horizontalSpacing() { return 6; }
	/// Horizontal margin before the decoration on the left side
	static int horizontalMargin() { return 6; }
	/// Vertical margin before above the decoration and text
	static int verticalMargin() { return 3; }

	/// Set the font used for the text
	void setFont(const QFont& font= QFont("Lucida Grande"));
	/// Set the text color used for the top and bottom lines.
	void setTextColor(const QColor &topLineColor= QColor(47,79,79), const QColor &bottomLineColor = QColor(96,123,139));
	/// Set the font size used for the top and bottom lines
	void setFontSize(int size1=12,int size2=10);
	/// Request a specific \c height for each item
	void setItemHeight(int height=40);


protected:
	QFont font_, fontItalic_;
	QColor color1_;
	QColor color2_;
	int size1_;
	int size2_;
	int height_;
};

#endif // AMDETAILEDITEMDELEGATE_H
