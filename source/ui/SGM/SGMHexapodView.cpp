#include "SGMHexapodView.h"

#include <QVBoxLayout>
#include <QGridLayout>

#include <QGroupBox>

#include "ui/SGM/SGMHexapodSystemRotationView.h"
#include "ui/SGM/SGMHexapodTrajectoryView.h"

#include "beamline/AMMotorGroup.h"
#include "ui/AMMotorGroupView.h"
#include "beamline/SGM/SGMBeamline.h"
SGMHexapodView::SGMHexapodView(SGMHexapod* hexapod, QWidget *parent) :
    QWidget(parent)
{
	hexapod_ = hexapod;
	setupUi();
}

void SGMHexapodView::setupUi()
{
	QVBoxLayout* mainLayout = new QVBoxLayout();

	hexapodSystemRotationView_ = new SGMHexapodSystemRotationView(hexapod_);
	hexapodTrajectoryView_ = new SGMHexapodTrajectoryView(hexapod_);
	AMMotorGroupObjectView* hexapodPositionView = new AMMotorGroupObjectView(SGMBeamline::sgm()->sampleManipulatorsMotorGroup()->motorGroupObject("Hexapod Manipulator"));
	QGroupBox* rotationGroupBox = new QGroupBox("Rotate Coordinate System");
	QGroupBox* trajectoryGroupBox = new QGroupBox("Hexapod Trajectory Movement");
	QGroupBox* positionGroupBox = new QGroupBox("Hexapod Position");

	QVBoxLayout* positionLayout = new QVBoxLayout();
	positionLayout->addWidget(hexapodPositionView);
	positionGroupBox->setLayout(positionLayout);

	QVBoxLayout* trajectoryLayout = new QVBoxLayout();
	trajectoryLayout->addWidget(hexapodTrajectoryView_);
	trajectoryGroupBox->setLayout(trajectoryLayout);

	QVBoxLayout* rotationLayout = new QVBoxLayout();
	rotationLayout->addWidget(hexapodSystemRotationView_);
	rotationGroupBox->setLayout(rotationLayout);

	QHBoxLayout* layout1 = new QHBoxLayout();
	layout1->addWidget(positionGroupBox);
	layout1->addWidget(trajectoryGroupBox);
	layout1->addStretch();

	QHBoxLayout* layout2 = new QHBoxLayout();
	layout2->addWidget(rotationGroupBox);
	layout2->addStretch();

	mainLayout->addLayout(layout1,1);
	mainLayout->addLayout(layout2,1);

	setLayout(mainLayout);

}
