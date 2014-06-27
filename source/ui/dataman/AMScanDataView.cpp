#include "AMScanDataView.h"

#include "AMScanDataChildTableView.h"
AMScanDataView::AMScanDataView(AMDatabase *database, QWidget *parent) :
	QWidget(parent)
{
	AMLightweightScanInfoCollection* scanCollection = new AMLightweightScanInfoCollection(database);
	model_ = new AMLightweightScanInfoModel(scanCollection);
	proxyModel_ = new QSortFilterProxyModel();
	proxyModel_->setSourceModel(model_);

	tabWidget_  = new QTabWidget();
	buttonLayout_ = new QHBoxLayout();

	QVBoxLayout* layout = new QVBoxLayout();

	layout->addLayout(buttonLayout_);
	layout->addWidget(tabWidget_);

	addChildView(new AMScanDataChildTableView());

	setLayout(layout);
}

void AMScanDataView::addChildView(AMAbstractScanDataChildView *childView)
{
	childView->setModel(proxyModel_);
	tabWidget_->addTab(childView, childView->title());
	QToolButton* toolButton = new QToolButton();
	toolButton->setObjectName(childView->title());
	toolButton->setIcon(childView->icon());
	toolButton->setIconSize(QSize(22, 22));
	toolButton->setCheckable(true);
	buttonLayout_->addWidget(toolButton);
}
