#include "AMScanTreeView.h"

AMScanTreeView::AMScanTreeView(QWidget *parent) :
	QTreeView(parent)
{

}

void AMScanTreeView::setModel(QAbstractItemModel *model)
{
	QTreeView::setModel(model);
	setColumnWidth(0, 140);
	setColumnWidth(1, 280);
	setColumnWidth(2, 140);
	setColumnWidth(3, 120);
	setColumnWidth(4, 120);
}
