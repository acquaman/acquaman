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


#include "AMDataSourcesEditor.h"
#include "ui/AMCloseItemDelegate.h"
#include <QRegExpValidator>

#include "dataman/datasource/AMDataSource.h"
#include "dataman/AMScan.h"

 AMDataSourcesEditor::~AMDataSourcesEditor(){}
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
	nameOfAnalysisBlockToBeAdded_ = "";

	connect(ui_.descriptionEdit, SIGNAL(editingFinished()), this, SLOT(descriptionEditingFinished()));

	showAllDataSources_ = false;
	ui_.scanSetView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui_.scanSetView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
}

void AMDataSourcesEditor::onCustomContextMenuRequested(QPoint point)
{
	QMenu popup(this);
	QAction *temp = popup.addAction("Show all data sources");
	temp->setCheckable(true);
	temp->setChecked(showAllDataSources_);

	temp = popup.exec(mapToGlobal(ui_.scanSetView->mapToParent(point)));

	// If a valid action was selected.
	if (temp && temp->text() == "Show all data sources"){

		showAllDataSources_ = !showAllDataSources_;
		showAllDataSources(showAllDataSources_);
	}
}

void AMDataSourcesEditor::showAllDataSources(bool showAll)
{
	int scanIndex = currentScanIndex();
	QModelIndex modelIndex;

	// No scans to access.
	if (scanIndex < 0 || scanIndex >= model_->scanCount())
		return;

	else
		modelIndex = model_->indexForScan(scanIndex);

	showAllDataSources_ = showAll;

	if (showAllDataSources_){

		for (int i = 0; i < model_->scanAt(scanIndex)->dataSourceCount(); i++)
			ui_.scanSetView->setRowHidden(i, modelIndex, false);
	}

	else{

		for (int di = 0; di < model_->scanAt(scanIndex)->dataSourceCount(); di++)
			ui_.scanSetView->setRowHidden(di, modelIndex, model_->dataSourceAt(scanIndex, di)->hiddenFromUsers());
	}
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
	showAllDataSources(false);
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

	// Remove old connection to the data source description.
	int oldSi = deselected.parent().row();
	int oldDi = deselected.row();
	AMDataSource *oldDataSource = model_->dataSourceAt(oldSi, oldDi);
	if (oldDataSource)
		disconnect(oldDataSource->signalSource(), SIGNAL(infoChanged()), this, SLOT(onDataSourceDescriptionChanged()));

	// Setup new data source.
	int si = selected.parent().row();
	int di = selected.row();
	AMDataSource* dataSource = model_->dataSourceAt(si, di);
	if(!dataSource)
		return;

	connect(dataSource->signalSource(), SIGNAL(infoChanged()), this, SLOT(onDataSourceDescriptionChanged()));

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

void AMDataSourcesEditor::onDataSourceDescriptionChanged()
{
	QModelIndex index = ui_.scanSetView->currentIndex();
	AMDataSource *dataSource = model_->dataSourceAt(index.parent().row(), index.row());
	if (dataSource){

		ui_.descriptionEdit->setText(dataSource->description());
		ui_.scanSetView->update(index);
	}
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

#include "ui/AMAddAnalysisBlockDialog.h"

void AMDataSourcesEditor::onAddDataSourceButtonClicked() {

	int scanIndex = currentScanIndex();
	QString newAnalysisBlock = AMAddAnalysisBlockDialog::getAnalysisBlock(model_->scanAt(scanIndex)->scanRank());

	if (!newAnalysisBlock.isEmpty()){

		nameOfAnalysisBlockToBeAdded_ = newAnalysisBlock;
		removeDetailEditor();

		// Data source names aren't editable after they've been created. Here we re-enable the nameEdit for editing shortly, and connect its editingFinished ... We'll complete the operation in onNewChannelNamed().
		editingNewDataSourceName_ = true;
		ui_.nameEdit->setReadOnly(false);
		ui_.nameEdit->setText("newDataSourceName");
		ui_.nameEdit->setFocus();
		ui_.nameEdit->selectAll();

		ui_.descriptionEdit->clear();

		connect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onNewDataSourceNamed()));
	}

}

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AM2DSummingAB.h"
#include "analysis/AM1DIntegralAB.h"
#include "analysis/AM2DNormalizationAB.h"
#include "analysis/AM1DNormalizationAB.h"
#include "analysis/AM1DCalibrationAB.h"
#include "analysis/AM3DBinningAB.h"
//#include "analysis/REIXS/REIXSXESImageInterpolationAB.h"

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

	AMScan* scan = model_->scanAt(si);
	QList<AMDataSource*> singleDimDataSources;
	QList<AMDataSource *> twoDimDataSources;
	QList<AMDataSource *> threeDimDataSources;
	AMAnalysisBlock *newAnalysisBlock = 0;
	AMDataSource *tempSource = 0;

	// find out all the available 1D and 2D data sources, to use for inputs on this new data source.
	/// \todo Currently this only provides access to the raw data sources, until we can figure out a way to detect or work-around the circular reference problem if analyzed data sources could be used as input.
	/// \note DH: I have changed this to use all of the data sources.  There currently is still no work-around for the circular reference problem.  We are just going to have to be careful about using this.

	for(int i=0; i<scan->dataSourceCount(); i++){

		tempSource = scan->dataSourceAt(i);

		if(tempSource->rank() == 1)
			singleDimDataSources << tempSource;

		else if(tempSource->rank() == 2)
			twoDimDataSources << tempSource;

		else if (tempSource->rank() == 3)
			threeDimDataSources << tempSource;
	}

	if (nameOfAnalysisBlockToBeAdded_ == "Derivative"){

		newAnalysisBlock = new AM1DDerivativeAB(chName);
		newAnalysisBlock->setInputDataSources(singleDimDataSources);
	}

	else if (nameOfAnalysisBlockToBeAdded_ == "Integral"){

		newAnalysisBlock = new AM1DIntegralAB(chName);
		newAnalysisBlock->setInputDataSources(singleDimDataSources);
	}

	else if (nameOfAnalysisBlockToBeAdded_ == "Expression"){

		newAnalysisBlock = new AM1DExpressionAB(chName);
		newAnalysisBlock->setInputDataSources(singleDimDataSources);
	}

	else if (nameOfAnalysisBlockToBeAdded_ == "2D Summing"){

		newAnalysisBlock = new AM2DSummingAB(chName);
		newAnalysisBlock->setInputDataSources(twoDimDataSources);
	}

	else if (nameOfAnalysisBlockToBeAdded_ == "Normalization"){

		newAnalysisBlock = new AM1DNormalizationAB(chName);
		newAnalysisBlock->setInputDataSources(singleDimDataSources);
	}

	else if (nameOfAnalysisBlockToBeAdded_ == "Calibrated Normalization"){

		newAnalysisBlock = new AM1DCalibrationAB(chName);
		newAnalysisBlock->setInputDataSources(singleDimDataSources);
	}

	else if (nameOfAnalysisBlockToBeAdded_ == "2D Map Normalization"){

		newAnalysisBlock = new AM2DNormalizationAB(chName);
		newAnalysisBlock->setInputDataSources(twoDimDataSources);
	}

	else if (nameOfAnalysisBlockToBeAdded_ == "3D Binning"){

		newAnalysisBlock = new AM3DBinningAB(chName);
		newAnalysisBlock->setInputDataSources(threeDimDataSources);
	}

	else if (nameOfAnalysisBlockToBeAdded_ == "Interpolated Curve Correction"){

//		newAnalysisBlock = new REIXSXESImageInterpolationAB(chName);
//		newAnalysisBlock->setInputDataSources(twoDimDataSources);
	}


	// This should always happen.  But just to be safe.
	if (newAnalysisBlock)
		scan->addAnalyzedDataSource(newAnalysisBlock);

	int di = scan->dataSourceCount()-1;
	ui_.scanSetView->setCurrentIndex(model_->indexForDataSource(si, di));
		// this should automatically create a new detail editor for this new scan.
}



void AMDataSourcesEditor::descriptionEditingFinished() {
	int si = currentScanIndex();
	int di = currentDataSourceIndex();
	int rawCount = model_->scanAt(si)->rawDataSourceCount();

	if(si <0 || di <0 || di>=model_->scanAt(si)->dataSourceCount())
		return;

	if (di < rawCount)
		model_->scanAt(si)->rawDataSources()->at(di)->setDescription(ui_.descriptionEdit->text());
	else
		model_->scanAt(si)->analyzedDataSources()->at(di-rawCount)->setDescription(ui_.descriptionEdit->text());
}



