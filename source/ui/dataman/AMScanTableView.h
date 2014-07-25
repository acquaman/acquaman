#ifndef AMSCANTABLEVIEW_H
#define AMSCANTABLEVIEW_H

#include <QTableView>
#include <QHeaderView>

/**
 * Class which overrides the Qt standard QTableView. Simple sets a few properties on
 * construction, and when the model is set, for convenience.
 */
class AMScanTableView : public QTableView
{
	Q_OBJECT
public:
	explicit AMScanTableView(QWidget *parent = 0);
	/// Sets the model by calling base class implementation, then sets standard column widths now
	/// we know the data is in place
	virtual void setModel(QAbstractItemModel *model);
signals:
	
public slots:
	
};

#endif // AMSCANTABLEVIEW_H
