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


#include "SGMDataViewWithActionButtons.h"

#include <QPushButton>

#include "ui/dataman/AMScanSearchView.h"

SGMDataViewWithActionButtons::SGMDataViewWithActionButtons(AMDatabase* database, QWidget *parent) :
	AMDataViewWithActionButtons(database, parent)
{
}

AMDataView* SGMDataViewWithActionButtons::createDataView(AMDatabase *database){
	return new SGMDataView(database);
}

SGMDataView::SGMDataView(AMDatabase* database, QWidget *parent) :
	AMDataView(database, parent)
{
	searchDialog_ = 0;

	// Add Search... button
	QPushButton* searchScans = new QPushButton(QString("Search Scans..."));
	this->horizontalLayout->addSpacing(40);
	horizontalLayout->addWidget(searchScans);

	connect(searchScans, SIGNAL(clicked()), this, SLOT(onSearchScansButtonClicked()));
}

void SGMDataView::onSearchScansButtonClicked(){
	if(searchDialog_ == 0)
	{
		QApplication::setOverrideCursor(Qt::WaitCursor);
		searchDialog_ = new AMScanSearchView();
		QApplication::restoreOverrideCursor();
		connect(searchDialog_, SIGNAL(editScanRequested(QList<QUrl>)), this, SLOT(onScanSearchEditScanRequested(QList<QUrl>)) );
		connect(searchDialog_, SIGNAL(editConfigurationRequested(QList<QUrl>)), this, SLOT(onScanSearchLaunchScanConfigurationRequested(QList<QUrl>)));
		connect(searchDialog_, SIGNAL(exportScanRequested(QList<QUrl>)), this, SLOT(onScanSearchExportScanRequested(QList<QUrl>)));

	}
	if(!searchDialog_->isVisible())
		searchDialog_->show();
	searchDialog_->raise();
}
