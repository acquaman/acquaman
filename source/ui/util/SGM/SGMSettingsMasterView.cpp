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


#include "SGMSettingsMasterView.h"
#include <QHBoxLayout>
#include <QMessageBox>

SGMSettingsMasterView::SGMSettingsMasterView(QWidget *parent) :
	QWidget(parent)
{
	sgmPluginsLocationView_ = new SGMPluginsLocationView();
	sgmDacqConfigurationFileView_ = new SGMDacqConfigurationFileView();
	sgmDetectorsMasterView_ = new SGMDetectorsMasterView();

	connect(sgmPluginsLocationView_, SIGNAL(unsavedChanges(bool)), this, SLOT(onUnsavedChanges(bool)));
	connect(sgmDacqConfigurationFileView_, SIGNAL(unsavedChanges(bool)), this, SLOT(onUnsavedChanges(bool)));

	vl_ = new QVBoxLayout();
	vl_->addWidget(sgmPluginsLocationView_);
	vl_->addWidget(sgmDacqConfigurationFileView_);
	vl_->addWidget(sgmDetectorsMasterView_);

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

void SGMSettingsMasterView::onUnsavedChanges(bool hasUnsavedChanges){
	if(hasUnsavedChanges)
		applyButton_->setDefault(true);
	else
		applyButton_->setDefault(false);
}

void SGMSettingsMasterView::onCancelButtonClicked(){
	sgmPluginsLocationView_->discardChanges();
	sgmDacqConfigurationFileView_->discardChanges();
	close();
}

void SGMSettingsMasterView::onApplyButtonClicked(){
	sgmPluginsLocationView_->applyChanges();
	sgmDacqConfigurationFileView_->applyChanges();
}

void SGMSettingsMasterView::onOkButtonClicked(){
	onApplyButtonClicked();
	close();
}

#include <QCloseEvent>
void SGMSettingsMasterView::closeEvent(QCloseEvent *e){
	if(!sgmPluginsLocationView_->hasUnsavedChanges() && !sgmDacqConfigurationFileView_->hasUnsavedChanges()){
		e->accept();
		return;
	}
	else{
		int ret;
		ret = QMessageBox::warning(this, tr("SGM Beamline Settings Changed"),
								   tr("SGM's settings have been changed\n"
									  "Do you wish to save the changes?"),
								   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if(ret == QMessageBox::Save){
			sgmPluginsLocationView_->applyChanges();
			sgmDacqConfigurationFileView_->applyChanges();
			e->accept();
		}
		else if(ret == QMessageBox::Discard){
			sgmPluginsLocationView_->discardChanges();
			sgmDacqConfigurationFileView_->discardChanges();
			e->accept();
		}
		else
			e->ignore();
	}
}

SGMPluginsLocationView::SGMPluginsLocationView(QWidget *parent) :
	QGroupBox("Plugin Locations", parent)
{
	unsavedChanges_ = false;

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	SGMPluginsLocation *tempFileLoaderLocation = new SGMPluginsLocation();
	QList<int> matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "FileLoaders");
	//QList<int> matchIDs = AMDatabase::database("SGMBeamine")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "FileLoaders");
	if(matchIDs.count() > 0)
		tempFileLoaderLocation->loadFromDb(dbSGM, matchIDs.at(0));
	SGMPluginsLocation *tempAnalysisBlockLocation = new SGMPluginsLocation();
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "AnalysisBlocks");
	if(matchIDs.count() > 0)
		tempAnalysisBlockLocation->loadFromDb(dbSGM, matchIDs.at(0));


	fileLoaderPluginsFolderLineEdit_ = new QLineEdit();
	if(tempFileLoaderLocation)
		fileLoaderPluginsFolderLineEdit_->setText(tempFileLoaderLocation->pluginFolderPath());
	else
		fileLoaderPluginsFolderLineEdit_->setText("Load Fail");

	analysisBlockPluginsFolderLineEdit_ = new QLineEdit();
	if(tempAnalysisBlockLocation)
		analysisBlockPluginsFolderLineEdit_->setText(tempAnalysisBlockLocation->pluginFolderPath());
	else
		analysisBlockPluginsFolderLineEdit_->setText("Load Fail");

	connect(fileLoaderPluginsFolderLineEdit_, SIGNAL(textEdited(QString)), this, SLOT(onLineEditsChanged()));
	connect(analysisBlockPluginsFolderLineEdit_, SIGNAL(textEdited(QString)), this, SLOT(onLineEditsChanged()));

	fl_ = new QFormLayout();
	fl_->addRow("File Loaders Folder", fileLoaderPluginsFolderLineEdit_);
	fl_->addRow("Analysis Blocks Folder", analysisBlockPluginsFolderLineEdit_);

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

bool SGMPluginsLocationView::hasUnsavedChanges(){
	return unsavedChanges_;
}

void SGMPluginsLocationView::applyChanges() {
	bool shouldReloadPlugins = false;

	if(unsavedChanges_){
		AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
		SGMPluginsLocation *tempFileLoaderLocation = new SGMPluginsLocation();
		QList<int> matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "FileLoaders");
		if(matchIDs.count() > 0)
			tempFileLoaderLocation->loadFromDb(dbSGM, matchIDs.at(0));
		SGMPluginsLocation *tempAnalysisBlockLocation = new SGMPluginsLocation();
		matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "AnalysisBlocks");
		if(matchIDs.count() > 0)
			tempAnalysisBlockLocation->loadFromDb(dbSGM, matchIDs.at(0));

		if(!tempFileLoaderLocation || !tempAnalysisBlockLocation)
			return;

		if(tempFileLoaderLocation->pluginFolderPath() != fileLoaderPluginsFolderLineEdit_->text()){
			tempFileLoaderLocation->setPluginFolderPath(fileLoaderPluginsFolderLineEdit_->text());
			tempFileLoaderLocation->storeToDb(dbSGM);
			shouldReloadPlugins = true;
		}
		if(tempAnalysisBlockLocation->pluginFolderPath() != analysisBlockPluginsFolderLineEdit_->text()){
			tempAnalysisBlockLocation->setPluginFolderPath(analysisBlockPluginsFolderLineEdit_->text());
			tempAnalysisBlockLocation->storeToDb(dbSGM);
			shouldReloadPlugins = true;
		}

		if(shouldReloadPlugins)
			AMPluginsManager::s()->loadApplicationPlugins(tempFileLoaderLocation->pluginFolderPath(), tempAnalysisBlockLocation->pluginFolderPath());

		storeInitialState();
	}
}

