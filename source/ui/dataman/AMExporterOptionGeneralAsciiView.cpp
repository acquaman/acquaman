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


#include "AMExporterOptionGeneralAsciiView.h"
#include "ui_AMExporterOptionGeneralAsciiView.h"

#include "dataman/export/AMExporterOptionGeneralAscii.h"

#include <QButtonGroup>
#include <QInputDialog>

AMExporterOptionGeneralAsciiView::AMExporterOptionGeneralAsciiView(AMExporterOptionGeneralAscii* option, QStandardItemModel* availableDataSourcesModel, QWidget *parent) :
	QTabWidget(parent)
{
	setupUi(this);
	option_ = option;

	dataSourceLocationGroup_ = new QButtonGroup(this);
	dataSourceLocationGroup_->addButton(sourcesMainTableButton, AMExporterOptionGeneralAscii::CombineInColumnsMode);
	dataSourceLocationGroup_->addButton(sourcesSeparateSectionButton, AMExporterOptionGeneralAscii::SeparateSectionsMode);
	dataSourceLocationGroup_->addButton(sourcesSeparateFileButton, AMExporterOptionGeneralAscii::SeparateFilesMode);

	columnSeparatorGroup_ = new QButtonGroup(this);
	columnSeparatorGroup_->addButton(columnSeparatorTabButton, 0);
	columnSeparatorGroup_->addButton(columnSeparatorSpaceButton, 1);
	columnSeparatorGroup_->addButton(columnSeparatorCommaButton, 2);
	columnSeparatorGroup_->addButton(columnSeparatorSemicolonButton, 3);
	columnSeparatorGroup_->addButton(columnSeparatorOtherButton, 4);

	newlineGroup_ = new QButtonGroup(this);
	newlineGroup_->addButton(newlineCRLFCheck, 0);
	newlineGroup_->addButton(newlineLFCheck, 1);
	newlineGroup_->addButton(newlineOtherCheck, 2);

	if(availableDataSourcesModel)
		sourcesAvailableListView->setModel(availableDataSourcesModel);


	initializeViewFromOption();

	makeUIConnections();
	makeOptionConnections();
}

AMExporterOptionGeneralAsciiView::~AMExporterOptionGeneralAsciiView()
{
}

void AMExporterOptionGeneralAsciiView::initializeViewFromOption()
{
	headerText->setPlainText(option_->headerText());
	headerIncludeCheck->setChecked(option_->headerIncluded());
	headerText->setEnabled(option_->headerIncluded());

	sourcesIncludedListWidget->clear();
	QStringList dataSources = option_->dataSources();
	foreach(QString ds, dataSources)
		sourcesIncludedListWidget->addItem(ds);
	sourcesIncludedListWidget->clearSelection();

	sourcesDetailsBox->setDisabled(true);
	sourcesLocationBox->setDisabled(true);

	includeAllSourcesCheck->setChecked(option_->includeAllDataSources());
	firstColumnOnlyCheck->setChecked(option_->firstColumnOnly());
	separateHigherDimensionSourcesCheck->setChecked(option_->separateHigherDimensionalSources());
	sourcesSelectorFrame->setEnabled(!option_->includeAllDataSources());
	higherDimensionsInRows->setChecked(option_->higherDimensionsInRows());


	columnHeaderText->setText(option_->columnHeader());
	columnHeaderCheck->setChecked(option_->columnHeaderIncluded());
	columnHeaderText->setEnabled(option_->columnHeaderIncluded());

	sectionHeaderText->setPlainText(option_->sectionHeader());
	sectionHeaderCheck->setChecked(option_->sectionHeaderIncluded());
	sectionHeaderText->setEnabled(option_->sectionHeaderIncluded());


	columnSeparatorOtherText->setDisabled(true);
	QString columnDelimiter = option_->columnDelimiter();
	if(columnDelimiter == "\t")
		columnSeparatorTabButton->setChecked(true);
	else if(columnDelimiter == " ")
		columnSeparatorSpaceButton->setChecked(true);
	else if(columnDelimiter == ",")
		columnSeparatorCommaButton->setChecked(true);
	else if(columnDelimiter == ";")
		columnSeparatorSemicolonButton->setChecked(true);
	else {
		columnSeparatorOtherButton->setChecked(true);
		columnSeparatorOtherText->setText(columnDelimiter);
		columnSeparatorOtherText->setEnabled(true);
	}


	newlineOtherText->setDisabled(true);
	QString newlineString = option_->newlineDelimiter();
	if(newlineString == "\r\n")
		newlineCRLFCheck->setChecked(true);
	else if(newlineString == "\n")
		newlineLFCheck->setChecked(true);
	else {
		newlineOtherCheck->setChecked(true);
		newlineOtherText->setText(newlineString);
		newlineOtherText->setEnabled(true);
	}


	fileNameText->setText(option_->fileName());
	fileNameSeparateSectionText->setText(option_->separateSectionFileName());
}

