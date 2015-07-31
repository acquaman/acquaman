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


#include "AMExportWizard.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"
#include "dataman/database/AMDbObjectSupport.h"

#include <QHashIterator>
#include <QFormLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QProgressBar>
#include "ui/AMFolderPathLineEdit.h"
#include <QFileDialog>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QStringBuilder>

#include "acquaman.h"

AMExportWizard::AMExportWizard(AMExportController* controller, QWidget *parent) :
	QWizard(parent)
{
	controller_ = controller;
	optionsDatabases_ << "user";
	connect(controller_, SIGNAL(destroyed()), this, SLOT(onControllerDeleted()));

	addPage(new AMExportWizardChooseExporterPage);
	optionsPage_ = new AMExportWizardOptionPage();
	addPage(optionsPage_);
	addPage(new AMExportWizardProgressPage);

	setWindowTitle(QString("Exporting %1 Scans...").arg(controller_->scanCount()));

	setOption(QWizard::NoBackButtonOnLastPage, true);
}

void AMExportWizard::setOptionsDatabases(const QStringList &optionsDatabases){
	 optionsDatabases_ = optionsDatabases;
	 if(optionsPage_)
		 optionsPage_->setOptionsDatabases(optionsDatabases_);
}

void AMExportWizard::done(int result) {

	// check for cancel while export in progress...
	if(result == QDialog::Rejected && controller_ && (controller_->state() == AMExportController::Exporting || controller_->state() == AMExportController::Paused)) {
		controller_->pause();

		if(QMessageBox::question(this,
								 "Export in Progress...",
								 "Are you sure you want to cancel?\n\nIf you say yes here, the remaining scans will not be exported.",
								 QMessageBox::Yes | QMessageBox::No,
								 QMessageBox::Yes)
				== QMessageBox::No) {
			controller_->resume();
			return;	// don't actually be "done".
		}

	}

	if(controller_ && controller_->state() == AMExportController::Exporting)
		controller_->cancel();

	QWizard::done(result);
}

 AMExportWizardChooseExporterPage::~AMExportWizardChooseExporterPage(){}
AMExportWizardChooseExporterPage::AMExportWizardChooseExporterPage(QWidget *parent)
	: QWizardPage(parent)
{
	exporterComboBox_ = new QComboBox();
	destinationFolder_ = new AMFolderPathLineEdit();
	browseButton_ = new QPushButton("Browse...");

	populateExporterComboBox();
	connect(exporterComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onExporterComboBoxIndexChanged(int)));
	connect(browseButton_, SIGNAL(clicked()), this, SLOT(onBrowseButtonClicked()));

	QFormLayout* fl = new QFormLayout();
	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(destinationFolder_);
	hl->addWidget(browseButton_);

	fl->addRow("Export location", hl);
	fl->addRow("Export as", exporterComboBox_);

	groupBox_ = new QGroupBox();
	descriptionLabel_ = new QLabel();
	descriptionLabel_->setWordWrap(true);
	QVBoxLayout* il = new QVBoxLayout();
	il->addWidget(descriptionLabel_);
	groupBox_->setLayout(il);


	QVBoxLayout* vl = new QVBoxLayout();
	vl->addStretch();
	vl->addLayout(fl);
	vl->addStretch();
	vl->addWidget(groupBox_);

	setLayout(vl);

	setTitle("Choose an Export Location and File Format");
	setSubTitle("Acquaman can help you export your data in a variety of file formats.  On the next page, you'll be able to specify more options on how the data is formatted.");

	if(exporterComboBox_->count())
		onExporterComboBoxIndexChanged(0);

}



void AMExportWizardChooseExporterPage::onExporterComboBoxIndexChanged(int index)
{
	groupBox_->setTitle(exporterComboBox_->itemData(index, Qt::DisplayRole).toString());
	descriptionLabel_->setText(exporterComboBox_->itemData(index, AM::DescriptionRole).toString());
}

bool AMExportWizardChooseExporterPage::validatePage()
{
	AMExportController* c = qobject_cast<AMExportWizard*>(wizard())->controller();

	c->setDestinationFolderPath(destinationFolder_->text());
	QString exporterClassName = exporterComboBox_->itemData(exporterComboBox_->currentIndex(), AM::UserRole).toString();
	return c->chooseExporter(exporterClassName);
}



