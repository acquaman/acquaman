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


#include "AMBeamlineSampleManagementView.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QMessageBox>

#include "ui/beamline/camera/AMSampleCameraBrowserView.h"
#include "beamline/camera/AMSampleCameraBrowser.h"
#include "ui/dataman/AMSamplePlateView.h"
#include "ui/dataman/AMSamplePlateBrowserView.h"
#include "beamline/AMBeamline.h"
#include "beamline/camera/AMSampleCamera.h"
#include "ui/beamline/camera/AMSampleCameraView.h"
#include "ui/beamline/camera/AMSampleCameraWizardSelector.h"
#include "ui/AMMotorGroupView.h"

AMBeamlineSampleManagementView::AMBeamlineSampleManagementView(AMBeamline *beamline, AMMotorGroup *motorGroup, QWidget *parent) :
	QWidget(parent)
{
	beamline_ = beamline;


	cameraBrowserView_ = new AMSampleCameraBrowserView(new AMSampleCameraBrowser());
	motorGroupView_ = new AMMotorGroupView(motorGroup, AMMotorGroupView::CompactView);
	samplePlateBrowserView_ = new AMSamplePlateBrowserView(beamline_->samplePlateBrowser());
	samplePlateView_ = new AMSamplePlateView(beamline_->samplePlate());

	createSamplePlateButton_ = new QPushButton("Create Sample Plate");
	loadSamplePlateButton_ = new QPushButton("Load Sample Plate");

	cameraBrowserView_->sampleCameraView()->sampleCamera()->enableMotorTracking(true);

	wizardSelectorView_ = new AMSampleCameraWizardSelector();

	QSplitter *leftSplitter = new QSplitter(Qt::Vertical);
	leftVL_ = new QVBoxLayout();
	rightVL_ = new QVBoxLayout();

	leftSplitter->addWidget(cameraBrowserView_);
	QWidget *bottomLeftWidget = new QWidget();
	bottomLeftWidget->setLayout(leftVL_);
	leftSplitter->addWidget(bottomLeftWidget);
	leftVL_->setContentsMargins(10,0,0,0);

	rightVL_->addWidget(createSamplePlateButton_);
	rightVL_->addWidget(loadSamplePlateButton_);
	rightVL_->addWidget(samplePlateView_);
	rightVL_->addWidget(wizardSelectorView_);
	rightVL_->setContentsMargins(0,0,10,0);

	samplePlateBrowserView_->hide();

	QSplitter *mainSplitter = new QSplitter();
	QWidget *rightWidget = new QWidget();
	rightWidget->setLayout(rightVL_);
	mainSplitter->addWidget(leftSplitter);
	mainSplitter->addWidget(rightWidget);

	QHBoxLayout *mainHL = new QHBoxLayout();
	mainHL->addWidget(mainSplitter);
	mainHL->setContentsMargins(0,0,0,0);

	setLayout(mainHL);

	connect(createSamplePlateButton_, SIGNAL(clicked()), this, SLOT(onCreateSamplePlateButtonClicked()));
	connect(loadSamplePlateButton_, SIGNAL(clicked()), this, SLOT(onLoadSamplePlateButtonClicked()));
	connect(beamline_, SIGNAL(samplePlateAboutToChange(AMSamplePlate*)), this, SLOT(onBeamlineSamplePlateAboutToChange(AMSamplePlate*)));
	connect(beamline_, SIGNAL(samplePlateChanged(AMSamplePlate*)), this, SLOT(onBeamlineSamplePlateChanged(AMSamplePlate*)));
	connect(samplePlateBrowserView_, SIGNAL(samplePlateSelected(AMSamplePlate*)), this, SLOT(onSamplePlateSelected(AMSamplePlate*)));
	connect(samplePlateView_, SIGNAL(sampleAboutToBeRemoved(int)), this, SLOT(onSampleAboutToBeRemoved(int)));

	// wizard selection buttons
	connect(wizardSelectorView_, SIGNAL(beamWizardPressed()), cameraBrowserView_, SIGNAL(beamWizardPressed()));
	connect(wizardSelectorView_, SIGNAL(cameraWizardPressed()), cameraBrowserView_, SIGNAL(cameraWizardPressed()));
	connect(wizardSelectorView_, SIGNAL(samplePlateWizardPressed()), cameraBrowserView_, SIGNAL(samplePlateWizardPressed()));
	connect(wizardSelectorView_, SIGNAL(rotationWizardPressed()), cameraBrowserView_, SIGNAL(rotationWizardPressed()));
	connect(wizardSelectorView_, SIGNAL(requestLoadBeamConfiguration()), cameraBrowserView_, SIGNAL(requestLoadBeam()));
	connect(wizardSelectorView_, SIGNAL(requestLoadCameraConfiguration()), cameraBrowserView_, SIGNAL(requestLoadCamera()));
	connect(wizardSelectorView_, SIGNAL(requestLoadSamplePlate()), cameraBrowserView_, SIGNAL(requestLoadSamplePlate()));
	connect(wizardSelectorView_, SIGNAL(requestLoadRotationConfiguration()), cameraBrowserView_, SIGNAL(requestLoadRotationConfiguration()));
	connect(wizardSelectorView_, SIGNAL(requestLoadBeamConfigurationFromDatabase()), cameraBrowserView_, SIGNAL(requestLoadBeamFromDatabase()));
	connect(wizardSelectorView_, SIGNAL(requestLoadCameraConfigurationFromDatabase()), cameraBrowserView_, SIGNAL(requestLoadCameraFromDatabase()));
	connect(wizardSelectorView_, SIGNAL(requestLoadSamplePlateConfigurationFromDatabase()), cameraBrowserView_, SIGNAL(requestLoadSamplePlateFromDatabase()));
	connect(wizardSelectorView_, SIGNAL(requestLoadRotationConfigurationFromDatabase()), cameraBrowserView_, SIGNAL(requestLoadRotationConfigurationFromDatabase()));

	connect(cameraBrowserView_, SIGNAL(beamWizardFinished()), wizardSelectorView_, SLOT(onBeamWizardFinished()));
	connect(cameraBrowserView_, SIGNAL(cameraWizardFinished()), wizardSelectorView_, SLOT(onCameraWizardFinished()));
	connect(cameraBrowserView_, SIGNAL(samplePlateWizardFinished()), wizardSelectorView_, SLOT(onSamplePlateWizardFinished()));
	connect(cameraBrowserView_, SIGNAL(rotationWizardFinished()), wizardSelectorView_, SLOT(onRotationWizardFinished()));

	connect(cameraBrowserView_->sampleCameraBrowser()->shapeDataSet(), SIGNAL(sampleShapeDeleted(AMShapeData*)), this, SLOT(onSampleShapeDeleted(AMShapeData*)));

	connect(samplePlateBrowserView_, SIGNAL(samplePlateSelected(AMSamplePlate*)), AMSampleCamera::set(), SLOT(onSamplePlateLoaded(AMSamplePlate*)));
}

