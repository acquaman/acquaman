#include "SGMPersistentView.h"

#include <QVBoxLayout>
#include <QGroupBox>

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMHexapod.h"
#include "ui/AMMotorGroupView.h"
#include "ui/SGM/SGMHexapodTrajectoryView.h"

SGMPersistentView::SGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
	setFixedWidth(400);

	setupUi();
}

void SGMPersistentView::setupUi()
{
	energyControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->energy());
	exitSlitControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->exitSlitGap());
	gratingSelectionControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->grating());

	hexapodVelocityControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->hexapod()->systemVelocity());
	hexapodTrajectoryView_ = new SGMHexapodTrajectoryView(SGMBeamline::sgm()->hexapod());

	AMMotorGroupView* manipulatorsMotorGroupView =
			new AMMotorGroupView(SGMBeamline::sgm()->motorGroup(), AMMotorGroupView::CompactView);


	QGroupBox* sgmControlsGroupBox = new QGroupBox("SGM Beamline");
	sgmControlsGroupBox->setFlat(true);


	QVBoxLayout* controlsGroupLayout = new QVBoxLayout();

	controlsGroupLayout->addWidget(energyControlEditor_);
	controlsGroupLayout->addWidget(exitSlitControlEditor_);
	controlsGroupLayout->addWidget(gratingSelectionControlEditor_);
	controlsGroupLayout->addWidget(hexapodVelocityControlEditor_);
	controlsGroupLayout->addWidget(manipulatorsMotorGroupView);

	controlsGroupLayout->addStretch();

	sgmControlsGroupBox->setLayout(controlsGroupLayout);

	QGroupBox* hexapodTrajectoryGroupBox = new QGroupBox("Hexapod Trajectory Motions");
	hexapodTrajectoryGroupBox->setFlat(true);

	QVBoxLayout* trajectoryGroupLayout = new QVBoxLayout();
	hexapodTrajectoryGroupBox->setLayout(trajectoryGroupLayout);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(sgmControlsGroupBox);
	mainLayout->addWidget(hexapodTrajectoryGroupBox);
	setLayout(mainLayout);
}
