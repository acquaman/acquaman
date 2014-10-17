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


#include "SGMPeriodicTableView.h"

#include <QBoxLayout>
#include <QMenu>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QStringBuilder>
#include <QMessageBox>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QDoubleSpinBox>

#include "ui/dataman/AMDbObjectGeneralView.h"
#include "ui/dataman/AMDbObjectGeneralViewSupport.h"
#include "ui/util/SGM/SGMFastScanParametersView.h"
#include "ui/util/SGM/SGMEnergyPositionView.h"
#include "ui/util/SGM/SGMFastScanSettingsView.h"
#include "util/AMDateTimeUtils.h"
#include "dataman/database/AMDbObjectSupport.h"

 SGMPeriodicTableView::~SGMPeriodicTableView(){}
SGMPeriodicTableView::SGMPeriodicTableView(SGMPeriodicTable *sgmPeriodicTable, QWidget *parent) :
	QWidget(parent)
{
	sgmPeriodicTable_ = sgmPeriodicTable;
	periodicTableView_ = new AMPeriodicTableView(this);

	availableScansMenu_ = 0;

	AMElement *tempElement;
	for(int x = 0; x < AMPeriodicTable::table()->elements().count(); x++){
		tempElement = AMPeriodicTable::table()->elements().at(x);
		periodicTableView_->button(tempElement)->setEnabled(false);
		if( (tempElement->KEdge().energy() > 200 && tempElement->KEdge().energy() < 2000) || (tempElement->L2Edge().energy() > 200 && tempElement->L2Edge().energy() < 2000) || (tempElement->M3Edge().energy() > 200 && tempElement->M3Edge().energy() < 2000) )
			periodicTableView_->button(tempElement)->setEnabled(true);
	}

	connect(periodicTableView_, SIGNAL(clicked(int)), this, SLOT(onClicked(int)));

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(periodicTableView_);
	setLayout(vl);
}

void SGMPeriodicTableView::onClicked(int atomicNumber){
	qDebug() << "Clicked on atomic number " << atomicNumber;

	QString elementName = AMPeriodicTable::table()->elementByAtomicNumber(atomicNumber)->name();
	QList<SGMFastScanParameters*> allFastScans = SGMPeriodicTable::sgmTable()->fastScanPresets(SGMPeriodicTable::SGMPeriodicTableAllDatabasesConnectionName());

	if(availableScansMenu_)
		availableScansMenu_->deleteLater();
	availableScansMenu_ = new QMenu();
	QAction *tempAction;
	bool foundOne = false;
	for(int x = 0; x < allFastScans.count(); x++){
		if(allFastScans.at(x)->element() == elementName){
			qDebug() << "Found that element " << elementName << " at index " << x << " from " << allFastScans.at(x)->database()->connectionName();
			foundOne = true;

			tempAction = availableScansMenu_->addAction(QString("%1 (%2)").arg(allFastScans.at(x)->name()).arg(allFastScans.at(x)->database()->connectionName()));
			tempAction->setData(x);
			connect(tempAction, SIGNAL(hovered()), this, SLOT(onHoveredAvailableScansMenu()));
			connect(tempAction, SIGNAL(triggered()), this, SLOT(onTriggerAvailableScansMenu()));
		}
	}
	if(!foundOne){
		tempAction = availableScansMenu_->addAction("None Available");
		tempAction->setEnabled(false);
	}

	availableScansMenu_->popup(QCursor::pos());
	availableScansMenu_->show();
}

void SGMPeriodicTableView::onHoveredAvailableScansMenu(){
	QAction *tempAction;
	tempAction = availableScansMenu_->activeAction();
	activeMenuData_ = tempAction->data().toInt();
}

void SGMPeriodicTableView::onTriggerAvailableScansMenu(){
	emit fastScanChosen(activeMenuData_);
}


 SGMFastScanParametersModificationWizardPeriodicTablePage::~SGMFastScanParametersModificationWizardPeriodicTablePage(){}
SGMFastScanParametersModificationWizardPeriodicTablePage::SGMFastScanParametersModificationWizardPeriodicTablePage(QWidget *parent) :
	QWizardPage(parent)
{
	indexOfFastScan_ = -1;
	sgmPeriodicTableView_ = new SGMPeriodicTableView(SGMPeriodicTable::sgmTable());

	selectedScanLabel_ = new QLabel();

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(sgmPeriodicTableView_);
	vl->addWidget(selectedScanLabel_);
	setLayout(vl);

	setTitle("Select an Element and Scan to Modify or Copy");
	setSubTitle("Acquaman can help you modify the settings for an existing fast scan or allow you to copy a scan to make a brand new one");

	connect(sgmPeriodicTableView_, SIGNAL(fastScanChosen(int)), this, SLOT(onFastScanChosen(int)));
}

bool SGMFastScanParametersModificationWizardPeriodicTablePage::validatePage(){
	if(indexOfFastScan_ < 0)
		return false;

	fastScanWizard_->setOriginatingFastScanIndex(indexOfFastScan_);
	return true;
}

bool SGMFastScanParametersModificationWizardPeriodicTablePage::isComplete() const{
	bool retVal = true;
	if(indexOfFastScan_ < 0)
		retVal = false;
	fastScanWizard_->button(QWizard::NextButton)->setEnabled(retVal);
	return retVal;
}

void SGMFastScanParametersModificationWizardPeriodicTablePage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());

	QTimer::singleShot(0, this, SLOT(checkIsComplete()));
}

void SGMFastScanParametersModificationWizardPeriodicTablePage::onFastScanChosen(int indexOfFastScan){
	indexOfFastScan_ = indexOfFastScan;
	selectedScanLabel_->setText(QString("Selected Scan: %1").arg(SGMPeriodicTable::sgmTable()->fastScanPresets(SGMPeriodicTable::SGMPeriodicTableAllDatabasesConnectionName()).at(indexOfFastScan_)->name()));
	isComplete();
}

void SGMFastScanParametersModificationWizardPeriodicTablePage::checkIsComplete(){
	isComplete();
}

 SGMFastScanParametersModificationWizardEditOrCopyPage::~SGMFastScanParametersModificationWizardEditOrCopyPage(){}
