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


#include "SGMFastScanParametersView.h"

#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QStringBuilder>
#include <QMessageBox>

#include "util/SGM/SGMPeriodicTable.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"
#include "util/AMDateTimeUtils.h"

 SGMFastScanParametersView::~SGMFastScanParametersView(){}
SGMFastScanParametersView::SGMFastScanParametersView(SGMFastScanParameters *fastScanParameters, bool disableCopy, QWidget *parent) :
	QWidget(parent)
{
	fastScanParameters_ = fastScanParameters;
	disableCopy_ = disableCopy;
	hasUnsavedChanges_ = false;

	startPositionCopy_ = fastScanParameters_->scanInfo().start();
	middlePositionCopy_ = fastScanParameters_->scanInfo().middle();
	endPositionCopy_ = fastScanParameters_->scanInfo().end();
	fastScanSettingsCopy_ = fastScanParameters_->fastScanSettings();

	//startPositionView_ = new SGMEnergyPositionView(&startPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	//middlePositionView_ = new SGMEnergyPositionView(&middlePositionCopy_, SGMEnergyPositionView::ViewModeMiddle);
	//endPositionView_ = new SGMEnergyPositionView(&endPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	startPositionView_ = new SGMEnergyPositionWBeamlineAndDatabaseView(&startPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	middlePositionView_ = new SGMEnergyPositionWBeamlineAndDatabaseView(&middlePositionCopy_, SGMEnergyPositionView::ViewModeMiddle);
	endPositionView_ = new SGMEnergyPositionWBeamlineAndDatabaseView(&endPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	fastScanSettingsView_ = new SGMFastScanSettingsView(&fastScanSettingsCopy_);

	copyButton_ = new QPushButton("Copy");
	editButton_ = new QPushButton("Edit");

	saveButton_ = new QPushButton("Save");
	cancelButton_ = new QPushButton("Cancel");

	startPositionView_->setEnabled(false);
	middlePositionView_->setEnabled(false);
	endPositionView_->setEnabled(false);
	fastScanSettingsView_->setEnabled(false);

	if(!disableCopy_  && fastScanParameters_->database() && fastScanParameters_->database()->isReadOnly() )
		editButton_->setEnabled(false);

	QVBoxLayout *settingsLayout = new QVBoxLayout();
	settingsLayout->addWidget(fastScanSettingsView_);
	settingsLayout->addStretch(8);
	settingsLayout->setContentsMargins(5, 0, 5, 0);

	QVBoxLayout *positionsLayout = new QVBoxLayout();
	positionsLayout->addWidget(startPositionView_);
	positionsLayout->addWidget(middlePositionView_);
	positionsLayout->addWidget(endPositionView_);
	positionsLayout->addStretch(8);
	positionsLayout->setContentsMargins(5, 0, 5, 0);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addLayout(settingsLayout);
	hl->addLayout(positionsLayout);

	buttonsHL1_ = new QHBoxLayout();
	buttonsHL1_->addStretch(8);
	buttonsHL1_->addWidget(editButton_);
	if(!disableCopy_)
		buttonsHL1_->addWidget(copyButton_);

	buttonsHL2_ = new QHBoxLayout();
	buttonsHL2_->addStretch(8);
	if(!disableCopy_)
		buttonsHL2_->addWidget(saveButton_);
	buttonsHL2_->addWidget(cancelButton_);

	masterVL_ = new QVBoxLayout();
	masterVL_->addLayout(hl);
	masterVL_->addLayout(buttonsHL1_);

	setLayout(masterVL_);

	connect(copyButton_, SIGNAL(clicked()), this, SLOT(onCopyButtonClicked()));
	connect(editButton_, SIGNAL(clicked()), this, SLOT(onEditButtonClicked()));
	connect(saveButton_, SIGNAL(clicked()), this, SLOT(onSaveButtonClicked()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));

	connect(&startPositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onAnySettingChanged()));
	connect(&middlePositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onAnySettingChanged()));
	connect(&endPositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onAnySettingChanged()));
	connect(&fastScanSettingsCopy_, SIGNAL(fastScanSettingsChanged()), this, SLOT(onAnySettingChanged()));
}

