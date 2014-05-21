/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMDataViewWithActionButtons.h"
#include <QBoxLayout>

AMDataViewWithActionButtons::AMDataViewWithActionButtons(AMDatabase* database, QWidget *parent) :
	QWidget(parent)
{
	database_ = database;
}

AMDataViewWithActionButtons::~AMDataViewWithActionButtons(){}

void AMDataViewWithActionButtons::buildView(){
	QVBoxLayout* vl = new QVBoxLayout;
	dataView_ = createDataView(database_);

	QFrame* actionsBarFrame = new QFrame;
	ui_ = new Ui::AMDataViewActionsBar;
	ui_->setupUi(actionsBarFrame);

	vl->setContentsMargins(0,0,0,0);
	vl->setSpacing(0);

	vl->addWidget(dataView_);
	vl->addWidget(actionsBarFrame);
	setLayout(vl);

	connect(ui_->sizeSlider, SIGNAL(valueChanged(int)), dataView_, SLOT(setItemSize(int)));
	connect(ui_->expandAllButton, SIGNAL(clicked()), dataView_, SLOT(expandAll()));
	connect(ui_->collapseAllButton, SIGNAL(clicked()), dataView_, SLOT(collapseAll()));

	connect(ui_->openSameEditorButton, SIGNAL(clicked()), this, SLOT(onCompareScansAction()));
	connect(ui_->openSeparateEditorButton, SIGNAL(clicked()), this, SLOT(onEditScansAction()));
	connect(ui_->exportButton, SIGNAL(clicked()), this, SLOT(onExportScansAction()));
	connect(ui_->scanConfigurationButton, SIGNAL(clicked()), this, SLOT(onLaunchScanConfigurationsFromDb()));

	connect(dataView_, SIGNAL(viewDoubleClicked()), this, SLOT(onDoubleClick()));
	connect(dataView_, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
	connect(dataView_, SIGNAL(editScansFromDb()), this, SLOT(onEditScansAction()));
	connect(dataView_, SIGNAL(editScansFromDb(QList<QUrl>)), this, SLOT(onEditScansAction(QList<QUrl>)));
	connect(dataView_, SIGNAL(compareScansFromDb()), this, SLOT(onCompareScansAction()));
	connect(dataView_, SIGNAL(exportScansFromDb()), this, SLOT(onExportScansAction()));
	connect(dataView_, SIGNAL(exportScansFromDb(QList<QUrl>)), this, SLOT(onExportScansAction(QList<QUrl>)));
	connect(dataView_, SIGNAL(launchScanConfigurationsFromDb()), this, SLOT(onLaunchScanConfigurationsFromDb()));
	connect(dataView_, SIGNAL(launchScanConfigurationsFromDb(QList<QUrl>)), this, SLOT(onLaunchScanConfigurationsFromDb(QList<QUrl>)));
	connect(dataView_, SIGNAL(fixCDF()), this, SLOT(onFixCDF()));
}

void AMDataViewWithActionButtons::onLaunchScanConfigurationsFromDb()
{
	QList<QUrl> urls = dataView_->selectedItems();

	if(urls.count() > 0)
		emit launchScanConfigurationsFromDb(urls);
}

void AMDataViewWithActionButtons::onFixCDF()
{
	QList<QUrl> urls = dataView_->selectedItems();

	if (urls.count() == 1)
		emit fixCDF(urls.at(0));
}

AMDataView* AMDataViewWithActionButtons::createDataView(AMDatabase *database){
	return new AMDataView(database);
}

void AMDataViewWithActionButtons::onDoubleClick()
{
	QList<QUrl> urls = dataView_->selectedItems();

	if(urls.count() > 0)
		emit selectionActivated(urls);
}

void AMDataViewWithActionButtons::onSelectionChanged()
{
	if(dataView_->numberOfSelectedItems() > 0) {
		ui_->openSeparateEditorButton->setEnabled(true);
		ui_->openSameEditorButton->setEnabled(true);
		ui_->exportButton->setEnabled(true);
		ui_->scanConfigurationButton->setEnabled(true);
	}
	else {
		ui_->openSeparateEditorButton->setEnabled(false);
		ui_->openSameEditorButton->setEnabled(false);
		ui_->exportButton->setEnabled(false);
		ui_->scanConfigurationButton->setEnabled(false);
	}
}
