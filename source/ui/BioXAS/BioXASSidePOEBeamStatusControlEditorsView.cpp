#include "BioXASSidePOEBeamStatusControlEditorsView.h"

BioXASSidePOEBeamStatusControlEditorsView::BioXASSidePOEBeamStatusControlEditorsView(BioXASSidePOEBeamStatusControl *control, QWidget *parent) :
    BioXASSidePOEBeamStatusControlView(parent)
{
	// Create UI elements.

	beamStatusEditor_ = new AMExtendedControlEditor(0);
	beamStatusEditor_->setNoUnitsBox(true);

	mirrorMaskEditor_ = new AMExtendedControlEditor(0);

	monoMaskEditor_ = new AMExtendedControlEditor(0);
	monoMaskEditor_->setNoUnitsBox(true);

	// Create and set layouts.

	QVBoxLayout *controlsBoxLayout = new QVBoxLayout();
	controlsBoxLayout->addWidget(mirrorMaskEditor_);
	controlsBoxLayout->addWidget(monoMaskEditor_);

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

BioXASSidePOEBeamStatusControlEditorsView::~BioXASSidePOEBeamStatusControlEditorsView()
{

}

void BioXASSidePOEBeamStatusControlEditorsView::clear()
{
	beamStatusEditor_->setControl(0);
	mirrorMaskEditor_->setControl(0);
	monoMaskEditor_->setControl(0);
}

void BioXASSidePOEBeamStatusControlEditorsView::update()
{
	beamStatusEditor_->setControl(control_);

	if (control_) {
		mirrorMaskEditor_->setControl(control_->mirrorMask());
		monoMaskEditor_->setControl(control_->monoMask());
	}
}

void BioXASSidePOEBeamStatusControlEditorsView::refresh()
{
	// Clears the view.

	clear();

	// Updates the view.

	update();
}
