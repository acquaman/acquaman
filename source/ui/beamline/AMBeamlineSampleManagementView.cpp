#include "AMBeamlineSampleManagementView.h"

#include <QBoxLayout>

#include "ui/AMCameraBrowserView.h"
#include "ui/AMCameraBrowser.h"
#include "ui/dataman/AMSamplePlateView.h"
#include "beamline/AMBeamline.h"

AMBeamlineSampleManagementView::AMBeamlineSampleManagementView(AMBeamline *beamline, QWidget *parent) :
	QWidget(parent)
{
	beamline_ = beamline;

	cameraBrowserView_ = new AMCameraBrowserView(new AMCameraBrowser());
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
