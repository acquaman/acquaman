#include "AMDataSourcesEditor.h"
#include "ui/AMCloseItemDelegate.h"
#include <QRegExpValidator>

AMDataSourcesEditor::AMDataSourcesEditor(AMScanSetModel* model, QWidget *parent) :
		QWidget(parent)
{
	detailEditor_ = 0;
	ui_.setupUi(this);

	// Modify and adjust UI components
	ui_.nameEdit->setReadOnly(true);	// you can't edit existing data source names.
	// When making new names for data sources, they better be mathematically sound variable names (No spaces, alphabetic character at beginning, no funky symbols, etc...)
	ui_.nameEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z_]\\w*"), this));



	// apply 'close item' boxes onto the list of data sources
	AMCloseItemDelegate* itemDelegate = new AMCloseItemDelegate(this);
	itemDelegate->setCloseButtonsEnabled(true);
	connect(itemDelegate, SIGNAL(closeButtonClicked(QModelIndex)), this, SLOT(onCloseButtonClicked(QModelIndex)));
	ui_.scanSetView->setItemDelegate(itemDelegate);

	// apply the existing scan set model to the data source list view:
	model_ = model;
	ui_.scanSetView->setModel(model_);
	connect(ui_.scanSetView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onSetViewIndexChanged(QModelIndex,QModelIndex)));

	// don't show the drop-down arrows for scans. (Saves a bit of room inside the list of channels.)
	ui_.scanSetView->setRootIsDecorated(false);

	ui_.addDataSourceButton->setDisabled(true);
	connect(ui_.addDataSourceButton, SIGNAL(clicked()), this, SLOT(onAddDataSourceButtonClicked()));

	editingNewDataSourceName_ =  false;
}

void AMDataSourcesEditor::setCurrentScan(AMScan *scan) {
	if(!scan)
		setCurrentScan(-1);
	else
		setCurrentScan(model_->indexOf(scan));
}

void AMDataSourcesEditor::setCurrentScan(int scanIndex) {

	// get rid of any current 'detail' editors.
	removeDetailEditor();

	if(scanIndex < 0 || scanIndex >= model_->scanCount()) {
		ui_.scanSetView->setCurrentIndex(QModelIndex());
		ui_.scanSetView->collapseAll();
		return;
	}

	ui_.scanSetView->collapseAll();
	QModelIndex i = model_->indexForScan(scanIndex);
	ui_.scanSetView->expand(i);
	ui_.scanSetView->setCurrentIndex(i);

}

void AMDataSourcesEditor::onSetViewIndexChanged(const QModelIndex &selected, const QModelIndex &deselected) {

	removeDetailEditor();

	// Nothing selected?
	////////////////////////
	if(!selected.isValid()) {
		ui_.nameEdit->clear();
		ui_.descriptionEdit->clear();
		ui_.descriptionEdit->setReadOnly(true);
		ui_.addDataSourceButton->setDisabled(true);
		return;
	}

	ui_.addDataSourceButton->setEnabled(true);

	// Scan selected? (No selected data source)
	/////////////////////////
	if(!selected.parent().isValid()) {
		ui_.nameEdit->clear();
		ui_.descriptionEdit->clear();
		ui_.descriptionEdit->setReadOnly(true);
		return;
	}


	// Data source selected.
	/////////////////
	int si = selected.parent().row();
	int di = selected.row();
	AMDataSource* dataSource = model_->dataSourceAt(si, di);
	if(!dataSource)
		return;

	ui_.nameEdit->setText(dataSource->name());
	ui_.descriptionEdit->setText(dataSource->description());
	ui_.descriptionEdit->setReadOnly(false);

	installDetailEditor(dataSource->createEditorWidget());

	/* AM1DExpressionAB detail editor should now do this on startup:
	ui_.expressionEdit->setText(dataSource->expression());
	ui_.expressionEdit->setReadOnly(false);
	ui_.insertButton->setEnabled(true);
	populateExpressionMenu(si);
	*/

}

