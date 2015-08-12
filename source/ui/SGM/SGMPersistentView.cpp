#include "SGMPersistentView.h"

#include <QVBoxLayout>
#include <QGroupBox>

#include "beamline/SGM/SGMBeamline.h"
#include "ui/AMMotorGroupView.h"
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
	AMMotorGroupView* ssaManipulatorMotorGroupView =
			new AMMotorGroupView(SGMBeamline::sgm()->motorGroup(), AMMotorGroupView::CompactView);

	QGroupBox* sgmControlsGroupBox = new QGroupBox("SGM Beamline");
	sgmControlsGroupBox->setFlat(true);


	QVBoxLayout* controlsGroupLayout = new QVBoxLayout();

	controlsGroupLayout->addWidget(energyControlEditor_);
	controlsGroupLayout->addWidget(exitSlitControlEditor_);
	controlsGroupLayout->addWidget(gratingSelectionControlEditor_);
	controlsGroupLayout->addWidget(ssaManipulatorMotorGroupView);

	controlsGroupLayout->addStretch();

	sgmControlsGroupBox->setLayout(controlsGroupLayout);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(sgmControlsGroupBox);

	setLayout(mainLayout);
}
