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
#include "AMBrowseScansView.h"

 AMChooseScanDialog::~AMChooseScanDialog(){}
AMChooseScanDialog::AMChooseScanDialog(AMDatabase* db, const QString& title, const QString& prompt, bool multipleSelectionAllowed, QWidget *parent) :
	QDialog(parent)
{
	setWindowTitle(title);
	resize(1024,768);
	browseScansView_ = new AMBrowseScansView(db);

	QVBoxLayout* layout = new QVBoxLayout();

	dialogButtons_ = new QButtonGroup(this);

	QPushButton* okayButton = new QPushButton("Okay");
	QPushButton* cancelButton = new QPushButton("Cancel");

	dialogButtons_->addButton(okayButton, 0);
	dialogButtons_->button(0)->setEnabled(false);
	dialogButtons_->addButton(cancelButton, 1);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(cancelButton);
	buttonLayout->addWidget(okayButton);
	buttonLayout->addStretch();

	promptLabel_ = new QLabel(prompt);

	contextMenu_ = new QMenu(this);
	contextMenu_->addAction("Select All", browseScansView_, SLOT(selectAll()));
	contextMenu_->addAction("Clear Selection", browseScansView_, SLOT(clearSelection()));

	layout->addWidget(promptLabel_);
	layout->addWidget(browseScansView_);
	layout->addLayout(buttonLayout);

	setLayout(layout);

	connect(browseScansView_, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
	connect(browseScansView_, SIGNAL(childContextMenuRequested(const QPoint&)), this, SLOT(onContextMenuRequested(const QPoint&)));
	connect(browseScansView_, SIGNAL(childViewDoubleClicked(const QModelIndex&)), this, SLOT(onDoubleClick()));
	connect(dialogButtons_->button(0), SIGNAL(clicked()), this, SLOT(accept()));
	connect(dialogButtons_->button(1), SIGNAL(clicked()), this, SLOT(reject()));
}

QList<QUrl> AMChooseScanDialog::getSelectedScans() const
{
	return browseScansView_->selectedItems();
}

void AMChooseScanDialog::clearSelection()
{
	browseScansView_->clearSelection();
}

void AMChooseScanDialog::onSelectionChanged()
{
	dialogButtons_->button(0)->setEnabled(browseScansView_->numberOfSelectedItems() != 0);
}

void AMChooseScanDialog::onDoubleClick()
{
	if(browseScansView_->numberOfSelectedItems() > 0)
		accept();
}

void AMChooseScanDialog::onContextMenuRequested(const QPoint &menuPosition)
{
	QPoint globalPosition = browseScansView_->mapToGlobal(menuPosition);
	contextMenu_->exec(globalPosition);
}

void AMChooseScanDialog::setPrompt(const QString &prompt)
{
	promptLabel_->setText(prompt);
}

void AMChooseScanDialog::setFilterRegExp(const QString &pattern)
{
	browseScansView_->setFilterRegExp(pattern);
}

void AMChooseScanDialog::setFilterKeyColumn(int column)
{
	browseScansView_->setFilterKeyColumn(column);
}