SGMFastScanParametersModificationWizardEditOrCopyPage::SGMFastScanParametersModificationWizardEditOrCopyPage(QWidget *parent) :
	QWizardPage(parent)
{
	editInformationLabel_ = new QLabel();
	copyInformationLabel_ = new QLabel();

	editRadioButton_ = new QRadioButton("Edit this Scan");
	copyRadioButton_ = new QRadioButton("Copy this Scan");
	editOrCopyButtonGroup_ = new QButtonGroup();
	editOrCopyButtonGroup_->addButton(editRadioButton_);
	editOrCopyButtonGroup_->addButton(copyRadioButton_);

	QHBoxLayout *tempHL;
	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(editInformationLabel_);
	tempHL = new QHBoxLayout();
	tempHL->addWidget(editRadioButton_);
	tempHL->addStretch(8);
	vl->addLayout(tempHL);
	vl->addStretch(4);
	vl->addWidget(copyInformationLabel_);
	tempHL = new QHBoxLayout();
	tempHL->addWidget(copyRadioButton_);
	tempHL->addStretch(8);
	vl->addLayout(tempHL);
	vl->addStretch(8);

	QHBoxLayout *masterHL = new QHBoxLayout();
	masterHL->addLayout(vl);
	masterHL->addStretch(8);

	setLayout(masterHL);

	setTitle("Available Editing and Copying Options");

	connect(editOrCopyButtonGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
}

bool SGMFastScanParametersModificationWizardEditOrCopyPage::validatePage(){
	if(!editOrCopyButtonGroup_->checkedButton())
		return false;
	return true;
}

bool SGMFastScanParametersModificationWizardEditOrCopyPage::isComplete() const{
	bool retVal = true;
	if(!editOrCopyButtonGroup_->checkedButton())
		retVal = false;
	fastScanWizard_->button(QWizard::NextButton)->setEnabled(retVal);
	return retVal;
}

void SGMFastScanParametersModificationWizardEditOrCopyPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());

	if(fastScanWizard_->originatingFastScanParameters()->database()->isReadOnly()){
		editInformationLabel_->setText(QString("The fast scan named %1 is located in the %2 database, which you do not have write access to. You cannot edit this scan.").arg(fastScanWizard_->originatingFastScanParameters()->name()).arg(fastScanWizard_->originatingFastScanParameters()->database()->connectionName()));
		editRadioButton_->setEnabled(false);
	}
	else
		editInformationLabel_->setText(QString("Click this button to edit the scan named %1").arg(fastScanWizard_->originatingFastScanParameters()->name()));

	copyInformationLabel_->setText("Click this button to make a copy of the scan you can edit and save for later");

	setSubTitle(QString("Choose to edit or copy the fast scan named %1, depending on database access you have.").arg(fastScanWizard_->originatingFastScanParameters()->name()));
	QTimer::singleShot(0, this, SLOT(checkIsComplete()));
}

int SGMFastScanParametersModificationWizardEditOrCopyPage::nextId() const{
	if(editOrCopyButtonGroup_->checkedButton() == editRadioButton_)
		return SGMFastScanParametersModificationWizard::EditPage;
	else if(editOrCopyButtonGroup_->checkedButton() == copyRadioButton_)
		return SGMFastScanParametersModificationWizard::CopyDestinationSelectionPage;
	return 0;
}

void SGMFastScanParametersModificationWizardEditOrCopyPage::checkIsComplete(){
	isComplete();
}

void SGMFastScanParametersModificationWizardEditOrCopyPage::onButtonClicked(QAbstractButton *button){
	Q_UNUSED(button)
	emit completeChanged();
}

 SGMFastScanParametersModificationWizardEditPage::~SGMFastScanParametersModificationWizardEditPage(){}
SGMFastScanParametersModificationWizardEditPage::SGMFastScanParametersModificationWizardEditPage(QWidget *parent) :
	QWizardPage(parent)
{
	startPositionView_ = 0; //NULL
	middlePositionView_ = 0; //NULL
	endPositionView_ = 0; //NULL
	fastScanSettingsView_ = 0; //NULL

	setTitle("Edit the Scan's Parameters");
}

bool SGMFastScanParametersModificationWizardEditPage::validatePage(){
	bool isModified = false;
	QStringList startAlsoList, middleAlsoList, endAlsoList;
	SGMEnergyPositionWBeamlineAndDatabaseView *dbSaveView;
	if(fastScanParameters_->scanInfo().start() != startPositionCopy_){
		dbSaveView = qobject_cast<SGMEnergyPositionWBeamlineAndDatabaseView*>(startPositionView_);
		if(dbSaveView && dbSaveView->alsoUsedByList().count() > 1){
			startAlsoList = dbSaveView->alsoUsedByList();
			startAlsoList.removeAll(fastScanParameters_->name());
		}
		isModified = true;
	}
	if(fastScanParameters_->scanInfo().middle() != middlePositionCopy_){
		dbSaveView = qobject_cast<SGMEnergyPositionWBeamlineAndDatabaseView*>(middlePositionView_);
		if(dbSaveView && dbSaveView->alsoUsedByList().count() > 1){
			middleAlsoList = dbSaveView->alsoUsedByList();
			middleAlsoList.removeAll(fastScanParameters_->name());
		}
		isModified = true;
	}
	if(fastScanParameters_->scanInfo().end() != endPositionCopy_){
		dbSaveView = qobject_cast<SGMEnergyPositionWBeamlineAndDatabaseView*>(endPositionView_);
		if(dbSaveView && dbSaveView->alsoUsedByList().count() > 1){
			endAlsoList = dbSaveView->alsoUsedByList();
			endAlsoList.removeAll(fastScanParameters_->name());
		}
		isModified = true;
	}
	if(fastScanParameters_->fastScanSettings() != fastScanSettingsCopy_)
		isModified = true;

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
			return true;
		}
	}
	return false;
}

bool SGMFastScanParametersModificationWizardEditPage::isComplete() const{
	fastScanWizard_->button(QWizard::FinishButton)->setEnabled(hasUnsavedChanges_);
	return !hasUnsavedChanges_;
}

void SGMFastScanParametersModificationWizardEditPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());
	fastScanParameters_ = fastScanWizard_->originatingFastScanParameters();
	hasUnsavedChanges_ = false;

	startPositionCopy_ = fastScanParameters_->scanInfo().start();
	middlePositionCopy_ = fastScanParameters_->scanInfo().middle();
	endPositionCopy_ = fastScanParameters_->scanInfo().end();
	fastScanSettingsCopy_ = fastScanParameters_->fastScanSettings();

	if(!startPositionView_){
		startPositionView_ = new SGMEnergyPositionWBeamlineAndDatabaseView(&startPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
		middlePositionView_ = new SGMEnergyPositionWBeamlineAndDatabaseView(&middlePositionCopy_, SGMEnergyPositionView::ViewModeMiddle);
		endPositionView_ = new SGMEnergyPositionWBeamlineAndDatabaseView(&endPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
		fastScanSettingsView_ = new SGMFastScanSettingsView(&fastScanSettingsCopy_);

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

		setLayout(hl);

		connect(&startPositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onAnySettingsChanged()));
		connect(&middlePositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onAnySettingsChanged()));
		connect(&endPositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onAnySettingsChanged()));
		connect(&fastScanSettingsCopy_, SIGNAL(fastScanSettingsChanged()), this, SLOT(onAnySettingsChanged()));
	}

	setSubTitle(QString("Using the fields below you can edit the scan named %1. You may need to be cautious, changes to the Energy Positions often affect other scans as well. You will be warned if this is the case.").arg(fastScanParameters_->name()));
	QTimer::singleShot(0, this, SLOT(onAnySettingsChanged()));
}

void SGMFastScanParametersModificationWizardEditPage::onAnySettingsChanged(){
	if( (fastScanParameters_->scanInfo().start() != startPositionCopy_)
		|| (fastScanParameters_->scanInfo().middle() != middlePositionCopy_)
		|| (fastScanParameters_->scanInfo().end() != endPositionCopy_)
		|| (fastScanParameters_->fastScanSettings() != fastScanSettingsCopy_))
		hasUnsavedChanges_ = true;
	else
		hasUnsavedChanges_ = false;

	isComplete();
}


 SGMFastScanParametersModificationWizardCopyDestinationSelectionPage::~SGMFastScanParametersModificationWizardCopyDestinationSelectionPage(){}
SGMFastScanParametersModificationWizardCopyDestinationSelectionPage::SGMFastScanParametersModificationWizardCopyDestinationSelectionPage(QWidget *parent) :
	QWizardPage(parent)
{
	sourceDatabaseLabel_ = new QLabel();
	sourceElementLabel_ = new QLabel();
	sourceNameLabel_ = new QLabel();

	databasesComboBox_ = new QComboBox();
	elementsComboBox_ = new QComboBox();

	warningsLabel_ = new QLabel();

	QVBoxLayout *sourceVL = new QVBoxLayout();
	sourceVL->addWidget(sourceDatabaseLabel_);
	sourceVL->addWidget(sourceElementLabel_);
	sourceVL->addWidget(sourceNameLabel_);

	QHBoxLayout *tempHL;
	QVBoxLayout *desintationVL = new QVBoxLayout();
	tempHL = new QHBoxLayout();
	tempHL->addWidget(new QLabel("Destination Database:"));
	tempHL->addWidget(databasesComboBox_);
	desintationVL->addLayout(tempHL);
	tempHL = new QHBoxLayout();
	tempHL->addWidget(new QLabel("Destination Element:"));
	tempHL->addWidget(elementsComboBox_);
	desintationVL->addLayout(tempHL);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addLayout(sourceVL);
	hl->addStretch(8);
	hl->addLayout(desintationVL);

	QVBoxLayout *masterVL = new QVBoxLayout();
	masterVL->addLayout(hl);
	masterVL->addWidget(warningsLabel_);

	setLayout(masterVL);

	setTitle("Select the Destination Database and Element of Interest");

	connect(databasesComboBox_, SIGNAL(activated(int)), this, SLOT(onDatabaseComboBoxIndexChanged(int)));
}

