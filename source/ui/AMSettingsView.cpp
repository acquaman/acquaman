#include "AMSettingsView.h"

#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QFontMetrics>
#include <QMessageBox>

#include "util/AMSettings.h"

AMUserSettingsView::AMUserSettingsView(QWidget *parent) :
	QGroupBox("User Settings", parent)
{
	userDataFolderLineEdit_ = new QLineEdit();
	userDataFolderLineEdit_->setText(AMUserSettings::userDataFolder);
	userDataFolderLineEdit_->setEnabled(false);
	userDatabaseFileNameLineEdit_  = new QLineEdit();
	userDatabaseFileNameLineEdit_->setText(AMUserSettings::userDatabaseFilename);
	userDatabaseFileNameLineEdit_->setEnabled(false);

	fl_ = new QFormLayout();
	fl_->addRow("Data Folder", userDataFolderLineEdit_);
	fl_->addRow("Database", userDatabaseFileNameLineEdit_);

	int maxFontWidth = 0;
	QFontMetrics fontMetric(userDataFolderLineEdit_->font());
	for(int x = 0; x < fl_->rowCount(); x++){
		QLineEdit *lineEdit = qobject_cast<QLineEdit*>(fl_->itemAt(x, QFormLayout::FieldRole)->widget());
		if(lineEdit && (fontMetric.width(lineEdit->text()) > maxFontWidth))
			maxFontWidth = fontMetric.width(lineEdit->text());
	}
	if(maxFontWidth > 400)
		maxFontWidth = 400;
	for(int x = 0; x < fl_->rowCount(); x++)
		fl_->itemAt(x, QFormLayout::FieldRole)->widget()->setMinimumWidth(maxFontWidth);


	setLayout(fl_);
}

AMSettingsView::AMSettingsView(QWidget *parent) :
	QGroupBox("Public Settings", parent)
{
	unsavedChanges_ = false;

	publicDataFolderLineEdit_ = new QLineEdit();
	publicDataFolderLineEdit_->setText(AMSettings::publicDataFolder);
	publicDataFolderLineEdit_->setEnabled(false);
	publicDatabaseFileNameLineEdit_  = new QLineEdit();
	publicDatabaseFileNameLineEdit_->setText(AMSettings::publicDatabaseFilename);
	publicDatabaseFileNameLineEdit_->setEnabled(false);
	fileLoaderPluginsFolderLineEdit_ = new QLineEdit();
	fileLoaderPluginsFolderLineEdit_->setText(AMSettings::fileLoaderPluginsFolder);

	connect(publicDataFolderLineEdit_, SIGNAL(textEdited(QString)), this, SLOT(onLineEditsChanged()));
	connect(publicDatabaseFileNameLineEdit_, SIGNAL(textEdited(QString)), this, SLOT(onLineEditsChanged()));
	connect(fileLoaderPluginsFolderLineEdit_, SIGNAL(textEdited(QString)), this, SLOT(onLineEditsChanged()));

	fl_ = new QFormLayout();
	fl_->addRow("Data Folder", publicDataFolderLineEdit_);
	fl_->addRow("Database", publicDatabaseFileNameLineEdit_);
	fl_->addRow("File Loaders Folder", fileLoaderPluginsFolderLineEdit_);

	int maxFontWidth = 0;
	QFontMetrics fontMetric(fileLoaderPluginsFolderLineEdit_->font());
	for(int x = 0; x < fl_->rowCount(); x++){
		QLineEdit *lineEdit = qobject_cast<QLineEdit*>(fl_->itemAt(x, QFormLayout::FieldRole)->widget());
		if(lineEdit && (fontMetric.width(lineEdit->text()) > maxFontWidth))
			maxFontWidth = fontMetric.width(lineEdit->text());
	}
	if(maxFontWidth > 400)
		maxFontWidth = 400;
	for(int x = 0; x < fl_->rowCount(); x++)
		fl_->itemAt(x, QFormLayout::FieldRole)->widget()->setMinimumWidth(maxFontWidth);


	setLayout(fl_);
}

bool AMSettingsView::hasUnsavedChanges(){
	return unsavedChanges_;
}

