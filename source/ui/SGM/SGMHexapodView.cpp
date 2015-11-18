#include "SGMHexapodView.h"

#include <QVBoxLayout>
#include <QGridLayout>

#include <QGroupBox>

#include "ui/SGM/SGMHexapodSystemRotationView.h"
#include "ui/SGM/SGMHexapodTrajectoryView.h"

#include "beamline/AMMotorGroup.h"
#include "ui/AMMotorGroupView.h"
#include "beamline/SGM/SGMBeamline.h"

#include "beamline/SGM/SGMHexapod.h"
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

	// Test
	testCoordinatedMoveButton_ = new QPushButton("Test Move");
	mainLayout->addWidget(testCoordinatedMoveButton_);
	connect(testCoordinatedMoveButton_, SIGNAL(clicked()), this, SLOT(onTestMoveButtonPushed()));
	// End Test


}
// TEst
#include "actions3/AMActionRunner3.h"
void SGMHexapodView::onTestMoveButtonPushed()
{
	AMListAction3* testMovement = new AMListAction3(new AMListActionInfo3("",""), AMListAction3::Sequential);
	testMovement->addSubAction(hexapod_->xAxisPrimeControl()->createSetParameterActions(0, 2, 10));
	testMovement->addSubAction(hexapod_->xAxisPrimeControl()->createStartCoordinatedMovementActions());
	testMovement->addSubAction(hexapod_->xAxisPrimeControl()->createWaitForCompletionActions());

	AMActionRunner3::workflow()->addActionToQueue(testMovement);

}
// End Test