bool SGMFastScanParametersModificationWizardCopyDestinationSelectionPage::validatePage(){
	qDebug() << "You want to proceed";
	qDebug() << "Database - From: " << originatingFastScanParameters_->database()->connectionName() << " To: " << databasesComboBox_->currentText();
	qDebug() << "Element - From: " << originatingFastScanParameters_->element() << " To: " << elementsComboBox_->currentText();

	// This call always copies the fastScanParameters and does a dissociate(false) and dissociate on the fastScanSettings
	fastScanWizard_->copyOriginalFastScanParametersToNew(AMDatabase::database(databasesComboBox_->currentText()), AMPeriodicTable::table()->elementByName(elementsComboBox_->currentText()));

	// Set the Destination Selection
	// Set as [Same, Same]
	if( (originatingFastScanParameters_->database()->connectionName() == databasesComboBox_->currentText()) && (originatingFastScanParameters_->element() == elementsComboBox_->currentText()) )
		fastScanWizard_->setCopyDestinationSelection(SGMFastScanParametersModificationWizard::CopySameSame);
	// Set as [Same, New]
	else if( (originatingFastScanParameters_->database()->connectionName() == databasesComboBox_->currentText()) && (originatingFastScanParameters_->element() != elementsComboBox_->currentText()) )
		fastScanWizard_->setCopyDestinationSelection(SGMFastScanParametersModificationWizard::CopySameNew);
	// Set as [New, Same]
	else if( (originatingFastScanParameters_->database()->connectionName() != databasesComboBox_->currentText()) && (originatingFastScanParameters_->element() == elementsComboBox_->currentText()) )
		fastScanWizard_->setCopyDestinationSelection(SGMFastScanParametersModificationWizard::CopyNewSame);
	// Set as [New, New]
	else
		fastScanWizard_->setCopyDestinationSelection(SGMFastScanParametersModificationWizard::CopyNewNew);

	// Except for [Same Db, Same Element] always dissociate scanInfo
	if(fastScanWizard_->copyDestinationSelection() != SGMFastScanParametersModificationWizard::CopySameSame){
		qDebug() << "Not doing [Same, Same], need to dissociate scanInfo and all children";

		SGMScanInfo dissociatedScanInfo = fastScanWizard_->newFastScanParameters()->scanInfo();
		dissociatedScanInfo.dissociateFromDb();
		fastScanWizard_->newFastScanParameters()->setScanInfo(dissociatedScanInfo, false);
	}

	// NOT SURE I WANT TO DO THIS HERE
	// In the case of [New Db, Same Element] we will silently call setDb SGMElementInfo and SGMScanInfo into the new database
	if( (originatingFastScanParameters_->database()->connectionName() != databasesComboBox_->currentText()) && (originatingFastScanParameters_->element() == elementsComboBox_->currentText())){
		qDebug() << "In [New Db, Same Element] ... need to silently copy the SGMElementInfo and SGMScanInfo into the new database";
	}

	// In the cases of [Same Db, New Element] and [New Db, New Element], we need to make a new SGMElementInfo and a new SGMScanInfo
	if( (fastScanWizard_->copyDestinationSelection() == SGMFastScanParametersModificationWizard::CopySameNew) || (fastScanWizard_->copyDestinationSelection() == SGMFastScanParametersModificationWizard::CopyNewNew) ){
		qDebug() << "Making a new element, we need a new elementInfo";
		fastScanWizard_->createNewElementInfo();
		fastScanWizard_->createNewScanInfo();

		fastScanWizard_->newFastScanParameters()->setElement(elementsComboBox_->currentText());
		QString newFastScanParametersGuessName = elementsComboBox_->currentText()%fastScanWizard_->newScanInfo()->edge();
		if(fastScanWizard_->newFastScanParameters()->name().contains("5s"))
			newFastScanParametersGuessName.append("5s");
		else if(fastScanWizard_->newFastScanParameters()->name().contains("20s"))
			newFastScanParametersGuessName.append("20s");
		else
			newFastScanParametersGuessName.append("NNs");
		fastScanWizard_->newFastScanParameters()->setName(newFastScanParametersGuessName);

		SGMEnergyPosition editEnergyPosition;
		QString elementEdge = fastScanWizard_->newElementInfo()->element()->name()%fastScanWizard_->newScanInfo()->edge();
		qDebug() << "Element edge is now " << elementEdge;
		editEnergyPosition = fastScanWizard_->newScanInfo()->start();
		editEnergyPosition.setName(elementEdge%"Start");
		fastScanWizard_->newScanInfo()->setStart(editEnergyPosition, false);
		qDebug() << "Check start " << fastScanWizard_->newScanInfo()->start().name() << editEnergyPosition.name();
		editEnergyPosition = fastScanWizard_->newScanInfo()->middle();
		editEnergyPosition.setName(elementEdge%"Middle");
		fastScanWizard_->newScanInfo()->setMiddle(editEnergyPosition, false);
		editEnergyPosition = fastScanWizard_->newScanInfo()->end();
		editEnergyPosition.setName(elementEdge%"End");
		fastScanWizard_->newScanInfo()->setEnd(editEnergyPosition, false);
	}

	return true;
}

bool SGMFastScanParametersModificationWizardCopyDestinationSelectionPage::isComplete() const{
	return true;
}