void AMExporterOptionGeneralAsciiView::makeUIConnections()
{
	connect(headerIncludeCheck, SIGNAL(clicked(bool)), headerText, SLOT(setEnabled(bool)));
	connect(headerText, SIGNAL(textChanged()), this, SLOT(onHeaderTextChanged()));

	connect(includeAllSourcesCheck, SIGNAL(clicked(bool)), sourcesSelectorFrame, SLOT(setDisabled(bool)));
	connect(includeAllSourcesCheck, SIGNAL(toggled(bool)), firstColumnOnlyCheck, SLOT(setEnabled(bool)));
	connect(includeAllSourcesCheck, SIGNAL(toggled(bool)), separateHigherDimensionSourcesCheck, SLOT(setEnabled(bool)));

	connect(sourcesAddButton, SIGNAL(clicked()), this, SLOT(onSourcesAddButtonClicked()));
	connect(sourcesRemoveButton, SIGNAL(clicked()), this, SLOT(onSourcesRemoveButtonClicked()));
	connect(sourcesAddExtraButton, SIGNAL(clicked()), this, SLOT(onSourcesAddExtraButtonClicked()));
	connect(sourcesIncludedListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onSourcesSelectedChanged(int)));


	connect(sourcesRequiredCheck, SIGNAL(clicked(bool)), this, SLOT(onSourcesRequiredClicked(bool)));
	connect(sourcesOmitAxisValuesCheck, SIGNAL(clicked(bool)), this, SLOT(onSourcesOmitAxisValuesClicked(bool)));
	connect(dataSourceLocationGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onSourcesLocationChanged(int)));

	connect(columnHeaderCheck, SIGNAL(clicked(bool)), columnHeaderText, SLOT(setEnabled(bool)));
	connect(sectionHeaderCheck, SIGNAL(clicked(bool)), sectionHeaderText, SLOT(setEnabled(bool)));
	connect(sectionHeaderText, SIGNAL(textChanged()), this, SLOT(onSectionHeaderTextChanged()));

	connect(columnSeparatorGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onColumnSeparatorChanged(int)));
	connect(columnSeparatorOtherText, SIGNAL(textChanged(QString)), this, SLOT(onColumnSeparatorOtherTextChanged(QString)));
	connect(newlineGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onNewlineButtonChanged(int)));
	connect(newlineOtherText, SIGNAL(textChanged(QString)), this, SLOT(onNewlineOtherTextChanged(QString)));

	connect(higherDimensionsInRows, SIGNAL(toggled(bool)), this, SLOT(onHigherDimensionsInRowsToggled(bool)));

	connect(sourcesAvailableListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSourcesAvailableSelectedChanged()));
}


