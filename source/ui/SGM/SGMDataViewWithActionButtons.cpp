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