bool SGMFastScanParametersView::hasUnsavedChanges() const{
	return hasUnsavedChanges_;
}

void SGMFastScanParametersView::onEditButtonClicked(){
	startPositionView_->setEnabled(true);
	middlePositionView_->setEnabled(true);
	endPositionView_->setEnabled(true);
	fastScanSettingsView_->setEnabled(true);

	masterVL_->removeItem(buttonsHL1_);
	editButton_->hide();
	if(!disableCopy_)
		copyButton_->hide();

	masterVL_->addLayout(buttonsHL2_);
	if(!disableCopy_){
		saveButton_->show();
		saveButton_->setEnabled(false);
	}
	cancelButton_->show();
}

void SGMFastScanParametersView::onCopyButtonClicked(){
	SGMFastScanParameters *fastScanParametersCopy = new SGMFastScanParameters();
	fastScanParametersCopy->operator =(*fastScanParameters_);
	fastScanParametersCopy->dissociateFromDb(false);
	fastScanParametersCopy->fastScanSettings().dissociateFromDb();
	SGMFastScanParametersDatabaseSaveView *copyParametersView = new SGMFastScanParametersDatabaseSaveView(fastScanParametersCopy);
	copyParametersView->show();
}

void SGMFastScanParametersView::onSaveButtonClicked(){
	bool isModified = false;
	QStringList startAlsoList, middleAlsoList, endAlsoList;
	SGMEnergyPositionWBeamlineAndDatabaseView *dbSaveView;
	if(fastScanParameters_->scanInfo().start() != startPositionCopy_){
//		fastScanParameters_->setStartPosition(startPositionCopy_);
		dbSaveView = qobject_cast<SGMEnergyPositionWBeamlineAndDatabaseView*>(startPositionView_);
		if(dbSaveView && dbSaveView->alsoUsedByList().count() > 1){
			startAlsoList = dbSaveView->alsoUsedByList();
			startAlsoList.removeAll(fastScanParameters_->name());
		}
		isModified = true;
	}
	if(fastScanParameters_->scanInfo().middle() != middlePositionCopy_){
//		fastScanParameters_->setMiddlePosition(middlePositionCopy_);
		dbSaveView = qobject_cast<SGMEnergyPositionWBeamlineAndDatabaseView*>(middlePositionView_);
		if(dbSaveView && dbSaveView->alsoUsedByList().count() > 1){
			middleAlsoList = dbSaveView->alsoUsedByList();
			middleAlsoList.removeAll(fastScanParameters_->name());
		}
		isModified = true;
	}
	if(fastScanParameters_->scanInfo().end() != endPositionCopy_){
//		fastScanParameters_->setEndPosition(endPositionCopy_);
		dbSaveView = qobject_cast<SGMEnergyPositionWBeamlineAndDatabaseView*>(endPositionView_);
		if(dbSaveView && dbSaveView->alsoUsedByList().count() > 1){
			endAlsoList = dbSaveView->alsoUsedByList();
			endAlsoList.removeAll(fastScanParameters_->name());
		}
		isModified = true;
	}
	if(fastScanParameters_->fastScanSettings() != fastScanSettingsCopy_){
//		fastScanParameters_->setFastScanSettings(fastScanSettingsCopy_);
		isModified = true;
	}

	QStringList allAlsoList;
	allAlsoList.append(startAlsoList);
	allAlsoList.append(middleAlsoList);
	allAlsoList.append(endAlsoList);
	allAlsoList.removeDuplicates();

	if(isModified){

		bool actuallySave = true;
		if(allAlsoList.count() > 0){
			QMessageBox modifyingManyChoice;
			modifyingManyChoice.setText(QString("These changes will modify other scans."));

			QStringList modifyingPositionsList;
			if(startAlsoList.count() > 0)
				modifyingPositionsList << "start";
			if(middleAlsoList.count() > 0)
				modifyingPositionsList << "middle";
			if(endAlsoList.count() > 0)
				modifyingPositionsList << "end";
			QString modifyingPositions = AMDateTimeUtils::gramaticallyCorrectList(modifyingPositionsList);
			if(modifyingPositionsList.count() > 1)
				modifyingPositions.append(" positions");
			else
				modifyingPositions.append(" position");

			modifyingManyChoice.setInformativeText(QString("By modifying the %1 you will also affect the following scans:\n %2\n\nDo you wish to proceed?").arg(modifyingPositions).arg(allAlsoList.join("\n")));
			modifyingManyChoice.setIcon(QMessageBox::Question);
			modifyingManyChoice.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			modifyingManyChoice.setDefaultButton(QMessageBox::Ok);
			modifyingManyChoice.setEscapeButton(QMessageBox::Cancel);

			if(modifyingManyChoice.exec() != QMessageBox::Ok)
				actuallySave = false;
		}

		if(actuallySave){
			if(fastScanParameters_->scanInfo().start() != startPositionCopy_)
				fastScanParameters_->setStartPosition(startPositionCopy_);
			if(fastScanParameters_->scanInfo().middle() != middlePositionCopy_)
				fastScanParameters_->setMiddlePosition(middlePositionCopy_);
			if(fastScanParameters_->scanInfo().end() != endPositionCopy_)
				fastScanParameters_->setEndPosition(endPositionCopy_);
			if(fastScanParameters_->fastScanSettings() != fastScanSettingsCopy_)
				fastScanParameters_->setFastScanSettings(fastScanSettingsCopy_);

			fastScanParameters_->storeToDb(fastScanParameters_->database());
		}
		else
			onCancelButtonClicked();
	}
}