void AMExporterOptionGeneralAsciiView::makeOptionConnections() {

	connect(headerIncludeCheck, SIGNAL(clicked(bool)), option_, SLOT(setHeaderIncluded(bool)));

	connect(columnHeaderText, SIGNAL(textChanged(QString)), option_, SLOT(setColumnHeader(QString)));
	connect(columnHeaderCheck, SIGNAL(clicked(bool)), option_, SLOT(setColumnHeaderIncluded(bool)));

	connect(sectionHeaderCheck, SIGNAL(clicked(bool)), option_, SLOT(setSectionHeaderIncluded(bool)));

	connect(fileNameText, SIGNAL(textChanged(QString)), option_, SLOT(setFileName(QString)));
	connect(fileNameSeparateSectionText, SIGNAL(textChanged(QString)), option_, SLOT(setSeparateSectionFileName(QString)));

	connect(includeAllSourcesCheck, SIGNAL(clicked(bool)), option_, SLOT(setIncludeAllDataSources(bool)));

	connect(firstColumnOnlyCheck, SIGNAL(toggled(bool)), option_, SLOT(setFirstColumnOnly(bool)));

	connect(separateHigherDimensionSourcesCheck, SIGNAL(toggled(bool)), option_, SLOT(setSeparateHigherDimensionalSources(bool)));
}


// disable add button when none selected, enable when one is.
void AMExporterOptionGeneralAsciiView::onSourcesAvailableSelectedChanged() {

	if(sourcesAvailableListView->selectionModel()->selectedIndexes().count() > 0)
		sourcesAddButton->setEnabled(true);
	else
		sourcesAddButton->setEnabled(false);
}

// add source
void AMExporterOptionGeneralAsciiView::onSourcesAddButtonClicked() {

	foreach(QModelIndex index, sourcesAvailableListView->selectionModel()->selectedIndexes()) {
		if(index.column() == 0) {
			QString newSourceName =  index.data(AM::NameRole).toString();
			option_->addDataSource(newSourceName, false);
			sourcesIncludedListWidget->addItem(newSourceName);
		}
	}

	// question... does this fire up onSourcesSelectedChanged() automatically?
}

// remove source
void AMExporterOptionGeneralAsciiView::onSourcesRemoveButtonClicked() {
	int removeAtIndex = sourcesIncludedListWidget->currentRow();
	if(removeAtIndex == -1)
		return;

	int newSelectedIndex = removeAtIndex - 1;
	if(newSelectedIndex == -1 && sourcesIncludedListWidget->count() > 1)
		newSelectedIndex = 0;

	// block signals so we don't trigger onSourcesSelectedChanged()
	sourcesIncludedListWidget->blockSignals(true);

	sourcesIncludedListWidget->takeItem(removeAtIndex);
	option_->removeDataSourceAt(removeAtIndex);
	sourcesIncludedListWidget->blockSignals(false);
	sourcesIncludedListWidget->setCurrentRow(newSelectedIndex, QItemSelectionModel::ClearAndSelect);
	// check that onSourcesSelectedChanged happens here? Nope... Need to call manually.
	onSourcesSelectedChanged(newSelectedIndex);
}

