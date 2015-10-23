#include "BioXASFrontEndBeamStatusControlEditorsView.h"

BioXASFrontEndBeamStatusControlEditorsView::BioXASFrontEndBeamStatusControlEditorsView(BioXASFrontEndBeamStatusControl *control, QWidget *parent) :
    BioXASFrontEndBeamStatusControlView(parent)
{
	// Create UI elements.

	beamStatusEditor_ = new AMExtendedControlEditor(0);
	beamStatusEditor_->setNoUnitsBox(true);

	shuttersEditor_ = new AMExtendedControlEditor(0);
	shuttersEditor_->setNoUnitsBox(true);

	valvesEditor_ = new AMExtendedControlEditor(0);
	valvesEditor_->setNoUnitsBox(true);

	// Create and set layouts.

	QVBoxLayout *controlsBoxLayout = new QVBoxLayout();
	controlsBoxLayout->addWidget(shuttersEditor_);
	controlsBoxLayout->addWidget(valvesEditor_);

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

BioXASFrontEndBeamStatusControlEditorsView::~BioXASFrontEndBeamStatusControlEditorsView()
{

}

void BioXASFrontEndBeamStatusControlEditorsView::clear()
{
	beamStatusEditor_->setControl(0);
	shuttersEditor_->setControl(0);
	valvesEditor_->setControl(0);
}

void BioXASFrontEndBeamStatusControlEditorsView::update()
{
	beamStatusEditor_->setControl(control_);

	if (control_) {
		shuttersEditor_->setControl(control_->shutters());
		valvesEditor_->setControl(control_->valves());
	}
}

void BioXASFrontEndBeamStatusControlEditorsView::refresh()
{
	// Clears the view.

	clear();

	// Updates the view.

	update();
}
