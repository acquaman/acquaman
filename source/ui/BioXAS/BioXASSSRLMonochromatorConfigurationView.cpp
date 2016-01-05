#include "BioXASSSRLMonochromatorConfigurationView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorEnergyView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorCalibrationView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorMaskView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorCrystalsView.h"
#include "ui/CLS/CLSMAXvMotorConfigurationView.h"

#define BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_COLUMN_WIDTH_MIN 300

BioXASSSRLMonochromatorConfigurationView::BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mono_ = 0;

	// Create motors column contents.

	maskView_ = new BioXASSSRLMonochromatorMaskView(0);

	QVBoxLayout *maskBoxLayout = new QVBoxLayout();
	maskBoxLayout->addWidget(maskView_);

	QGroupBox *maskBox = new QGroupBox();
	maskBox->setTitle("Mask");
	maskBox->setLayout(maskBoxLayout);

	heightEditor_ = new AMExtendedControlEditor(0);
	heightEditor_->setTitle("Height");
	heightEditor_->setControlFormat('f', 3);

	lateralEditor_ = new AMExtendedControlEditor(0);
	lateralEditor_->setTitle("Lateral");
	lateralEditor_->setControlFormat('f', 3);

	paddleEditor_ = new AMExtendedControlEditor(0);
	paddleEditor_->setTitle("Paddle");

	QVBoxLayout *motorsBoxLayout = new QVBoxLayout();
	motorsBoxLayout->setMargin(0);
	motorsBoxLayout->addWidget(maskBox);
	motorsBoxLayout->addWidget(heightEditor_);
	motorsBoxLayout->addWidget(lateralEditor_);
	motorsBoxLayout->addWidget(paddleEditor_);

	QWidget *motorsBox = new QWidget();
	motorsBox->setLayout(motorsBoxLayout);
	motorsBox->setMinimumWidth(BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_COLUMN_WIDTH_MIN);

	// Create energy column contents.

	energyView_ = new BioXASSSRLMonochromatorEnergyView(0);

	QVBoxLayout *energyBoxLayout = new QVBoxLayout();
	energyBoxLayout->addWidget(energyView_);

	QGroupBox *energyBox = new QGroupBox();
	energyBox->setTitle("Energy");
	energyBox->setLayout(energyBoxLayout);
	energyBox->setMinimumWidth(BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_COLUMN_WIDTH_MIN);

	// Create crystals column contents.

	regionEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(0);
	regionEditor_->setTitle("Region");

	regionStatusWidget_ = new BioXASSSRLMonochromatorRegionControlView(0);

	crystalsView_ = new BioXASSSRLMonochromatorCrystalsView(0);

	QVBoxLayout *regionStatusViewLayout = new QVBoxLayout();
	regionStatusViewLayout->setMargin(0);
	regionStatusViewLayout->addWidget(regionStatusWidget_);

	QGroupBox *regionStatusView = new QGroupBox("Region Status");
	regionStatusView->setFlat(true);
	regionStatusView->setLayout(regionStatusViewLayout);

	QVBoxLayout *crystalsBoxLayout = new QVBoxLayout();
	crystalsBoxLayout->addWidget(regionEditor_);
	crystalsBoxLayout->addWidget(regionStatusView);
	crystalsBoxLayout->addWidget(crystalsView_);
	crystalsBoxLayout->addStretch();

	QGroupBox *crystalsBox = new QGroupBox("Crystals");
	crystalsBox->setLayout(crystalsBoxLayout);
	crystalsBox->setMinimumWidth(BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_COLUMN_WIDTH_MIN);

	// Create and set main layouts.

	QVBoxLayout *motorsColumnLayout = new QVBoxLayout();
	motorsColumnLayout->addWidget(motorsBox);
	motorsColumnLayout->addStretch();

	QVBoxLayout *energyColumnLayout = new QVBoxLayout();
	energyColumnLayout->addWidget(energyBox);
	energyColumnLayout->addStretch();

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addLayout(motorsColumnLayout);
	layout->addLayout(energyColumnLayout);
	layout->addWidget(crystalsBox);

	setLayout(layout);

	// Current settings

	setMono(mono);

	refresh();
}

BioXASSSRLMonochromatorConfigurationView::~BioXASSSRLMonochromatorConfigurationView()
{

}

void BioXASSSRLMonochromatorConfigurationView::refresh()
{
	// Clear UI elements.

	maskView_->setMask(0);

	heightEditor_->setControl(0);
	lateralEditor_->setControl(0);
	paddleEditor_->setControl(0);

	energyView_->setMono(0);

	regionEditor_->setControl(0);
	regionStatusWidget_->setControl(0);
	crystalsView_->setMono(0);

	// Update view elements.

	updateMaskView();

	updateHeightEditor();
	updateLateralEditor();
	updatePaddleEditor();

	energyView_->setMono(mono_);

	crystalsView_->setMono(mono_);

	updateRegionEditor();
	updateRegionStatusView();
}

void BioXASSSRLMonochromatorConfigurationView::setMono(BioXASSSRLMonochromator *newMono)
{
	if (mono_ != newMono) {

		if (mono_)
			disconnect( mono_, 0, this, 0 );

		mono_ = newMono;

		if (mono_) {
			connect( mono_, SIGNAL(verticalChanged(CLSMAXvMotor*)), this, SLOT(updateHeightEditor()) );
			connect( mono_, SIGNAL(lateralChanged(CLSMAXvMotor*)), this, SLOT(updateLateralEditor()) );
			connect( mono_, SIGNAL(paddleChanged(CLSMAXvMotor*)), this, SLOT(updatePaddleEditor()) );
			connect( mono_, SIGNAL(regionChanged(AMControl*)), this, SLOT(updateRegionEditor()) );
			connect( mono_, SIGNAL(regionChanged(AMControl*)), this, SLOT(updateRegionStatusView()) );
		}

		refresh();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorConfigurationView::updateMaskView()
{
	BioXASSSRLMonochromatorMask *mask = 0;

	if (mono_)
		mask = mono_->mask();

	maskView_->setMask(mask);
}

void BioXASSSRLMonochromatorConfigurationView::updateHeightEditor()
{
	AMControl *heightControl = 0;

	if (mono_)
		heightControl = mono_->vertical();

	heightEditor_->setControl(heightControl);
}

void BioXASSSRLMonochromatorConfigurationView::updateLateralEditor()
{
	AMControl *lateralControl = 0;

	if (mono_)
		lateralControl = mono_->lateral();

	lateralEditor_->setControl(lateralControl);
}

void BioXASSSRLMonochromatorConfigurationView::updatePaddleEditor()
{
	AMControl *paddleControl = 0;

	if (mono_)
		paddleControl = mono_->paddle();

	paddleEditor_->setControl(paddleControl);
}

void BioXASSSRLMonochromatorConfigurationView::updateRegionEditor()
{
	BioXASSSRLMonochromatorRegionControl *regionControl = 0;

	if (mono_)
		regionControl = mono_->region();

	regionEditor_->setControl(regionControl);
}

void BioXASSSRLMonochromatorConfigurationView::updateRegionStatusView()
{
	BioXASSSRLMonochromatorRegionControl *regionControl = 0;

	if (mono_)
		regionControl = mono_->region();

	regionStatusWidget_->setControl(regionControl);
}
