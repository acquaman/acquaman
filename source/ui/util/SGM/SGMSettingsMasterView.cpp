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


#pragma GCC diagnostic ignored "-Wunused-private-field"
#include <QCloseEvent>
#pragma GCC diagnostic warning "-Wunused-private-field"

#include "SGMSettingsMasterView.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCheckBox>
#include <QLabel>
#include <QShowEvent>

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/AMDetectorSelector.h"
#include "ui/beamline/AMDetectorSelectorRequiredView.h"

 SGMSettingsMasterView::~SGMSettingsMasterView(){}
SGMSettingsMasterView::SGMSettingsMasterView(QWidget *parent) :
	QWidget(parent)
{
	sgmPluginsLocationView_ = new SGMPluginsLocationView();
	sgmDetectorsMasterView_ = new SGMDetectorsMasterView();

	connect(sgmPluginsLocationView_, SIGNAL(unsavedChanges(bool)), this, SLOT(onUnsavedChanges(bool)));
	connect(sgmDetectorsMasterView_, SIGNAL(unsavedChanges(bool)), this, SLOT(onUnsavedChanges(bool)));

	vl_ = new QVBoxLayout();
	vl_->addWidget(sgmPluginsLocationView_);
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
	sgmDetectorsMasterView_->discardChanges();
	close();
}

void SGMSettingsMasterView::onApplyButtonClicked(){
	sgmPluginsLocationView_->applyChanges();
	sgmDetectorsMasterView_->applyChanges();
}

void SGMSettingsMasterView::onOkButtonClicked(){
	onApplyButtonClicked();
	close();
}

void SGMSettingsMasterView::closeEvent(QCloseEvent *e){
	if(!sgmPluginsLocationView_->hasUnsavedChanges() && !sgmDetectorsMasterView_->hasUnsavedChanges()){
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
			sgmDetectorsMasterView_->applyChanges();
			e->accept();
		}
		else if(ret == QMessageBox::Discard){
			sgmPluginsLocationView_->discardChanges();
			sgmDetectorsMasterView_->discardChanges();
			e->accept();
		}
		else
			e->ignore();
	}
}

 SGMPluginsLocationView::~SGMPluginsLocationView(){}
SGMPluginsLocationView::SGMPluginsLocationView(QWidget *parent) :
	QGroupBox("Plugin Locations", parent)
{
	unsavedChanges_ = false;

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	SGMPluginsLocation *tempFileLoaderLocation = new SGMPluginsLocation();
	QList<int> matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "FileLoaders");
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

 SGMDetectorsMasterView::~SGMDetectorsMasterView(){}
SGMDetectorsMasterView::SGMDetectorsMasterView(QWidget *parent) :
	QGroupBox("Detectors", parent)
{
	unsavedChanges_ = false;

	criticalDetectorSet_ = SGMBeamline::sgm()->criticalDetectorSet(this);

	requiredDetectorSelector_ = new AMDetectorSelector(SGMBeamline::sgm()->allDetectorGroup());
	for(int x = 0, size = criticalDetectorSet_->count(); x < size; x++)
		requiredDetectorSelector_->setDetectorDefault(criticalDetectorSet_->at(x), true);
	requiredDetectorSelector_->setDefaultsSelected();
	requiredDetectorSelectorView_ = new AMDetectorSelectorRequiredView(requiredDetectorSelector_);

	connect(requiredDetectorSelector_, SIGNAL(selectedChanged(AMDetector*)), this, SLOT(onSelectedChanged(AMDetector*)));

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(requiredDetectorSelectorView_);
	setLayout(vl);
}

bool SGMDetectorsMasterView::hasUnsavedChanges() const{
	return unsavedChanges_;
}

void SGMDetectorsMasterView::applyChanges(){
	if(unsavedChanges_){

		AMDetectorInfoSet selectedDetectorInfos = requiredDetectorSelector_->selectedDetectorInfos();
		AMDetectorInfoSet unselectedDetectorInfos = requiredDetectorSelector_->unselectedDetectorInfos();

		for(int x = 0, size = selectedDetectorInfos.count(); x < size; x++)
			if(!criticalDetectorSet_->detectorNamed(selectedDetectorInfos.at(x).name()))
				criticalDetectorSet_->addDetector(requiredDetectorSelector_->detectorGroup()->detectorByName(selectedDetectorInfos.at(x).name()));

		for(int x = 0, size = unselectedDetectorInfos.count(); x < size; x++)
			if(criticalDetectorSet_->detectorNamed(unselectedDetectorInfos.at(x).name()))
				criticalDetectorSet_->removeDetector(requiredDetectorSelector_->detectorGroup()->detectorByName(unselectedDetectorInfos.at(x).name()));
	}
}

void SGMDetectorsMasterView::discardChanges(){
	if(unsavedChanges_){
		disconnect(requiredDetectorSelector_, SIGNAL(selectedChanged(AMDetector*)), this, SLOT(onSelectedChanged(AMDetector*)));

		AMDetectorInfoSet selectedDetectorInfos = requiredDetectorSelector_->selectedDetectorInfos();
		for(int x = 0, size = criticalDetectorSet_->count(); x < size; x++){
			requiredDetectorSelector_->setDetectorSelected(criticalDetectorSet_->detectorAt(x), true);
			selectedDetectorInfos.removeDetector(criticalDetectorSet_->detectorAt(x)->toInfo());
		}
		for(int x = 0, size = selectedDetectorInfos.count(); x < size; x++)
			requiredDetectorSelector_->setDetectorSelectedByName(selectedDetectorInfos.at(x).name(), false);

		connect(requiredDetectorSelector_, SIGNAL(selectedChanged(AMDetector*)), this, SLOT(onSelectedChanged(AMDetector*)));

		unsavedChanges_ = false;
	}
}

void SGMDetectorsMasterView::onSelectedChanged(AMDetector *detector){
	Q_UNUSED(detector)
	AMDetectorInfoSet selectedDetectorInfos = requiredDetectorSelector_->selectedDetectorInfos();
	if(selectedDetectorInfos.count() != criticalDetectorSet_->count()){
		unsavedChangesHelper(true);
		return;
	}

	for(int x = 0, size = criticalDetectorSet_->count(); x < size; x++){
		if(selectedDetectorInfos.indexOf(criticalDetectorSet_->detectorAt(x)->toInfo()) == -1){
			unsavedChangesHelper(true);
			return;
		}
		selectedDetectorInfos.removeDetector(criticalDetectorSet_->detectorAt(x)->toInfo());
	}
	if(selectedDetectorInfos.count() != 0){
		unsavedChangesHelper(true);
		return;
	}
	unsavedChangesHelper(false);
}

void SGMDetectorsMasterView::showEvent(QShowEvent *e){
	unsavedChanges_ = false;
	e->accept();
}

void SGMDetectorsMasterView::unsavedChangesHelper(bool newUnsavedChanges){
	if(newUnsavedChanges != unsavedChanges_){
		unsavedChanges_ = newUnsavedChanges;
		emit unsavedChanges(unsavedChanges_);
	}
}
