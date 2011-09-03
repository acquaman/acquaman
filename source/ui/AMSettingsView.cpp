#include "AMSettingsView.h"

#include <QDebug>
#include <QFontMetrics>

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
	publicDataFolderLineEdit_ = new QLineEdit();
	publicDataFolderLineEdit_->setText(AMSettings::publicDataFolder);
	publicDataFolderLineEdit_->setEnabled(false);
	publicDatabaseFileNameLineEdit_  = new QLineEdit();
	publicDatabaseFileNameLineEdit_->setText(AMSettings::publicDatabaseFilename);
	publicDatabaseFileNameLineEdit_->setEnabled(false);
	fileLoaderPluginsFolderLineEdit_ = new QLineEdit();
	fileLoaderPluginsFolderLineEdit_->setText(AMSettings::fileLoaderPluginsFolder);
	fileLoaderPluginsFolderLineEdit_->setEnabled(false);

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

AMSettingsMasterView::AMSettingsMasterView(QWidget *parent) :
	QWidget(parent)
{
	userSettingsView_ = new AMUserSettingsView();
	settingsView_ = new AMSettingsView();

	vl_ = new QVBoxLayout();
	vl_->addWidget(userSettingsView_);
	vl_->addWidget(settingsView_);

	setLayout(vl_);
}
