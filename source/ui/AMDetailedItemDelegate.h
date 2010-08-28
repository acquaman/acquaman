#ifndef AMDETAILEDITEMDELEGATE_H
#define AMDETAILEDITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QStyleOptionViewItemV4>
#include <QPainter>
#include <QPixmap>

#include "AMDateTimeUtils.h"

class AMDetailedItemDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:

	enum CloseButtonAction { SignalOnly, RemoveFromModel };

	explicit AMDetailedItemDelegate(QObject *parent = 0);

	/// Re-implemented from QStyledItemDelegate to request the height requested in setItemHeight().
	virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const {
		QSize rv = QStyledItemDelegate::sizeHint(option, index);
		rv.setHeight(height_);
		return rv;
	}

	/// Re-implemented from QStyledItemDelegate to draw too lines of text, the pixmap/decoration, and a close button if closeButtonEnabled().
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	/// Re-implemented from QStyledItemDelegate to check if the close button has been clicked. If closeButtonsEnabled() and one of the close buttons is clicked, it will emit closeButtonClicked() with the index of the item.
	bool editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index );


	static int horizontalSpacing() { return 6; }
	static int horizontalMargin() { return 6; }
	static int verticalMargin() { return 3; }

	/// Set the font used for the text
	void setFont(const QFont& font= QFont("Lucida Grande"));
	/// Set the text color used for the top and bottom lines.
	void setTextColor(const QColor &topLineColor= QColor(47,79,79), const QColor &bottomLineColor = QColor(96,123,139));
	/// Set the font size used for the top and bottom lines
	void setFontSize(int size1=12,int size2=10);
	/// Request a specific \c height for each item
	void setItemHeight(int height=40);

	/// indicates whether close buttons are displayed for each item
	bool closeButtonsEnabled() const { return closeButtonEnabled_; }
	/// indicates whether items are immediately removed and deleted from the model when a close button is clicked
	CloseButtonAction closeButtonAction() const { return closeButtonAction_; }


signals:
	void closeButtonClicked(const QModelIndex& itemIndex);

public slots:
	void setCloseButtonsEnabled(bool closeButtonsOn = true) {
		if( (closeButtonEnabled_ = closeButtonsOn) )
			closeButton_ = QPixmap(":/closeButton.png");
		else
			closeButton_ = QPixmap();
	}
	/// This controls the behaviour when the close buttons are enabled, and the user clicks the close button on an item.  If the \c closeButtonAction is AMDetailedItemDelegate::SignalOnly, then only the closeButtonClicked() signal is emitted with the index of the item.  If the \c closeButtonAction is AMDetailedItemDelegate::RemoveFromModel, then the item will be instantly removed from the model as well.
	void setCloseButtonAction(int closeButtonAction = SignalOnly) {
		closeButtonAction_ = (CloseButtonAction)closeButtonAction;
	}

protected:
	QFont font_;
	QColor color1_;
	QColor color2_;
	int size1_;
	int size2_;
	int height_;

	QPixmap closeButton_;
	bool closeButtonEnabled_;
	mutable QRect closeButtonRect_;

	CloseButtonAction closeButtonAction_;

	QPoint mouseDownPosition_;
};

#endif // AMDETAILEDITEMDELEGATE_H
