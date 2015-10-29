#include "BioXASSideSOEBeamStatusControlEditorsView.h"

BioXASSideSOEBeamStatusControlEditorsView::BioXASSideSOEBeamStatusControlEditorsView(BioXASSideSOEBeamStatusControl *control, QWidget *parent) :
    BioXASSideSOEBeamStatusControlView(parent)
{
	// Create UI elements.

	beamStatusEditor_ = new AMExtendedControlEditor(0);
	beamStatusEditor_->setNoUnitsBox(true);

	shutterEditor_ = new AMExtendedControlEditor(0);
	shutterEditor_->setNoUnitsBox(true);

	// Create and set layouts.

	QVBoxLayout *controlsBoxLayout = new QVBoxLayout();
	controlsBoxLayout->addWidget(shutterEditor_);

	QGroupBox *controlsBox = new QGroupBox();
	controlsBox->setFlat(true);
	controlsBox->setLayout(controlsBoxLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(beamStatusEditor_);
	layout->addWidget(controlsBox);

	setLayout(layout);

	// Current settings.

	setControl(control);
	refresh();
}

BioXASSideSOEBeamStatusControlEditorsView::~BioXASSideSOEBeamStatusControlEditorsView()
{

}

void BioXASSideSOEBeamStatusControlEditorsView::clear()
{
	beamStatusEditor_->setControl(0);
	shutterEditor_->setControl(0);
}

void BioXASSideSOEBeamStatusControlEditorsView::update()
{
	beamStatusEditor_->setControl(control_);

	if (control_)
		shutterEditor_->setControl(control_->endstationShutter());
	else
		shutterEditor_->setControl(0);
}
