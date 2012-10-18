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


#include "SGMPeriodicTableView.h"

#include <QBoxLayout>
#include <QMenu>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QStringBuilder>
#include <QMessageBox>

#include "ui/dataman/AMDbObjectGeneralView.h"
#include "ui/dataman/AMDbObjectGeneralViewSupport.h"
#include "ui/util/SGM/SGMFastScanParametersView.h"
#include "ui/util/SGM/SGMEnergyPositionView.h"
#include "ui/util/SGM/SGMFastScanSettingsView.h"
#include "util/AMDateTimeUtils.h"

SGMPeriodicTableView::SGMPeriodicTableView(SGMPeriodicTable *sgmPeriodicTable, QWidget *parent) :
	QWidget(parent)
{
	sgmPeriodicTable_ = sgmPeriodicTable;
	periodicTableView_ = new AMPeriodicTableView(this);

	availableScansMenu_ = 0;

	const AMElement *tempElement;
	for(int x = 0; x < AMPeriodicTable::table()->elements().count(); x++){
		tempElement = AMPeriodicTable::table()->elements().at(x);
		periodicTableView_->button(tempElement)->setEnabled(false);
		if( (tempElement->KEdge().second.toDouble() > 200 && tempElement->KEdge().second.toDouble() < 2000) || (tempElement->L2Edge().second.toDouble() > 200 && tempElement->L2Edge().second.toDouble() < 2000) || (tempElement->M3Edge().second.toDouble() > 200 && tempElement->M3Edge().second.toDouble() < 2000) )
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
	QList<SGMFastScanParameters*> allFastScans = SGMPeriodicTable::sgmTable()->fastScanPresets();

	if(availableScansMenu_)
		delete availableScansMenu_;
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
//	SGMFastScanParameters *fastScanToView = SGMPeriodicTable::sgmTable()->fastScanPresets().at(activeMenuData_);

//	SGMFastScanParametersView *fastScanParametersView = new SGMFastScanParametersView(fastScanToView);
//	fastScanParametersView->show();
}


SGMFastScanParametersModificationWizardPeriodicTablePage::SGMFastScanParametersModificationWizardPeriodicTablePage(QWidget *parent) :
	QWizardPage(parent)
{
	indexOfFastScan_ = -1;
	sgmPeriodicTableView_ = new SGMPeriodicTableView(SGMPeriodicTable::sgmTable());

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(sgmPeriodicTableView_);
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

bool SGMFastScanParametersModificationWizardPeriodicTablePage::isComplete(){
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
	isComplete();
}

void SGMFastScanParametersModificationWizardPeriodicTablePage::checkIsComplete(){
	isComplete();
}

SGMFastScanParametersModificationWizardEditOrCopyPage::SGMFastScanParametersModificationWizardEditOrCopyPage(QWidget *parent) :
	QWizardPage(parent)
{
	editInformationLabel_ = new QLabel();
	chooseToEditPushButton_ = new QPushButton("Edit this Scan");

	copyInformationLabel_ = new QLabel();
	chooseToCopyPushButton_ = new QPushButton("Copy this Scan");

	choiceLabel_ = new QLabel("No Option Chosen");

	QHBoxLayout *tempHL;
	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(editInformationLabel_);
	tempHL = new QHBoxLayout();
	tempHL->addWidget(chooseToEditPushButton_);
	tempHL->addStretch(8);
	vl->addLayout(tempHL);
	vl->addStretch(8);
	vl->addWidget(copyInformationLabel_);
	tempHL = new QHBoxLayout();
	tempHL->addWidget(chooseToCopyPushButton_);
	tempHL->addStretch(8);
	vl->addLayout(tempHL);
	vl->addStretch(8);
	vl->addWidget(choiceLabel_);
	setLayout(vl);

	connect(chooseToEditPushButton_, SIGNAL(clicked()), this, SLOT(onEditButtonClicked()));
	connect(chooseToCopyPushButton_, SIGNAL(clicked()), this, SLOT(onCopyButtonClicked()));

	setTitle("Available Editing and Copying Options");
}

bool SGMFastScanParametersModificationWizardEditOrCopyPage::validatePage(){
	if(choiceLabel_->text() == "No Option Chosen")
		return false;
	return true;
}

bool SGMFastScanParametersModificationWizardEditOrCopyPage::isComplete(){
	bool retVal = true;
	if(choiceLabel_->text() == "No Option Chosen")
		retVal = false;
	fastScanWizard_->button(QWizard::NextButton)->setEnabled(retVal);
	return retVal;
}

void SGMFastScanParametersModificationWizardEditOrCopyPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());

	if(fastScanWizard_->originatingFastScanParameters()->database()->isReadOnly()){
		editInformationLabel_->setText(QString("The fast scan named %1 is located in the %2 database, which you do not have write access to. You cannot edit this scan.").arg(fastScanWizard_->originatingFastScanParameters()->name()).arg(fastScanWizard_->originatingFastScanParameters()->database()->connectionName()));
		chooseToEditPushButton_->setEnabled(false);
	}
	else
		editInformationLabel_->setText(QString("Click this button to edit the scan named %1").arg(fastScanWizard_->originatingFastScanParameters()->name()));

	copyInformationLabel_->setText("Click this button to make a copy of the scan you can edit and save for later");
	choiceLabel_->setText("No Option Chosen");

	setSubTitle(QString("Choose to edit or copy the fast scan named %1, depending on database access you have.").arg(fastScanWizard_->originatingFastScanParameters()->name()));
	QTimer::singleShot(0, this, SLOT(checkIsComplete()));
}

int SGMFastScanParametersModificationWizardEditOrCopyPage::nextId(){
	if(choiceLabel_->text() == "Press Next to Edit")
		return SGMFastScanParametersModificationWizard::EditPage;
	else if(choiceLabel_->text() == "Press Next to Copy")
		return SGMFastScanParametersModificationWizard::CopyPage;
	return 0;
}

void SGMFastScanParametersModificationWizardEditOrCopyPage::onEditButtonClicked(){
	choiceLabel_->setText("Press Next to Edit");
	isComplete();
}

void SGMFastScanParametersModificationWizardEditOrCopyPage::onCopyButtonClicked(){
	choiceLabel_->setText("Press Next to Copy");
	isComplete();
}

void SGMFastScanParametersModificationWizardEditOrCopyPage::checkIsComplete(){
	isComplete();
}

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

bool SGMFastScanParametersModificationWizardEditPage::isComplete(){
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

/////////////////////////////
//// ACTUAL WIZARD HERE ////
///////////////////////////

SGMFastScanParametersModificationWizard::SGMFastScanParametersModificationWizard(QWidget *parent) :
	QWizard(parent)
{
	indexOfOriginatingFastScan_ = -1;

	setPage(SGMFastScanParametersModificationWizard::PeriodicTablePage, new SGMFastScanParametersModificationWizardPeriodicTablePage);
	setPage(SGMFastScanParametersModificationWizard::EditOrCopyPage, new SGMFastScanParametersModificationWizardEditOrCopyPage);
	setPage(SGMFastScanParametersModificationWizard::EditPage, new SGMFastScanParametersModificationWizardEditPage);
}

SGMFastScanParameters* SGMFastScanParametersModificationWizard::originatingFastScanParameters(){
	return originatingFastScanParameters_;
}

void SGMFastScanParametersModificationWizard::setOriginatingFastScanIndex(int indexOfFastScan){
	indexOfOriginatingFastScan_ = indexOfFastScan;
	originatingFastScanParameters_ = SGMPeriodicTable::sgmTable()->fastScanPresets().at(indexOfOriginatingFastScan_);
}
