#ifndef AMSCANTREEVIEW_H
#define AMSCANTREEVIEW_H

#include <QTreeView>
#include <QItemDelegate>
class AMScanTreeViewDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	explicit AMScanTreeViewDelegate(QObject *parent = 0);
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class AMScanTreeView : public QTreeView
{
	Q_OBJECT
public:
	explicit AMScanTreeView(QWidget *parent = 0);
	virtual void setModel(QAbstractItemModel *model);
signals:
	
public slots:
	
};

#endif // AMSCANTREEVIEW_H