void SGMPluginsLocationView::discardChanges(){
	if(unsavedChanges_){
		AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
		SGMPluginsLocation *tempFileLoaderLocation = new SGMPluginsLocation();
		QList<int> matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "FileLoaders");
		if(matchIDs.count() > 0)
			tempFileLoaderLocation->loadFromDb(dbSGM, matchIDs.at(0));
		SGMPluginsLocation *tempAnalysisBlockLocation = new SGMPluginsLocation();
		matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "AnalysisBlocks");
		if(matchIDs.count() > 0)
			tempAnalysisBlockLocation->loadFromDb(dbSGM, matchIDs.at(0));

		if(!tempFileLoaderLocation || !tempAnalysisBlockLocation)
			return;

		fileLoaderPluginsFolderLineEdit_->setText(tempFileLoaderLocation->pluginFolderPath());
		analysisBlockPluginsFolderLineEdit_->setText(tempAnalysisBlockLocation->pluginFolderPath());
		storeInitialState();
	}
}

#include <QShowEvent>
void SGMPluginsLocationView::showEvent(QShowEvent *e){
	storeInitialState();
	e->accept();
}

void SGMPluginsLocationView::storeInitialState(){
	unsavedChanges_ = false;
	initialLineEdits_.clear();
	for(int x = 0; x < fl_->rowCount(); x++){
		QLineEdit *lineEdit = qobject_cast<QLineEdit*>(fl_->itemAt(x, QFormLayout::FieldRole)->widget());
		if(lineEdit)
			initialLineEdits_.append(lineEdit->text());
	}
}

