#ifndef AMSCANTREEVIEW_H
#define AMSCANTREEVIEW_H

#include <QTreeView>

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