void AMExportWizardChooseExporterPage::populateExporterComboBox()
{
	QHashIterator<QString, AMExporterInfo> iRegisteredExporters(AMExportController::registeredExporters());

	while(iRegisteredExporters.hasNext()) {
		iRegisteredExporters.next();
		const AMExporterInfo& exporter = iRegisteredExporters.value();
		exporterComboBox_->addItem(exporter.description); /// \todo Icon
		exporterComboBox_->setItemData(exporterComboBox_->count()-1, exporter.longDescription, AM::DescriptionRole);
		exporterComboBox_->setItemData(exporterComboBox_->count()-1, iRegisteredExporters.key(), AM::UserRole);
	}
}

void AMExportWizardChooseExporterPage::initializePage()
{
	if(destinationFolder_->text().isEmpty()) {
		AMExportController* c = qobject_cast<AMExportWizard*>(wizard())->controller();
		destinationFolder_->setText(c->destinationFolderPath());
	}
}

void AMExportWizardChooseExporterPage::onBrowseButtonClicked()
{	
	int pos(0);
	QString inputText(QFileDialog::getExistingDirectory(this, "Export Location", destinationFolder_->text()));
	destinationFolder_->setText(inputText);
	destinationFolder_->validator()->validate(inputText, pos);
}


 AMExportWizardOptionPage::~AMExportWizardOptionPage(){}
AMExportWizardOptionPage::AMExportWizardOptionPage(QWidget *parent)
{
	Q_UNUSED(parent)

	option_ = 0;
	optionView_ = 0;

	optionSelector_ = new QComboBox();
	optionViewContainer_ = new QWidget();
	optionViewContainer_->setLayout(new QVBoxLayout());
	optionViewContainer_->layout()->setContentsMargins(0,0,0,0);
	saveOptionButton_ = new QPushButton("Save Settings");
	saveAsOptionButton_ = new QPushButton("Save As...");

	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(new QLabel("Saved Templates:"));
	hl->addWidget(optionSelector_);
	hl->addStretch();
	hl->addWidget(saveOptionButton_);
	hl->addWidget(saveAsOptionButton_);

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addLayout(hl);
	vl->addWidget(optionViewContainer_);

	setLayout(vl);

	connect(optionSelector_, SIGNAL(currentIndexChanged(int)), this, SLOT(onOptionSelectorIndexChanged(int)));
	connect(saveOptionButton_, SIGNAL(clicked()), this, SLOT(onSaveOptionButtonClicked()));
	connect(saveAsOptionButton_, SIGNAL(clicked()), this, SLOT(onSaveAsOptionButtonClicked()));

	setTitle("Choose Export Options");
	setSubTitle(("The options available here depend on the file format you've selected.  You can save a set of options as a template for next time using the 'Save'/'Save As...' button."));
}

bool AMExportWizardOptionPage::onSaveOptionButtonClicked() {
	// is this option previously saved?
	if(option_->database() != 0 && option_->id() > 0) {
		if(option_->database() == AMDatabase::database("user")){
			option_->storeToDb(option_->database());
			return true;
		}
		else{
			QMessageBox::information(this, "Cannot Overwrite", "This template belongs to someone else, if you wish to alter it you can choose \"Save As...\" to make your own copy.", QMessageBox::Ok);
			return false;
		}
	}

	else {

		// not stored yet... Need to ask for a name
		bool ok = true;
		QString name;
		while(ok && name.isEmpty()) {	// on second round, means they accepted the dialog, but gave a blank name.
			name = QInputDialog::getText(this, "Save Settings As...",
										 "Please provide a name for this template:",
										 QLineEdit::Normal,
										 exporter_->description() % " " % QDateTime::currentDateTime().toString("MMM d yyyy"),
										 &ok);

			if(ok && name.isEmpty())
				QMessageBox::information(this, "Missing name", "You must provide a name if you want to save this template.", QMessageBox::Ok);
			else if(ok) {
				option_->setName(name);
				if(option_->storeToDb(AMDatabase::database("user"))) {
					optionSelector_->blockSignals(true);
					optionSelector_->addItem(option_->name(), QString("%1||%2").arg(option_->database()->connectionName()).arg(option_->id()));
					optionSelector_->setCurrentIndex(optionSelector_->count()-1);
					optionSelector_->blockSignals(false);
					saveOptionButton_->setText("Save");
					saveOptionButton_->setEnabled(option_->modified());
					saveAsOptionButton_->setEnabled(option_->modified());
					return true;
				}
				else
					return false;	// storeToDb save failed.
			}
		}
		// prompt was cancelled, not saved.
		return false;
	}
}