void SGMFastScanParametersModificationWizardCopyDestinationSelectionPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());
	originatingFastScanParameters_ = fastScanWizard_->originatingFastScanParameters();

	sourceDatabaseLabel_->clear();
	sourceElementLabel_->clear();
	sourceNameLabel_->clear();

	databasesComboBox_->clear();
	elementsComboBox_->clear();

	warningsLabel_->clear();

	sourceDatabaseLabel_->setText(QString("Source Database: %1").arg(originatingFastScanParameters_->database()->connectionName()));
	sourceElementLabel_->setText(QString("Source Element: %1").arg(originatingFastScanParameters_->element()));
	sourceNameLabel_->setText(QString("Source Scan: %1").arg(originatingFastScanParameters_->name()));

	QStandardItemModel *databasesComboBoxModel = qobject_cast<QStandardItemModel*>(databasesComboBox_->model());
	QStringList availableDatabases = AMDatabase::registeredDatabases();
	availableDatabases.removeAll("actions");
	AMDatabase *tempDatabase;
	for(int x = 0; x < availableDatabases.count(); x++){
		tempDatabase = AMDatabase::database(availableDatabases.at(x));
		if(tempDatabase){
			databasesComboBox_->addItem(availableDatabases.at(x), QVariant(availableDatabases.at(x)));

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

	AMElement *tempElement;
	for(int x = 0; x < AMPeriodicTable::table()->elements().count(); x++){
		tempElement = AMPeriodicTable::table()->elements().at(x);
		if( (tempElement->KEdge().energy() > 200 && tempElement->KEdge().energy() < 2000) || (tempElement->L2Edge().energy() > 200 && tempElement->L2Edge().energy() < 2000) || (tempElement->M3Edge().energy() > 200 && tempElement->M3Edge().energy() < 2000) )
			elementsComboBox_->addItem(tempElement->name(), QVariant(tempElement->atomicNumber()));
	}
	elementsComboBox_->setCurrentIndex(elementsComboBox_->findData(AMPeriodicTable::table()->elementByName(originatingFastScanParameters_->element())->atomicNumber()));
	checkValidElementsHelper();

	setSubTitle(QString("The scan %1 comes from a database named %2. Use the boxes on the right to select a database to save your copy to as well as the element you intend to scan.\nPlease note that some databases may not be available to write to.\nAs well, some elements in some databases already have information saved. If the element you wish to select is grayed out, go back and select that element from the periodic table.").arg(originatingFastScanParameters_->name()).arg(originatingFastScanParameters_->database()->connectionName()));
}

int SGMFastScanParametersModificationWizardCopyDestinationSelectionPage::nextId() const{
	// In the case of [Same, Same] go to the ShareEnergyPositionsPage
	if(fastScanWizard_->copyDestinationSelection() == SGMFastScanParametersModificationWizard::CopySameSame)
		return SGMFastScanParametersModificationWizard::ShareEnergyPositionsPage;
	// In the case of [New, Same] go directly to the EditFastScanParametersNamePage
	else if(fastScanWizard_->copyDestinationSelection() == SGMFastScanParametersModificationWizard::CopyNewSame)
		return SGMFastScanParametersModificationWizard::EditFastScanParametersNamePage;
	// In the cases of [Same, New] and [New, New] go to the EditScanInfoPage
	else
		return SGMFastScanParametersModificationWizard::EditScanInfoPage;
}

void SGMFastScanParametersModificationWizardCopyDestinationSelectionPage::onDatabaseComboBoxIndexChanged(int index){
	Q_UNUSED(index)
	checkValidElementsHelper();
}

void SGMFastScanParametersModificationWizardCopyDestinationSelectionPage::checkValidElementsHelper(){
	AMDatabase *destinationDatabase = AMDatabase::database(databasesComboBox_->currentText());
	QStandardItemModel *elementsComboBoxModel = qobject_cast<QStandardItemModel*>(elementsComboBox_->model());

	for(int x = 0; x < elementsComboBox_->count(); x++)
		elementsComboBoxModel->item(x, 0)->setFlags(elementsComboBoxModel->item(x, 0)->flags() | Qt::ItemIsEnabled);

	QVariantList allElementInfoNamesAsVariants = destinationDatabase->retrieve(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name");
	QStringList allElementsInfos;
	for(int x = 0; x < allElementInfoNamesAsVariants.count(); x++)
		allElementsInfos << allElementInfoNamesAsVariants.at(x).toString().remove("ElementInfo");

	for(int x = 0; x < elementsComboBox_->count(); x++)
		if(allElementsInfos.contains(elementsComboBox_->itemText(x)) && (elementsComboBox_->itemText(x) != originatingFastScanParameters_->element()) )
			elementsComboBoxModel->item(x, 0)->setFlags(elementsComboBoxModel->item(x, 0)->flags() ^ Qt::ItemIsEnabled);

}

/////////////////////////////

 SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage::~SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage(){}
SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage::SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage(QWidget *parent) :
	QWizardPage(parent)
{
	shareEnergyPositionsButton_ = new QRadioButton("Share Energy Positions");
	createNewEnergyPositionsButton_ = new QRadioButton("Create Your Own Energy Positions");
	shareOrCreateButtonGroup_ = new QButtonGroup();
	shareOrCreateButtonGroup_->addButton(shareEnergyPositionsButton_);
	shareOrCreateButtonGroup_->addButton(createNewEnergyPositionsButton_);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(shareEnergyPositionsButton_);
	vl->addWidget(createNewEnergyPositionsButton_);
	vl->addStretch(8);

	masterHL_ = new QHBoxLayout();
	masterHL_->addLayout(vl);
	masterHL_->addStretch(8);

	setLayout(masterHL_);

	setTitle("Element Information is Already Available");

	connect(shareOrCreateButtonGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
}

bool SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage::validatePage(){
	// for now, figure out disassociation later
	return true;
}

bool SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage::isComplete() const{
	if(!shareOrCreateButtonGroup_->checkedButton())
		return false;
	return true;
}

void SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());
	originatingFastScanParameters_ = fastScanWizard_->originatingFastScanParameters();
	newFastScanParameters_ = fastScanWizard_->newFastScanParameters();

	startPositionCopy_ = newFastScanParameters_->scanInfo().start();
	middlePositionCopy_ = newFastScanParameters_->scanInfo().middle();
	endPositionCopy_ = newFastScanParameters_->scanInfo().end();

	SGMEnergyPositionWBeamlineAndDatabaseView *startPositionView = new SGMEnergyPositionWBeamlineAndDatabaseView(&startPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	SGMEnergyPositionWBeamlineAndDatabaseView *middlePositionView = new SGMEnergyPositionWBeamlineAndDatabaseView(&middlePositionCopy_, SGMEnergyPositionView::ViewModeMiddle);
	SGMEnergyPositionWBeamlineAndDatabaseView *endPositionView = new SGMEnergyPositionWBeamlineAndDatabaseView(&endPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);

	startPositionView->setEnabled(false);
	middlePositionView->setEnabled(false);
	endPositionView->setEnabled(false);

	QVBoxLayout *positionsLayout = new QVBoxLayout();
	positionsLayout->addWidget(startPositionView);
	positionsLayout->addWidget(middlePositionView);
	positionsLayout->addWidget(endPositionView);
	positionsLayout->addStretch(8);
	positionsLayout->setContentsMargins(5, 0, 5, 0);

	masterHL_->addLayout(positionsLayout);

	createNewEnergyPositionsButton_->setEnabled(false);

	setSubTitle(QString("There are already energy position settings in the %1 database for %2.\nYou can see these on the right hand side.\nDo you wish to use the same energy position settings for your scan?").arg(originatingFastScanParameters_->database()->connectionName()).arg(newFastScanParameters_->element()));
}

int SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage::nextId() const{
	return SGMFastScanParametersModificationWizard::EditFastScanParametersNamePage;
}

void SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage::onButtonClicked(QAbstractButton *button){
	Q_UNUSED(button)
	emit completeChanged();
}

/////////////////////////////

 SGMFastScanParametersModificationWizardCopyEditScanInfoPage::~SGMFastScanParametersModificationWizardCopyEditScanInfoPage(){}
SGMFastScanParametersModificationWizardCopyEditScanInfoPage::SGMFastScanParametersModificationWizardCopyEditScanInfoPage(QWidget *parent) :
	QWizardPage(parent)
{
	scanInfoNameLineEdit_ = new QLineEdit();
	scanInfoScanNameLineEdit_ = new QLineEdit();
	edgeComboBox_ = new QComboBox();
	edgeComboBox_->addItem("K", QVariant("K"));
	edgeComboBox_->addItem("L", QVariant("L"));
	edgeComboBox_->addItem("M", QVariant("M"));
	edgeEnergySpinBox_ = new QDoubleSpinBox();
	edgeEnergySpinBox_->setMinimum(200);
	edgeEnergySpinBox_->setMaximum(2000);

	QVBoxLayout *labelsVL = new QVBoxLayout();
	labelsVL->addWidget(new QLabel("Name"));
	labelsVL->addWidget(new QLabel("Scan Name"));
	labelsVL->addWidget(new QLabel("Edge"));
	labelsVL->addWidget(new QLabel("Edge Energy"));

	QVBoxLayout *editsVL = new QVBoxLayout();
	editsVL->addWidget(scanInfoNameLineEdit_);
	editsVL->addWidget(scanInfoScanNameLineEdit_);
	editsVL->addWidget(edgeComboBox_);
	editsVL->addWidget(edgeEnergySpinBox_);

	QHBoxLayout *mainHL = new QHBoxLayout();
	mainHL->addLayout(labelsVL);
	mainHL->addLayout(editsVL);

	setLayout(mainHL);

	setTitle("Creating New Element Information, Check the Settings");
}

bool SGMFastScanParametersModificationWizardCopyEditScanInfoPage::validatePage(){
	return false;
}

bool SGMFastScanParametersModificationWizardCopyEditScanInfoPage::isComplete() const{
	return true;
}

void SGMFastScanParametersModificationWizardCopyEditScanInfoPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());
	originatingFastScanParameters_ = fastScanWizard_->originatingFastScanParameters();
	newFastScanParameters_ = fastScanWizard_->newFastScanParameters();

	scanInfoNameLineEdit_->setText(fastScanWizard_->newScanInfo()->name());
	scanInfoScanNameLineEdit_->setText(fastScanWizard_->newScanInfo()->scanName());
	edgeComboBox_->setCurrentIndex(0);
	if(fastScanWizard_->newScanInfo()->edge().contains("L"))
		edgeComboBox_->setCurrentIndex(1);
	else if(fastScanWizard_->newScanInfo()->edge().contains("M"))
		edgeComboBox_->setCurrentIndex(2);
	edgeEnergySpinBox_->setValue(fastScanWizard_->newScanInfo()->energy());

	setSubTitle(QString("You are creating information for a new element in the \"%1\" database.\nWe have tentatively filled in the appropriate information; however, you should double check it.\nNotably, you can name the scan however you wish (ex,, \"David's Carbon Scan\" if you wish to make it more obvious).\nThe edge and energy have been automatically selected from general Periodic Table information, it is unlikely you will have to change it.\n").arg(fastScanWizard_->newDatabase()->connectionName()));
}

