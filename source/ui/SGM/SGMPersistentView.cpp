#include "SGMPersistentView.h"

#include <QVBoxLayout>
#include <QGroupBox>

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMHexapod.h"
#include "beamline/SGM/SGMSampleChamber.h"
#include "ui/AMMotorGroupView.h"
#include "ui/SGM/SGMHexapodTrajectoryView.h"
#include "ui/SGM/SGMEnergyView.h"
#include "ui/SGM/SGMChamberLightSlider.h"
SGMPersistentView::SGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
	setFixedWidth(400);

	setupUi();
}

void SGMPersistentView::setupUi()
{

	energyView_ = new SGMEnergyView(SGMBeamline::sgm()->energyControlSet());
        chamberLightSlider_  = new SGMChamberLightSlider(SGMBeamline::sgm()->sampleChamber()->chamberLight(), Qt::Horizontal);
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

        QVBoxLayout* chamberLightLayout = new QVBoxLayout();
        chamberLightLayout->addWidget(new QLabel("Chamber Light"));
        chamberLightLayout->addWidget(chamberLightSlider_);
        controlsGroupLayout->addLayout(chamberLightLayout);

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
