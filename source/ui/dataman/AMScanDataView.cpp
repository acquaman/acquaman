#include "AMScanDataView.h"

#include "AMScanDataChildTableView.h"

#include <QApplication>
AMScanDataView::AMScanDataView(AMDatabase *database, QWidget *parent) :
	QWidget(parent)
{
	AMLightweightScanInfoCollection* scanCollection = new AMLightweightScanInfoCollection(database);
	model_ = new AMLightweightScanInfoModel(scanCollection);
	proxyModel_ = new QSortFilterProxyModel();
	proxyModel_->setSourceModel(model_);

	tabWidget_  = new QTabWidget();
	buttonLayout_ = new QHBoxLayout();
	QHBoxLayout* searchButtonLayout = new QHBoxLayout();
	searchButton_ = new QToolButton();
	searchButton_->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirLinkIcon));
	searchButtonLayout->addWidget(searchButton_);
	connect(searchButton_, SIGNAL(clicked()), this, SLOT(onSearchButtonClicked()));

	sortFilterWidget_ = new AMSortFilterWidget(proxyModel_);
	sortFilterWidget_->setWindowFlags(Qt::FramelessWindowHint);
	connect(sortFilterWidget_, SIGNAL(lostFocus()), this, SLOT(onSortFilterWidgetLostFocus()));

	QVBoxLayout* layout = new QVBoxLayout();

	layout->addLayout(buttonLayout_);
	layout->addLayout(searchButtonLayout);
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

void AMScanDataView::showRun(int runId)
{
}

void AMScanDataView::onSearchButtonClicked()
{
	QPoint globalPosition = mapToGlobal(searchButton_->rect().bottomRight());
	sortFilterWidget_->move(globalPosition.x() + (width() / 2), globalPosition.y() );

	if(sortFilterWidget_->isHidden())
	{
		sortFilterWidget_->show();
		sortFilterWidget_->raise();
		sortFilterWidget_->setFocus();
	}

}

void AMScanDataView::onSortFilterWidgetLostFocus()
{
	sortFilterWidget_->hide();
}
