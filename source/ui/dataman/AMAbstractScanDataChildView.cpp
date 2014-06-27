#include "AMAbstractScanDataChildView.h"
AMAbstractScanDataChildView::AMAbstractScanDataChildView(const QString &title, QWidget *parent) :
	QWidget(parent)
{
	model_ = 0;
	title_ = title;
}

void AMAbstractScanDataChildView::setModel(QSortFilterProxyModel *model)
{
	model_ = model;
	onModelChangedImplementation();
}

QString AMAbstractScanDataChildView::title() const
{
	return title_;
}

QIcon AMAbstractScanDataChildView::icon() const
{
	return icon_;
}
