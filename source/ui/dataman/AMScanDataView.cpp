#include "AMScanDataView.h"
#include "dataman/AMUser.h"
#include "ui/dataman/AMBrowseScansView.h"
AMScanDataView::AMScanDataView(AMDatabase *database, QWidget *parent) :
	QWidget(parent)
{


	// Widgets
	browseScansView_ = new AMBrowseScansView(database);
	titleLabel_ = new QLabel();
	titleLabel_->setObjectName(QString::fromUtf8("headingLabel_"));
	titleLabel_->setStyleSheet(QString::fromUtf8("font: 20pt \"Lucida Grande\";\n"
	"color: rgb(79, 79, 79);"));
	titleLabel_->setText(QString("%1: Data").arg(AMUser::user()->name()));

	QIcon searchIcon;
	searchIcon.addFile(QString::fromUtf8(":/system-search-2.png"), QSize(), QIcon::Normal, QIcon::Off);

	// Action bar Widgets
	QHBoxLayout* actionButtonLayout = new QHBoxLayout();

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

	connect(editButton_, SIGNAL(clicked()), this, SLOT(onEditScan()));
	connect(compareButton_, SIGNAL(clicked()), this, SLOT(onCompareScans()));
	connect(exportButton_, SIGNAL(clicked()), this, SLOT(onExportScans()));
	connect(configButton_, SIGNAL(clicked()), this, SLOT(onShowScanConfiguration()));

	// Context menu
	contextMenu_  = new QMenu(this);
	QAction* editScan = contextMenu_->addAction("Edit");
	QAction* compareScans = contextMenu_->addAction("Compare");
	QAction* exportScans = contextMenu_->addAction("Export");
	QAction* viewScanConfig = contextMenu_->addAction("Show Scan Configuration ...");
	contextMenu_->addSeparator();
	contextMenu_->addAction("Select All", browseScansView_, SLOT(selectAll()));
	contextMenu_->addAction("Clear Selection", browseScansView_, SLOT(clearSelection()));

	// Signals, Slots
	connect(editScan, SIGNAL(triggered()), this, SLOT(onEditScan()));
	connect(compareScans, SIGNAL(triggered()), this, SLOT(onCompareScans()));
	connect(exportScans, SIGNAL(triggered()), this, SLOT(onExportScans()));
	connect(viewScanConfig, SIGNAL(triggered()), this, SLOT(onShowScanConfiguration()));
	connect(browseScansView_, SIGNAL(childContextMenuRequested(const QPoint&)), this, SLOT(onCustomContextMenuRequested(const QPoint&)));
	connect(browseScansView_, SIGNAL(filterChanged(bool)), this, SLOT(onFilterChanged(bool)));
	connect(browseScansView_, SIGNAL(selectionChanged()), this, SLOT(onChildViewSelectionChanged()));
	connect(browseScansView_, SIGNAL(childViewDoubleClicked(const QModelIndex&)), this, SLOT(onChildViewDoubleClicked()));

	// Layout
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(titleLabel_);
	layout->addWidget(browseScansView_);
	layout->addLayout(actionButtonLayout);

	setLayout(layout);
}


QList<QUrl> AMScanDataView::selectedItems()
{
	return browseScansView_->selectedItems();
}

int AMScanDataView::numberOfSelectedItems()
{
	return browseScansView_->numberOfSelectedItems();
}

void AMScanDataView::showRun(int runId)
{
	browseScansView_->setRunId(runId);
}

void AMScanDataView::showExperiment(int experimentId)
{
	browseScansView_->setExperimentId(experimentId);
}

void AMScanDataView::onFilterChanged(bool isCurrentlyFiltered)
{
	if(isCurrentlyFiltered)
		titleLabel_->setText(QString("%1: Data (Filtered to show %2 scans)").arg(AMUser::user()->name()).arg(browseScansView_->numberOfDisplayedRows()));
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
	QPoint globalPos  = browseScansView_->mapToGlobal(position);

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

void AMScanDataView::onChildViewSelectionChanged()
{
	int selectedItemCount = numberOfSelectedItems();

	if(selectedItemCount < 1)
	{
		editButton_->setEnabled(false);
		compareButton_->setEnabled(false);
		exportButton_->setEnabled(false);
		configButton_->setEnabled(false);
	}
	else if(selectedItemCount == 1)
	{
		editButton_->setEnabled(true);
		compareButton_->setEnabled(false);
		exportButton_->setEnabled(true);
		configButton_->setEnabled(true);
	}
	else
	{
		editButton_->setEnabled(true);
		compareButton_->setEnabled(true);
		exportButton_->setEnabled(true);
		configButton_->setEnabled(true);
	}
	emit selectionChanged();
}

