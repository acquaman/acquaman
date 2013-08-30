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
	motorGroupView_ = new AMMotorGroupView(motorGroup);
	samplePlateBrowserView_ = new AMSamplePlateBrowserView(beamline_->samplePlateBrowser());
	samplePlateView_ = new AMSamplePlateView(beamline_->samplePlate());

	createSamplePlateButton_ = new QPushButton("Create Sample Plate");
	loadSamplePlateButton_ = new QPushButton("Load Sample Plate");

	wizardSelectorView_ = new AMSampleCameraWizardSelector();

	QVBoxLayout *leftVL = new QVBoxLayout();
	QVBoxLayout *rightVL = new QVBoxLayout();

	leftVL->addWidget(cameraBrowserView_);
	leftVL->addWidget(motorGroupView_);
	leftVL->setContentsMargins(10,0,0,0);

	rightVL->addWidget(createSamplePlateButton_);
	rightVL->addWidget(loadSamplePlateButton_);
	rightVL->addWidget(samplePlateView_);
	rightVL->addWidget(wizardSelectorView_);
	rightVL->setContentsMargins(0,0,10,0);

	samplePlateBrowserView_->hide();

	QSplitter *mainSplitter = new QSplitter();
	QWidget *leftWidget = new QWidget();
	QWidget *rightWidget = new QWidget();
	leftWidget->setLayout(leftVL);
	rightWidget->setLayout(rightVL);
	mainSplitter->addWidget(leftWidget);
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
    connect(wizardSelectorView_, SIGNAL(requestLoadBeamConfiguration()), cameraBrowserView_, SIGNAL(requestLoadBeam()));
    connect(wizardSelectorView_, SIGNAL(requestLoadCameraConfiguration()), cameraBrowserView_, SIGNAL(requestLoadCamera()));
    connect(wizardSelectorView_, SIGNAL(requestLoadSamplePlate()), cameraBrowserView_, SIGNAL(requestLoadSamplePlate()));

	connect(cameraBrowserView_, SIGNAL(beamWizardFinished()), wizardSelectorView_, SLOT(onBeamWizardFinished()));
	connect(cameraBrowserView_, SIGNAL(cameraWizardFinished()), wizardSelectorView_, SLOT(onCameraWizardFinished()));
	connect(cameraBrowserView_, SIGNAL(samplePlateWizardFinished()), wizardSelectorView_, SLOT(onSamplePlateWizardFinished()));
	connect(cameraBrowserView_->sampleCameraBrowser()->shapeDataSet(), SIGNAL(sampleShapeDeleted(AMShapeData*)), this, SLOT(onSampleShapeDeleted(AMShapeData*)));
}

void AMBeamlineSampleManagementView::onCreateSamplePlateButtonClicked(){
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

void AMBeamlineSampleManagementView::onLoadSamplePlateButtonClicked(){
	if(!checkSamplePlateModifiedHelper())
		return;

	if(!samplePlateBrowserView_->isHidden())
		samplePlateBrowserView_->raise();
	else
		samplePlateBrowserView_->show();
	samplePlateBrowserView_->clearViewSelection();
}

void AMBeamlineSampleManagementView::onBeamlineSamplePlateAboutToChange(AMSamplePlate *lastSamplePlate){
	if(lastSamplePlate){
		disconnect(cameraBrowserView_->sampleCameraView(), SIGNAL(shapePropertyUpdated(AMShapeData*)), lastSamplePlate, SLOT(onShapeDataPropertyUpdated(AMShapeData*)));
		AMSampleCamera *sampleCamera = cameraBrowserView_->sampleCameraBrowser()->shapeDataSet();
		for(int x = lastSamplePlate->sampleCount()-1; x >= 0; x--)
			sampleCamera->removeSample(lastSamplePlate->sampleAt(x));
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
		cameraBrowserView_->sampleCameraBrowser()->shapeDataSet()->removeSample(sample);
		cameraBrowserView_->sampleCameraView()->requestUpdate();
		sample->removeSample();
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