// dialog prompt and add source
void AMExporterOptionGeneralAsciiView::onSourcesAddExtraButtonClicked() {
	QString customSourceName = QInputDialog::getText(this, "Add custom data set", "Include a custom data set named:");
	if(!customSourceName.isEmpty()) {
		option_->addDataSource(customSourceName, false, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sourcesIncludedListWidget->addItem(customSourceName);
	}
}

// if index valid: enable edit boxes, block signals and set info boxes to match source, re-enable signals, enable remove button.  If not valid: disable remove button, disable edit boxes
void AMExporterOptionGeneralAsciiView::onSourcesSelectedChanged(int index) {

	if(index == -1) {
		sourcesRemoveButton->setEnabled(false);
		sourcesLocationBox->setEnabled(false);
		sourcesDetailsBox->setEnabled(false);
	}
	else {
		sourcesRemoveButton->setEnabled(true);
		sourcesLocationBox->setEnabled(true);
		sourcesDetailsBox->setEnabled(true);

		sourcesRequiredCheck->blockSignals(true);
		sourcesOmitAxisValuesCheck->blockSignals(true);
		dataSourceLocationGroup_->blockSignals(true);

		sourcesRequiredCheck->setChecked(option_->isDataSourceRequired(index));
		sourcesOmitAxisValuesCheck->setChecked(option_->isDataSourceAxisValueColumnOmitted(index));
		dataSourceLocationGroup_->button(option_->dataSourceOrganizeMode(index))->setChecked(true);

		sourcesRequiredCheck->blockSignals(false);
		sourcesOmitAxisValuesCheck->blockSignals(false);
		dataSourceLocationGroup_->blockSignals(false);
	}
}

// if source selected, set its required state
void AMExporterOptionGeneralAsciiView::onSourcesRequiredClicked(bool isRequired) {

	int index = sourcesIncludedListWidget->currentRow();
	if(index == -1)
		return;

	option_->setDataSourceRequired(index, isRequired);
}

// if source selected, set its omit state
void AMExporterOptionGeneralAsciiView::onSourcesOmitAxisValuesClicked(bool isOmitted) {
	int index = sourcesIncludedListWidget->currentRow();
	if(index == -1)
		return;

	option_->setDataSourceAxisValueColumnOmitted(index, isOmitted);
}

// if source selected, set source's organize mode to match locationMode
void AMExporterOptionGeneralAsciiView::onSourcesLocationChanged(int locationMode) {
	int index = sourcesIncludedListWidget->currentRow();
	if(index == -1)
		return;

	option_->setDataSourceOrganizeMode(index, locationMode);
}

// if separatorButton is not other: set option's separator, disable otherBox. If separatorButton is other, enable otherBox, set option's separator to text of otherBox, selectAll in otherBox.
void AMExporterOptionGeneralAsciiView::onColumnSeparatorChanged(int separatorButton) {

	QString separator;

	switch(separatorButton) {

	case 0:
		separator = "\t";
		break;

	case 1:
		separator = " ";
		break;

	case 2:
		separator = ",";
		break;

	case 3:
		separator = ";";
		break;

	case 4:
		separator = columnSeparatorOtherText->text();	/// \todo Handle escape chars?
		break;
	}

	option_->setColumnDelimiter(separator);

	if(separatorButton == 4) {
		columnSeparatorOtherText->setEnabled(true);
		columnSeparatorOtherText->setFocus();
		columnSeparatorOtherText->selectAll();	// hint for the user to edit this.
	}
	else {
		columnSeparatorOtherText->setEnabled(false);
	}
}

// if "other" mode selected: set option's separator to otherText
void AMExporterOptionGeneralAsciiView::onColumnSeparatorOtherTextChanged(const QString& otherText) {
	if(columnSeparatorOtherButton->isChecked()) {
		option_->setColumnDelimiter(otherText); /// \todo Handle escape characters?
	}
}

// if newlineButton is not "other": set option's newline, disable otherBox.  If newlineButton is "other", enable otherBox, set option's newline to text of otherBox, selectAll in otherBox.
void AMExporterOptionGeneralAsciiView::onNewlineButtonChanged(int newlineButton) {
	QString newline;

	switch(newlineButton) {
	case 0:
		newline = "\r\n";
		break;
	case 1:
		newline = "\n";
		break;
	case 2:
		newline = newlineOtherText->text();	/// \todo Handle escape chars?
		break;
	}

	option_->setNewlineDelimiter(newline);

	if(newlineButton == 2) {
		newlineOtherText->setEnabled(true);
		newlineOtherText->setFocus();
		newlineOtherText->selectAll();
	}
	else {
		newlineOtherText->setEnabled(false);
	}
}

// if "other" mode selected: set option's newline to otherText.
void AMExporterOptionGeneralAsciiView::onNewlineOtherTextChanged(const QString& otherText) {
	if(newlineOtherCheck->isChecked()) {
		option_->setNewlineDelimiter(otherText); /// \todo Handle escape chars?
	}
}

void AMExporterOptionGeneralAsciiView::onHeaderTextChanged()
{
	option_->setHeaderText(headerText->toPlainText());
}

void AMExporterOptionGeneralAsciiView::onSectionHeaderTextChanged()
{
	option_->setSectionHeader(sectionHeaderText->toPlainText());
}

void AMExporterOptionGeneralAsciiView::onHigherDimensionsInRowsToggled(bool checked)
{
	option_->setHigherDimensionsInRows(checked);
}