void SGMFastScanParametersView::onCancelButtonClicked(){
	if(fastScanParameters_->scanInfo().start() != startPositionCopy_)
		startPositionCopy_ = fastScanParameters_->scanInfo().start();
	if(fastScanParameters_->scanInfo().middle() != middlePositionCopy_)
		middlePositionCopy_ = fastScanParameters_->scanInfo().middle();
	if(fastScanParameters_->scanInfo().end() != endPositionCopy_)
		endPositionCopy_ = fastScanParameters_->scanInfo().end();
	if(fastScanParameters_->fastScanSettings() != fastScanSettingsCopy_)
		fastScanSettingsCopy_ = fastScanParameters_->fastScanSettings();

	startPositionView_->setEnabled(false);
	middlePositionView_->setEnabled(false);
	endPositionView_->setEnabled(false);
	fastScanSettingsView_->setEnabled(false);

	masterVL_->removeItem(buttonsHL2_);
	if(!disableCopy_)
		saveButton_->hide();
	cancelButton_->hide();

	masterVL_->addLayout(buttonsHL1_);
	editButton_->show();
	if(!disableCopy_)
		copyButton_->show();

	hasUnsavedChanges_ = false;
	emit unsavedChanges(hasUnsavedChanges_);
}

void SGMFastScanParametersView::onAnySettingChanged(){
	saveButton_->setEnabled(true);
	hasUnsavedChanges_ = true;
	emit unsavedChanges(hasUnsavedChanges_);
}

 SGMFastScanParametersDatabaseSaveView::~SGMFastScanParametersDatabaseSaveView(){}