void AMSettingsView::applyChanges(){
	if(unsavedChanges_){
		if(AMSettings::publicDataFolder != publicDataFolderLineEdit_->text())
			AMSettings::publicDataFolder = publicDataFolderLineEdit_->text();
		if(AMSettings::publicDatabaseFilename != publicDatabaseFileNameLineEdit_->text())
			AMSettings::publicDatabaseFilename = publicDatabaseFileNameLineEdit_->text();
		if(AMSettings::fileLoaderPluginsFolder != fileLoaderPluginsFolderLineEdit_->text())
			AMSettings::fileLoaderPluginsFolder = fileLoaderPluginsFolderLineEdit_->text();

		AMSettings::save();
		AMSettings::load();
		storeInitialState();
	}
}

void AMSettingsView::cancelChanges(){
	if(unsavedChanges_){
		publicDataFolderLineEdit_->setText(AMSettings::publicDataFolder);
		publicDatabaseFileNameLineEdit_->setText(AMSettings::publicDatabaseFilename);
		fileLoaderPluginsFolderLineEdit_->setText(AMSettings::fileLoaderPluginsFolder);
		storeInitialState();
	}
}

#include <QShowEvent>
void AMSettingsView::showEvent(QShowEvent *e){
	storeInitialState();
	e->accept();
}

void AMSettingsView::storeInitialState(){
	unsavedChanges_ = false;
	initialLineEdits_.clear();
	for(int x = 0; x < fl_->rowCount(); x++){
		QLineEdit *lineEdit = qobject_cast<QLineEdit*>(fl_->itemAt(x, QFormLayout::FieldRole)->widget());
		if(lineEdit)
			initialLineEdits_.append(lineEdit->text());
	}
}

void AMSettingsView::onLineEditsChanged(){
	for(int x = 0; x < fl_->rowCount(); x++){
		QLineEdit *lineEdit = qobject_cast<QLineEdit*>(fl_->itemAt(x, QFormLayout::FieldRole)->widget());
		if(lineEdit && (initialLineEdits_.at(x) != lineEdit->text()) ){
			if(!unsavedChanges_){
				unsavedChanges_ = true;
				emit unsavedChanges(true);
			}
			return;
		}
	}
	if(unsavedChanges_){
		unsavedChanges_ = false;
		emit unsavedChanges(false);
	}
}

AMSettingsMasterView::AMSettingsMasterView(QWidget *parent) :
	QWidget(parent)
{
	userSettingsView_ = new AMUserSettingsView();
	settingsView_ = new AMSettingsView();

	connect(settingsView_, SIGNAL(unsavedChanges(bool)), this, SLOT(onUnsavedChanges(bool)));

	vl_ = new QVBoxLayout();
	vl_->addWidget(userSettingsView_);
	vl_->addWidget(settingsView_);

	applyButton_ = new QPushButton("Apply");
	cancelButton_ = new QPushButton("Cancel");
	okButton_ = new QPushButton("Ok");

	connect(applyButton_, SIGNAL(clicked()), this, SLOT(onApplyButtonClicked()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
	connect(okButton_, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(applyButton_);
	hl->addStretch(4);
	hl->addWidget(cancelButton_);
	hl->addWidget(okButton_);

	vl_->addLayout(hl);

	setLayout(vl_);
}

void AMSettingsMasterView::onUnsavedChanges(bool hasUnsavedChanges){
	if(hasUnsavedChanges)
		applyButton_->setDefault(true);
	else
		applyButton_->setDefault(false);
}

void AMSettingsMasterView::onCancelButtonClicked(){
	settingsView_->cancelChanges();
	close();
}

void AMSettingsMasterView::onApplyButtonClicked(){
	settingsView_->applyChanges();
}

void AMSettingsMasterView::onOkButtonClicked(){
	onApplyButtonClicked();
	close();
}

#include <QCloseEvent>
void AMSettingsMasterView::closeEvent(QCloseEvent *e){
	if(!settingsView_->hasUnsavedChanges()){
		e->accept();
		return;
	}
	else{
		int ret;
		ret = QMessageBox::warning(this, tr("Acquaman Settings Changed"),
					   tr("Acquaman's settings have been changed\n"
					      "Do you wish to save the changes?"),
					   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if(ret == QMessageBox::Save){
			settingsView_->applyChanges();
			e->accept();
		}
		else if(ret == QMessageBox::Discard){
			settingsView_->cancelChanges();
			e->accept();
		}
		else
			e->ignore();
	}
}
