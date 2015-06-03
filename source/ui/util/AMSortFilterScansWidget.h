#ifndef AMSORTFILTERSCANSWIDGET_H
#define AMSORTFILTERSCANSWIDGET_H

#include "AMSortFilterWidget.h"
#include <QComboBox>
#include <QFormLayout>
#include <QSet>

/**
  * A class which specializes the generic AMSortFilterWidget for the filtering
  * of an AMLightweightScanInfoFilterProxyModel. Adds a run combo box for the
  * filtering of scans based on the selected run
  */
class AMLightweightScanInfoFilterProxyModel;
class AMSortFilterScansWidget : public AMSortFilterWidget
{
	Q_OBJECT
public:
	/**
	  * Creates an instance of an AMSortFilterScansWidget which manages the filtering
	  * of the passed AMLightweightScanInfoFilterProxyModel
	  */
	explicit AMSortFilterScansWidget(AMLightweightScanInfoFilterProxyModel *model, QWidget *parent = 0);
	
	/**
	  * Sets the current run id against which the filter model will match scans.
	  * A value of -1 for the passed runId will ensure scans from all runs meet
	  * the filter criteria
	  */
	void setRunId(int runId);
signals:
	
public slots:
protected slots:
protected:
	QComboBox* runComboBox_;

private slots:
	/**
	  * Refreshes the runs which are displayed in the combo box
	  */
	void refreshRunComboBox();
	/**
	  * Ensures that the current selected run in the combo box matches that
	  * of the passed index
	  */
	void onRunSelected(int index);
private:
	AMLightweightScanInfoFilterProxyModel* model_;
};

#endif // AMSORTFILTERSCANSWIDGET_H