#include <QMessageBox>
void AMDataSourcesEditor::onCloseButtonClicked(const QModelIndex &index) {

	/// handle data source-level indexes only:
	if(!index.parent().isValid())
		return;

	int dataSourceIndex = index.row();
	int scanIndex = index.parent().row();

	AMScan* scan = model_->scanAt(scanIndex);
	if(!scan || dataSourceIndex >= scan->dataSourceCount())
		return;

	AMDataSource* dataSource = scan->dataSourceAt(dataSourceIndex);
	int response = QMessageBox::question(this, "Remove Data Source?", QString("Remove this data source? \n\n'%1' (%2)\n\nThe data source will be deleted, and no longer visible in any plots. Any other data sources that depend on this data source will be reset.  Raw data will NOT be deleted, and you can re-create the data source later if you need it.").arg(dataSource->description()).arg(dataSource->name()), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

	if(response == QMessageBox::Ok)
		scan->deleteDataSourceAt(dataSourceIndex);
}

int AMDataSourcesEditor::currentScanIndex() const {
	QModelIndex i = ui_.scanSetView->currentIndex();
	if(!i.isValid())
		return -1;

	if(!i.parent().isValid())
		return i.row();

	else
		return i.parent().row();

}


int AMDataSourcesEditor::currentDataSourceIndex() const {
	QModelIndex i = ui_.scanSetView->currentIndex();
	if(!i.parent().isValid())
		return -1;

	return i.row();
}

/// \todo Eventually, this button should support creating all kinds of available data sources, using a beautiful popup dialogue with buttons and icons and descriptions. For now, we only create AM1DExpressionAB (expression editors for 1D channels, to allow simply 1D normalization and calibration)
void AMDataSourcesEditor::onAddDataSourceButtonClicked() {

	removeDetailEditor();

	/// Data source names aren't editable after they've been created. Here we re-enable the nameEdit for editing shortly, and connect its editingFinished ... We'll complete the operation in onNewChannelNamed().
	editingNewDataSourceName_ = true;
	ui_.nameEdit->setReadOnly(false);
	ui_.nameEdit->setText("newDataSourceName");
	ui_.nameEdit->setFocus();
	ui_.nameEdit->selectAll();

	ui_.descriptionEdit->clear();

	connect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onNewDataSourceNamed()));

}

#include "AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
/// \todo Eventually, this button should support creating all kinds of available data sources (raw, and analysis blocks), using a beautiful popup dialogue with buttons and icons and descriptions. For now, we only create AM1DExpressionAB (expression editors for 1D channels, to allow simply 1D normalization and calibration)
void AMDataSourcesEditor::onNewDataSourceNamed() {

	if(!editingNewDataSourceName_)
		return;

	editingNewDataSourceName_ = false;
	QString chName = ui_.nameEdit->text();
	disconnect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onNewDataSourceNamed()));
	ui_.nameEdit->clearFocus();
	ui_.nameEdit->setReadOnly(true);


	int si = currentScanIndex();
	if(si < 0 || si >= model_->scanCount()) {
		// this should have never happened. How did we get here? You shouldn't have been able to press the addNewChannel button without a current scan.
		return;
	}

	if(chName.isEmpty())
		return;


	if(model_->scanAt(si)->indexOfDataSource(chName) != -1) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -1, QString("Couldn't create a new data source with the name \"%1\". Make sure to choose a name that doesn't exist already in this scan.").arg(chName)));
		ui_.scanSetView->setCurrentIndex(ui_.scanSetView->currentIndex());
		return;
	}

	AM1DExpressionAB* newExpression = new AM1DExpressionAB(chName);
	// find out all the available 1D data sources, to use for inputs on this new data source.
	/// \todo Currently this only provides access to the raw data sources, until we can figure out a way to detect or work-around the circular reference problem if analyzed data sources could be used as input.
	QList<AMDataSource*> singleDimDataSources;
	AMScan* scan = model_->scanAt(si);
	for(int i=0; i<scan->rawDataSources()->count(); i++)
		if(scan->rawDataSources()->at(i)->rank() == 1)
			singleDimDataSources << scan->rawDataSources()->at(i);
	newExpression->setInputDataSources(singleDimDataSources);
	scan->addAnalyzedDataSource(newExpression);

	int di = scan->dataSourceCount()-1;
	ui_.scanSetView->setCurrentIndex(model_->indexForDataSource(si, di));
		// this should automatically create a new detail editor for this new scan.
}



void AMDataSourcesEditor::descriptionEditingFinished() {
	int si = currentScanIndex();
	int di = currentDataSourceIndex();

	if(si <0 || di <0 || di>=model_->scanAt(si)->dataSourceCount())
		return;

	model_->dataSourceAt(si, di)->setDescription(ui_.descriptionEdit->text());
}



