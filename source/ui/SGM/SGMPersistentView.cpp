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
	exitSlitGapControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->exitSlitGap());
	exitSlitPositionControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->exitSlitPosition());
	endStationTranslationSetpointControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->endStationTranslationSetpoint());
	endStationTranslationFeedbackControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->endStationTranslationFeedback());
	gratingSelectionControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->grating());

	hexapodVelocityControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->hexapod()->systemVelocity());
	hexapodTrajectoryView_ = new SGMHexapodTrajectoryView(SGMBeamline::sgm()->hexapod());

	AMMotorGroupView* manipulatorsMotorGroupView =
			new AMMotorGroupView(SGMBeamline::sgm()->sampleManipulatorsMotorGroup(), AMMotorGroupView::CompactView);


	QGroupBox* sgmControlsGroupBox = new QGroupBox("SGM Beamline");
	sgmControlsGroupBox->setFlat(true);
	QGroupBox* endStationGroupBox = new QGroupBox("End Station Translation");

	QVBoxLayout* controlsGroupLayout = new QVBoxLayout();

	controlsGroupLayout->addWidget(energyControlEditor_);
	controlsGroupLayout->addWidget(exitSlitGapControlEditor_);
	controlsGroupLayout->addWidget(exitSlitPositionControlEditor_);
	controlsGroupLayout->addWidget(gratingSelectionControlEditor_);
	controlsGroupLayout->addWidget(hexapodVelocityControlEditor_);
	QHBoxLayout* endStationTranslationLayout = new QHBoxLayout();

	endStationTranslationLayout->addWidget(endStationTranslationSetpointControlEditor_);
	endStationTranslationLayout->addWidget(endStationTranslationFeedbackControlEditor_);
	endStationGroupBox->setLayout(endStationTranslationLayout);
	controlsGroupLayout->addWidget(endStationGroupBox);
	controlsGroupLayout->addWidget(manipulatorsMotorGroupView);


	sgmControlsGroupBox->setLayout(controlsGroupLayout);

	QGroupBox* hexapodTrajectoryGroupBox = new QGroupBox("Hexapod Trajectory Motions");
	hexapodTrajectoryGroupBox->setFlat(true);

	QVBoxLayout* trajectoryGroupLayout = new QVBoxLayout();
	trajectoryGroupLayout->addWidget(hexapodTrajectoryView_);
	hexapodTrajectoryGroupBox->setLayout(trajectoryGroupLayout);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(sgmControlsGroupBox);
	mainLayout->addWidget(hexapodTrajectoryGroupBox);
	mainLayout->addStretch();
	setLayout(mainLayout);
}
