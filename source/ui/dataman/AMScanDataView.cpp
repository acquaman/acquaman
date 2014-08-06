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


	QIcon searchIcon;
	searchIcon.addFile(QString::fromUtf8(":/system-search-2.png"), QSize(), QIcon::Normal, QIcon::Off);

	sortFilterWidget_ = new AMSortFilterWidget(proxyModel_);
	sortFilterWidget_->addManualColumn("Data Source");

	stackedWidget_  = new QStackedWidget();

	// Action bar Widgets
	QHBoxLayout* actionButtonLayout = new QHBoxLayout();

	selectedItemsCount_ = new QLabel();
	selectedItemsCount_->setVisible(false);


	QIcon editButtonIcon;
	QString actionButtonStyle = QString::fromUtf8("QToolButton:hover {\n"
												  "	border-top: 1px solid transparent;\n"
												  "}\n"
												  "\n"
												  "QToolButton:pressed {\n"
												  "border-top: 3px solid transparent;\n"
												  "}\n"
												  "\n"
												  "QToolButton {\n"
												  "border: none;\n"
												  "}\n"
												  "\n"
												  "");

	editButtonIcon.addFile(QString::fromUtf8(":/32x32/edit-find-replace.png"), QSize(), QIcon::Normal, QIcon::Off);

	editButton_ = new QToolButton();
	editButton_->setIcon(editButtonIcon);
	editButton_->setIconSize(QSize(32, 32));
	editButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	editButton_->setAutoRaise(true);
	editButton_->setStyleSheet(actionButtonStyle);
	editButton_->setEnabled(false);
	editButton_->setToolTip("View Scans in Separate Windows");
	actionButtonLayout->addWidget(editButton_);

	QIcon compareButtonIcon;
	compareButtonIcon.addFile(QString::fromUtf8(":/32x32/preferences-desktop-theme.png"), QSize(), QIcon::Normal, QIcon::Off);

	compareButton_ = new QToolButton();
	compareButton_->setIcon(compareButtonIcon);
	compareButton_->setIconSize(QSize(32, 32));
	compareButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	compareButton_->setAutoRaise(true);
	compareButton_->setStyleSheet(actionButtonStyle);
	compareButton_->setEnabled(false);
	compareButton_->setToolTip("View Scans in Same Window");
	actionButtonLayout->addWidget(compareButton_);

	QIcon exportButtonIcon;
	exportButtonIcon.addFile(QString::fromUtf8(":/32x32/system-file-manager.png"), QSize(), QIcon::Normal, QIcon::Off);

	exportButton_ = new QToolButton();
	exportButton_->setIcon(exportButtonIcon);
	exportButton_->setIconSize(QSize(32, 32));
	exportButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	exportButton_->setAutoRaise(true);
	exportButton_->setStyleSheet(actionButtonStyle);
	exportButton_->setEnabled(false);
	exportButton_->setToolTip("Export Selected Scan(s)");
	actionButtonLayout->addWidget(exportButton_);

	QIcon configButtonIcon;
	configButtonIcon.addFile(QString::fromUtf8(":/32x32/hammer-wrench.png"), QSize(), QIcon::Normal, QIcon::Off);

	configButton_ = new QToolButton();
	configButton_->setIcon(configButtonIcon);
	configButton_->setIconSize(QSize(32, 32));
	configButton_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	configButton_->setAutoRaise(true);
	configButton_->setStyleSheet(actionButtonStyle);
	configButton_->setEnabled(false);
	configButton_->setToolTip("Show Scan Config(s)");
	actionButtonLayout->addWidget(configButton_);
	actionButtonLayout->addStretch();
	actionButtonLayout->addWidget(selectedItemsCount_);

	connect(editButton_, SIGNAL(clicked()), this, SLOT(onEditScan()));
	connect(compareButton_, SIGNAL(clicked()), this, SLOT(onCompareScans()));
	connect(exportButton_, SIGNAL(clicked()), this, SLOT(onExportScans()));
	connect(configButton_, SIGNAL(clicked()), this, SLOT(onShowScanConfiguration()));

	// Context menu
	contextMenu_  = new QMenu(this);
	QAction* editScan = contextMenu_->addAction("Edit");
	QAction* compareScans = contextMenu_->addAction("Compare");
	QAction* exportScans = contextMenu_->addAction("Export");
	QAction* viewScanConfig = contextMenu_->addAction("Show Scan Configuration");
	QAction* fixCDFile = contextMenu_->addAction("Fix CDF");
	contextMenu_->addSeparator();
	QAction* selectAll = contextMenu_->addAction("Select All");
	QAction* clearSelection = contextMenu_->addAction("Clear Selection");

	connect(editScan, SIGNAL(triggered()), this, SLOT(onEditScan()));
	connect(compareScans, SIGNAL(triggered()), this, SLOT(onCompareScans()));
	connect(exportScans, SIGNAL(triggered()), this, SLOT(onExportScans()));
	connect(viewScanConfig, SIGNAL(triggered()), this, SLOT(onShowScanConfiguration()));
	connect(fixCDFile, SIGNAL(triggered()), this, SLOT(onFixCDF()));
	connect(selectAll, SIGNAL(triggered()), this, SLOT(onSelectAll()));
	connect(clearSelection, SIGNAL(triggered()),this, SLOT(onClearSelection()));

	// Signals, Slots
	connect(viewButtons_, SIGNAL(buttonClicked(int)), this, SLOT(onChildViewChanged(int)));
	connect(sortFilterWidget_, SIGNAL(filterChanged(bool)), this, SLOT(onFilterChanged(bool)));

	// Layout
	QVBoxLayout* layout = new QVBoxLayout();

	layout->addLayout(buttonLayout_);

	layout->addWidget(sortFilterWidget_);
	layout->addWidget(stackedWidget_);
	layout->addLayout(actionButtonLayout);
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

	childView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(childView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onChildViewDoubleClicked()));
	connect(childView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	connect(childView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onChildViewSelectionChanged()));
}

