#include "AMBeamlineSampleManagementView.h"

#include <QBoxLayout>
#include <QPushButton>

#include "ui/AMCameraBrowserView.h"
#include "ui/AMCameraBrowser.h"
#include "ui/dataman/AMSamplePlateView.h"
#include "ui/dataman/AMSamplePlateBrowserView.h"
#include "beamline/AMBeamline.h"

AMBeamlineSampleManagementView::AMBeamlineSampleManagementView(AMBeamline *beamline, QWidget *parent) :
	QWidget(parent)
{
	beamline_ = beamline;

	cameraBrowserView_ = new AMCameraBrowserView(new AMCameraBrowser());
	samplePlateBrowserView_ = new AMSamplePlateBrowserView(new AMSamplePlateBrowser(AMDatabase::database("user")));
	samplePlateView_ = new AMSamplePlateView(beamline_->samplePlate());

	createSamplePlateButton_ = new QPushButton("Create Sample Plate");
	loadSamplePlateButton_ = new QPushButton("Load Sample Plate");

	QVBoxLayout *leftVL = new QVBoxLayout();
	QVBoxLayout *rightVL = new QVBoxLayout();

	leftVL->addWidget(cameraBrowserView_);

	rightVL->addWidget(createSamplePlateButton_);
	rightVL->addWidget(loadSamplePlateButton_);
	rightVL->addWidget(samplePlateView_);

	samplePlateBrowserView_->hide();

	QHBoxLayout *mainHL = new QHBoxLayout();
	mainHL->addLayout(leftVL);
	mainHL->addLayout(rightVL);

	setLayout(mainHL);

	connect(createSamplePlateButton_, SIGNAL(clicked()), this, SLOT(onCreateSamplePlateButtonClicked()));
	connect(loadSamplePlateButton_, SIGNAL(clicked()), this, SLOT(onLoadSamplePlateButtonClicked()));
	connect(beamline_, SIGNAL(samplePlateChanged(AMSamplePlate*)), samplePlateView_, SLOT(setSamplePlate(AMSamplePlate*)));
}

void AMBeamlineSampleManagementView::onCreateSamplePlateButtonClicked(){
	AMSamplePlateCreationDialog creationDialog;
	creationDialog.exec();

	AMSamplePlate *samplePlate = new AMSamplePlate();
	samplePlate->setName(creationDialog.samplePlateName());

	beamline_->setSamplePlate(samplePlate);
}

void AMBeamlineSampleManagementView::onLoadSamplePlateButtonClicked(){
	if(!samplePlateBrowserView_->isHidden())
		samplePlateBrowserView_->raise();
	else
		samplePlateBrowserView_->show();
}
