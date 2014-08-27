#include "AMScanTableView.h"

AMScanTableView::AMScanTableView(QWidget *parent) :
	QTableView(parent)
{
	setAlternatingRowColors(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	verticalHeader()->setVisible(false);
	setShowGrid(false);
	QFont newFont = font();
	newFont.setPointSize(10);
	setFont(newFont);
	setContextMenuPolicy(Qt::CustomContextMenu);
	setSortingEnabled(true);
}

void AMScanTableView::setModel(QAbstractItemModel *model)
{
	QTableView::setModel(model);
	setColumnWidth(0, 80);
	setColumnWidth(1, 200);
	setColumnWidth(2, 40);
	setColumnWidth(3, 110);
	setColumnWidth(4, 110);
	setColumnWidth(5, 60);
	setColumnWidth(6, 110);
	setColumnWidth(7, 200);
}
