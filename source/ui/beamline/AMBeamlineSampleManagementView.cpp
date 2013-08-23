#include "AMBeamlineSampleManagementView.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QSplitter>

#include "ui/beamline/camera/AMSampleCameraBrowserView.h"
#include "beamline/camera/AMSampleCameraBrowser.h"
#include "ui/dataman/AMSamplePlateView.h"
#include "ui/dataman/AMSamplePlateBrowserView.h"
#include "beamline/AMBeamline.h"
#include "beamline/camera/AMSampleCamera.h"
#include "ui/beamline/camera/AMSampleCameraView.h"
#include "ui/beamline/camera/AMSampleCameraWizardSelector.h"

AMBeamlineSampleManagementView::AMBeamlineSampleManagementView(AMBeamline *beamline, QWidget *parent) :
	QWidget(parent)
{
	beamline_ = beamline;


	cameraBrowserView_ = new AMSampleCameraBrowserView(new AMSampleCameraBrowser());
	samplePlateBrowserView_ = new AMSamplePlateBrowserView(beamline_->samplePlateBrowser());
	samplePlateView_ = new AMSamplePlateView(beamline_->samplePlate());

	createSamplePlateButton_ = new QPushButton("Create Sample Plate");
	loadSamplePlateButton_ = new QPushButton("Load Sample Plate");

	wizardSelectorView_ = new AMSampleCameraWizardSelector();

	QVBoxLayout *leftVL = new QVBoxLayout();
	QVBoxLayout *rightVL = new QVBoxLayout();

	leftVL->addWidget(cameraBrowserView_);

	rightVL->addWidget(createSamplePlateButton_);
	rightVL->addWidget(loadSamplePlateButton_);
	rightVL->addWidget(samplePlateView_);
	rightVL->addWidget(wizardSelectorView_);

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

	setLayout(mainHL);

	connect(createSamplePlateButton_, SIGNAL(clicked()), this, SLOT(onCreateSamplePlateButtonClicked()));
	connect(loadSamplePlateButton_, SIGNAL(clicked()), this, SLOT(onLoadSamplePlateButtonClicked()));
	connect(beamline_, SIGNAL(samplePlateAboutToChange(AMSamplePlate*)), this, SLOT(onBeamlineSamplePlateAboutToChange(AMSamplePlate*)));
	connect(beamline_, SIGNAL(samplePlateChanged(AMSamplePlate*)), this, SLOT(onBeamlineSamplePlateChanged(AMSamplePlate*)));
	connect(samplePlateBrowserView_, SIGNAL(samplePlateSelected(AMSamplePlate*)), this, SLOT(onSamplePlateSelected(AMSamplePlate*)));

	connect(wizardSelectorView_, SIGNAL(beamWizardPressed()), cameraBrowserView_, SIGNAL(beamWizardPressed()));
	connect(wizardSelectorView_, SIGNAL(cameraWizardPressed()), cameraBrowserView_, SIGNAL(cameraWizardPressed()));
	connect(wizardSelectorView_, SIGNAL(samplePlateWizardPressed()), cameraBrowserView_, SIGNAL(samplePlateWizardPressed()));
	connect(cameraBrowserView_, SIGNAL(beamWizardFinished()), wizardSelectorView_, SLOT(onBeamWizardFinished()));
	connect(cameraBrowserView_, SIGNAL(cameraWizardFinished()), wizardSelectorView_, SLOT(onCameraWizardFinished()));
	connect(cameraBrowserView_, SIGNAL(samplePlateWizardFinished()), wizardSelectorView_, SLOT(onSamplePlateWizardFinished()));
}

void AMBeamlineSampleManagementView::onCreateSamplePlateButtonClicked(){
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
		connect(cameraBrowserView_->sampleCameraView(), SIGNAL(shapePropertyUpdated(AMShapeData*)), samplePlate, SLOT(onShapeDataPropertyUpdated(AMShapeData*)));
	}
}

void AMBeamlineSampleManagementView::onSamplePlateSelected(AMSamplePlate *samplePlate){
	beamline_->setSamplePlate(samplePlate);
}