int SGMFastScanParametersModificationWizardCopyEditScanInfoPage::nextId() const{
	return SGMFastScanParametersModificationWizard::EditFastScanParametersNamePage;
}

/////////////////////////////

 SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage::~SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage(){}
SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage::SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage(QWidget *parent) :
	QWizardPage(parent)
{
	QLabel *newNameLabel = new QLabel("New Name: ");
	newNameLineEdit_ = new QLineEdit();

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(newNameLabel);
	hl->addWidget(newNameLineEdit_);

	setLayout(hl);

	setTitle("Choose a Name for your Scan");

	connect(newNameLineEdit_, SIGNAL(textEdited(QString)), this, SLOT(onNewNameLineEditTextEdited(QString)));
}

bool SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage::validatePage(){
	return isComplete();
}

bool SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage::isComplete() const{
	if(!newNameLineEdit_ || takenNames_.contains(newNameLineEdit_->text()) || newNameLineEdit_->text().isEmpty())
		return false;
	return true;
}

void SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());
	originatingFastScanParameters_ = fastScanWizard_->originatingFastScanParameters();
	newFastScanParameters_ = fastScanWizard_->newFastScanParameters();

	QVariantList takenNamesAsVariants = fastScanWizard_->newDatabase()->retrieve(AMDbObjectSupport::s()->tableNameForClass<SGMFastScanParameters>(), "name");
	for(int x = 0; x < takenNamesAsVariants.count(); x++)
		takenNames_ << takenNamesAsVariants.at(x).toString();

	QString newNameGuess = newFastScanParameters_->name();
	if(takenNames_.contains(newNameGuess)){
		newNameGuess.append("Copy0");
		bool foundValidCopyIndex = false;
		for(int x = 1; !foundValidCopyIndex; x++){
			newNameGuess.remove(newNameGuess.count()-1, 1);
			newNameGuess.append(QString("%1").arg(x));
			if(!takenNames_.contains(newNameGuess))
				foundValidCopyIndex = true;
		}
	}
	newNameLineEdit_->setText(newNameGuess);
	onNewNameLineEditTextEdited(newNameLineEdit_->text());

	setSubTitle(QString("You must select a name for your new scan. A default name is suggested below, but is likely not good.\nFor example, if wish to make a 15 second fast scan on the Carbon K edge, use CarbonK15s"));
}

int SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage::nextId() const{
	if(fastScanWizard_->copyDestinationSelection() == SGMFastScanParametersModificationWizard::CopySameSame)
		return SGMFastScanParametersModificationWizard::EditFastScanSettingsPage;
	else
		return SGMFastScanParametersModificationWizard::EditEnergyPositionsPage;
}

void SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage::onNewNameLineEditTextEdited(const QString &text){
	QPalette editPalette = newNameLineEdit_->palette();

	if(takenNames_.contains(text)){
		editPalette.setBrush(QPalette::WindowText, QBrush(Qt::red));
		editPalette.setBrush(QPalette::HighlightedText, QBrush(Qt::red));
		editPalette.setBrush(QPalette::Text, QBrush(Qt::red));
	}
	else{
		editPalette.setBrush(QPalette::WindowText, QBrush(Qt::green));
		editPalette.setBrush(QPalette::HighlightedText, QBrush(Qt::green));
		editPalette.setBrush(QPalette::Text, QBrush(Qt::green));
	}
	newNameLineEdit_->setPalette(editPalette);
	emit completeChanged();
}

/////////////////////////////

 SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage::~SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage(){}
SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage::SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage(QWidget *parent) :
	QWizardPage(parent)
{
	masterHL_ = new QHBoxLayout();
	setLayout(masterHL_);

	setTitle("Edit or Confirm the Energy Positions");
}

bool SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage::validatePage(){
	// In the cases of [Same, New] and [New, New] we need to compare to the newScanInfo object
	if( (fastScanWizard_->copyDestinationSelection() == SGMFastScanParametersModificationWizard::CopySameNew) || (fastScanWizard_->copyDestinationSelection() == SGMFastScanParametersModificationWizard::CopyNewNew) ){
		if(startPositionCopy_ != fastScanWizard_->newScanInfo()->start())
			fastScanWizard_->newScanInfo()->setStart(startPositionCopy_);
		if(middlePositionCopy_ != fastScanWizard_->newScanInfo()->middle())
			fastScanWizard_->newScanInfo()->setMiddle(middlePositionCopy_);
		if(endPositionCopy_ != fastScanWizard_->newScanInfo()->end())
			fastScanWizard_->newScanInfo()->setEnd(endPositionCopy_);
	}
	// Otherwise, compare to the newFastScanParamters object
	else{
		SGMScanInfo scanInfoCopy = newFastScanParameters_->scanInfo();
		bool changesMade = false;
		if(startPositionCopy_ != newFastScanParameters_->scanInfo().start()){
			scanInfoCopy.setStart(startPositionCopy_);
			changesMade = true;
		}
		if(middlePositionCopy_ != newFastScanParameters_->scanInfo().middle()){
			scanInfoCopy.setMiddle(middlePositionCopy_);
			changesMade = true;
		}
		if(endPositionCopy_ != newFastScanParameters_->scanInfo().end()){
			scanInfoCopy.setEnd(endPositionCopy_);
			changesMade = true;
		}

		if(changesMade)
			newFastScanParameters_->setScanInfo(scanInfoCopy);
	}
	return true;
}