SGMFastScanParametersDatabaseSaveView::SGMFastScanParametersDatabaseSaveView(SGMFastScanParameters *fastScanParameters, QWidget *parent) :
	QWidget(parent)
{
	fastScanParameters_ = fastScanParameters;
	parametersView_ = new SGMFastScanParametersView(fastScanParameters_, true);

	databasesComboBox_ = new QComboBox();
	QStandardItemModel *databasesComboBoxModel = qobject_cast<QStandardItemModel*>(databasesComboBox_->model());
	QStringList availableDatabases = AMDatabase::registeredDatabases();
	availableDatabases.removeAll("actions");
	AMDatabase *tempDatabase;
	for(int x = 0; x < availableDatabases.count(); x++){
		tempDatabase = AMDatabase::database(availableDatabases.at(x));
		if(tempDatabase){
			databasesComboBox_->addItem(availableDatabases.at(x), QVariant(availableDatabases.at(x)));

			QVariantList takenNamesInDatabaseAsVariants = tempDatabase->retrieve(AMDbObjectSupport::s()->tableNameForClass<SGMFastScanParameters>(), "name");
			QStringList takenNamesInDatabaseAsString;
			for(int x = 0; x < takenNamesInDatabaseAsVariants.count(); x++)
				takenNamesInDatabaseAsString.append(takenNamesInDatabaseAsVariants.at(x).toString());
			takenNames_.append(takenNamesInDatabaseAsString);

			if(databasesComboBoxModel && tempDatabase->isReadOnly())
				databasesComboBoxModel->item(databasesComboBox_->count()-1, 0)->setFlags(databasesComboBoxModel->item(databasesComboBox_->count()-1, 0)->flags() ^ Qt::ItemIsEnabled);
		}
	}

	bool foundValidItem = false;
	for(int x = 0; (x < databasesComboBox_->count()) && (!foundValidItem); x++){
		if(databasesComboBoxModel->item(x, 0)->flags().testFlag(Qt::ItemIsEnabled)){
			databasesComboBox_->setCurrentIndex(x);
			foundValidItem = true;
		}
	}

	elementsComboBox_ = new QComboBox();
	AMElement *tempElement;
	SGMElementInfo *tempElementInfo;
	for(int x = 0; x < AMPeriodicTable::table()->elements().count(); x++){
		tempElement = AMPeriodicTable::table()->elements().at(x);
		if( (tempElement->KEdge().energy() > 200 && tempElement->KEdge().energy() < 2000) || (tempElement->L2Edge().energy() > 200 && tempElement->L2Edge().energy() < 2000) || (tempElement->M3Edge().energy() > 200 && tempElement->M3Edge().energy() < 2000) ){
			tempElementInfo = SGMPeriodicTable::sgmTable()->elementInfoByName(tempElement->name());
			if(tempElementInfo)
				elementsComboBox_->addItem(tempElement->name(), QVariant(tempElement->atomicNumber()));
			else
				elementsComboBox_->addItem(tempElement->name()%" (!)", QVariant(tempElement->atomicNumber()));
		}
	}
	elementsComboBox_->setCurrentIndex(elementsComboBox_->findData(AMPeriodicTable::table()->elementByName(fastScanParameters_->element())->atomicNumber()));

	QLabel *newNameLabel = new QLabel("New Name");
	newNameLineEdit_ = new QLineEdit();

	QString newNameGuess = fastScanParameters_->name()%"Copy0";
	bool foundValidCopyIndex = false;
	for(int x = 1; !foundValidCopyIndex; x++){
		newNameGuess.remove(newNameGuess.count()-1, 1);
		newNameGuess.append(QString("%1").arg(x));
		if(!takenNames_.at(databasesComboBox_->currentIndex()).contains(newNameGuess)){
			foundValidCopyIndex = true;
			newNameLineEdit_->setText(newNameGuess);
		}
	}

	saveButton_ = new QPushButton("Save");
	saveButton_->setEnabled(false);
	cancelButton_ = new QPushButton("Cancel");

	QHBoxLayout *infoHL = new QHBoxLayout();
	infoHL->addWidget(databasesComboBox_);
	infoHL->addWidget(elementsComboBox_);
	infoHL->addStretch(8);
	infoHL->addWidget(newNameLabel);
	infoHL->addWidget(newNameLineEdit_);

	QHBoxLayout *buttonsHL = new QHBoxLayout();
	buttonsHL->addStretch(8);
	buttonsHL->addWidget(saveButton_);
	buttonsHL->addWidget(cancelButton_);

	QGroupBox *fastScanParametersViewGroupBox = new QGroupBox("New Settings");
	QHBoxLayout *fastScanParametersHL = new QHBoxLayout();
	fastScanParametersHL->addWidget(parametersView_);
	fastScanParametersViewGroupBox->setLayout(fastScanParametersHL);

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addLayout(infoHL);
	mainVL->addWidget(fastScanParametersViewGroupBox);
	mainVL->addLayout(buttonsHL);

	setLayout(mainVL);

	connect(parametersView_, SIGNAL(unsavedChanges(bool)), this, SLOT(onUnsavedChanges(bool)));
	connect(databasesComboBox_, SIGNAL(activated(int)), this, SLOT(onDatabasesComboBoxIndexChanged(int)));
	connect(newNameLineEdit_, SIGNAL(textEdited(QString)), this, SLOT(onNewNameLineEditTextEdited(QString)));
	connect(saveButton_, SIGNAL(clicked()), this, SLOT(onSaveButtonClicked()));

	checkNewNameIsValid();
}

