#include "SGMPersistentView.h"

#include <QVBoxLayout>
#include <QGroupBox>

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMHexapod.h"
#include "ui/AMMotorGroupView.h"
#include "ui/SGM/SGMHexapodTrajectoryView.h"
#include "ui/SGM/SGMEnergyView.h"
#include "ui/SGM/SGMBeamOnControl.h"

SGMPersistentView::SGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
	setFixedWidth(400);

	setupUi();
}

void SGMPersistentView::setupUi()
{

	energyView_ = new SGMEnergyView(SGMBeamline::sgm()->energyControlSet());
	beamControl_ = new SGMBeamOnControl(SGMBeamline::sgm()->beamStatusControl(),
					    SGMBeamline::sgm()->beamOnOperationControl(),
					    SGMBeamline::sgm()->beamOffOperationControl());

	exitSlitGapControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->exitSlitGap());
	endStationTranslationSetpointControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->endStationTranslationSetpoint());
	endStationTranslationFeedbackControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->endStationTranslationFeedback());
	endStationLinearStageControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->endStationLinearStage());

	hexapodVelocityControlEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->hexapod()->systemVelocity());


	AMMotorGroupView* manipulatorsMotorGroupView =
			new AMMotorGroupView(SGMBeamline::sgm()->sampleManipulatorsMotorGroup(), AMMotorGroupView::CompactView);


	QGroupBox* sgmControlsGroupBox = new QGroupBox("SGM Beamline");
	sgmControlsGroupBox->setFlat(true);
	QGroupBox* endStationGroupBox = new QGroupBox("End Station Translation");

	QVBoxLayout* controlsGroupLayout = new QVBoxLayout();

	controlsGroupLayout->addWidget(energyView_);
	controlsGroupLayout->addWidget(beamControl_);
	controlsGroupLayout->addWidget(exitSlitGapControlEditor_);
	controlsGroupLayout->addWidget(hexapodVelocityControlEditor_);
	QHBoxLayout* endStationTranslationLayout = new QHBoxLayout();

	endStationTranslationLayout->addWidget(endStationTranslationSetpointControlEditor_);
	endStationTranslationLayout->addWidget(endStationTranslationFeedbackControlEditor_);
	endStationTranslationFeedbackControlEditor_->setPrecision(12);
	endStationGroupBox->setLayout(endStationTranslationLayout);
	controlsGroupLayout->addWidget(endStationGroupBox);
	controlsGroupLayout->addWidget(manipulatorsMotorGroupView);
	controlsGroupLayout->addWidget(endStationLinearStageControlEditor_);
	endStationLinearStageControlEditor_->setPrecision(6);

	sgmControlsGroupBox->setLayout(controlsGroupLayout);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(sgmControlsGroupBox);
	mainLayout->addStretch();
	setLayout(mainLayout);
}
