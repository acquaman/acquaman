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
		rv.setHeight(40);
		return rv;
	}

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	static int horizontalSpacing() { return 6; }
	static int horizontalMargin() { return 6; }
	static int verticalMargin() { return 3; }

signals:

public slots:

protected:


};

#endif // AMDETAILEDITEMDELEGATE_H