bool SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage::isComplete() const{
	return true;
}

void SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());
	originatingFastScanParameters_ = fastScanWizard_->originatingFastScanParameters();
	newFastScanParameters_ = fastScanWizard_->newFastScanParameters();

	// In the cases of [Same, New] and [New, New] we need to copy from the newScanInfo object
	if( (fastScanWizard_->copyDestinationSelection() == SGMFastScanParametersModificationWizard::CopySameNew) || (fastScanWizard_->copyDestinationSelection() == SGMFastScanParametersModificationWizard::CopyNewNew) ){
		startPositionCopy_ = fastScanWizard_->newScanInfo()->start();
		middlePositionCopy_ = fastScanWizard_->newScanInfo()->middle();
		endPositionCopy_ = fastScanWizard_->newScanInfo()->end();
	}
	// Otherwise, copy directly from the newFastScanParamters object
	else{
		startPositionCopy_ = newFastScanParameters_->scanInfo().start();
		middlePositionCopy_ = newFastScanParameters_->scanInfo().middle();
		endPositionCopy_ = newFastScanParameters_->scanInfo().end();
	}


	SGMEnergyPositionWBeamlineAndDatabaseView *startPositionView = new SGMEnergyPositionWBeamlineAndDatabaseView(&startPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	SGMEnergyPositionWBeamlineAndDatabaseView *middlePositionView = new SGMEnergyPositionWBeamlineAndDatabaseView(&middlePositionCopy_, SGMEnergyPositionView::ViewModeMiddle);
	SGMEnergyPositionWBeamlineAndDatabaseView *endPositionView = new SGMEnergyPositionWBeamlineAndDatabaseView(&endPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);

	QVBoxLayout *positionsLayout = new QVBoxLayout();
	positionsLayout->addWidget(startPositionView);
	positionsLayout->addWidget(middlePositionView);
	positionsLayout->addWidget(endPositionView);
	positionsLayout->addStretch(8);
	positionsLayout->setContentsMargins(5, 0, 5, 0);

	masterHL_->addLayout(positionsLayout);

	setSubTitle(QString("You have copied a scan and these energy positions are likely incorrect for your new purpose.\nEdit the setting below directly or using the \"Set From Beamline\" buttons."));

}

int SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage::nextId() const{
	return SGMFastScanParametersModificationWizard::EditFastScanSettingsPage;
}

/////////////////////////////

 SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::~SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage(){}
SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage(QWidget *parent) :
	QWizardPage(parent)
{
	fastScanSettingsView_ = 0; //NULL

	setTitle("Edit the Scan's Parameters");
}

bool SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::validatePage(){
	if(hasUnsavedChanges_)
		newFastScanParameters_->setFastScanSettings(fastScanSettingsCopy_);
	return true;
}

bool SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::isComplete() const{
	fastScanWizard_->button(QWizard::NextButton)->setEnabled(hasUnsavedChanges_);
	return !hasUnsavedChanges_;
}

void SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());
	originatingFastScanParameters_ = fastScanWizard_->originatingFastScanParameters();
	newFastScanParameters_ = fastScanWizard_->newFastScanParameters();
	hasUnsavedChanges_ = false;

	fastScanSettingsCopy_ = newFastScanParameters_->fastScanSettings();
	if(!fastScanSettingsView_){
		fastScanSettingsView_ = new SGMFastScanSettingsView(&fastScanSettingsCopy_);

		QVBoxLayout *settingsLayout = new QVBoxLayout();
		settingsLayout->addWidget(fastScanSettingsView_);
		settingsLayout->addStretch(8);
		settingsLayout->setContentsMargins(5, 0, 5, 0);

		setLayout(settingsLayout);
		connect(&fastScanSettingsCopy_, SIGNAL(fastScanSettingsChanged()), this, SLOT(onAnySettingsChanged()));
	}

	setSubTitle(QString("Using the fields below you can edit the scan named %1.").arg(newFastScanParameters_->name()));
	QTimer::singleShot(0, this, SLOT(onAnySettingsChanged()));
}

void SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::onAnySettingsChanged(){
	if(newFastScanParameters_->fastScanSettings() != fastScanSettingsCopy_)
		hasUnsavedChanges_ = true;
	else
		hasUnsavedChanges_ = false;
	emit completeChanged();
}

int SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::nextId() const{
	return SGMFastScanParametersModificationWizard::CopyConfirmationPage;
}

/////////////////////////////

 SGMFastScanParametersModificationWizardCopyConfirmationPage::~SGMFastScanParametersModificationWizardCopyConfirmationPage(){}
SGMFastScanParametersModificationWizardCopyConfirmationPage::SGMFastScanParametersModificationWizardCopyConfirmationPage(QWidget *parent) :
	QWizardPage(parent)
{
	setTitle("Confirm Copied Specifications");
}

bool SGMFastScanParametersModificationWizardCopyConfirmationPage::validatePage(){
	return true;
}

bool SGMFastScanParametersModificationWizardCopyConfirmationPage::isComplete() const{
	return true;
}

void SGMFastScanParametersModificationWizardCopyConfirmationPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());
	originatingFastScanParameters_ = fastScanWizard_->originatingFastScanParameters();
	newFastScanParameters_ = fastScanWizard_->newFastScanParameters();

	qDebug() << "Do I know I'm the final page? " << isFinalPage();
}

int SGMFastScanParametersModificationWizardCopyConfirmationPage::nextId() const{
	return -1;
}

/////////////////////////////
//// ACTUAL WIZARD HERE ////
///////////////////////////

