#include "BioXASSSRLMonochromatorConfigurationView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorEnergyView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorCrystalsView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASSSRLMonochromatorConfigurationView::BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mono_ = 0;

	// Create energy view.

	energyView_ = new BioXASSSRLMonochromatorEnergyView(0);

	QVBoxLayout *energyBoxLayout = new QVBoxLayout();
	energyBoxLayout->addWidget(energyView_);

	QGroupBox *energyBox = new QGroupBox();
	energyBox->setTitle("Energy");
	energyBox->setLayout(energyBoxLayout);

	// Create motors view.

	upperBladeEditor_ = new AMExtendedControlEditor(0);
	upperBladeEditor_->setTitle("Upper blade");
	upperBladeEditor_->setControlFormat('f', 3);

	lowerBladeEditor_ = new AMExtendedControlEditor(0);
	lowerBladeEditor_->setTitle("Lower blade");
	lowerBladeEditor_->setControlFormat('f', 3);

	heightEditor_ = new AMExtendedControlEditor(0);
	heightEditor_->setTitle("Height");
	heightEditor_->setControlFormat('f', 3);

	lateralEditor_ = new AMExtendedControlEditor(0);
	lateralEditor_->setTitle("Lateral");
	lateralEditor_->setControlFormat('f', 3);

	paddleEditor_ = new AMExtendedControlEditor(0);
	paddleEditor_->setTitle("Paddle");

	QVBoxLayout *motorsBoxLayout = new QVBoxLayout();
	motorsBoxLayout->addWidget(upperBladeEditor_);
	motorsBoxLayout->addWidget(lowerBladeEditor_);
	motorsBoxLayout->addWidget(heightEditor_);
	motorsBoxLayout->addWidget(lateralEditor_);
	motorsBoxLayout->addWidget(paddleEditor_);

	QGroupBox *motorsBox = new QGroupBox("Motors");
	motorsBox->setLayout(motorsBoxLayout);

	// Create crystals view.

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

	// Create and set main layouts.

	QVBoxLayout *energyColumnLayout = new QVBoxLayout();
	energyColumnLayout->addWidget(energyBox);
	energyColumnLayout->addWidget(motorsBox);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addLayout(energyColumnLayout);
	layout->addWidget(crystalsBox);

	setLayout(layout);

	// Current settings

	setMono(mono);
}

BioXASSSRLMonochromatorConfigurationView::~BioXASSSRLMonochromatorConfigurationView()
{

}

void BioXASSSRLMonochromatorConfigurationView::refresh()
{
	// Clear UI elements.

	energyView_->setMono(0);

	upperBladeEditor_->setControl(0);
	lowerBladeEditor_->setControl(0);
	heightEditor_->setControl(0);
	lateralEditor_->setControl(0);
	paddleEditor_->setControl(0);

	regionEditor_->setControl(0);
	regionStatusWidget_->setRegionControl(0);
	crystalsView_->setMono(0);

	// Update view elements.

	energyView_->setMono(mono_);

	updateUpperBladeEditor();
	updateLowerBladeEditor();
	updateHeightEditor();
	updateLateralEditor();
	updatePaddleEditor();

	crystalsView_->setMono(mono_);

	if (mono_) {

		regionEditor_->setControl(mono_->region());
		regionStatusWidget_->setRegionControl(mono_->region());
	}
}

void BioXASSSRLMonochromatorConfigurationView::setMono(BioXASSSRLMonochromator *newMono)
{
	if (mono_ != newMono) {

		if (mono_)
			disconnect( mono_, 0, this, 0 );

		mono_ = newMono;

		if (mono_) {
			connect( mono_, SIGNAL(upperSlitChanged(CLSMAXvMotor*)), this, SLOT(updateUpperBladeEditor()) );
			connect( mono_, SIGNAL(lowerSlitChanged(CLSMAXvMotor*)), this, SLOT(updateLowerBladeEditor()) );
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

void BioXASSSRLMonochromatorConfigurationView::updateUpperBladeEditor()
{
	AMControl *upperBladeControl = 0;

	if (mono_)
		upperBladeControl = mono_->upperSlit();

	upperBladeEditor_->setControl(upperBladeControl);
}

void BioXASSSRLMonochromatorConfigurationView::updateLowerBladeEditor()
{
	AMControl *lowerBladeControl = 0;

	if (mono_)
		lowerBladeControl = mono_->lowerSlit();

	lowerBladeEditor_->setControl(lowerBladeControl);
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

	regionStatusWidget_->setRegionControl(regionControl);
}