bool AMExportWizardOptionPage::onSaveAsOptionButtonClicked(){
	AMExporterOption *copiedOption = option_->createCopy();
	controller_->setOption(copiedOption);
	option_ = copiedOption;
	// not stored yet... Need to ask for a name
	bool ok = true;
	QString name;
	while(ok && name.isEmpty()) {	// on second round, means they accepted the dialog, but gave a blank name.
		name = QInputDialog::getText(this, "Save Settings As...",
									 "Please provide a name for this template:",
									 QLineEdit::Normal,
									 exporter_->description() % " " % QDateTime::currentDateTime().toString("MMM d yyyy"),
									 &ok);

		if(ok && name.isEmpty())
			QMessageBox::information(this, "Missing name", "You must provide a name if you want to save this template.", QMessageBox::Ok);
		else if(ok) {
			option_->setName(name);
			if(option_->storeToDb(AMDatabase::database("user"))) {
				optionSelector_->blockSignals(true);
				optionSelector_->addItem(option_->name(), QString("%1||%2").arg(option_->database()->connectionName()).arg(option_->id()));
				optionSelector_->setCurrentIndex(optionSelector_->count()-1);
				optionSelector_->blockSignals(false);
				saveOptionButton_->setText("Save");
				saveOptionButton_->setEnabled(option_->modified());
				saveAsOptionButton_->setEnabled(option_->modified());
				return true;
			}
			else
				return false;	// storeToDb save failed.
		}
	}
	// prompt was cancelled, not saved.
	return false;
}

bool AMExportWizardOptionPage::validatePage()
{
	// new option, unsaved, and modified. Do they want to save it?
	int response = QMessageBox::Discard;

	if(option_->modified() && option_->id() < 1) {
		response = QMessageBox::question(this, "Save changes to template?",
										 "You've made changes to this template.\n\nDo you want to store this template to use again next time?",
										 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
										 QMessageBox::Save);
	}
	else if(option_->modified()) {
		response = QMessageBox::question(this,
										 "Save changes to template?",
										 "You've made changes to the template '" % option_->name() % "'.\n\nDo you want to save these changes for next time?",
										 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
										 QMessageBox::Save);
	}

	// existing option, modified but unsaved: same as answering Discard above. Proceed no matter what.

	switch(response) {
	case QMessageBox::Save:
		return onSaveOptionButtonClicked();
	case QMessageBox::Cancel:
		return false;	// don't go on to next page.
	case QMessageBox::Discard:
	default:
		return true;		// proceed on to next page.
	}
}

void AMExportWizardOptionPage::setOptionsDatabases(const QStringList &optionsDatabases){
	optionsDatabases_ = optionsDatabases;
}



void AMExportWizardOptionPage::initializePage()
{
	controller_ = qobject_cast<AMExportWizard*>(wizard())->controller();
	exporter_ = controller_->exporter();

	controller_->searchForAvailableDataSources();

	populateOptionSelector();
}

void AMExportWizardOptionPage::onOptionSelectorIndexChanged(int index)
{
	optionView_->deleteLater();	// might be 0 if no option yet... or if last option couldn't create an editor. but that's okay.

	// add new option...
	if(index < 1) {
		option_ = exporter_->createDefaultOption();
		controller_->setOption(option_);	// will delete the previous option_
		saveOptionButton_->setEnabled(false);
		saveAsOptionButton_->setEnabled(true);
	}

	// or load saved option
	else {
		QString optionInfo = optionSelector_->itemData(optionSelector_->currentIndex()).toString();

		option_ = qobject_cast<AMExporterOption*>(
					AMDbObjectSupport::s()->createAndLoadObjectAt(
						AMDatabase::database(optionInfo.section("||", 0, 0)),
						AMDbObjectSupport::s()->tableNameForClass(exporter_->exporterOptionClassName()),
						optionInfo.section("||", 1, 1).toInt()));
		if (!option_) {
			QMessageBox::warning(this, "Exporter error", QString("No matching export found for '%1'").arg(optionInfo), QMessageBox::Ok);
			return;
		}

		controller_->setOption(option_); // will delete the previous option_
		saveOptionButton_->setText("Save");
	}

	optionView_ = option_->createEditorWidget();
	if(optionView_) {
		optionViewContainer_->layout()->addWidget(optionView_);
	}


	saveOptionButton_->setEnabled(option_->modified());
	saveAsOptionButton_->setEnabled(option_->modified());
	connect(option_, SIGNAL(modifiedChanged(bool)), saveOptionButton_, SLOT(setEnabled(bool)));
	connect(option_, SIGNAL(modifiedChanged(bool)), saveAsOptionButton_, SLOT(setEnabled(bool)));
}