void SGMFastScanParametersDatabaseSaveView::onUnsavedChanges(bool hasUnsavedChanges){
	saveButton_->setEnabled(hasUnsavedChanges);
}

void SGMFastScanParametersDatabaseSaveView::onDatabasesComboBoxIndexChanged(int index){
	Q_UNUSED(index)
	checkNewNameIsValid();
}

void SGMFastScanParametersDatabaseSaveView::onNewNameLineEditTextEdited(const QString &text){
	Q_UNUSED(text)
	checkNewNameIsValid();
}

void SGMFastScanParametersDatabaseSaveView::checkNewNameIsValid(){
	QPalette editPalette = newNameLineEdit_->palette();

	if(takenNames_.at(databasesComboBox_->currentIndex()).contains(newNameLineEdit_->text())){
		saveButton_->setEnabled(false);
		editPalette.setBrush(QPalette::WindowText, QBrush(Qt::red));
		editPalette.setBrush(QPalette::HighlightedText, QBrush(Qt::red));
		editPalette.setBrush(QPalette::Text, QBrush(Qt::red));
	}
	else{
		saveButton_->setEnabled(parametersView_->hasUnsavedChanges());
		editPalette.setBrush(QPalette::WindowText, QBrush(Qt::green));
		editPalette.setBrush(QPalette::HighlightedText, QBrush(Qt::green));
		editPalette.setBrush(QPalette::Text, QBrush(Qt::green));
	}
	newNameLineEdit_->setPalette(editPalette);
}

void SGMFastScanParametersDatabaseSaveView::onSaveButtonClicked(){
	AMDatabase *saveToDatabase = AMDatabase::database(databasesComboBox_->currentText());
	if(!saveToDatabase){
		AMErrorMon::alert(this, SGMFASTSCANPARAMETERSVIEW_FAILED_TO_LOAD_SAVE_DATABASE, QString("Failed to open to database named %1 for saving new SGMFastScanParameters.").arg(databasesComboBox_->currentText()));
		return;
	}

	if(takenNames_.at(databasesComboBox_->currentIndex()).contains(newNameLineEdit_->text())){
		AMErrorMon::alert(this, SGMFASTSCANPARAMETERSVIEW_FAILED_NAMING_CHECK, QString("Failed to save new SGMFastScanParameters to because the name %1 is already taken.").arg(newNameLineEdit_->text()));
		return;
	}
	fastScanParameters_->setName(newNameLineEdit_->text());

	if(!fastScanParameters_->storeToDb(saveToDatabase))
		AMErrorMon::alert(this, SGMFASTSCANPARAMETERSVIEW_FAILED_TO_SAVE_NEW_PARAMETERS, QString("Failed to save new SGMFastScanParameters to database named %1.").arg(saveToDatabase->connectionName()));
}
