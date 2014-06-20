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

#include "ui/util/AMSettingsView.h"

#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFontMetrics>
#include <QMessageBox>

#include "util/AMSettings.h"
#include "application/AMPluginsManager.h"

 AMUserSettingsView::~AMUserSettingsView(){}
AMUserSettingsView::AMUserSettingsView(QWidget *parent) :
	QGroupBox("User Settings", parent)
{
	userDataFolderLineEdit_ = new QLineEdit();
	userDataFolderLineEdit_->setText(AMUserSettings::userDataFolder);
	userDataFolderLineEdit_->setEnabled(false);
	userDatabaseFileNameLineEdit_  = new QLineEdit();
	userDatabaseFileNameLineEdit_->setText(AMUserSettings::userDatabaseFilename);
	userDatabaseFileNameLineEdit_->setEnabled(false);
	remoteDataFolderLineEdit_= new QLineEdit();
	remoteDataFolderLineEdit_->setText(AMUserSettings::remoteDataFolder);
	remoteDataFolderLineEdit_->setEnabled(false);
	fl_ = new QFormLayout();
	fl_->addRow("Data Folder", userDataFolderLineEdit_);
	fl_->addRow("Database", userDatabaseFileNameLineEdit_);
	fl_->addRow("Remote Location", remoteDataFolderLineEdit_);
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

 AMSettingsView::~AMSettingsView(){}
AMSettingsView::AMSettingsView(QWidget *parent) :
	QGroupBox("Public Settings", parent)
{
	unsavedChanges_ = false;

	publicDataFolderLineEdit_ = new QLineEdit();
	publicDataFolderLineEdit_->setText(AMSettings::s()->publicDataFolder());
	publicDataFolderLineEdit_->setEnabled(false);
	publicDatabaseFileNameLineEdit_  = new QLineEdit();
	publicDatabaseFileNameLineEdit_->setText(AMSettings::s()->publicDatabaseFilename());
	publicDatabaseFileNameLineEdit_->setEnabled(false);
	fileLoaderPluginsFolderLineEdit_ = new QLineEdit();
	fileLoaderPluginsFolderLineEdit_->setText(AMSettings::s()->fileLoaderPluginsFolder());

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

void AMSettingsView::applyChanges() {
	bool shouldReloadPlugins = false;

	if(unsavedChanges_){
		if(AMSettings::s()->publicDataFolder() != publicDataFolderLineEdit_->text())
			AMSettings::s()->setPublicDataFolder(publicDataFolderLineEdit_->text());
		if(AMSettings::s()->publicDatabaseFilename() != publicDatabaseFileNameLineEdit_->text())
			AMSettings::s()->setPublicDatabaseFilename(publicDatabaseFileNameLineEdit_->text());
		if(AMSettings::s()->fileLoaderPluginsFolder() != fileLoaderPluginsFolderLineEdit_->text()) {
			AMSettings::s()->setFileLoaderPluginsFolder(fileLoaderPluginsFolderLineEdit_->text());
			shouldReloadPlugins = true;
		}

		AMSettings::s()->save();
		if(shouldReloadPlugins)
			AMPluginsManager::s()->loadApplicationPlugins();

		storeInitialState();
	}
}

void AMSettingsView::discardChanges(){
	if(unsavedChanges_){
		publicDataFolderLineEdit_->setText(AMSettings::s()->publicDataFolder());
		publicDatabaseFileNameLineEdit_->setText(AMSettings::s()->publicDatabaseFilename());
		fileLoaderPluginsFolderLineEdit_->setText(AMSettings::s()->fileLoaderPluginsFolder());
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

 AMSettingsMasterView::~AMSettingsMasterView(){}
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
	settingsView_->discardChanges();
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
			settingsView_->discardChanges();
			e->accept();
		}
		else
			e->ignore();
	}
}