void SGMPluginsLocationView::onLineEditsChanged(){
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

#include "util/SGM/SGMDacqConfigurationFile.h"

SGMDacqConfigurationFileView::SGMDacqConfigurationFileView(QWidget *parent) :
	QGroupBox("Configuration Files", parent)
{
	unsavedChanges_ = false;

	fl_ = new QFormLayout();

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	SGMDacqConfigurationFile *configFile = new SGMDacqConfigurationFile();
	QLineEdit *tempLineEdit;
	QList<int> allConfigurationFileIDs = dbSGM->objectsWhere(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>());

	for(int x = 0; x < allConfigurationFileIDs.count(); x++){
		if(configFile->loadFromDb(dbSGM, allConfigurationFileIDs.at(x))){
			tempLineEdit = new QLineEdit();
			tempLineEdit->setText(configFile->configurationFileFullPath());
			fl_->addRow(configFile->name(), tempLineEdit);
			connect(tempLineEdit, SIGNAL(textEdited(QString)), this, SLOT(onLineEditsChanged()));
			configurationFileIDs_.append(allConfigurationFileIDs.at(x));
		}
	}

	int maxFontWidth = 0;
	QLineEdit *lineEdit = qobject_cast<QLineEdit*>(fl_->itemAt(0, QFormLayout::FieldRole)->widget());
	QFontMetrics fontMetric(lineEdit->font());
	for(int x = 0; x < fl_->rowCount(); x++){
		QLineEdit *lineEdit = qobject_cast<QLineEdit*>(fl_->itemAt(x, QFormLayout::FieldRole)->widget());
		if(lineEdit && (fontMetric.width(lineEdit->text()) > maxFontWidth))
			maxFontWidth = fontMetric.width(lineEdit->text());
	}
	if(maxFontWidth > 500)
		maxFontWidth = 500;
	for(int x = 0; x < fl_->rowCount(); x++)
		fl_->itemAt(x, QFormLayout::FieldRole)->widget()->setMinimumWidth(maxFontWidth);


	setLayout(fl_);
}

bool SGMDacqConfigurationFileView::hasUnsavedChanges(){
	return unsavedChanges_;
}

void SGMDacqConfigurationFileView::applyChanges() {

	if(unsavedChanges_){
		AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
		SGMDacqConfigurationFile *configFile = new SGMDacqConfigurationFile();
		QList<int> allConfigurationFileIDs = dbSGM->objectsWhere(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>());

		for(int x = 0; x < allConfigurationFileIDs.count(); x++){
			if(allConfigurationFileIDs.at(x) != configurationFileIDs_.at(x)){
				//errormon here
				return;
			}
		}

		for(int x = 0; x < configurationFileIDs_.count(); x++){
			QLineEdit *lineEdit = qobject_cast<QLineEdit*>(fl_->itemAt(x, QFormLayout::FieldRole)->widget());
			if(!configFile->loadFromDb(dbSGM, configurationFileIDs_.at(x))){
				//errormon here
				return;
			}
			if(configFile->configurationFileFullPath() != lineEdit->text()){
				configFile->setConfigurationFileName(lineEdit->text().section('/', -1));
				configFile->setConfigurationFilePath("/"+lineEdit->text().section('/', 1, -2));
				configFile->storeToDb(dbSGM);
			}
		}

		storeInitialState();
	}
}

void SGMDacqConfigurationFileView::discardChanges(){
	if(unsavedChanges_){
		AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
		SGMDacqConfigurationFile *configFile = new SGMDacqConfigurationFile();
		QList<int> allConfigurationFileIDs = dbSGM->objectsWhere(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>());

		for(int x = 0; x < allConfigurationFileIDs.count(); x++){
			if(allConfigurationFileIDs.at(x) != configurationFileIDs_.at(x)){
				//errormon here
				return;
			}
		}

		for(int x = 0; x < configurationFileIDs_.count(); x++){
			QLineEdit *lineEdit = qobject_cast<QLineEdit*>(fl_->itemAt(x, QFormLayout::FieldRole)->widget());
			if(!configFile->loadFromDb(dbSGM, configurationFileIDs_.at(x))){
				//errormon here
				return;
			}
			lineEdit->setText(configFile->configurationFileFullPath());
		}

		storeInitialState();
	}
}

#include <QShowEvent>
void SGMDacqConfigurationFileView::showEvent(QShowEvent *e){
	storeInitialState();
	e->accept();
}

void SGMDacqConfigurationFileView::storeInitialState(){
	unsavedChanges_ = false;
	initialLineEdits_.clear();
	for(int x = 0; x < fl_->rowCount(); x++){
		QLineEdit *lineEdit = qobject_cast<QLineEdit*>(fl_->itemAt(x, QFormLayout::FieldRole)->widget());
		if(lineEdit)
			initialLineEdits_.append(lineEdit->text());
	}
}

void SGMDacqConfigurationFileView::onLineEditsChanged(){
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

#include "beamline/SGM/SGMBeamline.h"
#include <QCheckBox>
#include <QLabel>

SGMDetectorsMasterView::SGMDetectorsMasterView(QWidget *parent) :
	QGroupBox("Detectors", parent)
{
	unsavedChanges_ = false;

	fl_ = new QFormLayout();

	QCheckBox *tempCheckBox;
	QLabel *tempLabel;
	QHBoxLayout *tempHL;
	AMDetectorSet *allDetectors = SGMBeamline::sgm()->rawDetectors();
	AMDetectorSet *criticalDetectors = SGMBeamline::sgm()->criticalDetectorsSet();
	for(int x = 0; x < allDetectors->count(); x++){
		tempCheckBox = new QCheckBox("(Required)");
		if(criticalDetectors->indexOf(allDetectors->detectorAt(x)) != -1)
			tempCheckBox->setChecked(true);
		tempLabel = new QLabel();
		QPalette labelPallete = tempLabel->palette();
		if(allDetectors->detectorAt(x)->isConnected()){
			tempLabel->setText("Connected");
			labelPallete.setColor(tempLabel->foregroundRole(), QColor(0, 255, 0));
		}
		else{
			tempLabel->setText("Not Connected");
			labelPallete.setColor(tempLabel->foregroundRole(), QColor(255, 0, 0));
		}
		tempLabel->setPalette(labelPallete);
		tempHL = new QHBoxLayout();
		tempHL->addWidget(tempCheckBox);
		tempHL->addWidget(tempLabel);

		//fl_->addRow(allDetectors->detectorAt(x)->detectorName(), tempCheckBox);
		fl_->addRow(allDetectors->detectorAt(x)->description(), tempHL);
		connect(tempCheckBox, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxesChanged(bool)));
		connect(allDetectors->detectorAt(x)->signalSource(), SIGNAL(availabilityChagned(AMDetector*,bool)), this, SLOT(onDetectorAvailabilityChanged(AMDetector*,bool)));
	}

	setLayout(fl_);
}

void SGMDetectorsMasterView::onCheckBoxesChanged(bool toggled){

}

void SGMDetectorsMasterView::onDetectorAvailabilityChanged(AMDetector *detector, bool isAvailable){
	for(int x = 0; x < fl_->rowCount(); x++){
		QLabel *label = qobject_cast<QLabel*>(fl_->itemAt(x, QFormLayout::LabelRole)->widget());
		if(label && (label->text() == detector->description()) ){
			QLabel *connectedLabel = qobject_cast<QLabel*>(fl_->itemAt(x, QFormLayout::FieldRole)->layout()->itemAt(1)->widget());
			if(connectedLabel){
				QPalette labelPallete = connectedLabel->palette();
				if(isAvailable){
					connectedLabel->setText("Connected");
					labelPallete.setColor(connectedLabel->foregroundRole(), QColor(0, 255, 0));
				}
				else{
					connectedLabel->setText("Not Connected");
					labelPallete.setColor(connectedLabel->foregroundRole(), QColor(255, 0, 0));
				}
				connectedLabel->setPalette(labelPallete);
			}
			return;
		}
	}
}
