#ifndef AMDETAILEDITEMDELEGATE_H
#define AMDETAILEDITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QStyleOptionViewItemV4>
#include <QPainter>

class AMDetailedItemDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
	explicit AMDetailedItemDelegate(QObject *parent = 0);

	virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const {
		QSize rv = QStyledItemDelegate::sizeHint(option, index);
		rv.setHeight(height_);
		return rv;
	}

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	static int horizontalSpacing() { return 6; }
	static int horizontalMargin() { return 6; }
	static int verticalMargin() { return 3; }

	void setFont(const QFont& font= QFont("Times"));
	void setTextColor(const QColor &color1= QColor(47,79,79), const QColor &color2 = QColor(96,123,139));
	void setFontSize(int size1=10,int size2=9);
	void setItemHeight(int height=40);
signals:

public slots:

protected:
	QFont font_;
	QColor color1_;
	QColor color2_;
	int size1_;
	int size2_;
	int height_;
};

#endif // AMDETAILEDITEMDELEGATE_H
