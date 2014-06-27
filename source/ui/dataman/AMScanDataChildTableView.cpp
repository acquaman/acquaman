#include "AMScanDataChildTableView.h"

AMScanDataChildTableView::AMScanDataChildTableView(QWidget *parent)
	: AMAbstractScanDataChildView("Table View", QIcon(), parent)
{
	scanDataTable_ = new QTableView();
	QHBoxLayout* mainLayout = new QHBoxLayout();

	icon_.addFile(QString::fromUtf8(":/22x22/view-list-details-symbolic.png"), QSize(), QIcon::Normal, QIcon::Off);
	icon_.addFile(QString::fromUtf8(":/22x22/view-list-details-symbolic_dark.png"), QSize(), QIcon::Normal, QIcon::On);

	scanDataTable_->setAlternatingRowColors(true);
	scanDataTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
	scanDataTable_->setSelectionMode(QAbstractItemView::ExtendedSelection);
	scanDataTable_->verticalHeader()->setVisible(false);
	scanDataTable_->setShowGrid(false);
	QFont font = scanDataTable_->font();
	font.setPointSize(10);
	scanDataTable_->setFont(font);
	scanDataTable_->setContextMenuPolicy(Qt::CustomContextMenu);
	scanDataTable_->setSortingEnabled(true);

	mainLayout->addWidget(scanDataTable_);

	setLayout(mainLayout);
}

void AMScanDataChildTableView::onModelChangedImplementation()
{
	scanDataTable_->setModel(model_);
	scanDataTable_->resizeColumnsToContents();
	scanDataTable_->setColumnWidth(7, 80);
}
