#include "BioXASSSRLMonochromatorEnergyView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASSSRLMonochromatorEnergyView::BioXASSSRLMonochromatorEnergyView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	mono_ = 0;

	// Create UI elements.

	stepEnergyEditor_ = new AMExtendedControlEditor(0);
	stepEnergyEditor_->setTitle("Energy (step)");
	stepEnergyEditor_->setControlFormat('f', 2);

	encoderEnergyEditor_ = new AMExtendedControlEditor(0);
	encoderEnergyEditor_->setTitle("Energy (encoder)");
	encoderEnergyEditor_->setControlFormat('f', 2);

	stepBraggEditor_ = new AMExtendedControlEditor(0);
	stepBraggEditor_->setTitle("Goniometer (step)");
	stepBraggEditor_->setControlFormat('f', 2);

	encoderBraggEditor_ = new AMExtendedControlEditor(0);
	encoderBraggEditor_->setTitle("Goniometer (encoder)");
	encoderBraggEditor_->setControlFormat('f', 2);

	mirrorPitchEditor_ = new AMExtendedControlEditor(0);
	mirrorPitchEditor_->setTitle("M1 mirror pitch");
	mirrorPitchEditor_->setControlFormat('f', 2);

	// Create and set main layout.

	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->addWidget(stepEnergyEditor_, 0, 0);
	gridLayout->addWidget(encoderEnergyEditor_, 0, 1);
	gridLayout->addWidget(stepBraggEditor_, 1, 0);
	gridLayout->addWidget(encoderBraggEditor_, 1, 1);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addLayout(gridLayout);
	layout->addWidget(mirrorPitchEditor_);

	setLayout(layout);

	// Current settings.

	setMono(mono);

	refresh();
}

BioXASSSRLMonochromatorEnergyView::~BioXASSSRLMonochromatorEnergyView()
{

}

void BioXASSSRLMonochromatorEnergyView::refresh()
{
	// Clear the view.

	stepEnergyEditor_->setControl(0);
	encoderEnergyEditor_->setControl(0);
	stepBraggEditor_->setControl(0);
	encoderBraggEditor_->setControl(0);
	mirrorPitchEditor_->setControl(0);

	// Update view elements.

	updateStepEnergyEditor();
	updateEncoderEnergyEditor();
	updateStepBraggEditor();
	updateEncoderBraggEditor();
	updateMirrorPitchEditor();
}

void BioXASSSRLMonochromatorEnergyView::setMono(BioXASSSRLMonochromator *newControl)
{
	if (mono_ != newControl) {

		if (mono_)
			disconnect( mono_, 0, this, 0 );

		mono_ = newControl;

		if (mono_) {
			connect( mono_, SIGNAL(stepEnergyChanged(BioXASSSRLMonochromatorEnergyControl*)), this, SLOT(updateStepEnergyEditor()) );
			connect( mono_, SIGNAL(encoderEnergyChanged(BioXASSSRLMonochromatorEnergyControl*)), this, SLOT(updateEncoderEnergyEditor()) );
			connect( mono_, SIGNAL(stepBraggChanged(CLSMAXvMotor*)), this, SLOT(updateStepBraggEditor()) );
			connect( mono_, SIGNAL(encoderBraggChanged(CLSMAXvMotor*)), this, SLOT(updateEncoderBraggEditor()) );
			connect( mono_, SIGNAL(m1MirrorPitchControlChanged(AMControl*)), this, SLOT(updateMirrorPitchEditor()) );
		}

		refresh();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorEnergyView::updateStepEnergyEditor()
{
	AMControl *energyControl = 0;

	if (mono_)
		energyControl = mono_->stepEnergy();

	stepEnergyEditor_->setControl(energyControl);
}

void BioXASSSRLMonochromatorEnergyView::updateEncoderEnergyEditor()
{
	AMControl *energyControl = 0;

	if (mono_)
		energyControl = mono_->encoderEnergy();

	encoderEnergyEditor_->setControl(energyControl);
}

void BioXASSSRLMonochromatorEnergyView::updateStepBraggEditor()
{
	AMControl *braggControl = 0;

	if (mono_)
		braggControl = mono_->stepBragg();

	stepBraggEditor_->setControl(braggControl);
}

void BioXASSSRLMonochromatorEnergyView::updateEncoderBraggEditor()
{
	AMControl *braggControl = 0;

	if (mono_)
		braggControl = mono_->encoderBragg();

	encoderBraggEditor_->setControl(braggControl);
}

void BioXASSSRLMonochromatorEnergyView::updateMirrorPitchEditor()
{
	AMControl *pitchControl = 0;

	if (mono_)
		pitchControl = mono_->m1MirrorPitch();

	mirrorPitchEditor_->setControl(pitchControl);
}