SGMFastScanParametersModificationWizard::SGMFastScanParametersModificationWizard(QWidget *parent) :
	QWizard(parent)
{
	indexOfOriginatingFastScan_ = -1;

	originatingFastScanParameters_ = 0; //NULL
	newFastScanParameters_ = 0; //NULL
	newDatabase_ = 0; //NULL
	newElement_ = 0; //NULL
	newElementInfo_ = 0; //NULL
	newScanInfo_ = 0; //NULL

	copyDestinationSelection_ = SGMFastScanParametersModificationWizard::CopyInvalidDestination;

	setPage(SGMFastScanParametersModificationWizard::PeriodicTablePage, new SGMFastScanParametersModificationWizardPeriodicTablePage);
	setPage(SGMFastScanParametersModificationWizard::EditOrCopyPage, new SGMFastScanParametersModificationWizardEditOrCopyPage);
	setPage(SGMFastScanParametersModificationWizard::EditPage, new SGMFastScanParametersModificationWizardEditPage);
	setPage(SGMFastScanParametersModificationWizard::CopyDestinationSelectionPage, new SGMFastScanParametersModificationWizardCopyDestinationSelectionPage);

	setPage(SGMFastScanParametersModificationWizard::ShareEnergyPositionsPage, new SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage);
	setPage(SGMFastScanParametersModificationWizard::EditScanInfoPage, new SGMFastScanParametersModificationWizardCopyEditScanInfoPage);
	setPage(SGMFastScanParametersModificationWizard::EditFastScanParametersNamePage, new SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage);
	setPage(SGMFastScanParametersModificationWizard::EditEnergyPositionsPage, new SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage);
	setPage(SGMFastScanParametersModificationWizard::EditFastScanSettingsPage, new SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage);
	setPage(SGMFastScanParametersModificationWizard::CopyConfirmationPage, new SGMFastScanParametersModificationWizardCopyConfirmationPage);
}

SGMFastScanParameters* SGMFastScanParametersModificationWizard::originatingFastScanParameters(){
	return originatingFastScanParameters_;
}

SGMFastScanParameters* SGMFastScanParametersModificationWizard::newFastScanParameters(){
	return newFastScanParameters_;
}

AMDatabase* SGMFastScanParametersModificationWizard::newDatabase(){
	return newDatabase_;
}

AMElement* SGMFastScanParametersModificationWizard::newElement(){
	return newElement_;
}

SGMElementInfo* SGMFastScanParametersModificationWizard::newElementInfo(){
	return newElementInfo_;
}

SGMScanInfo* SGMFastScanParametersModificationWizard::newScanInfo(){
	return newScanInfo_;
}

SGMFastScanParametersModificationWizard::CopyDestination SGMFastScanParametersModificationWizard::copyDestinationSelection(){
	return copyDestinationSelection_;
}

void SGMFastScanParametersModificationWizard::setOriginatingFastScanIndex(int indexOfFastScan){
	indexOfOriginatingFastScan_ = indexOfFastScan;
	originatingFastScanParameters_ = SGMPeriodicTable::sgmTable()->fastScanPresets(SGMPeriodicTable::SGMPeriodicTableAllDatabasesConnectionName()).at(indexOfOriginatingFastScan_);
}

void SGMFastScanParametersModificationWizard::copyOriginalFastScanParametersToNew(AMDatabase *database, AMElement *element){
	newDatabase_ = database;
	newElement_ = element;

	newFastScanParameters_ = new SGMFastScanParameters();
	newFastScanParameters_->operator =(*originatingFastScanParameters_);
	newFastScanParameters_->dissociateFromDb(false);

	SGMFastScanSettings dissociatedFastScanSettings = newFastScanParameters_->fastScanSettings();
	dissociatedFastScanSettings.dissociateFromDb();
	newFastScanParameters_->setFastScanSettings(dissociatedFastScanSettings);

	qDebug() << "Looking into fastScanParameters. " << originatingFastScanParameters_->id() << originatingFastScanParameters_->fastScanSettings().id() << newFastScanParameters_->id() << newFastScanParameters_->fastScanSettings().id();
	qDebug() << originatingFastScanParameters_->scanInfo().id() << newFastScanParameters_->scanInfo().id();
}

void SGMFastScanParametersModificationWizard::createNewElementInfo(){
	if(!newDatabase_ || !newElement_)
		return;

	QString elementInfoName = newElement_->name()%"ElementInfo";
	if(newDatabase_->connectionName() != "SGMBeamline")
		elementInfoName.prepend(newDatabase_->connectionName());
	newElementInfo_ = new SGMElementInfo(elementInfoName, newElement_);

	qDebug() << "Just created new elementInfo " << newElementInfo_->id();
}

void SGMFastScanParametersModificationWizard::createNewScanInfo(){
	AMAbsorptionEdge edge;
	QString elementEdge = "K";
	if(newElement_->KEdge().energy() > 200 && newElement_->KEdge().energy() < 2000){
		elementEdge = "K";
		edge = AMPeriodicTable::table()->elementByName(newElement_->name())->KEdge();
	}
	else if(newElement_->L2Edge().energy() > 200 && newElement_->L2Edge().energy() < 2000){
		elementEdge = "L";
		edge = AMPeriodicTable::table()->elementByName(newElement_->name())->L2Edge();
	}
	else if(newElement_->M3Edge().energy() > 200 && newElement_->M3Edge().energy() < 2000){
		elementEdge = "M";
		edge = AMPeriodicTable::table()->elementByName(newElement_->name())->M3Edge();
	}

	QString appendDatabaseName = "";
	if(newDatabase_->connectionName() != "SGMBeamline"){
		appendDatabaseName = newDatabase_->connectionName();
		appendDatabaseName[0] = appendDatabaseName[0].toUpper();
		appendDatabaseName.append(" Database");
	}

	SGMEnergyPosition startPosition = newFastScanParameters_->scanInfo().start();
	SGMEnergyPosition middlePosition = newFastScanParameters_->scanInfo().middle();
	SGMEnergyPosition endPosition = newFastScanParameters_->scanInfo().end();
	newScanInfo_ = new SGMScanInfo(newElement_->name()%" "%elementEdge%" "%appendDatabaseName, edge, startPosition, middlePosition, endPosition);

	qDebug() << "Just created new scanInfo";
}


void SGMFastScanParametersModificationWizard::setCopyDestinationSelection(SGMFastScanParametersModificationWizard::CopyDestination copyDestinationSelection){
	copyDestinationSelection_ = copyDestinationSelection;
}
