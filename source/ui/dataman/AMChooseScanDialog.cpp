/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMChooseScanDialog.h"
#include "dataman/AMLightweightScanInfoModel.h"
#include "dataman/AMLightweightScanInfoFilterProxyModel.h"
#include "ui/util/AMSortFilterWidget.h"
#include "ui/dataman/AMScanTableView.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/AMLightweightScanInfoCollection.h"

 AMChooseScanDialog::~AMChooseScanDialog(){}
AMChooseScanDialog::AMChooseScanDialog(AMDatabase* db, const QString& title, const QString& prompt, bool multipleSelectionAllowed, QWidget *parent) :
	QDialog(parent)
{
	setWindowTitle(title);
	resize(800,600);
	AMLightweightScanInfoCollection* collection = new AMLightweightScanInfoCollection(db);
	model_ = new AMLightweightScanInfoModel(collection, this);
	AMLightweightScanInfoFilterProxyModel* proxy = new AMLightweightScanInfoFilterProxyModel(this);
	proxy->setSourceModel(model_);

	QVBoxLayout* layout = new QVBoxLayout();

	dialogButtons_ = new QButtonGroup(this);

	QPushButton* okayButton = new QPushButton("Okay", this);
	QPushButton* cancelButton = new QPushButton("Cancel", this);

	dialogButtons_->addButton(okayButton, 0);
	dialogButtons_->button(0)->setEnabled(false);
	dialogButtons_->addButton(cancelButton, 1);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(cancelButton);
	buttonLayout->addWidget(okayButton);
	buttonLayout->addStretch();

	promptLabel_ = new QLabel(prompt);

	tableView_ = new AMScanTableView(this);
	if(multipleSelectionAllowed)
		tableView_->setSelectionMode(QAbstractItemView::MultiSelection);
	else
		tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
	tableView_->setModel(proxy);

	contextMenu_ = new QMenu(this);
	contextMenu_->addAction("Clear Selection", tableView_->selectionModel(), SLOT(clearSelection()));


	tableView_->setContextMenuPolicy(Qt::CustomContextMenu);

	filter_ = new AMSortFilterWidget(proxy, this);
	layout->addWidget(promptLabel_);
	layout->addWidget(filter_);
	layout->addWidget(tableView_);
	layout->addLayout(buttonLayout);



	setLayout(layout);

	connect(tableView_->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)), this, SLOT(onSelectionChanged()));
	connect(tableView_, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onContextMenuRequested(const QPoint&)));
	connect(dialogButtons_->button(0), SIGNAL(clicked()), this, SLOT(accept()));
	connect(dialogButtons_->button(1), SIGNAL(clicked()), this, SLOT(reject()));
}

QList<QUrl> AMChooseScanDialog::getSelectedScans() const
{
	QList<QUrl> returnList;

	QModelIndexList indices = tableView_->selectionModel()->selectedIndexes();

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

void AMChooseScanDialog::clearSelection()
{
	tableView_->selectionModel()->clearSelection();
}

void AMChooseScanDialog::onSelectionChanged()
{
	dialogButtons_->button(0)->setEnabled(tableView_->selectionModel()->selectedRows().count() != 0);
}

void AMChooseScanDialog::onDoubleClick()
{
	if(tableView_->selectionModel()->selectedRows().count() != 0)
		accept();
}

void AMChooseScanDialog::onContextMenuRequested(const QPoint &menuPosition)
{
	QPoint globalPosition = tableView_->mapToGlobal(menuPosition);
	contextMenu_->exec(globalPosition);
}

void AMChooseScanDialog::setPrompt(const QString &prompt)
{
	promptLabel_->setText(prompt);
}


