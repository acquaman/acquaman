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

#include "ui/dataman/AMDbObjectGeneralView.h"
#include "ui/dataman/AMDbObjectGeneralViewSupport.h"
#include "ui/util/SGM/SGMFastScanParametersView.h"
#include "ui/util/SGM/SGMEnergyPositionView.h"
#include "ui/util/SGM/SGMFastScanSettingsView.h"
#include "util/AMDateTimeUtils.h"
#include "dataman/database/AMDbObjectSupport.h"

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

int SGMFastScanParametersModificationWizardEditOrCopyPage::nextId() const{
	if(choiceLabel_->text() == "Press Next to Edit")
		return SGMFastScanParametersModificationWizard::EditPage;
	else if(choiceLabel_->text() == "Press Next to Copy")
		return SGMFastScanParametersModificationWizard::CopyDestinationSelectionPage;
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

	fastScanWizard_->copyOriginalFastScanParametersToNew(AMDatabase::database(databasesComboBox_->currentText()), AMPeriodicTable::table()->elementByName(elementsComboBox_->currentText()));

	// Except for [Same Db, Same Element] always dissociate scanInfo
	if( !((originatingFastScanParameters_->database()->connectionName() == databasesComboBox_->currentText()) && (originatingFastScanParameters_->element() == elementsComboBox_->currentText())) )
		fastScanWizard_->newFastScanParameters()->scanInfo().dissociateFromDb();

	// In the case of [New Db, Same Element] we will silently copy SGMElementInfo and SGMScanInfo into the new database
	if( (originatingFastScanParameters_->database()->connectionName() != databasesComboBox_->currentText()) && (originatingFastScanParameters_->element() == elementsComboBox_->currentText())){

	}

	return true;
}

bool SGMFastScanParametersModificationWizardCopyDestinationSelectionPage::isComplete(){

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

	const AMElement *tempElement;
	for(int x = 0; x < AMPeriodicTable::table()->elements().count(); x++){
		tempElement = AMPeriodicTable::table()->elements().at(x);
		if( (tempElement->KEdge().second.toDouble() > 200 && tempElement->KEdge().second.toDouble() < 2000) || (tempElement->L2Edge().second.toDouble() > 200 && tempElement->L2Edge().second.toDouble() < 2000) || (tempElement->M3Edge().second.toDouble() > 200 && tempElement->M3Edge().second.toDouble() < 2000) )
			elementsComboBox_->addItem(tempElement->name(), QVariant(tempElement->atomicNumber()));
	}
	elementsComboBox_->setCurrentIndex(elementsComboBox_->findData(AMPeriodicTable::table()->elementByName(originatingFastScanParameters_->element())->atomicNumber()));
	checkValidElementsHelper();

	setSubTitle(QString("The scan %1 comes from a database named %2. Use the boxes on the right to select a database to save your copy to as well as the element you intend to scan.\nPlease note that some databases may not be available to write to.\nAs well, some elements in some databases already have information saved. If the element you wish to select is grayed out, go back and select that element from the periodic table.").arg(originatingFastScanParameters_->name()).arg(originatingFastScanParameters_->database()->connectionName()));
}

int SGMFastScanParametersModificationWizardCopyDestinationSelectionPage::nextId() const{
	if( (originatingFastScanParameters_->database()->connectionName() == databasesComboBox_->currentText()) && (originatingFastScanParameters_->element() == elementsComboBox_->currentText()) )
		return SGMFastScanParametersModificationWizard::ShareEnergyPositionsPage;
	else if(originatingFastScanParameters_->element() == elementsComboBox_->currentText())
		return SGMFastScanParametersModificationWizard::EditFastScanParametersNamePage;
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
	emit completeChanged();
}

/////////////////////////////

SGMFastScanParametersModificationWizardCopyEditScanInfoPage::SGMFastScanParametersModificationWizardCopyEditScanInfoPage(QWidget *parent) :
	QWizardPage(parent)
{

}

