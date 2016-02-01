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


#include <QRegExpValidator>
#include <QMessageBox>

#include "ui/AMCloseItemDelegate.h"
#include "dataman/datasource/AMDataSource.h"
#include "dataman/AMScan.h"
#include "ui/AMAddAnalysisBlockDialog.h"
#include "util/AMErrorMonitor.h"
#include "dataman/database/AMDbObjectSupport.h"

AMDataSourcesEditor::AMDataSourcesEditor(AMScanSetModel* model, QWidget *parent) :
		QWidget(parent)
{
	detailEditor_ = 0;
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("AMDataSourcesEditor"));
	resize(317, 230);
	QSizePolicy localSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	localSizePolicy.setHorizontalStretch(0);
	localSizePolicy.setVerticalStretch(0);
	localSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
	setSizePolicy(localSizePolicy);
	verticalLayout_ = new QVBoxLayout();
	verticalLayout_->setObjectName(QString::fromUtf8("verticalLayout"));
	scanSetView_ = new QTreeView();
	scanSetView_->setObjectName(QString::fromUtf8("scanSetView_"));
	scanSetView_->setAlternatingRowColors(true);

	verticalLayout_->addWidget(scanSetView_);

	addAnalysisToolButtonLayout_ = new QHBoxLayout();
	addAnalysisToolButtonLayout_->setObjectName(QString::fromUtf8("addAnalysisToolButtonLayout_"));
	addAnalysisToolButtonLayout_->setContentsMargins(12, 0, 12, -1);
	addDataSourceButton_ = new QToolButton();
	addDataSourceButton_->setObjectName(QString::fromUtf8("addDataSourceButton_"));

	addAnalysisToolButtonLayout_->addWidget(addDataSourceButton_);

	horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	addAnalysisToolButtonLayout_->addItem(horizontalSpacer_2);


	verticalLayout_->addLayout(addAnalysisToolButtonLayout_);

	analysisDetailsLayout_ = new QGridLayout();
	analysisDetailsLayout_->setObjectName(QString::fromUtf8("analysisDetailsLayout_"));
	analysisDetailsLayout_->setContentsMargins(-1, 0, -1, -1);
	nameEdit_ = new QLineEdit();
	nameEdit_->setObjectName(QString::fromUtf8("nameEdit_"));
	nameEdit_->setFrame(false);

	analysisDetailsLayout_->addWidget(nameEdit_, 0, 1, 1, 1);

	descriptionLabel_ = new QLabel();
	descriptionLabel_->setObjectName(QString::fromUtf8("descriptionLabel_"));
	descriptionLabel_->setStyleSheet(QString::fromUtf8("font: bold \"Lucida Grande\";\n"
"color: rgb(121,121,121);"));
	descriptionLabel_->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

	analysisDetailsLayout_->addWidget(descriptionLabel_, 1, 0, 1, 1);

	descriptionEdit_ = new QLineEdit();
	descriptionEdit_->setObjectName(QString::fromUtf8("descriptionEdit_"));
	descriptionEdit_->setFrame(false);

	analysisDetailsLayout_->addWidget(descriptionEdit_, 1, 1, 1, 1);

	nameLabel_ = new QLabel();
	nameLabel_->setObjectName(QString::fromUtf8("nameLabel_"));
	nameLabel_->setStyleSheet(QString::fromUtf8("font: bold \"Lucida Grande\";\n"
"color: rgb(121,121,121);"));
	nameLabel_->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

	analysisDetailsLayout_->addWidget(nameLabel_, 0, 0, 1, 1);


	verticalLayout_->addLayout(analysisDetailsLayout_);

	lineBreak_ = new QFrame();
	lineBreak_->setObjectName(QString::fromUtf8("lineBreak_"));
	lineBreak_->setFrameShape(QFrame::HLine);
	lineBreak_->setFrameShadow(QFrame::Sunken);

	verticalLayout_->addWidget(lineBreak_);

	detailEditorLayout_ = new QVBoxLayout();
	detailEditorLayout_->setObjectName(QString::fromUtf8("detailEditorLayout_"));

	verticalLayout_->addLayout(detailEditorLayout_);

	setWindowTitle(QApplication::translate("AMDataSourcesEditor", "Form", 0, QApplication::UnicodeUTF8));
	addDataSourceButton_->setText(QApplication::translate("AMDataSourcesEditor", "Add Analysis Tool", 0, QApplication::UnicodeUTF8));
	descriptionLabel_->setText(QApplication::translate("AMDataSourcesEditor", "description", 0, QApplication::UnicodeUTF8));
	nameLabel_->setText(QApplication::translate("AMDataSourcesEditor", "name", 0, QApplication::UnicodeUTF8));

	// Modify and adjust UI components
	nameEdit_->setReadOnly(true);	// you can't edit existing data source names.
	// When making new names for data sources, they better be mathematically sound variable names (No spaces, alphabetic character at beginning, no funky symbols, etc...)
	nameEdit_->setValidator(new QRegExpValidator(QRegExp("[A-Za-z_]\\w*"), this));



	// apply 'close item' boxes onto the list of data sources
	itemDelegate_ = new AMCloseItemDelegate();
	itemDelegate_->setCloseButtonsEnabled(true);
	connect(itemDelegate_, SIGNAL(closeButtonClicked(QModelIndex)), this, SLOT(onCloseButtonClicked(QModelIndex)));
	scanSetView_->setItemDelegate(itemDelegate_);

	// apply the existing scan set model to the data source list view:
	model_ = model;
	scanSetView_->setModel(model_);
	connect(scanSetView_->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onSetViewIndexChanged(QModelIndex,QModelIndex)));

	// don't show the drop-down arrows for scans. (Saves a bit of room inside the list of channels.)
	scanSetView_->setRootIsDecorated(false);

	addDataSourceButton_->setDisabled(true);
	connect(addDataSourceButton_, SIGNAL(clicked()), this, SLOT(onAddDataSourceButtonClicked()));

	editingNewDataSourceName_ =  false;
	nameOfAnalysisBlockToBeAdded_ = "";

	connect(descriptionEdit_, SIGNAL(editingFinished()), this, SLOT(descriptionEditingFinished()));

	showAllDataSources_ = false;
	scanSetView_->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(scanSetView_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));

	setLayout(verticalLayout_);
}

