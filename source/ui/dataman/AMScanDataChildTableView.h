#ifndef AMSCANDATACHILDLISTVIEW_H
#define AMSCANDATACHILDLISTVIEW_H

#include <QTableView>
#include <QHBoxLayout>
#include <QHeaderView>

#include "AMAbstractScanDataChildView.h"
/// Class to display scan data in a TableView from within a AMScanDataView
class AMScanDataChildTableView : public AMAbstractScanDataChildView
{
	Q_OBJECT
private:
	/// A QTableView to display the scan data
	QTableView* scanDataTable_;
public:
	/// Creates an instance of an AMScanDataChildTableView with no model
	AMScanDataChildTableView(QWidget* parent = 0);
protected:
	/// Sets the model of the QTableView to be that of this class. Causes a resize of the columns in the QTableView
	/// to match that of the data found in the model
	virtual void onModelChangedImplementation();
};

#endif // AMSCANDATACHILDLISTVIEW_H