bool SGMFastScanParametersModificationWizardCopyEditScanInfoPage::validatePage(){

}

bool SGMFastScanParametersModificationWizardCopyEditScanInfoPage::isComplete(){

}

void SGMFastScanParametersModificationWizardCopyEditScanInfoPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());
	originatingFastScanParameters_ = fastScanWizard_->originatingFastScanParameters();
	newFastScanParameters_ = fastScanWizard_->newFastScanParameters();
}

int SGMFastScanParametersModificationWizardCopyEditScanInfoPage::nextId() const{

}

/////////////////////////////

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

SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage::SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage(QWidget *parent) :
	QWizardPage(parent)
{

}

bool SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage::validatePage(){

}

bool SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage::isComplete(){

}

void SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());
	originatingFastScanParameters_ = fastScanWizard_->originatingFastScanParameters();
	newFastScanParameters_ = fastScanWizard_->newFastScanParameters();
}

int SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage::nextId() const{

}

/////////////////////////////

SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage(QWidget *parent) :
	QWizardPage(parent)
{

}

bool SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::validatePage(){

}

bool SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::isComplete(){

}

void SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::initializePage(){
	fastScanWizard_ = qobject_cast<SGMFastScanParametersModificationWizard*>(wizard());
	originatingFastScanParameters_ = fastScanWizard_->originatingFastScanParameters();
	newFastScanParameters_ = fastScanWizard_->newFastScanParameters();
}

int SGMFastScanParametersModificationWizardCopyEditFastScanSettingsPage::nextId() const{

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

	setPage(SGMFastScanParametersModificationWizard::PeriodicTablePage, new SGMFastScanParametersModificationWizardPeriodicTablePage);
	setPage(SGMFastScanParametersModificationWizard::EditOrCopyPage, new SGMFastScanParametersModificationWizardEditOrCopyPage);
	setPage(SGMFastScanParametersModificationWizard::EditPage, new SGMFastScanParametersModificationWizardEditPage);
	setPage(SGMFastScanParametersModificationWizard::CopyDestinationSelectionPage, new SGMFastScanParametersModificationWizardCopyDestinationSelectionPage);

	setPage(SGMFastScanParametersModificationWizard::ShareEnergyPositionsPage, new SGMFastScanParametersModificationWizardCopyShareEnergyPositionsPage);
	setPage(SGMFastScanParametersModificationWizard::EditScanInfoPage, new SGMFastScanParametersModificationWizardCopyEditScanInfoPage);
	setPage(SGMFastScanParametersModificationWizard::EditFastScanParametersNamePage, new SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage);
	setPage(SGMFastScanParametersModificationWizard::EditEnergyPositionsPage, new SGMFastScanParametersModificationWizardCopyEditEnergyPositionsPage);
	setPage(SGMFastScanParametersModificationWizard::EditFastScanSettingsPage, new SGMFastScanParametersModificationWizardCopyEditFastScanParametersNamePage);
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

const AMElement* SGMFastScanParametersModificationWizard::newElement(){
	return newElement_;
}

void SGMFastScanParametersModificationWizard::setOriginatingFastScanIndex(int indexOfFastScan){
	indexOfOriginatingFastScan_ = indexOfFastScan;
	originatingFastScanParameters_ = SGMPeriodicTable::sgmTable()->fastScanPresets().at(indexOfOriginatingFastScan_);
}

void SGMFastScanParametersModificationWizard::copyOriginalFastScanParametersToNew(AMDatabase *database, const AMElement *element){
	newDatabase_ = database;
	newElement_ = element;

	newFastScanParameters_ = new SGMFastScanParameters();
	newFastScanParameters_->operator =(*originatingFastScanParameters_);
	newFastScanParameters_->dissociateFromDb(false);
	newFastScanParameters_->fastScanSettings().dissociateFromDb();
}