void AMScanDataView::initializeChildViews()
{

	// Scan Thumbnail View
	AMScanThumbnailView* thumbnailView = new AMScanThumbnailView();

	QIcon thumbnailIcon;
	thumbnailIcon.addFile(QString::fromUtf8(":/22x22/view-list-icons-symbolic.png"), QSize(), QIcon::Normal, QIcon::Off);
	thumbnailIcon.addFile(QString::fromUtf8(":/22x22/view-list-icons-symbolic_dark.png"), QSize(), QIcon::Normal, QIcon::On);

	addChildView(thumbnailView, thumbnailIcon);

	// Table Data View
	QIcon tableIcon;
	tableIcon.addFile(QString::fromUtf8(":/22x22/view-list-details-symbolic.png"), QSize(), QIcon::Normal, QIcon::Off);
	tableIcon.addFile(QString::fromUtf8(":/22x22/view-list-details-symbolic_dark.png"), QSize(), QIcon::Normal, QIcon::On);


	addChildView(new AMScanTableView(), tableIcon);

	// Tree Data View
	AMScanTreeView* treeView = new AMScanTreeView();

	QIcon treeIcon;
	treeIcon.addFile(QString::fromUtf8(":/22x22/view-list-compact-symbolic.png"), QSize(), QIcon::Normal, QIcon::Off);
	treeIcon.addFile(QString::fromUtf8(":/22x22/view-list-compact-symbolic_dark.png"), QSize(), QIcon::Normal, QIcon::On);

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
		if(indices.at(iSelectedItem).column() == 0)
		{
			QUrl scanUrl = model_->rowToUrl(indices.at(iSelectedItem));
			if(!returnList.contains(scanUrl))
				returnList.append(scanUrl);
		}
	}


	return returnList;
}

int AMScanDataView::numberOfSelectedItems()
{
	if(!currentView())
		return 0;

	return currentView()->selectionModel()->selectedRows().count();
}

void AMScanDataView::showRun(int runId)
{
	proxyModel_->setRunId(runId);
}

void AMScanDataView::showExperiment(int experimentId)
{
	proxyModel_->setExperimentId(experimentId);
}

void AMScanDataView::onFilterChanged(bool isCurrentlyFiltered)
{
	if(isCurrentlyFiltered)
		titleLabel_->setText(QString("%1: Data (Filtered to show %2 scans)").arg(AMUser::user()->name()).arg(proxyModel_->rowCount()));
	else
		titleLabel_->setText(QString("%1: Data").arg(AMUser::user()->name()));
}

void AMScanDataView::onChildViewDoubleClicked()
{
	if(numberOfSelectedItems() > 0)
	{
		emit selectionActivated(selectedItems());
	}
}

void AMScanDataView::onCustomContextMenuRequested(const QPoint& position)
{
	QPoint globalPos  = currentView()->viewport()->mapToGlobal(position);

	if(numberOfSelectedItems() < 2)
		contextMenu_->actions().at(1)->setEnabled(false);
	else
		contextMenu_->actions().at(1)->setEnabled(true);

	if(numberOfSelectedItems() > 1)
		contextMenu_->actions().at(4)->setEnabled(false);
	else
		contextMenu_->actions().at(4)->setEnabled(true);
	contextMenu_->popup(globalPos);
}

void AMScanDataView::onEditScan()
{
	emit selectionActivatedSeparateWindows(selectedItems());
}

void AMScanDataView::onCompareScans()
{
	emit selectionActivated(selectedItems());
}

void AMScanDataView::onExportScans()
{
	emit selectionExported(selectedItems());
}

void AMScanDataView::onShowScanConfiguration()
{
	emit launchScanConfigurationsFromDb(selectedItems());
}

void AMScanDataView::onFixCDF()
{
	emit fixCDF(selectedItems().front());
}

void AMScanDataView::onChildViewSelectionChanged()
{
	int selectedItemCount = numberOfSelectedItems();

	if(selectedItemCount < 1)
	{
		editButton_->setEnabled(false);
		compareButton_->setEnabled(false);
		exportButton_->setEnabled(false);
		configButton_->setEnabled(false);
		selectedItemsCount_->setVisible(false);
	}
	else if(selectedItemCount == 1)
	{
		editButton_->setEnabled(true);
		compareButton_->setEnabled(false);
		exportButton_->setEnabled(true);
		configButton_->setEnabled(true);
		selectedItemsCount_->setVisible(true);
	}
	else
	{
		editButton_->setEnabled(true);
		compareButton_->setEnabled(true);
		exportButton_->setEnabled(true);
		configButton_->setEnabled(true);
		selectedItemsCount_->setVisible(true);
	}


	selectedItemsCount_->setText(QString("%1 item(s) selected").arg(selectedItemCount));
	emit selectionChanged();
}

void AMScanDataView::onSelectAll()
{
	currentView()->selectAll();
}

void AMScanDataView::onClearSelection()
{
	currentView()->clearSelection();
}

void AMScanDataView::onChildViewChanged(int newIndex)
{
	stackedWidget_->setCurrentIndex(newIndex);
	onChildViewSelectionChanged();
}