AMDataSourcesEditor::~AMDataSourcesEditor()
{
	if(itemDelegate_){
		itemDelegate_->deleteLater();
		itemDelegate_ = 0;
	}
}

void AMDataSourcesEditor::onCustomContextMenuRequested(QPoint point)
{
	QMenu popup(this);
	QAction *temp = popup.addAction("Show all data sources");
	temp->setCheckable(true);
	temp->setChecked(showAllDataSources_);

	temp = popup.exec(mapToGlobal(scanSetView_->mapToParent(point)));

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
			scanSetView_->setRowHidden(i, modelIndex, false);
	}

	else{

		for (int di = 0; di < model_->scanAt(scanIndex)->dataSourceCount(); di++)
			scanSetView_->setRowHidden(di, modelIndex, model_->dataSourceAt(scanIndex, di)->hiddenFromUsers());
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
		scanSetView_->setCurrentIndex(QModelIndex());
		scanSetView_->collapseAll();
		return;
	}

	scanSetView_->collapseAll();
	QModelIndex i = model_->indexForScan(scanIndex);
	scanSetView_->expand(i);
	scanSetView_->setCurrentIndex(i);
	showAllDataSources(false);
}

void AMDataSourcesEditor::onSetViewIndexChanged(const QModelIndex &selected, const QModelIndex &deselected) {

	removeDetailEditor();

	// Nothing selected?
	////////////////////////
	if(!selected.isValid()) {
		nameEdit_->clear();
		descriptionEdit_->clear();
		descriptionEdit_->setReadOnly(true);
		addDataSourceButton_->setDisabled(true);
		return;
	}

	addDataSourceButton_->setEnabled(true);

	// Scan selected? (No selected data source)
	/////////////////////////
	if(!selected.parent().isValid()) {
		nameEdit_->clear();
		descriptionEdit_->clear();
		descriptionEdit_->setReadOnly(true);
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

	nameEdit_->setText(dataSource->name());
	descriptionEdit_->setText(dataSource->description());
	descriptionEdit_->setReadOnly(false);

	installDetailEditor(dataSource->createEditorWidget());

	// Set the new selected data source as the exclusive view preference.
	model_->setExclusiveDataSourceByName(dataSource->name());
}

void AMDataSourcesEditor::onDataSourceDescriptionChanged()
{
	QModelIndex index = scanSetView_->currentIndex();
	AMDataSource *dataSource = model_->dataSourceAt(index.parent().row(), index.row());
	if (dataSource){

		descriptionEdit_->setText(dataSource->description());
		scanSetView_->update(index);
	}
}


void AMDataSourcesEditor::onCloseButtonClicked(const QModelIndex &index) {

	// handle data source-level indexes only:
	if(!index.parent().isValid())
		return;

	int dataSourceIndex = index.row();
	int scanIndex = index.parent().row();

	AMScan* scan = model_->scanAt(scanIndex);
	if(!scan || dataSourceIndex >= scan->dataSourceCount())
		return;

	if (dataSourceIndex < scan->rawDataSourceCount()){

		QMessageBox::warning(this, "Can not remove raw data sources.", QString("Acquaman does not allow the deletion of raw data sources."));
		return;
	}

	AMDataSource* dataSource = scan->dataSourceAt(dataSourceIndex);
	int response = QMessageBox::question(this, "Remove Data Source?", QString("Remove this data source? \n\n'%1' (%2)\n\nThe data source will be deleted, and no longer visible in any plots. Any other data sources that depend on this data source will be reset.  Raw data will NOT be deleted, and you can re-create the data source later if you need it.").arg(dataSource->description()).arg(dataSource->name()), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

	if(response == QMessageBox::Ok)
		scan->deleteDataSourceAt(dataSourceIndex);
}

int AMDataSourcesEditor::currentScanIndex() const {
	QModelIndex i = scanSetView_->currentIndex();
	if(!i.isValid())
		return -1;

	if(!i.parent().isValid())
		return i.row();

	else
		return i.parent().row();

}


int AMDataSourcesEditor::currentDataSourceIndex() const {
	QModelIndex i = scanSetView_->currentIndex();
	if(!i.parent().isValid())
		return -1;

	return i.row();
}

void AMDataSourcesEditor::onAddDataSourceButtonClicked() {

	int scanIndex = currentScanIndex();
	QString newAnalysisBlock = AMAddAnalysisBlockDialog::getAnalysisBlock(model_->scanAt(scanIndex)->scanRank());

	if (!newAnalysisBlock.isEmpty()){

		nameOfAnalysisBlockToBeAdded_ = newAnalysisBlock;
		removeDetailEditor();

		// Data source names aren't editable after they've been created. Here we re-enable the nameEdit for editing shortly, and connect its editingFinished ... We'll complete the operation in onNewChannelNamed().
		editingNewDataSourceName_ = true;
		nameEdit_->setReadOnly(false);
		nameEdit_->setText("newDataSourceName");
		nameEdit_->setFocus();
		nameEdit_->selectAll();

		descriptionEdit_->clear();

		connect(nameEdit_, SIGNAL(editingFinished()), this, SLOT(onNewDataSourceNamed()));
	}

}

void AMDataSourcesEditor::onNewDataSourceNamed() {

	if(!editingNewDataSourceName_)
		return;

	disconnect(nameEdit_, SIGNAL(editingFinished()), this, SLOT(onNewDataSourceNamed()));

	editingNewDataSourceName_ = false;
	QString chName = nameEdit_->text();
	nameEdit_->clearFocus();
	nameEdit_->setReadOnly(true);


	int si = currentScanIndex();
	if(si < 0 || si >= model_->scanCount()) {
		// this should have never happened. How did we get here? You shouldn't have been able to press the addNewChannel button without a current scan.
		return;
	}

	if(chName.isEmpty())
		return;


	if(model_->scanAt(si)->indexOfDataSource(chName) != -1) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -1, QString("Couldn't create a new data source with the name \"%1\". Make sure to choose a name that doesn't exist already in this scan.").arg(chName)));
		scanSetView_->setCurrentIndex(scanSetView_->currentIndex());
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

	newAnalysisBlock = qobject_cast<AMAnalysisBlock *>(AMDbObjectSupport::s()->objectInfoForClass(nameOfAnalysisBlockToBeAdded_)->metaObject->newInstance(Q_ARG(QString, chName)));

	// This should always happen.  But just to be safe.
	if (newAnalysisBlock){

		if (newAnalysisBlock->desiredInputRank() == 1)
			newAnalysisBlock->setInputDataSources(singleDimDataSources);

		else if (newAnalysisBlock->desiredInputRank() == 2)
			newAnalysisBlock->setInputDataSources(twoDimDataSources);

		else if (newAnalysisBlock->desiredInputRank() == 3)
			newAnalysisBlock->setInputDataSources(threeDimDataSources);

		if ( ! scan->addAnalyzedDataSource(newAnalysisBlock))
			newAnalysisBlock->deleteLater();
	}

	int di = scan->dataSourceCount()-1;
	scanSetView_->setCurrentIndex(model_->indexForDataSource(si, di));
		// this should automatically create a new detail editor for this new scan.
}



void AMDataSourcesEditor::descriptionEditingFinished() {
	int si = currentScanIndex();
	int di = currentDataSourceIndex();
	int rawCount = model_->scanAt(si)->rawDataSourceCount();

	if(si <0 || di <0 || di>=model_->scanAt(si)->dataSourceCount())
		return;

	if (di < rawCount)
		model_->scanAt(si)->rawDataSources()->at(di)->setDescription(descriptionEdit_->text());
	else
		model_->scanAt(si)->analyzedDataSources()->at(di-rawCount)->setDescription(descriptionEdit_->text());
}