void AMExportWizardOptionPage::populateOptionSelector()
{
	optionSelector_->blockSignals(true);
	optionSelector_->clear();

	// add "New Option" item
	optionSelector_->addItem("New Template", -1);

	// fill option combo box (loop over the requested databases)
	QSqlQuery q;
	for(int x = 0; x < optionsDatabases_.count(); x++){
		q = AMDbObjectSupport::s()->select(AMDatabase::database(optionsDatabases_.at(x)), exporter_->exporterOptionClassName(), "id, name");
		q.exec();

		while(q.next()) {
			int dbId = q.value(0).toInt();
			optionSelector_->addItem(q.value(1).toString(),
						 QString("%1||%2").arg(optionsDatabases_.at(x)).arg(dbId));	// note: putting the database id in Qt::UserRole.
		}
		q.finish();
	}

	if(controller_->option())
	{
		QString controllerOptionName = controller_->option()->name();
		bool foundControllerOptionName = false;

		for (int iComboBoxOption = 0, size = optionSelector_->count(); iComboBoxOption < size; iComboBoxOption++)
		{
			if(optionSelector_->itemText(iComboBoxOption) == controllerOptionName){

				optionSelector_->setCurrentIndex(iComboBoxOption);
				foundControllerOptionName = true;
			}
		}

		if (!foundControllerOptionName)
			optionSelector_->setCurrentIndex(optionSelector_->count()-1);
	}

	else
		optionSelector_->setCurrentIndex(optionSelector_->count()-1);

	optionSelector_->blockSignals(false);

	onOptionSelectorIndexChanged(optionSelector_->count()-1);
}

/////////////////////////////////
// AMExportWizardProgressPage
/////////////////////////////////

 AMExportWizardProgressPage::~AMExportWizardProgressPage(){}
AMExportWizardProgressPage::AMExportWizardProgressPage(QWidget *parent)
{
	Q_UNUSED(parent)

	controller_ = 0;

	progressBar_ = new QProgressBar();
	progressBar_->setMinimum(0);
	progressBar_->setMaximum(0);
	progressLabel_ = new QLabel();
	statusLabel_ = new QLabel();
	statusLabel_->setWordWrap(true);

	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(progressBar_);
	hl->addWidget(progressLabel_);

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addStretch();
	vl->addLayout(hl);
	vl->addWidget(statusLabel_);
	vl->addStretch();

	setLayout(vl);

	setTitle("Writing files...");
	setSubTitle("Acquaman is now exporting your scans to disk.");
}

void AMExportWizardProgressPage::initializePage()
{
	if(controller_) {
		disconnect(controller_, 0, this, 0);
		disconnect(controller_, 0, statusLabel_, 0);
	}

	controller_ = qobject_cast<AMExportWizard*>(wizard())->controller();

	connect(controller_, SIGNAL(stateChanged(int)), this, SLOT(onControllerStateChanged(int)));
	connect(controller_, SIGNAL(progressChanged(int,int)), this, SLOT(onControllerProgressChanged(int,int)));
	connect(controller_, SIGNAL(statusChanged(QString)), statusLabel_, SLOT(setText(QString)));

	// re-enable the cancel button, if it's not shown by default (ie: Mac OS X)
	wizard()->setOption(QWizard::NoCancelButton, false);

	controller_->start();
}

bool AMExportWizardProgressPage::validatePage()
{
	return true;
}

bool AMExportWizardProgressPage::isComplete() const
{
	return (!controller_) || (controller_->state() == AMExportController::Finished || controller_->state() == AMExportController::Cancelled);
}

void AMExportWizardProgressPage::onControllerStateChanged(int exportControllerState)
{
	emit completeChanged();
	if(exportControllerState == AMExportController::Finished) {
		wizard()->setOption(QWizard::NoCancelButton, true);
		setTitle("Export Finished");
		QString message = QString::number(controller_->succeededCount()) % " scans were exported succesfully.";
		if(controller_->failedCount())
			message.append("\n\n" % QString::number(controller_->failedCount()) % " scans could not be exported.");
		setSubTitle(message);
	}
}

void AMExportWizardProgressPage::onControllerProgressChanged(int current, int total)
{
	progressLabel_->setText(QString("%1 of %2").arg(current).arg(total));
	progressBar_->setMaximum(total);
	progressBar_->setValue(current);
}


