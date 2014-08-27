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
#include "ui/dataman/AMDataView.h"
#include "dataman/database/AMDatabase.h"
#include <QPushButton>
#include <QButtonGroup>
#include "util/AMDateTimeUtils.h"
#include <QStringBuilder>

 AMChooseScanDialog::~AMChooseScanDialog(){}
AMChooseScanDialog::AMChooseScanDialog(AMDatabase* db, const QString& title, const QString& prompt, bool multipleSelectionAllowed, QWidget *parent) :
	QDialog(parent)
{
	multipleSelectionAllowed_ = multipleSelectionAllowed;

	ui_ = new Ui::AMChooseScanDialog;
	ui_->setupUi(this);

	db_ = db;
	dataView_ = new AMDataView(db_);
	dataView_->setItemSize(0);
	dataView_->setMinimumHeight(400);

	ui_->verticalLayoutMain->insertWidget(1, dataView_);
	ui_->horizontalLayoutTop->setAlignment(ui_->showRunsButton, Qt::AlignBottom | Qt::AlignRight);
	ui_->horizontalLayoutTop->setAlignment(ui_->showExperimentsButton, Qt::AlignBottom | Qt::AlignRight);

	ui_->titleLabel->setText(title);
	ui_->promptLabel->setText(prompt);
	setWindowTitle(title);

	ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	QButtonGroup* runExpButtonGroup = new QButtonGroup(this);
	runExpButtonGroup->addButton(ui_->showRunsButton, 1);
	runExpButtonGroup->addButton(ui_->showExperimentsButton, 2);
	connect(runExpButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onRunOrExperimentButtonClicked(int)));


	// does not delete the dialog when it is closed, so that you can check for the selected items
	setAttribute(Qt::WA_DeleteOnClose, false);

	// connect data view selectionChanged to handler
	connect(dataView_, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
	connect(dataView_, SIGNAL(viewDoubleClicked()), this, SLOT(onDoubleClick()));

	// connect controls for the data view
	connect(ui_->sizeSlider, SIGNAL(valueChanged(int)), dataView_, SLOT(setItemSize(int)));
	connect(ui_->expandAllButton, SIGNAL(clicked()), dataView_, SLOT(expandAll()));
	connect(ui_->collapseAllButton, SIGNAL(clicked()), dataView_, SLOT(collapseAll()));
}

QList<QUrl> AMChooseScanDialog::getSelectedScans() const
{
	return dataView_->selectedItems();
}

void AMChooseScanDialog::onSelectionChanged()
{
	int numSelectedScans = dataView_->numberOfSelectedItems();

	if(multipleSelectionAllowed_ == false) {	// single-selection mode
		switch(numSelectedScans) {
		case 0:
			ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			ui_->statusLabel->setText("No scan selected.");
			break;
		case 1: {
			ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
			ui_->statusLabel->setText(getScanDetails(dataView_->selectedItems().first()));
			break;
		}
		default:
			ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			ui_->statusLabel->setText(QString("%1 scans selected.  (Please select only one scan.)").arg(numSelectedScans));
			break;
		}
	}

	else {	// multiple selection mode
		switch(numSelectedScans) {
		case 0:
			ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			ui_->statusLabel->setText("No scans selected.");
			break;
		case 1:
			ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
			ui_->statusLabel->setText(getScanDetails(dataView_->selectedItems().first()));
			break;
		default:
			ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
			ui_->statusLabel->setText(QString("%1 scans selected.").arg(numSelectedScans));
			break;
		}
	}
}

void AMChooseScanDialog::onDoubleClick()
{
	int numSelectedScans = dataView_->numberOfSelectedItems();
	if(numSelectedScans == 0)
		return;

	if(multipleSelectionAllowed_ == false && numSelectedScans > 1)
		return;

	accept();
}

void AMChooseScanDialog::onRunOrExperimentButtonClicked(int code)
{
	if(code == 1)
		dataView_->showRun();
	else if(code == 2)
		dataView_->showExperiment();
}

QString AMChooseScanDialog::getScanDetails(QUrl url)
{
	if(url.scheme() != "amd")
		return QString();

	if(url.host() != db_->connectionName())
		return QString();	// this should never happen, the dataView_ and us share the same database.

	QStringList path = url.path().split('/', QString::SkipEmptyParts);
	if(path.count() != 2)
		return QString();

	QString tableName = path.at(0);
	bool idOkay;
	int id = path.at(1).toInt(&idOkay);
	if(!idOkay || id < 1)
		return QString();

	QVariantList results = db_->retrieve(id, tableName, QStringList() << "name" << "number" << "dateTime");

	if(results.count() != 3)
		return QString();

	return results.at(0).toString() % " #" % results.at(1).toString() % ", taken " % AMDateTimeUtils::prettyDateTime(results.at(2).toDateTime());
}

void AMChooseScanDialog::setPrompt(const QString &prompt)
{
	ui_->promptLabel->setText(prompt);
}

void AMChooseScanDialog::setTitle(const QString &title)
{
	ui_->titleLabel->setText(title);
}