AMBeamlineSampleManagementView::~AMBeamlineSampleManagementView()
{
	samplePlateBrowserView_->deleteLater();
}

void AMBeamlineSampleManagementView::requestAdvancedCameraOptionsWindow(){
	if(cameraBrowserView_)
		cameraBrowserView_->requestAdvancedCameraOptionsWindow();
}

void AMBeamlineSampleManagementView::onCreateSamplePlateButtonClicked(){
	if(wizardSelectorView_->cameraWizardComplete() && wizardSelectorView_->rotationWizardComplete() && wizardSelectorView_->samplePlateWizardComplete())
	{
		if(!checkSamplePlateModifiedHelper())
			return;

		AMSamplePlateCreationDialog creationDialog;
		int retVal = creationDialog.exec();

		if(retVal == QDialog::Accepted){
			AMSamplePlate *samplePlate = new AMSamplePlate();
			samplePlate->setName(creationDialog.samplePlateName());

			beamline_->samplePlateBrowser()->addSamplePlate(samplePlate);
			beamline_->setSamplePlate(samplePlate);
		}
	}
	else
	{
		QMessageBox configurationNotCompleteWarningDialog;
		configurationNotCompleteWarningDialog.setText("Cannot create sample plate until camera, rotation and the sample plate have been setup.");
		configurationNotCompleteWarningDialog.setIcon(QMessageBox::Warning);
		configurationNotCompleteWarningDialog.exec();
	}
}

