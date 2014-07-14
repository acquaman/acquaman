#include "AMScanDataView.h"
#include "dataman/AMLightweightScanInfoModel.h"
#include "dataman/AMLightweightScanInfoFilterProxyModel.h"
#include "AMAbstractScanDataChildView.h"
#include "ui/util/AMSortFilterWidget.h"
#include "dataman/AMUser.h"
#include "ui/dataman/AMScanTableView.h"
#include "ui/dataman/AMScanThumbnailView.h"
#include "ui/dataman/AMScanTreeView.h"
AMScanDataView::AMScanDataView(AMDatabase *database, QWidget *parent) :
	QWidget(parent)
{
	// Set up data
	AMLightweightScanInfoCollection* scanCollection = new AMLightweightScanInfoCollection(database);
	model_ = new AMLightweightScanInfoModel(scanCollection);
	proxyModel_ = new AMLightweightScanInfoFilterProxyModel();
	proxyModel_->setSourceModel(model_);

	// Widgets
	titleLabel_ = new QLabel(this);
	titleLabel_->setObjectName(QString::fromUtf8("headingLabel_"));
	titleLabel_->setStyleSheet(QString::fromUtf8("font: 20pt \"Lucida Grande\";\n"
	"color: rgb(79, 79, 79);"));
	titleLabel_->setText(QString("%1: Data").arg(AMUser::user()->name()));

	viewButtons_ = new QButtonGroup();


	buttonLayout_ = new QHBoxLayout();
	buttonLayout_->addWidget(titleLabel_);
	buttonLayout_->addStretch();

	searchButton_ = new QToolButton(this);
	QIcon searchIcon;
	searchIcon.addFile(QString::fromUtf8(":/system-search-2.png"), QSize(), QIcon::Normal, QIcon::Off);
	searchButton_->setIcon(searchIcon);
	searchButton_->setCheckable(true);

	sortFilterWidget_ = new AMSortFilterWidget(proxyModel_);
	sortFilterWidget_->addManualColumn("Data Source");
	sortFilterWidget_->setVisible(false);

	stackedWidget_  = new QStackedWidget();

	// Signals, Slots
	connect(searchButton_, SIGNAL(clicked(bool)), this, SLOT(onSearchButtonClicked(bool)));
	connect(viewButtons_, SIGNAL(buttonClicked(int)), stackedWidget_, SLOT(setCurrentIndex(int)));
	connect(sortFilterWidget_, SIGNAL(isCurrentlyFilteredStateChanged(bool)), this, SLOT(onFilterStateChanged(bool)));
	// Layout
	QVBoxLayout* layout = new QVBoxLayout();

	layout->addLayout(buttonLayout_);
	layout->addWidget(searchButton_);
	layout->addWidget(sortFilterWidget_);
	layout->addWidget(stackedWidget_);

	initializeChildViews();

	setLayout(layout);
}

void AMScanDataView::addChildView(QAbstractItemView *childView, const QIcon& icon)
{
	if(!childView)
		return;

	childViews_.append(childView);
	childView->setModel(proxyModel_);

	QToolButton* viewButton = new QToolButton();
	viewButton->setIcon(icon);
	viewButton->setCheckable(true);
	if(viewButtons_->buttons().isEmpty())
		viewButton->setChecked(true);

	buttonLayout_->addWidget(viewButton);
	viewButtons_->addButton(viewButton);
	stackedWidget_->addWidget(childView);
	viewButtons_->setId(viewButton, viewButtons_->buttons().count()-1);
}

void AMScanDataView::initializeChildViews()
{
	// Table Data View
	QIcon tableIcon;
	tableIcon.addFile(QString::fromUtf8(":/22x22/view-list-details-symbolic.png"), QSize(), QIcon::Normal, QIcon::Off);
	tableIcon.addFile(QString::fromUtf8(":/22x22/view-list-details-symbolic_dark.png"), QSize(), QIcon::Normal, QIcon::On);


	addChildView(new AMScanTableView(), tableIcon);

	AMScanTreeView* treeView = new AMScanTreeView();

	QIcon treeIcon;
	treeIcon.addFile(QString::fromUtf8("://22x22/view-list-compact-symbolic.png"), QSize(), QIcon::Normal, QIcon::Off);
	treeIcon.addFile(QString::fromUtf8("://22x22/view-list-compact-symbolic_dark.png"), QSize(), QIcon::Normal, QIcon::On);

	addChildView(treeView, treeIcon);
}

QAbstractItemView *AMScanDataView::currentView()
{
	if(stackedWidget_->currentIndex() < 0)
		return 0;

	return childViews_.at(stackedWidget_->currentIndex());
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
	proxyModel_->setRunId(runId);
}

void AMScanDataView::onSearchButtonClicked(bool checked)
{
	sortFilterWidget_->setVisible(checked);
}

void AMScanDataView::onFilterStateChanged(bool isCurrentlyFiltered)
{
	if(isCurrentlyFiltered)
		titleLabel_->setText(QString("%1: Data (Currently filtered)").arg(AMUser::user()->name()));
	else
		titleLabel_->setText(QString("%1: Data").arg(AMUser::user()->name()));
}


