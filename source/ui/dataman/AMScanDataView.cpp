#include "AMScanDataView.h"
#include "dataman/AMLightweightScanInfoModel.h"
#include "AMAbstractScanDataChildView.h"
#include "ui/util/AMSortFilterWidget.h"

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
	searchButtonLayout->addWidget(searchButton_);
	connect(searchButton_, SIGNAL(clicked()), this, SLOT(onSearchButtonClicked()));

	sortFilterWidget_ = new AMSortFilterWidget(proxyModel_);
	sortFilterWidget_->setWindowFlags(Qt::FramelessWindowHint);
	connect(sortFilterWidget_, SIGNAL(lostFocus()), this, SLOT(onSortFilterWidgetLostFocus()));

	QVBoxLayout* layout = new QVBoxLayout();

	layout->addLayout(buttonLayout_);
	layout->addLayout(searchButtonLayout);
	layout->addWidget(tabWidget_);

	initializeChildViews();

	setLayout(layout);
}

void AMScanDataView::addChildView(QAbstractItemView *childView, const QString& title)
{
	if(!childView)
		return;

	childViews_.append(childView);
	childView->setModel(proxyModel_);
	tabWidget_->addTab(childView, title);
}

void AMScanDataView::initializeChildViews()
{
	/// Table Data View
	QTableView* scanDataTable = new QTableView();

	addChildView(scanDataTable, "Table View");

	scanDataTable->setAlternatingRowColors(true);
	scanDataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	scanDataTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	scanDataTable->verticalHeader()->setVisible(false);
	scanDataTable->setShowGrid(false);
	QFont font = scanDataTable->font();
	font.setPointSize(10);
	scanDataTable->setFont(font);
	scanDataTable->setContextMenuPolicy(Qt::CustomContextMenu);
	scanDataTable->setSortingEnabled(true);

	scanDataTable->setColumnWidth(0, 80);
	scanDataTable->setColumnWidth(1, 200);
	scanDataTable->setColumnWidth(2, 40);
	scanDataTable->setColumnWidth(3, 110);
	scanDataTable->setColumnWidth(4, 110);
	scanDataTable->setColumnWidth(5, 60);
	scanDataTable->setColumnWidth(6, 110);
	scanDataTable->setColumnWidth(7, 200);
}

QAbstractItemView *AMScanDataView::currentView()
{
	if(tabWidget_->currentIndex() < 0)
		return 0;

	return childViews_.at(tabWidget_->currentIndex());
}

QList<QUrl> AMScanDataView::selectedItems()
{
	if(!currentView())
		return QList<QUrl>();

	QList<QUrl> returnList;

	QModelIndexList indices = currentView()->selectionModel()->selectedIndexes();

	for (int iSelectedItem = 0; iSelectedItem < indices.count(); iSelectedItem++)
	{
		returnList.append(model_->rowToUrl(indices.at(iSelectedItem).row()));
	}


	return returnList;
}

int AMScanDataView::numberOfSelectedItems()
{
	if(!currentView())
		return 0;

	return currentView()->selectionModel()->selectedIndexes().count();
}

void AMScanDataView::showRun(int runId)
{
}

void AMScanDataView::showExperiment(int experimentId)
{
}

void AMScanDataView::setOrganizeMode(int mode)
{
}

void AMScanDataView::setViewMode(int mode)
{
}

void AMScanDataView::expandAll()
{
}

void AMScanDataView::collapseAll()
{
}

void AMScanDataView::setItemSize(int newItemSize)
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
