#ifndef AMSORTFILTERSCANSWIDGET_H
#define AMSORTFILTERSCANSWIDGET_H

#include "AMSortFilterWidget.h"
#include <QComboBox>
#include <QFormLayout>

class AMLightweightScanInfoFilterProxyModel;
class AMSortFilterScansWidget : public AMSortFilterWidget
{
	Q_OBJECT
public:
	explicit AMSortFilterScansWidget(AMLightweightScanInfoFilterProxyModel *model, QWidget *parent = 0);
	
signals:
	
public slots:
protected slots:
protected:
	QComboBox* runComboBox_;
private slots:
	void onRunSelected(int index);
private:
};

#endif // AMSORTFILTERSCANSWIDGET_H
