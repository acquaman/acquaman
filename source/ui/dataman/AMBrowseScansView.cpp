#include "AMBrowseScansView.h"
#include "ui/util/AMSortFilterScansWidget.h"
#include "ui/dataman/AMScanTableView.h"
#include "ui/dataman/AMScanThumbnailGridView.h"
#include "ui/dataman/AMScanTreeView.h"
#include "dataman/AMLightweightScanInfoCollection.h"
#include "dataman/AMLightweightScanInfoModel.h"
#include "dataman/AMLightweightScanInfoFilterProxyModel.h"

AMBrowseScansView::AMBrowseScansView(AMDatabase *database, QWidget *parent) :
	QWidget(parent)
{
	// Set up data
	AMLightweightScanInfoCollection* scanCollection = new AMLightweightScanInfoCollection(database);
	model_ = new AMLightweightScanInfoModel(scanCollection, this);
	proxyModel_ = new AMLightweightScanInfoFilterProxyModel(this);
	proxyModel_->setSourceModel(model_);

	// Set up widgets
	viewButtonGroup_ = new QButtonGroup(this);
	childViewDisplayArea_ = new QStackedWidget();
	selectedItemCount_ = new QLabel();
	sortFilterWidget_ = new AMSortFilterScansWidget(proxyModel_);
	sortFilterWidget_->addManualColumn("Data Source");

	// Set up layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	viewButtonLayout_ = new QHBoxLayout();
	viewButtonLayout_->addStretch();
	mainLayout->addLayout(viewButtonLayout_);
	mainLayout->addWidget(sortFilterWidget_);
	mainLayout->addWidget(childViewDisplayArea_);
	mainLayout->addWidget(selectedItemCount_);

	setLayout(mainLayout);

	initializeChildViews();

	// Signal/Slot connections
	connect(viewButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onChildViewChanged(int)));
	connect(sortFilterWidget_, SIGNAL(filterChanged(bool)), this, SIGNAL(filterChanged(bool)));
}

QList<QUrl> AMBrowseScansView::selectedItems()
{
	if(!currentView())
		return QList<QUrl>();

	QList<QUrl> returnList;

	QModelIndexList indices = currentView()->selectionModel()->selectedIndexes();

	for (int iSelectedItem = 0; iSelectedItem < indices.count(); iSelectedItem++)
	{
		if(indices.at(iSelectedItem).column() == 0)
		{
			QUrl scanUrl = model_->rowToUrl(indices.at(iSelectedItem));
			if(!returnList.contains(scanUrl))
				returnList.append(scanUrl);
		}
	}


	return returnList;
}

int AMBrowseScansView::numberOfSelectedItems()
{
	if(!currentView())
		return 0;

	return currentView()->selectionModel()->selectedRows().count();
}

void AMBrowseScansView::setRunId(int runId)
{
	proxyModel_->setRunId(runId);
	sortFilterWidget_->setRunId(runId);
}

void AMBrowseScansView::setExperimentId(int experimentId)
{
	proxyModel_->setExperimentId(experimentId);
}

int AMBrowseScansView::numberOfDisplayedRows()
{
	return proxyModel_->rowCount();
}

void AMBrowseScansView::clearSelection()
{
	for(int iChildView = 0, number = childViews_.count(); iChildView < number; iChildView++)
	{
		childViews_.at(iChildView)->selectionModel()->clearSelection();
	}
}

void AMBrowseScansView::selectAll()
{
	currentView()->selectAll();
}

void AMBrowseScansView::onChildViewSelectionChanged()
{
	selectedItemCount_->setText(QString("%1 item(s) selected").arg(numberOfSelectedItems()));
	emit selectionChanged();
}

void AMBrowseScansView::onChildViewChanged(int newIndex)
{
	childViewDisplayArea_->setCurrentIndex(newIndex);
	onChildViewSelectionChanged();
}

void AMBrowseScansView::onContextMenuRequested(const QPoint &point)
{
	QPoint pointInWidget(point.x() + childViewDisplayArea_->x(), point.y() + childViewDisplayArea_->y());
	emit childContextMenuRequested(pointInWidget);
}

void AMBrowseScansView::addChildView(QAbstractItemView *childView, const QIcon &icon)
{
	if(!childView)
		return;

	childViews_.append(childView);
	childView->setModel(proxyModel_);

	QToolButton* viewButton = new QToolButton();
	viewButton->setIcon(icon);
	viewButton->setCheckable(true);
	if(viewButtonGroup_->buttons().isEmpty())
		viewButton->setChecked(true);

	viewButtonLayout_->addWidget(viewButton);
	viewButtonGroup_->addButton(viewButton);
	childViewDisplayArea_->addWidget(childView);
	viewButtonGroup_->setId(viewButton, viewButtonGroup_->buttons().count()-1);

	childView->setContextMenuPolicy(Qt::CustomContextMenu);
	childView->setDragEnabled(true);
	childView->setDropIndicatorShown(true);
	connect(childView, SIGNAL(doubleClicked(const QModelIndex&)), this, SIGNAL(childViewDoubleClicked(const QModelIndex&)));
	connect(childView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onContextMenuRequested(const QPoint&)));
	connect(childView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onChildViewSelectionChanged()));
}

void AMBrowseScansView::initializeChildViews()
{
	// Scan Thumbnail View
	AMScanThumbnailGridView* thumbnailView = new AMScanThumbnailGridView(this);
	thumbnailView->setContextMenuPolicy(Qt::CustomContextMenu);
	QIcon thumbnailIcon;
	thumbnailIcon.addFile(QString::fromUtf8(":/22x22/view-list-icons-symbolic.png"), QSize(), QIcon::Normal, QIcon::Off);
	thumbnailIcon.addFile(QString::fromUtf8(":/22x22/view-list-icons-symbolic_dark.png"), QSize(), QIcon::Normal, QIcon::On);

	addChildView(thumbnailView, thumbnailIcon);

	// Table Data View
	AMScanTableView* tableView = new AMScanTableView(this);
	tableView->setContextMenuPolicy(Qt::CustomContextMenu);
	QIcon tableIcon;
	tableIcon.addFile(QString::fromUtf8(":/22x22/view-list-details-symbolic.png"), QSize(), QIcon::Normal, QIcon::Off);
	tableIcon.addFile(QString::fromUtf8(":/22x22/view-list-details-symbolic_dark.png"), QSize(), QIcon::Normal, QIcon::On);


	addChildView(tableView, tableIcon);

	// Tree Data View
	AMScanTreeView* treeView = new AMScanTreeView(this);
	treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	QIcon treeIcon;
	treeIcon.addFile(QString::fromUtf8(":/22x22/view-list-compact-symbolic.png"), QSize(), QIcon::Normal, QIcon::Off);
	treeIcon.addFile(QString::fromUtf8(":/22x22/view-list-compact-symbolic_dark.png"), QSize(), QIcon::Normal, QIcon::On);

	addChildView(treeView, treeIcon);
}

QAbstractItemView *AMBrowseScansView::currentView()
{
	if(childViewDisplayArea_->currentIndex() < 0)
		return 0;

	return childViews_.at(childViewDisplayArea_->currentIndex());
}

void AMBrowseScansView::setFilterRegExp(const QString &pattern)
{
	proxyModel_->setFilterRegExp(pattern);
	sortFilterWidget_->setFilterRegExp(pattern);
}

void AMBrowseScansView::setFilterKeyColumn(int column)
{
	proxyModel_->setFilterKeyColumn(column);
	sortFilterWidget_->setFilterKeyColumn(column);
}