void AMBeamlineSampleManagementView::onLoadSamplePlateButtonClicked(){
	if(wizardSelectorView_->cameraWizardComplete() && wizardSelectorView_->rotationWizardComplete() && wizardSelectorView_->samplePlateWizardComplete())
	{
		if(!checkSamplePlateModifiedHelper())
			return;

		if(!samplePlateBrowserView_->isHidden())
			samplePlateBrowserView_->raise();
		else
			samplePlateBrowserView_->show();
		samplePlateBrowserView_->clearViewSelection();
	}
	else
	{
		QMessageBox configurationNotCompleteWarningDialog;
		configurationNotCompleteWarningDialog.setText("Cannot load sample plate until camera, rotation and the sample plate have been setup.");
		configurationNotCompleteWarningDialog.setIcon(QMessageBox::Warning);
		configurationNotCompleteWarningDialog.exec();
	}
}

void AMBeamlineSampleManagementView::onBeamlineSamplePlateAboutToChange(AMSamplePlate *lastSamplePlate){
	if(lastSamplePlate){
		disconnect(cameraBrowserView_->sampleCameraView(), SIGNAL(shapePropertyUpdated(AMShapeData*)), lastSamplePlate, SLOT(onShapeDataPropertyUpdated(AMShapeData*)));
		AMSampleCamera *sampleCamera = cameraBrowserView_->sampleCameraBrowser()->shapeDataSet();
		disconnect(lastSamplePlate, SIGNAL(sampleMoveToRequested(AMShapeData*)), sampleCamera, SLOT(moveToSampleRequested(AMShapeData*)));
		for(int x = lastSamplePlate->sampleCount()-1; x >= 0; x--)
                {
                    sampleCamera->removeSample(lastSamplePlate->sampleAt(x));

                }
		cameraBrowserView_->sampleCameraView()->requestUpdate();
	}
}

void AMBeamlineSampleManagementView::onBeamlineSamplePlateChanged(AMSamplePlate *samplePlate){
	samplePlateView_->setSamplePlate(samplePlate);

	if(samplePlate){
		AMSampleCamera *sampleCamera = cameraBrowserView_->sampleCameraBrowser()->shapeDataSet();
		for(int x = samplePlate->sampleCount()-1; x >= 0; x--)
			sampleCamera->addSample(samplePlate->sampleAt(x));
		cameraBrowserView_->sampleCameraView()->requestUpdate();
		cameraBrowserView_->setSamplePlateSelected();
		connect(cameraBrowserView_->sampleCameraView(), SIGNAL(shapePropertyUpdated(AMShapeData*)), samplePlate, SLOT(onShapeDataPropertyUpdated(AMShapeData*)));
		connect(samplePlate, SIGNAL(sampleMoveToRequested(AMShapeData*)), sampleCamera, SLOT(moveToSampleRequested(AMShapeData*)));
	}
}

void AMBeamlineSampleManagementView::onSamplePlateSelected(AMSamplePlate *samplePlate){
	beamline_->setSamplePlate(samplePlate);
}

void AMBeamlineSampleManagementView::onSampleShapeDeleted(AMShapeData *sampleShape){
	if(AMBeamline::bl()->samplePlate()){
		AMSample *relatedSample = AMBeamline::bl()->samplePlate()->sampleFromShape(sampleShape);
		if(relatedSample)
			AMBeamline::bl()->samplePlate()->removeSample(relatedSample);
	}
}

void AMBeamlineSampleManagementView::onSampleAboutToBeRemoved(int index){
	AMSample *sample = AMBeamline::bl()->samplePlate()->sampleAt(index);
	AMShapeData *sampleShapeData = sample->sampleShapePositionData();
	if(sampleShapeData){
		sample->removeSample();
                cameraBrowserView_->sampleCameraView()->requestUpdate();

	}
}

bool AMBeamlineSampleManagementView::checkSamplePlateModifiedHelper(){
	if(beamline_->samplePlate() && beamline_->samplePlate()->modified()){
		QMessageBox messageBox;
		messageBox.setText("The current sample plate has been modified.");
		messageBox.setInformativeText("Do you want to save your changes?");
		messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		messageBox.setDefaultButton(QMessageBox::Save);
		int ret = messageBox.exec();
		if(ret == QMessageBox::Save)
			return beamline_->samplePlate()->storeToDb(beamline_->samplePlate()->database());
		else if(ret == QMessageBox::Cancel)
			return false;
	}
	return true;
}
