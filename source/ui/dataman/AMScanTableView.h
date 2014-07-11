#ifndef AMSCANTABLEVIEW_H
#define AMSCANTABLEVIEW_H

#include <QTableView>
#include <QHeaderView>

class AMScanTableView : public QTableView
{
	Q_OBJECT
public:
	explicit AMScanTableView(QWidget *parent = 0);
	virtual void setModel(QAbstractItemModel *model);
signals:
	
public slots:
	
};

#endif // AMSCANTABLEVIEW_H
