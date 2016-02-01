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

AMChooseScanDialog::AMChooseScanDialog(AMDatabase* db, const QString& title, const QString& prompt, QWidget *parent)
	: QDialog(parent)
{
	buildView(QList<AMDatabase *>() << db, title, prompt);
}

AMChooseScanDialog::AMChooseScanDialog(QList<AMDatabase *> databases, const QString &title, const QString &prompt, QWidget *parent)
	: QDialog(parent)
{
	buildView(databases, title, prompt);
}

void AMChooseScanDialog::buildView(const QList<AMDatabase *> &databases, const QString &title, const QString &prompt)
{
	setWindowTitle(title);
	resize(1024,768);

	QComboBox *browseViewComboBox = new QComboBox;
	allBrowseScansViews_ = new QStackedWidget;

	foreach (AMDatabase *database, databases){

		AMBrowseScansView *currentView = new AMBrowseScansView(database);
		connect(currentView, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
		connect(currentView, SIGNAL(childContextMenuRequested(const QPoint&)), this, SLOT(onContextMenuRequested(const QPoint&)));
		connect(currentView, SIGNAL(childViewDoubleClicked(const QModelIndex&)), this, SLOT(onDoubleClick()));
		allBrowseScansViews_->addWidget(currentView);
		browseViewComboBox->addItem(database->connectionName());
	}

	connect(browseViewComboBox, SIGNAL(currentIndexChanged(int)), allBrowseScansViews_, SLOT(setCurrentIndex(int)));

	if (!databases.isEmpty())
		browseViewComboBox->setCurrentIndex(0);

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

	AMBrowseScansView *view = qobject_cast<AMBrowseScansView *>(allBrowseScansViews_->currentWidget());
	contextMenu_ = new QMenu(this);
	contextMenu_->addAction("Select All", view, SLOT(selectAll()));
	contextMenu_->addAction("Clear Selection", view, SLOT(clearSelection()));

	browseViewComboBox->setVisible(browseViewComboBox->count() > 1);

	layout->addWidget(browseViewComboBox, 0, Qt::AlignLeft);
	layout->addWidget(promptLabel_);
	layout->addWidget(allBrowseScansViews_);
	layout->addLayout(buttonLayout);

	setLayout(layout);

	connect(dialogButtons_->button(0), SIGNAL(clicked()), this, SLOT(accept()));
	connect(dialogButtons_->button(1), SIGNAL(clicked()), this, SLOT(reject()));
}

QList<QUrl> AMChooseScanDialog::getSelectedScans() const
{
	AMBrowseScansView *view = qobject_cast<AMBrowseScansView *>(allBrowseScansViews_->currentWidget());

	if (view)
		return view->selectedItems();

	return QList<QUrl>();
}

void AMChooseScanDialog::clearSelection()
{
	AMBrowseScansView *view = qobject_cast<AMBrowseScansView *>(allBrowseScansViews_->currentWidget());

	if (view)
		view->clearSelection();
}

void AMChooseScanDialog::onSelectionChanged()
{
	AMBrowseScansView *view = qobject_cast<AMBrowseScansView *>(allBrowseScansViews_->currentWidget());

	if (view)
		dialogButtons_->button(0)->setEnabled(view->numberOfSelectedItems() != 0);
}

void AMChooseScanDialog::onDoubleClick()
{
	AMBrowseScansView *view = qobject_cast<AMBrowseScansView *>(allBrowseScansViews_->currentWidget());

	if (view && view->numberOfSelectedItems() > 0)
		accept();
}

void AMChooseScanDialog::onContextMenuRequested(const QPoint &menuPosition)
{
	AMBrowseScansView *view = qobject_cast<AMBrowseScansView *>(allBrowseScansViews_->currentWidget());

	if (view){

		QPoint globalPosition = view->mapToGlobal(menuPosition);
		contextMenu_->exec(globalPosition);
	}
}

void AMChooseScanDialog::setPrompt(const QString &prompt)
{
	promptLabel_->setText(prompt);
}

void AMChooseScanDialog::setFilterRegExp(const QString &pattern)
{
	for (int i = 0, size = allBrowseScansViews_->count(); i < size; i++){

		AMBrowseScansView *view = qobject_cast<AMBrowseScansView *>(allBrowseScansViews_->widget(i));

		if (view)
			view->setFilterRegExp(pattern);
	}
}

void AMChooseScanDialog::setFilterKeyColumn(int column)
{
	for (int i = 0, size = allBrowseScansViews_->count(); i < size; i++){

		AMBrowseScansView *view = qobject_cast<AMBrowseScansView *>(allBrowseScansViews_->widget(i));

		if (view)
			view->setFilterKeyColumn(column);
	}
}
