#include "AMScanSearchView.h"

AMScanSearchView::AMScanSearchView(QWidget *parent) :
	QWidget(parent)
{
	initDialog();
}

void AMScanSearchView::initDialog()
{
	setWindowTitle(QString("Search Scans"));
}
