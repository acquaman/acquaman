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
	positionGroupBox->setLayout(positionLayout);
	positionLayout->addWidget(hexapodPositionView, 0, Qt::AlignTop);

	QVBoxLayout* trajectoryLayout = new QVBoxLayout();
	trajectoryLayout->addWidget(hexapodTrajectoryView_, 0, Qt::AlignTop);
	trajectoryGroupBox->setLayout(trajectoryLayout);

	QVBoxLayout* rotationLayout = new QVBoxLayout();
	rotationGroupBox->setLayout(rotationLayout);
	rotationLayout->addWidget(hexapodSystemRotationView_, 0, Qt::AlignTop);

	mainLayout->addWidget(positionGroupBox);
	mainLayout->addWidget(trajectoryGroupBox);
	mainLayout->addWidget(rotationGroupBox);

	mainLayout->addStretch();
	setLayout(mainLayout);

}
