#include "AMBeamlineSampleManagementView.h"

#include <QBoxLayout>

#include "ui/beamline/camera/AMSampleCameraBrowserView.h"
#include "beamline/camera/AMSampleCameraBrowser.h"
#include "ui/dataman/AMSamplePlateView.h"
#include "beamline/AMBeamline.h"

AMBeamlineSampleManagementView::AMBeamlineSampleManagementView(AMBeamline *beamline, QWidget *parent) :
	QWidget(parent)
{
	beamline_ = beamline;

    cameraBrowserView_ = new AMSampleCameraBrowserView(new AMSampleCameraBrowser());
	samplePlateView_ = new AMSamplePlateView(beamline_->samplePlate());

	QVBoxLayout *leftVL = new QVBoxLayout();
	QVBoxLayout *rightVL = new QVBoxLayout();

	leftVL->addWidget(cameraBrowserView_);

	rightVL->addWidget(samplePlateView_);

	QHBoxLayout *mainHL = new QHBoxLayout();
	mainHL->addLayout(leftVL);
	mainHL->addLayout(rightVL);

	setLayout(mainHL);
}
